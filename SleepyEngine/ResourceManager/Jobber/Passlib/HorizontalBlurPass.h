#pragma once
#include "FullscreenPass.h"
#include "../../../Bindable/Bindables/ConstantBuffersEx.h"

class GraphicsDeviceInterface;

namespace Bind
{
	class PixelShader;
	class RenderTarget;
}

class HorizontalBlurPass : public FullscreenPass
{
public:
	HorizontalBlurPass( std::string name, GraphicsDeviceInterface& gfx, unsigned int fullWidth, unsigned int fullHeight );
	void Execute( GraphicsDeviceInterface& gfx ) const noexcept override;
private:
	std::shared_ptr<Bind::Bindable> blurScratchIn;
	std::shared_ptr<Bind::Bindable> control;
	std::shared_ptr<Bind::CachingPixelConstantBufferEx> direction;
};