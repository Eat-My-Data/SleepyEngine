#include "TransformCbuf.h"

namespace Bind
{
	TransformCbuf::TransformCbuf( GraphicsDeviceInterface& gdi, const Drawable& parent, UINT slot )
		:
		m_DrawableParent( parent )
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

	void TransformCbuf::UpdateBindImpl( GraphicsDeviceInterface& gdi, const Transforms& tf ) noexcept
	{
		m_pVcbuf->Update( gdi, tf );
		m_pVcbuf->Bind( gdi );
	}

	TransformCbuf::Transforms TransformCbuf::GetTransforms( GraphicsDeviceInterface& gdi ) noexcept
	{
		const auto modelView = m_DrawableParent.GetTransformXM() * gdi.GetViewMatrix();

		return {
			DirectX::XMMatrixTranspose( m_DrawableParent.GetTransformXM() ),
			DirectX::XMMatrixTranspose( modelView ),
			DirectX::XMMatrixTranspose(
				modelView *
				gdi.GetProjMatrix()
			)
		};
	}

	std::unique_ptr<VertexConstantBuffer<TransformCbuf::Transforms>> TransformCbuf::m_pVcbuf;
}