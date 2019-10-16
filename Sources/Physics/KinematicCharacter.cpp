#include "KinematicCharacter.hpp"

#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletCollision/CollisionShapes/btCompoundShape.h>
#include <BulletSoftBody/btSoftRigidDynamicsWorld.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include "Maths/Transform.hpp"
#include "Scenes/Entity.inl"
#include "Scenes/Scenes.hpp"
#include "PhysicsSystem.hpp"

namespace acid {
bool KinematicCharacter::registered = Register("kinematicCharacter");

KinematicCharacter::KinematicCharacter(std::unique_ptr<Collider> &&collider, float mass, float friction) :
	CollisionObject({}, mass, friction),
	m_up(Vector3f::Up),
	m_stepHeight(0.0f),
	m_fallSpeed(55.0f),
	m_jumpSpeed(10.0f),
	m_maxHeight(1.5f),
	m_interpolate(true) {
	AddCollider(std::move(collider));
}

KinematicCharacter::~KinematicCharacter() {
}

bool KinematicCharacter::InFrustum(const Frustum &frustum) {
	btVector3 min;
	btVector3 max;

	if (m_body && m_shape) {
		m_shape->getAabb(Collider::Convert(*GetEntity()->GetComponent<Transform>()), min, max);
	}

	return frustum.CubeInFrustum(Collider::Convert(min), Collider::Convert(max));
}

void KinematicCharacter::ClearForces() {
	//m_controller->clearForces();
}

void KinematicCharacter::SetMass(float mass) {
	m_mass = mass;
	RecalculateMass();
}

void KinematicCharacter::SetGravity(const Vector3f &gravity) {
	m_gravity = gravity;
	m_controller->setGravity(Collider::Convert(gravity));
}

void KinematicCharacter::SetLinearFactor(const Vector3f &linearFactor) {
	m_linearFactor = linearFactor;
	//m_controller->setLinearFactor(Collider::Convert(m_linearFactor)); // TODO
}

void KinematicCharacter::SetAngularFactor(const Vector3f &angularFactor) {
	m_angularFactor = angularFactor;
	//m_controller->setAngularFactor(Collider::Convert(m_angularFactor)); // TODO
}

void KinematicCharacter::SetLinearVelocity(const Vector3f &linearVelocity) {
	m_linearVelocity = linearVelocity;
	m_controller->setLinearVelocity(Collider::Convert(m_linearVelocity));
}

void KinematicCharacter::SetAngularVelocity(const Vector3f &angularVelocity) {
	m_angularVelocity = angularVelocity;
	m_controller->setAngularVelocity(Collider::Convert(m_angularVelocity));
}

void KinematicCharacter::SetUp(const Vector3f &up) {
	m_up = up;
	m_controller->setUp(Collider::Convert(up));
}

void KinematicCharacter::SetStepHeight(float stepHeight) {
	m_stepHeight = stepHeight;
	m_controller->setStepHeight(stepHeight);
}

void KinematicCharacter::SetFallSpeed(float fallSpeed) {
	m_fallSpeed = fallSpeed;
	m_controller->setFallSpeed(fallSpeed);
}

void KinematicCharacter::SetJumpSpeed(float jumpSpeed) {
	m_jumpSpeed = jumpSpeed;
	m_controller->setJumpSpeed(jumpSpeed);
}

void KinematicCharacter::SetMaxJumpHeight(float maxHeight) {
	m_maxHeight = maxHeight;
	m_controller->setMaxJumpHeight(maxHeight);
}

void KinematicCharacter::SetInterpolate(bool interpolate) {
	m_interpolate = interpolate;
	m_controller->setUpInterpolate(interpolate);
}

bool KinematicCharacter::IsOnGround() const {
	return m_controller->onGround();
}

void KinematicCharacter::Jump(const Vector3f &direction) {
	m_controller->jump(Collider::Convert(direction));
}

void KinematicCharacter::SetWalkDirection(const Vector3f &direction) {
	m_controller->setWalkDirection(Collider::Convert(direction));
}

const Node &operator>>(const Node &node, KinematicCharacter &character) {
	node["colliders"].Get(character.m_colliders);
	node["mass"].Get(character.m_mass);
	node["friction"].Get(character.m_friction);
	node["frictionRolling"].Get(character.m_frictionRolling);
	node["frictionSpinning"].Get(character.m_frictionSpinning);
	node["up"].Get(character.m_up);
	node["stepHeight"].Get(character.m_stepHeight);
	node["fallSpeed"].Get(character.m_fallSpeed);
	node["jumpSpeed"].Get(character.m_jumpSpeed);
	node["maxHeight"].Get(character.m_maxHeight);
	node["interpolate"].Get(character.m_interpolate);
	return node;
}

Node &operator<<(Node &node, const KinematicCharacter &character) {
	node["colliders"].Set(character.m_colliders);
	node["mass"].Set(character.m_mass);
	node["friction"].Set(character.m_friction);
	node["frictionRolling"].Set(character.m_frictionRolling);
	node["frictionSpinning"].Set(character.m_frictionSpinning);
	node["up"].Set(character.m_up);
	node["stepHeight"].Set(character.m_stepHeight);
	node["fallSpeed"].Set(character.m_fallSpeed);
	node["jumpSpeed"].Set(character.m_jumpSpeed);
	node["maxHeight"].Set(character.m_maxHeight);
	node["interpolate"].Set(character.m_interpolate);
	return node;
}

void KinematicCharacter::CreateCollisionObject(Transform *transform) {
	btVector3 localInertia;

	// Rigidbody is dynamic if and only if mass is non zero, otherwise static.
	if (m_mass != 0.0f) {
		m_shape->calculateLocalInertia(m_mass, localInertia);
	}

	auto worldTransform = Collider::Convert(*transform);

	m_ghostObject = std::make_unique<btPairCachingGhostObject>();
	m_ghostObject->setWorldTransform(worldTransform);
	m_ghostObject->setCollisionShape(m_shape.get());
	m_ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
	m_ghostObject->setFriction(m_friction);
	m_ghostObject->setRollingFriction(m_frictionRolling);
	m_ghostObject->setSpinningFriction(m_frictionSpinning);
	m_ghostObject->setUserPointer(dynamic_cast<CollisionObject *>(this));
	m_body = m_ghostObject.get();

	m_controller = std::make_unique<btKinematicCharacterController>(m_ghostObject.get(), static_cast<btConvexShape *>(m_shape.get()), 0.03f);
	m_controller->setGravity(Collider::Convert(m_gravity));
	m_controller->setUp(Collider::Convert(m_up));
	m_controller->setStepHeight(m_stepHeight);
	m_controller->setFallSpeed(m_fallSpeed);
	m_controller->setJumpSpeed(m_jumpSpeed);
	m_controller->setMaxJumpHeight(m_maxHeight);
	m_controller->setUpInterpolate(m_interpolate);
}

void KinematicCharacter::RecalculateMass() {
	// TODO
}
}
