#include "Cube.h"
#include "BindableCommon.h"
#include "IndexedTriangleList.h"
#include "Vertex.h"
#include <sstream>

Cube::Cube( GraphicsDeviceInterface& gdi, f32 size )
{
	using namespace Bind;
	namespace dx = DirectX;

    using Type = Dvtx::VertexLayout::ElementType;
    auto model =  MakeIndependent( std::move( Dvtx::VertexLayout{}
        .Append( Type::Position3D )
        .Append( Type::Normal )
        .Append( Type::Texture2D )
    ) );

    model.Transform( dx::XMMatrixScaling( size, size, size ) );
    model.SetNormalsIndependentFlat();
    const auto geometryTag = "$cube." + std::to_string( size );
    AddBind( VertexBuffer::Resolve( gdi, geometryTag, model.vertices ) );
    AddBind( IndexBuffer::Resolve( gdi, geometryTag, model.indices ) );
    // lookup table for cube face colors
    struct ConstantBuffer2
    {
        struct
        {
            float r;
            float g;
            float b;
            float a;
        } face_colors[6];
    };
    const ConstantBuffer2 cb2 =
    {
        {
            {1.0f,0.0f,1.0f},
            {1.0f,0.0f,0.0f},
            {0.0f,1.0f,0.0f},
            {0.0f,0.0f,1.0f},
            {1.0f,1.0f,0.0f},
            {1.0f,1.0f,1.0f},
        }
    };
    ID3D11Buffer* pConstantBuffer2;
    D3D11_BUFFER_DESC cbd2;
    cbd2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd2.Usage = D3D11_USAGE_DEFAULT;
    cbd2.CPUAccessFlags = 0u;
    cbd2.MiscFlags = 0u;
    cbd2.ByteWidth = sizeof( cb2 );
    cbd2.StructureByteStride = 0u;
    D3D11_SUBRESOURCE_DATA csd2 = {};
    csd2.pSysMem = &cb2;
    gdi.GetDevice()->CreateBuffer( &cbd2, &csd2, &pConstantBuffer2 );

    // bind constant buffer to pixel shader
    gdi.GetContext()->PSSetConstantBuffers( 0u, 1u, &pConstantBuffer2 );




    auto pvs = VertexShader::Resolve( gdi, "VertexShader.cso" );
    auto pvsbc = pvs->GetBytecode();
    AddBind( std::move( pvs ) );
    AddBind( PixelShader::Resolve( gdi, "PixelShader.cso" ) );
    AddBind( InputLayout::Resolve( gdi, model.vertices.GetLayout(), pvsbc ) );

    // bind render target
    gdi.GetContext()->OMSetRenderTargets( 1u, gdi.GetTarget(), nullptr );


    // Set primitive topology to triangle list (groups of 3 vertices)
    gdi.GetContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    // configure viewport
    D3D11_VIEWPORT vp;
    vp.Width = (float)1280;
    vp.Height = (float)720;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    gdi.GetContext()->RSSetViewports( 1u, &vp );
}

void Cube::SetPos( DirectX::XMFLOAT3 pos ) noexcept
{
	this->pos = pos;
}

void Cube::SetRotation( float roll, float pitch, float yaw ) noexcept
{
	this->roll = roll;
	this->pitch = pitch;
	this->yaw = yaw;
}

DirectX::XMMATRIX Cube::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw( roll, pitch, yaw ) *
		DirectX::XMMatrixTranslation( pos.x, pos.y, pos.z );
}

static IndexedTriangleList MakeIndependent( Dvtx::VertexLayout layout )
{
    using namespace Dvtx;
    using Type = Dvtx::VertexLayout::ElementType;

    constexpr float side = 1.0f / 2.0f;

    VertexBuffer vertices( std::move( layout ), 24u );
    vertices[0].Attr<Type::Position3D>() = { -side,-side,-side };// 0 near side
    vertices[1].Attr<Type::Position3D>() = { side,-side,-side };// 1
    vertices[2].Attr<Type::Position3D>() = { -side,side,-side };// 2
    vertices[3].Attr<Type::Position3D>() = { side,side,-side };// 3
    vertices[4].Attr<Type::Position3D>() = { -side,-side,side };// 4 far side
    vertices[5].Attr<Type::Position3D>() = { side,-side,side };// 5
    vertices[6].Attr<Type::Position3D>() = { -side,side,side };// 6
    vertices[7].Attr<Type::Position3D>() = { side,side,side };// 7
    vertices[8].Attr<Type::Position3D>() = { -side,-side,-side };// 8 left side
    vertices[9].Attr<Type::Position3D>() = { -side,side,-side };// 9
    vertices[10].Attr<Type::Position3D>() = { -side,-side,side };// 10
    vertices[11].Attr<Type::Position3D>() = { -side,side,side };// 11
    vertices[12].Attr<Type::Position3D>() = { side,-side,-side };// 12 right side
    vertices[13].Attr<Type::Position3D>() = { side,side,-side };// 13
    vertices[14].Attr<Type::Position3D>() = { side,-side,side };// 14
    vertices[15].Attr<Type::Position3D>() = { side,side,side };// 15
    vertices[16].Attr<Type::Position3D>() = { -side,-side,-side };// 16 bottom side
    vertices[17].Attr<Type::Position3D>() = { side,-side,-side };// 17
    vertices[18].Attr<Type::Position3D>() = { -side,-side,side };// 18
    vertices[19].Attr<Type::Position3D>() = { side,-side,side };// 19
    vertices[20].Attr<Type::Position3D>() = { -side,side,-side };// 20 top side
    vertices[21].Attr<Type::Position3D>() = { side,side,-side };// 21
    vertices[22].Attr<Type::Position3D>() = { -side,side,side };// 22
    vertices[23].Attr<Type::Position3D>() = { side,side,side };// 23

    return{
        std::move( vertices ),{
            0,2, 1,    2,3,1,
            4,5, 7,    4,7,6,
            8,10, 9,  10,11,9,
            12,13,15, 12,15,14,
            16,17,18, 18,17,19,
            20,23,21, 20,22,23
        }
    };
}
