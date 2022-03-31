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
	class DeferredLightingPass : public RenderQueuePass
	{
	public: 
		void BindPLShadowCamera( const Camera& cam ) noexcept
		{
			pPLShadowCamera = &cam;
			pPLShadowCBuf->SetCamera( &cam );
		}
		void BindSLShadowCamera( const Camera& cam ) noexcept
		{
			pSLShadowCamera = &cam;
			pSLShadowCBuf->SetCamera( &cam );
		}
		void BindDLShadowCamera( const Camera& cam ) noexcept
		{
			pDLShadowCamera = &cam;
			pDLShadowCBuf->SetCamera( &cam );
		}
		DeferredLightingPass( Graphics& gfx, std::string name )
			:
			RenderQueuePass( std::move( name ) ),
			pPLShadowCBuf{ std::make_shared<Bind::ShadowCameraCBuf>( gfx ) },
			pSLShadowCBuf{ std::make_shared<Bind::ShadowCameraCBuf>( gfx ) }, // probably need to change what slot this is on
			pDLShadowCBuf{ std::make_shared<Bind::ShadowCameraCBuf>( gfx ) }  // this too
		{
			using namespace Bind;
			AddBind( pPLShadowCBuf );
			AddBind( pSLShadowCBuf );
			AddBind( pDLShadowCBuf );			
			RegisterSink( DirectBufferSink<RenderTarget>::Make( "renderTarget", renderTarget ) );
			RegisterSink( DirectBufferSink<ShaderInputDepthStencil>::Make( "depthMap", depthMap ) );
			RegisterSink( DirectBufferSink<DepthStencil>::Make( "depthStencil", depthStencil ) );
			RegisterSink( DirectBufferSink<GBufferRenderTargets>::Make( "gbuffer", gbuffer ) );
			AddBindSink<Bind::Bindable>( "plShadowMap" );
			AddBindSink<Bind::Bindable>( "slShadowMap" );
			AddBindSink<Bind::Bindable>( "dlShadowMap" );			
			AddBind( std::make_shared<Bind::ShadowSampler>( gfx ) );
			AddBind( std::make_shared<Bind::Sampler>( gfx, Bind::Sampler::Type::Anisotropic, false, 2 ) );
			RegisterSource( DirectBufferSource<RenderTarget>::Make( "renderTarget", renderTarget ) );
			RegisterSource( DirectBufferSource<DepthStencil>::Make( "depthStencil", depthStencil ) );

			AddBind( Stencil::Resolve( gfx, Stencil::Mode::Off ) );
		}
		void BindMainCamera( const Camera& cam ) noexcept
		{
			pMainCamera = &cam;
		}
		void Execute( Graphics& gfx ) const noexcept override
		{
			assert( pMainCamera );
			depthStencil->BindAsBuffer( gfx, renderTarget.get() );
			depthMap->Bind( gfx );
			pPLShadowCBuf->Update( gfx );
			pSLShadowCBuf->Update( gfx );
			pDLShadowCBuf->Update( gfx );
			pMainCamera->BindToGraphics( gfx );
			gbuffer->Bind( gfx );
			RenderQueuePass::Execute( gfx );
		}
	private:
		const Camera* pPLShadowCamera = nullptr;
		const Camera* pSLShadowCamera = nullptr;
		const Camera* pDLShadowCamera = nullptr;
		std::shared_ptr<Bind::GBufferRenderTargets> gbuffer;
		std::shared_ptr<Bind::ShadowCameraCBuf> pPLShadowCBuf;
		std::shared_ptr<Bind::ShadowCameraCBuf> pSLShadowCBuf;
		std::shared_ptr<Bind::ShadowCameraCBuf> pDLShadowCBuf;
		std::shared_ptr<Bind::ShaderInputDepthStencil> depthMap;
		const Camera* pMainCamera = nullptr;
	};
}