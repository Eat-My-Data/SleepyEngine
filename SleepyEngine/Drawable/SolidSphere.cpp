#include "SolidSphere.h"
#include "../Bindable/BindableCommon.h"
#include "../ResourceManager/Vertex.h"
#include "../ResourceManager/Geometry/Sphere.h"


SolidSphere::SolidSphere( GraphicsDeviceInterface& gdi, float radius )
{
	/*using namespace Bind;
	namespace dx = DirectX;

	auto model = Sphere::Make();

	model.Transform( dx::XMMatrixScaling( radius, radius, radius ) );
	AddBind( std::make_shared<VertexBuffer>( gdi, model.m_VBVertices ) );
	AddBind( std::make_shared<IndexBuffer>( gdi, model.m_vecOfIndices ) );

	auto pvs = std::make_shared<VertexShader>( gdi, "SolidVS.cso" );
	auto pvsbc = pvs->GetBytecode();
	AddBind( std::move( pvs ) );

	AddBind( std::make_shared<PixelShader>( gdi, L"SolidPS.cso" ) );

	struct PSColorConstant
	{
		dx::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
		float padding;
	} colorConst;
	AddBind( std::make_shared<PixelConstantBuffer<PSColorConstant>>( gdi, colorConst ) );

	AddBind( std::make_shared<InputLayout>( gdi, model.m_VBVertices.GetLayout().GetD3DLayout(), pvsbc ) );

	AddBind( std::make_shared<Topology>( gdi, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );

	AddBind( std::make_shared<TransformCbuf>( gdi, *this ) );*/
}

void SolidSphere::SetPos( DirectX::XMFLOAT3 pos ) noexcept
{
	this->pos = pos;
}

DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation( pos.x, pos.y, pos.z );
}