#include "MeshAnimated.hpp"

#include "Maths/Maths.hpp"
#include "Files/File.hpp"
#include "Maths/Matrix4.hpp"
#include "Scenes/Entity.hpp"
#include "Animation/AnimationLoader.hpp"
#include "Skeleton/SkeletonLoader.hpp"
#include "Skin/SkinLoader.hpp"
#include "Geometry/GeometryLoader.hpp"
#include "Maths/Transform.hpp"

namespace acid {
MeshAnimated::MeshAnimated(std::filesystem::path filename, std::unique_ptr<Material> &&material) :
	material(std::move(material)),
	filename(std::move(filename)) {
}

void MeshAnimated::Start() {
	if (material)
		material->CreatePipeline(GetVertexInput(), true);

	if (filename.empty())
		return;

	//File file(filename, std::make_unique<Xml>("COLLADA"));
	//file.Load();
	//auto &fileNode = *file.GetNode();
	File file(filename);
	file.Load();
	auto fileNode = (*file.GetNode())["COLLADA"];

	// Because in Blender z is up, but Acid is y up. A correction must be applied to positions and normals.
	static const auto Correction = Matrix4().Rotate(Maths::Radians(-90.0f), Vector3f::Right);

	SkinLoader skinLoader(fileNode["library_controllers"], MaxWeights);
	SkeletonLoader skeletonLoader(fileNode["library_visual_scenes"], skinLoader.GetJointOrder(), Correction);
	GeometryLoader geometryLoader(fileNode["library_geometries"], skinLoader.GetVertexWeights(), Correction);

	model = std::make_shared<Model>(geometryLoader.GetVertices(), geometryLoader.GetIndices());
	headJoint = skeletonLoader.GetHeadJoint();

	AnimationLoader animationLoader(fileNode["library_animations"], fileNode["library_visual_scenes"], Correction);

	animation = std::make_unique<Animation>(animationLoader.GetLengthSeconds(), animationLoader.GetKeyframes());
	animator.DoAnimation(animation.get());

/*#if defined(ACID_DEBUG)
	{
		File fileModel("Animation/Model.json");
		(*fileModel.GetNode())["vertices"].Set(model->GetVertices<VertexAnimated>());
		(*fileModel.GetNode())["indices"].Set(model->GetIndices());
		fileModel.Write(Node::Format::Beautified);
	}
	{
		File fileJoints("Animation/Joints.json");
		*fileJoints.GetNode() << headJoint;
		fileJoints.Write(Node::Format::Beautified);
	}
	{
		File fileAnimation0("Animation/Animation0.json");
		*fileAnimation0.GetNode() << *animation;
		fileAnimation0.Write(Node::Format::Beautified);
	}
#endif*/
}

void MeshAnimated::Update() {
	if (material) {
		auto transform = GetEntity()->GetComponent<Transform>();
		material->PushUniforms(uniformObject, transform);
	}
	
	std::vector<Matrix4> jointMatrices(MaxJoints);
	animator.Update(headJoint, jointMatrices);
	storageAnimation.Push(jointMatrices.data(), sizeof(Matrix4) * jointMatrices.size());
}

bool MeshAnimated::CmdRender(const CommandBuffer &commandBuffer, UniformHandler &uniformScene, const Pipeline::Stage &pipelineStage) {
	if (!model || !material)
		return false;

	// Checks if the mesh is in view.
	/*if (auto rigidbody = GetEntity()->GetComponent<Rigidbody>()) {
		if (!rigidbody->InFrustum(Scenes::Get()->GetCamera()->GetViewFrustum()))
			return false;
	}*/

	// Check if we are in the correct pipeline stage.
	auto materialPipeline = material->GetPipelineMaterial();
	if (!materialPipeline || materialPipeline->GetStage() != pipelineStage)
		return false;

	// Binds the material pipeline.
	if (!materialPipeline->BindPipeline(commandBuffer))
		return false;

	const auto &pipeline = *materialPipeline->GetPipeline();

	// Updates descriptors.
	descriptorSet.Push("UniformScene", uniformScene);
	descriptorSet.Push("UniformObject", uniformObject);
	descriptorSet.Push("BufferAnimation", storageAnimation);

	material->PushDescriptors(descriptorSet);

	if (!descriptorSet.Update(pipeline))
		return false;

	// Draws the object.
	descriptorSet.BindDescriptor(commandBuffer, pipeline);
	return model->CmdRender(commandBuffer);
}

void MeshAnimated::SetMaterial(std::unique_ptr<Material> &&material) {
	this->material = std::move(material);
	this->material->CreatePipeline(GetVertexInput(), true);
}

const Node &operator>>(const Node &node, MeshAnimated &meshAnimated) {
	node["filename"].Get(meshAnimated.filename);
	node["material"].Get(meshAnimated.material);
	return node;
}

Node &operator<<(Node &node, const MeshAnimated &meshAnimated) {
	node["filename"].Set(meshAnimated.filename);
	node["material"].Set(meshAnimated.material);
	return node;
}
}
