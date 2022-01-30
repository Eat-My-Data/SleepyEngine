#pragma once
#include "RenderQueuePass.h"
#include "../Job.h"
#include <vector>
#include "../../../Bindable/Bindables/NullPixelShader.h"
#include "../../../Bindable/Bindables/VertexShader.h"
#include "../../../Bindable/Bindables/Stencil.h"
#include "../../../Bindable/Bindables/Rasterizer.h"

class GraphicsDeviceInterface;

class OutlineMaskGenerationPass : public RenderQueuePass
{
public:
	OutlineMaskGenerationPass( GraphicsDeviceInterface& gfx, std::string name )
		:
		RenderQueuePass( std::move( name ) )
	{
		using namespace Bind;
		RegisterInput( BufferInput<Bind::DepthStencil>::Make( "depthStencil", depthStencil ) );
		RegisterOutput( BufferOutput<Bind::DepthStencil>::Make( "depthStencil", depthStencil ) );
		AddBind( VertexShader::Resolve( gfx, "./Shaders/Bin/Solid_VS.cso" ) );
		AddBind( NullPixelShader::Resolve( gfx ) );
		AddBind( Stencil::Resolve( gfx, Stencil::Mode::Write ) );
		AddBind( Rasterizer::Resolve( gfx, false ) );
	}
};