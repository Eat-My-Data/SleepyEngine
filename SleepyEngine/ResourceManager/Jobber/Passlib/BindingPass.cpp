#include "BindingPass.h"
#include "../../../Bindable/Bindable.h"
#include "../../../Bindable/Bindables/RenderTarget.h"
#include "../../../Bindable/Bindables/DepthStencil.h"
#include "../RenderGraphCompileException.h"

BindingPass::BindingPass( std::string name,std::vector<std::shared_ptr<Bind::Bindable>> binds )
	:
	Pass( std::move( name ) ),
	binds( std::move( binds ) )
{}

void BindingPass::AddBind( std::shared_ptr<Bind::Bindable> bind ) noexcept
{
	binds.push_back( std::move( bind ) );
}

void BindingPass::BindAll( GraphicsDeviceInterface& gfx ) const noexcept
{
	for( auto& bind : binds )
	{
		bind->Bind( gfx );
	}
	BindBufferResources( gfx );
}

void BindingPass::Finalize()
{
	Pass::Finalize();
	if ( !renderTarget && !depthStencil )
	{
		throw RGC_EXCEPTION( "BindingPass [" + GetName() + "] needs at least one of a renderTarget or depthStencil" );
	}
}