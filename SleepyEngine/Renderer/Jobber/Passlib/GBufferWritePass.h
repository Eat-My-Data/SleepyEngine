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
			RenderQueuePass( std::move( name ) )
		{
			using namespace Bind;

			// sampler is needed to access resorces
			AddBind( std::make_shared<Bind::Sampler>( gfx, Bind::Sampler::Type::Anisotropic, false, 2 ) );

			// gbuffer resources
			gbuffer = std::make_shared<Bind::GBufferRenderTargets>( gfx, 1280, 720, 6);
			AddBind( gbuffer );
			RegisterSource( DirectBufferSource<GBufferRenderTargets>::Make( "gbuffer", gbuffer ) );

			depthMap = std::make_shared<ShaderInputDepthStencil>( gfx, 9, DepthStencil::Usage::ShadowDepth );
			RegisterSource( DirectBufferSource<ShaderInputDepthStencil>::Make( "depthMap", depthMap ) );
			SetDepthBuffer( depthMap );

			// turn off stenciling
			//AddBind( Stencil::Resolve( gfx, Stencil::Mode::Off ));
		}
		void BindMainCamera( const Camera& cam ) noexcept
		{
			pMainCamera = &cam;
		}
		void Execute( Graphics& gfx ) const noexcept override
		{
			assert( pMainCamera );
			depthMap->Clear( gfx );
			gbuffer->Clear( gfx );
			pMainCamera->BindToGraphics( gfx );
			RenderQueuePass::Execute( gfx );
		}
	private:
		void SetDepthBuffer( std::shared_ptr<Bind::DepthStencil> ds ) const
		{
			const_cast<GBufferWritePass*>( this )->depthStencil = std::move( ds );
		}
		std::shared_ptr<Bind::GBufferRenderTargets> gbuffer;
		std::shared_ptr<Bind::ShaderInputDepthStencil> depthMap;
		const Camera* pMainCamera = nullptr;
	};
}