#pragma once
#include "RenderGraph.h"
#include <memory>
#include "../../Bindable/Bindables/ConstantBuffersEx.h"

class Graphics;

namespace Bind
{
	class Bindable;
	class RenderTarget;
}

namespace Rgph
{
	class BlurOutlineRenderGraph : public RenderGraph
	{
	public:
		BlurOutlineRenderGraph( Graphics& gfx );
		void RenderWidgets( Graphics& gfx );
	private:
		// private functions
		void SetKernelGauss( int radius, float sigma ) noexcept;
		void SetKernelBox( int radius ) noexcept;
		// private data
		enum class KernelType
		{
			Gauss,
			Box,
		} kernelType = KernelType::Gauss;
		static constexpr int maxRadius = 7;
		int radius = 4;
		float sigma = 2.0f;
		std::shared_ptr<Bind::CachingPixelConstantBufferEx> blurKernel;
		std::shared_ptr<Bind::CachingPixelConstantBufferEx> blurDirection;
	};
}