#pragma once
#include "RenderQueuePass.h"
#include "../Job.h"
#include <vector>
#include "../../../Bindable/Bindables/PixelShader.h"
#include "../../../Bindable/Bindables/VertexShader.h"
#include "../../../Bindable/Bindables/Stencil.h"
#include "../../../Bindable/Bindables/Rasterizer.h"
#include "../Source.h"
#include "../../../Bindable/Bindables/RenderTarget.h"
#include "../../../Bindable/Bindables/Blender.h"

class Graphics;

namespace Rgph
{
	class BlurOutlineDrawingPass : public RenderQueuePass
	{
	public:
		BlurOutlineDrawingPass( Graphics& gfx, std::string name, unsigned int fullWidth, unsigned int fullHeight )
			:
			RenderQueuePass( std::move( name ) )
		{
			using namespace Bind;
			renderTarget = std::make_unique<ShaderInputRenderTarget>( gfx, fullWidth / 2, fullHeight / 2, 0 );
			AddBind( VertexShader::Resolve( gfx, "./Shaders/Bin/Solid_VS.cso" ) );
			AddBind( PixelShader::Resolve( gfx, "./Shaders/Bin/Solid_PS.cso" ) );
			AddBind( Stencil::Resolve( gfx, Stencil::Mode::Mask ) );
			AddBind( Blender::Resolve( gfx, false ) );
			RegisterSource( DirectBindableSource<Bind::RenderTarget>::Make( "scratchOut", renderTarget ) );
		}
		void Execute( Graphics& gfx ) const noexcept override
		{
			renderTarget->Clear( gfx );
			RenderQueuePass::Execute( gfx );
		}
	};
}