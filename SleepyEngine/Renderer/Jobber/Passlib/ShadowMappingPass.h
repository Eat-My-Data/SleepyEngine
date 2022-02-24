#pragma once
#include "RenderQueuePass.h"
#include "../Job.h"
#include <vector>
#include "../Sink.h"
#include "../Source.h"
#include "../../../Bindable/Bindables/Stencil.h"
#include "../../../Bindable/Bindables/PixelShader.h"
#include "../../../Bindable/Bindables/VertexShader.h"
#include "../../../Bindable/Bindables/Rasterizer.h"
#include "../../../Bindable/Bindables/RenderTarget.h"
#include "../../../Bindable/Bindables/Blender.h"
#include "../../../Bindable/Bindables/NullPixelShader.h"

class Graphics;

namespace Rgph
{
	class ShadowMappingPass : public RenderQueuePass
	{
	public:
		void BindShadowCamera( const Camera& cam ) noexcept
		{
			pShadowCamera = &cam;
		}
		ShadowMappingPass( Graphics& gfx, std::string name )
			:
			RenderQueuePass( std::move( name ) )
		{
			using namespace Bind;
			depthStencil = std::make_unique<ShaderInputDepthStencil>( gfx, 3, DepthStencil::Usage::ShadowDepth );
			AddBind( VertexShader::Resolve( gfx, "./Shaders/Bin/Solid_VS.cso" ) );
			AddBind( NullPixelShader::Resolve( gfx ) );
			AddBind( Stencil::Resolve( gfx, Stencil::Mode::Off ) );
			AddBind( Blender::Resolve( gfx, false ) );
			RegisterSource( DirectBindableSource<Bind::DepthStencil>::Make( "map", depthStencil ) );
		}
		void Execute( Graphics& gfx ) const noexcept override
		{
			depthStencil->Clear( gfx );
			pShadowCamera->BindToGraphics( gfx );
			RenderQueuePass::Execute( gfx );
		}
		void DumpShadowMap( Graphics& gfx, const std::string& path ) const
		{
			depthStencil->ToSurface( gfx ).Save( path );
		}
	private:
		const Camera* pShadowCamera = nullptr;
	};
}