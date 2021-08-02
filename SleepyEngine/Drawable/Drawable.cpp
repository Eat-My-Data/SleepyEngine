#include "Drawable.h"
#include "../Bindable/Bindables/IndexBuffer.h"
#include <cassert>

using namespace Bind;

void Drawable::Draw( GraphicsDeviceInterface& gdi ) const noexcept
{
	// bindables
	for ( auto& b : binds )
	{
		b->Bind( gdi );
	}

	// draw
	gdi.DrawIndexed( pIndexBuffer->GetCount() );
}

void Drawable::DrawDepth( GraphicsDeviceInterface& gdi ) const noexcept
{
	// bindables
	for ( auto& b : binds )
	{
		b->Bind( gdi );
	}

	gdi.GetContext()->PSSetShader( nullptr, nullptr, 0u );

	// draw
	gdi.DrawIndexed( pIndexBuffer->GetCount() );
}

void Drawable::AddBind( std::shared_ptr<Bind::Bindable> bind ) noexcept
{
	if ( typeid( *bind ) == typeid( IndexBuffer ) )
	{
		assert( "Binding multiple index buffers not allowed" && pIndexBuffer == nullptr );
		pIndexBuffer = &static_cast<IndexBuffer&>( *bind );
	}
	binds.push_back( std::move( bind ) );
}
