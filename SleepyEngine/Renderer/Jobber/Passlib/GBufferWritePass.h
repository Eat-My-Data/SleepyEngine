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

			// get resources from clear pass
			RegisterSink( DirectBufferSink<DepthStencil>::Make( "depthStencil", depthStencil ) );

			// sampler is needed to access resorces
			AddBind( std::make_shared<Bind::Sampler>( gfx, Bind::Sampler::Type::Anisotropic, false, 2 ) );

			// gbuffer resources
			gbuffer = std::make_shared<Bind::GBufferRenderTargets>( gfx, 1280, 720, 4);
			RegisterSource( DirectBindableSource<GBufferRenderTargets>::Make( "gbuffer", gbuffer ) );
			RegisterSource( DirectBufferSource<DepthStencil>::Make( "depthStencil", depthStencil ) );

			// turn off stenciling
			AddBind( Stencil::Resolve( gfx, Stencil::Mode::Off ) );
		}
		void BindMainCamera( const Camera& cam ) noexcept
		{
			pMainCamera = &cam;
		}
		void Execute( Graphics& gfx ) const noexcept override
		{
			assert( pMainCamera );
			pMainCamera->BindToGraphics( gfx );
			gbuffer->BindAsBuffer( gfx, nullptr );
			RenderQueuePass::Execute( gfx );
		}
	private:
		std::shared_ptr<Bind::GBufferRenderTargets> gbuffer;
		const Camera* pMainCamera = nullptr;
	};
}