@@ - 0, 0 + 1, 30 @@
#pragma once
#include "RenderQueuePass.h"
#include "Job.h"
#include <vector>
#include "../../Bindable/Bindables/PixelShader.h"
#include "../../Bindable/Bindables/VertexShader.h"
#include "Stencil.h"
#include "../../Bindable/Bindables/Rasterizer.h"
#include "PassOutput.h"
#include "../../Bindable/Bindables/RenderTarget.h"
#include "../../Bindable/Bindables/Blender.h"

class GraphicsDeviceInterface;

class BlurOutlineDrawingPass : public RenderQueuePass
{
public:
	BlurOutlineDrawingPass( GraphicsDeviceInterface& gfx, std::string name, unsigned int fullWidth, unsigned int fullHeight )
		:
		RenderQueuePass( std::move( name ) )
	{
		using namespace Bind;
		renderTarget = std::make_unique<ShaderInputRenderTarget>( gfx, fullWidth / 2, fullHeight / 2, 0 );
		AddBind( VertexShader::Resolve( gfx, "Solid_VS.cso" ) );
		AddBind( PixelShader::Resolve( gfx, "Solid_PS.cso" ) );
		AddBind( Stencil::Resolve( gfx, Stencil::Mode::Mask ) );
		AddBind( Blender::Resolve( gfx, false ) );
		RegisterOutput( ImmutableOutput<Bind::RenderTarget>::Make( "scratchOut", renderTarget ) );
	}
};