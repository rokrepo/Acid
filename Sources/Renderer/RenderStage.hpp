#pragma once

#include <vulkan/vulkan.h>
#include "Renderpass/Renderpass.hpp"
#include "Swapchain/DepthStencil.hpp"
#include "Swapchain/Framebuffers.hpp"
#include "Swapchain/Swapchain.hpp"

namespace acid
{
	class ACID_EXPORT RenderStage
	{
	private:
		uint32_t m_stageIndex;
		RenderpassCreate m_renderpassCreate;

		std::shared_ptr<DepthStencil> m_depthStencil;
		std::shared_ptr<Renderpass> m_renderpass;
		std::shared_ptr<Framebuffers> m_framebuffers;

		std::vector<VkClearValue> m_clearValues;
		std::vector<uint32_t> m_subpassAttachmentCount;
		bool m_hasDepth;
		bool m_hasSwapchain;

		bool m_fitDisplaySize;

		uint32_t m_lastWidth;
		uint32_t m_lastHeight;
	public:
		RenderStage(const uint32_t &stageIndex, const RenderpassCreate &renderpassCreate);

		~RenderStage();

		void Rebuild(const Swapchain &swapchain);

		uint32_t SubpassCount() const { return static_cast<uint32_t>(m_renderpassCreate.GetSubpasses().size()); };

		uint32_t GetWidth() const;

		uint32_t GetHeight() const;

		bool IsOutOfDate(const VkExtent2D &extent2D);

		RenderpassCreate &GetRenderpassCreate() { return m_renderpassCreate; }

		std::shared_ptr<DepthStencil> GetDepthStencil() const { return m_depthStencil; };

		std::shared_ptr<Renderpass> GetRenderpass() const { return m_renderpass; };

		std::shared_ptr<Framebuffers> GetFramebuffers() const { return m_framebuffers; };

		VkFramebuffer GetActiveFramebuffer(const uint32_t &activeSwapchainImage) const;

		std::vector<VkClearValue> GetClearValues() const { return m_clearValues; }

		uint32_t GetAttachmentCount(const uint32_t &subpass) const { return m_subpassAttachmentCount[subpass]; }

		bool HasDepth() const { return m_hasDepth; }

		bool HasSwapchain() const { return m_hasSwapchain; }

		bool FitDisplaySize() const { return m_fitDisplaySize; }
	};
}
