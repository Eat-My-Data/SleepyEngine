#include "TransformCbuf.h"

namespace Bind
{
	TransformCbuf::TransformCbuf( GraphicsDeviceInterface& gdi, UINT slot )
	{
		if ( !m_pVcbuf )
		{
			m_pVcbuf = std::make_unique<VertexConstantBuffer<Transforms>>( gdi, slot );
		}
	}

	void TransformCbuf::Bind( GraphicsDeviceInterface& gdi ) noexcept
	{
		UpdateBindImpl( gdi, GetTransforms( gdi ) );
	}

	void TransformCbuf::InitializeParentReference( const Drawable& parent ) noexcept
	{
		m_DrawableParent = &parent;
	}

	std::unique_ptr<CloningBindable> TransformCbuf::Clone() const noexcept
	{
		return std::make_unique<TransformCbuf>( *this );
	}

	void TransformCbuf::UpdateBindImpl( GraphicsDeviceInterface& gdi, const Transforms& tf ) noexcept
	{
		assert( m_DrawableParent != nullptr );
		m_pVcbuf->Update( gdi, tf );
		m_pVcbuf->Bind( gdi );
	}

	TransformCbuf::Transforms TransformCbuf::GetTransforms( GraphicsDeviceInterface& gdi ) noexcept
	{
		assert( m_DrawableParent != nullptr );

		const auto modelView = m_DrawableParent->GetTransformXM() * gdi.GetViewMatrix();

		return {
			DirectX::XMMatrixTranspose( m_DrawableParent->GetTransformXM() ),
			DirectX::XMMatrixTranspose( modelView ),
			DirectX::XMMatrixTranspose(
				modelView *
				gdi.GetProjMatrix()
			)
		};
	}

	std::unique_ptr<VertexConstantBuffer<TransformCbuf::Transforms>> TransformCbuf::m_pVcbuf;
}