#pragma once
#include "RenderQueuePass.h"
#include "../Job.h"
#include <vector>
#include "../Sink.h"
#include "../Source.h"
#include "../../../Bindable/Bindables/Stencil.h"
#include "../../../Camera/Camera.h"
#include "../../../Bindable/Bindables/DepthStencil.h"
#include "../../../Bindable/Bindables/ShadowCameraCBuf.h"
#include "../../../Bindable/Bindables/ShadowSampler.h"
#include "../../../Bindable/Bindables/Sampler.h"

class Graphics;

namespace Rgph
{
	class GBufferWritePass : public RenderQueuePass
	{
	public:
		GBufferWritePass( Graphics& gfx, std::string name )
			:
			RenderQueuePass( std::move( name ) ),
			pShadowCBuf{ std::make_shared<Bind::ShadowCameraCBuf>( gfx ) }

		{
			using namespace Bind;
			AddBind( pShadowCBuf );
			RegisterSink( DirectBufferSink<RenderTarget>::Make( "renderTarget", renderTarget ) );
			RegisterSink( DirectBufferSink<DepthStencil>::Make( "depthStencil", depthStencil ) );
			AddBindSink<Bind::Bindable>( "shadowMap" );
			AddBind( std::make_shared<Bind::ShadowSampler>( gfx ) );
			AddBind( std::make_shared<Bind::Sampler>( gfx, Bind::Sampler::Type::Anisotropic, false, 2 ) );

			//RegisterSource( DirectBufferSource<RenderTarget>::Make( "renderTarget", renderTarget ) );
			//RegisterSource( DirectBufferSource<DepthStencil>::Make( "depthStencil", depthStencil ) );
			// Need to make gbuffer sources
			RegisterSource( DirectBufferSource<RenderTarget>::Make( "colorMap", colorMap ) );
			RegisterSource( DirectBufferSource<RenderTarget>::Make( "normalMap", normalMap ) );
			RegisterSource( DirectBufferSource<RenderTarget>::Make( "specularMap", specularMap ) );
			RegisterSource( DirectBufferSource<DepthStencil>::Make( "depthStencil", depthStencil ) );
			//

			AddBind( Stencil::Resolve( gfx, Stencil::Mode::Off ) );
		}
		void BindMainCamera( const Camera& cam ) noexcept
		{
			pMainCamera = &cam;
		}
		void BindShadowCamera( const Camera& cam ) noexcept
		{
			pShadowCBuf->SetCamera( &cam );
		}
		void Execute( Graphics& gfx ) const noexcept override
		{
			assert( pMainCamera );
			pShadowCBuf->Update( gfx );
			pMainCamera->BindToGraphics( gfx );
			RenderQueuePass::Execute( gfx );
		}
	private:
		std::shared_ptr<Bind::ShadowCameraCBuf> pShadowCBuf;
		std::shared_ptr<Bind::RenderTarget> colorMap;
		std::shared_ptr<Bind::RenderTarget> normalMap;
		std::shared_ptr<Bind::RenderTarget> specularMap;
		const Camera* pMainCamera = nullptr;
	};
}