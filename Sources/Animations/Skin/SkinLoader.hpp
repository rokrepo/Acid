#pragma once

#include <vector>
#include "Serialized/Serialized.hpp"
#include "VertexSkinData.hpp"

namespace acid
{
	class ACID_EXPORT SkinLoader
	{
	private:
		std::shared_ptr<Serialized> m_skinData;

		int m_maxWeights;

		std::vector<std::string> m_jointOrder;
		std::vector<VertexSkinData *> m_verticesSkinData;
	public:
		SkinLoader(const std::shared_ptr<Serialized> &libraryControllers, const int &maxWeights);

		~SkinLoader();

		std::vector<std::string> GetJointOrder() const { return m_jointOrder; }

		std::vector<VertexSkinData *> GetVerticesSkinData() const { return m_verticesSkinData; }

	private:
		void LoadJointsList();

		std::vector<float> LoadWeights();

		std::vector<int> GetEffectiveJointsCounts(std::shared_ptr<Serialized> &weightsDataNode);

		void GetSkinData(std::shared_ptr<Serialized> &weightsDataNode, const std::vector<int> &counts, const std::vector<float> &weights);
	};
}
