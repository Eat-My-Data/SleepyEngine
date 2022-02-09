#include "HorizontalBlurPass.h"
#include "../../../Bindable/Bindables/PixelShader.h"
#include "../../../Bindable/Bindables/RenderTarget.h"
#include "../Sink.h"
#include "../Source.h"
#include "../../../Bindable/Bindables/Blender.h"

namespace Rgph
{
	HorizontalBlurPass::HorizontalBlurPass( std::string name, GraphicsDeviceInterface& gfx, unsigned int fullWidth, unsigned int fullHeight )
		:
		FullscreenPass( std::move( name ), gfx )
	{
		AddBind( Bind::PixelShader::Resolve( gfx, "./Shaders/Bin/BlurOutline_PS.cso" ) );
		AddBind( Bind::Blender::Resolve( gfx, false ) );

		RegisterSink( DirectBindableSink<Bind::Bindable>::Make( "control", control ) );
		RegisterSink( DirectBindableSink<Bind::CachingPixelConstantBufferEx>::Make( "direction", direction ) );
		RegisterSink( DirectBindableSink<Bind::Bindable>::Make( "scratchIn", blurScratchIn ) );

		// the renderTarget is internally sourced and then exported as a Bindable
		renderTarget = std::make_shared<Bind::ShaderInputRenderTarget>( gfx, fullWidth / 2, fullHeight / 2, 0u );
		RegisterSource( DirectBindableSource<Bind::RenderTarget>::Make( "scratchOut", renderTarget ) );
	}

	// this override is necessary because we cannot (yet) link input bindables directly into
	// the container of bindables (mainly because vector growth buggers references)
	void HorizontalBlurPass::Execute( GraphicsDeviceInterface& gfx ) const noexcept
	{
		auto buf = direction->GetBuffer();
		buf["isHorizontal"] = true;
		direction->SetBuffer( buf );

		blurScratchIn->Bind( gfx );
		control->Bind( gfx );
		direction->Bind( gfx );
		FullscreenPass::Execute( gfx );
	}
}