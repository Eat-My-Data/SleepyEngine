#include "ShadowCameraCBuf.h"
#include "../../Camera/Camera.h"

namespace dx = DirectX;

namespace Bind
{
	ShadowCameraCBuf::ShadowCameraCBuf( Graphics& gfx, UINT slot )
		:
		pVcbuf{ std::make_unique<VertexConstantBuffer<Transform>>( gfx,slot ) }
	{}
	void ShadowCameraCBuf::Bind( Graphics& gfx ) noxnd
	{
		pVcbuf->Bind( gfx );
	}
	void ShadowCameraCBuf::Update( Graphics& gfx )
	{
		const auto pos = pCamera->GetPos();
		const Transform t{
			dx::XMMatrixTranspose(
				dx::XMMatrixTranslation( -pos.x,-pos.y,-pos.z )
			)
		};
		pVcbuf->Update( gfx, t );
	}
	void ShadowCameraCBuf::SetCamera( const Camera* p ) noexcept
	{
		pCamera = p;
	}


	ShadowCameraCBufEX::ShadowCameraCBufEX( Graphics& gfx, UINT slot )
		:
		pVcbuf{ std::make_unique<VertexConstantBuffer<Transform>>( gfx,slot ) }
	{}
	void ShadowCameraCBufEX::Bind( Graphics& gfx ) noxnd
	{
		pVcbuf->Bind( gfx );
	}
	void ShadowCameraCBufEX::Update( Graphics& gfx )
	{
		const auto pos = pCamera->GetPos();
		const Transform t{
			pCamera->GetMatrix() * pCamera->GetProjection()
		};
		pVcbuf->Update( gfx, t );
	}
	void ShadowCameraCBufEX::SetCamera( const Camera* p ) noexcept
	{
		pCamera = p;
	}
}