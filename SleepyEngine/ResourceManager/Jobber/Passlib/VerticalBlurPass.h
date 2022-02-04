#pragma once
#include "FullscreenPass.h"
#include "../../../Bindable/Bindables/ConstantBuffersEx.h"

class GraphicsDeviceInterface;

namespace Bind
{
	class PixelShader;
	class RenderTarget;
}

namespace Rgph
{
	class VerticalBlurPass : public FullscreenPass
	{
	public:
		VerticalBlurPass( std::string name, GraphicsDeviceInterface& gfx );
		void Execute( GraphicsDeviceInterface& gfx ) const noexcept override;
	private:
		std::shared_ptr<Bind::Bindable> blurScratchIn;
		std::shared_ptr<Bind::Bindable> control;
		std::shared_ptr<Bind::CachingPixelConstantBufferEx> direction;
	};
}