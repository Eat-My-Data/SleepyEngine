#include "VerticalBlurPass.h"
#include "../Source.h"
#include "../Sink.h"
#include "../../../Bindable/Bindables/PixelShader.h"
#include "../../../Bindable/Bindables/Blender.h"
#include "../../../Bindable/Bindables/Stencil.h"
#include "../../../Bindable/Bindables/Sampler.h"

namespace Rgph
{
	VerticalBlurPass::VerticalBlurPass( std::string name, Graphics& gfx )
		:
		FullscreenPass( std::move( name ), gfx )
	{
		using namespace Bind;
		AddBind( PixelShader::Resolve( gfx, "./Shaders/Bin/BlurOutline_PS.cso" ) );
		AddBind( Blender::Resolve( gfx, true ) );
		AddBind( Stencil::Resolve( gfx, Stencil::Mode::Mask ) );
		AddBind( Sampler::Resolve( gfx, Sampler::Type::Bilinear, true ) );

		AddBindSink<RenderTarget>( "scratchIn" );
		AddBindSink<CachingPixelConstantBufferEx>( "kernel" );
		RegisterSink( DirectBindableSink<CachingPixelConstantBufferEx>::Make( "direction", direction ) );
		RegisterSink( DirectBufferSink<RenderTarget>::Make( "renderTarget", renderTarget ) );
		RegisterSink( DirectBufferSink<DepthStencil>::Make( "depthStencil", depthStencil ) );

		RegisterSource( DirectBufferSource<RenderTarget>::Make( "renderTarget", renderTarget ) );
		RegisterSource( DirectBufferSource<DepthStencil>::Make( "depthStencil", depthStencil ) );
	}

	// see the note on HorizontalBlurPass::Execute
	void VerticalBlurPass::Execute( Graphics& gfx ) const noexcept
	{
		auto buf = direction->GetBuffer();
		buf["isHorizontal"] = false;
		direction->SetBuffer( buf );

		direction->Bind( gfx );
		FullscreenPass::Execute( gfx );
	}
}