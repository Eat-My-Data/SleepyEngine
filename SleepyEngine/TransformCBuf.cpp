#include "TransformCbuf.h"

namespace Bind
{
	TransformCbuf::TransformCbuf( GraphicsDeviceInterface& gfx, const Drawable& parent, UINT slot )
		:
		parent( parent )
	{
		if ( !pVcbuf )
		{
			pVcbuf = std::make_unique<VertexConstantBuffer<Transforms>>( gfx, slot );
		}
	}

	void TransformCbuf::Bind( GraphicsDeviceInterface& gfx ) noexcept
	{
		UpdateBindImpl( gfx, GetTransforms( gfx ) );
	}

	void TransformCbuf::UpdateBindImpl( GraphicsDeviceInterface& gfx, const Transforms& tf ) noexcept
	{
		pVcbuf->Update( gfx, tf );
		pVcbuf->Bind( gfx );
	}

	TransformCbuf::Transforms TransformCbuf::GetTransforms( GraphicsDeviceInterface& gfx ) noexcept
	{
		const auto modelView = parent.GetTransformXM() * gfx.GetViewMatrix();

		return {
			DirectX::XMMatrixTranspose( parent.GetTransformXM() ),
			DirectX::XMMatrixTranspose( modelView ),
			DirectX::XMMatrixTranspose(
				modelView *
				gfx.GetProjMatrix()
			)
		};
	}

	std::unique_ptr<VertexConstantBuffer<TransformCbuf::Transforms>> TransformCbuf::pVcbuf;
}