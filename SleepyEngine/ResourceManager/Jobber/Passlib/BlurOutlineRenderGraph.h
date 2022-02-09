#pragma once
#include "../RenderGraph.h"
#include <memory>
#include "../../../Bindable/Bindables/ConstantBuffersEx.h"

class GraphicsDeviceInterface;
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
		BlurOutlineRenderGraph( GraphicsDeviceInterface& gfx );
	private:
		// private functions
		void SetKernelGauss( int radius, float sigma ) noexcept;
		// private data
		static constexpr int maxRadius = 7;
		static constexpr int radius = 4;
		static constexpr float sigma = 2.0f;
		std::shared_ptr<Bind::CachingPixelConstantBufferEx> blurKernel;
		std::shared_ptr<Bind::CachingPixelConstantBufferEx> blurDirection;
	};
}