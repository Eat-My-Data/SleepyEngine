#pragma once
#include "FullscreenPass.h"
#include "../../../Bindable/Bindables/ConstantBuffersEx.h"

using namespace Bind;

class Graphics;

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
		VerticalBlurPass( std::string name, Graphics& gfx );
		void Execute( Graphics& gfx ) const noexcept override;
	private:
		std::shared_ptr<Bind::CachingPixelConstantBufferEx> direction;
	};
}