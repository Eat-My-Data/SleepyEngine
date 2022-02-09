#include "HorizontalBlurPass.h"
#include "../../../Bindable/Bindables/PixelShader.h"
#include "../../../Bindable/Bindables/RenderTarget.h"
#include "../Sink.h"
#include "../Source.h"
#include "../../../Bindable/Bindables/Blender.h"
#include "../../../Bindable/Bindables/ConstantBuffersEx.h"
#include "../../../Bindable/Bindables/Sampler.h"

using namespace Bind;

namespace Rgph
{
	HorizontalBlurPass::HorizontalBlurPass( std::string name, GraphicsDeviceInterface& gfx, unsigned int fullWidth, unsigned int fullHeight )
		:
		FullscreenPass( std::move( name ), gfx )
	{
		AddBind( PixelShader::Resolve( gfx, "./Shaders/Bin/BlurOutline_PS.cso" ) );
		AddBind( Blender::Resolve( gfx, false ) );

		AddBindSink<Bind::RenderTarget>( "scratchIn" );
		AddBindSink<Bind::CachingPixelConstantBufferEx>( "kernel" );
		RegisterSink( DirectBindableSink<CachingPixelConstantBufferEx>::Make( "direction", direction ) );

		// the renderTarget is internally sourced and then exported as a Bindable
		renderTarget = std::make_shared<ShaderInputRenderTarget>( gfx, fullWidth / 2, fullHeight / 2, 0u );
		RegisterSource( DirectBindableSource<RenderTarget>::Make( "scratchOut", renderTarget ) );
	}

	// this override is necessary because we cannot (yet) link input bindables directly into
	// the container of bindables (mainly because vector growth buggers references)
	void HorizontalBlurPass::Execute( GraphicsDeviceInterface& gfx ) const noexcept
	{
		auto buf = direction->GetBuffer();
		buf["isHorizontal"] = true;
		direction->SetBuffer( buf );

		direction->Bind( gfx );
		FullscreenPass::Execute( gfx );
	}
}