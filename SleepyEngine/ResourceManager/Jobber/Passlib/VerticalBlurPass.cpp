#include "VerticalBlurPass.h"
#include "../Source.h"
#include "../Sink.h"
#include "../../../Bindable/Bindables/PixelShader.h"
#include "../../../Bindable/Bindables/Blender.h"
#include "../../../Bindable/Bindables/Stencil.h"

namespace Rgph
{
	VerticalBlurPass::VerticalBlurPass( std::string name, GraphicsDeviceInterface& gfx )
		:
		FullscreenPass( std::move( name ), gfx )
	{
		using namespace Bind;
		AddBind( PixelShader::Resolve( gfx, "./Shaders/Bin/BlurOutline_PS.cso" ) );
		AddBind( Blender::Resolve( gfx, true ) );
		AddBind( Stencil::Resolve( gfx, Stencil::Mode::Mask ) );

		RegisterSink( DirectBindableSink<Bind::Bindable>::Make( "scratchIn", blurScratchIn ) );
		RegisterSink( DirectBindableSink<Bind::Bindable>::Make( "control", control ) );
		RegisterSink( DirectBindableSink<Bind::CachingPixelConstantBufferEx>::Make( "direction", direction ) );
		RegisterSink( DirectBufferSink<Bind::RenderTarget>::Make( "renderTarget", renderTarget ) );
		RegisterSink( DirectBufferSink<Bind::DepthStencil>::Make( "depthStencil", depthStencil ) );

		RegisterSource( DirectBufferSource<Bind::RenderTarget>::Make( "renderTarget", renderTarget ) );
		RegisterSource( DirectBufferSource<Bind::DepthStencil>::Make( "depthStencil", depthStencil ) );
	}

	// see the note on HorizontalBlurPass::Execute
	void VerticalBlurPass::Execute( GraphicsDeviceInterface& gfx ) const noexcept
	{
		auto buf = direction->GetBuffer();
		buf["isHorizontal"] = false;
		direction->SetBuffer( buf );

		control->Bind( gfx );
		direction->Bind( gfx );
		blurScratchIn->Bind( gfx );
		FullscreenPass::Execute( gfx );
	}
}