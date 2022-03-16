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
	class LambertianPass : public RenderQueuePass
	{
	public: 
		LambertianPass( Graphics& gfx, std::string name )
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
			RegisterSink( DirectBufferSink<DepthStencil>::Make( "depthStencil", depthStencil ) );
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
		void BindPLShadowCamera( const Camera& cam ) noexcept
		{
			pPLShadowCBuf->SetCamera( &cam );
		}
		void BindSLShadowCamera( const Camera& cam ) noexcept
		{
			pSLShadowCBuf->SetCamera( &cam );
		}
		void BindDLShadowCamera( const Camera& cam ) noexcept
		{
			pDLShadowCBuf->SetCamera( &cam );
		}
		void Execute( Graphics& gfx ) const noexcept override
		{
			assert( pMainCamera );
			pPLShadowCBuf->Update( gfx );
			pSLShadowCBuf->Update( gfx );
			pDLShadowCBuf->Update( gfx );
			pMainCamera->BindToGraphics( gfx );
			RenderQueuePass::Execute( gfx );
		}
	private:
		std::shared_ptr<Bind::ShadowCameraCBuf> pPLShadowCBuf;
		std::shared_ptr<Bind::ShadowCameraCBuf> pSLShadowCBuf;
		std::shared_ptr<Bind::ShadowCameraCBuf> pDLShadowCBuf;
		const Camera* pMainCamera = nullptr;
	};
}