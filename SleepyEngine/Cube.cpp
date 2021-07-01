#include "Cube.h"
#include "BindableCommon.h"
#include "ConstantBuffers.h"
#include <sstream>
#include <memory>

Cube::Cube( GraphicsDeviceInterface& gdi, Data data, f32 size )
    :
    m_Data( data )
{
	using namespace Bind;
	namespace dx = DirectX;

    using Type = Dvtx::VertexLayout::ElementType;
    IndexedTriangleList model =  MakeIndependent( std::move( Dvtx::VertexLayout{}
        .Append( Type::Position3D )
        .Append( Type::Normal )
    ) );
    model.Transform( dx::XMMatrixScaling( size, size, size ) );
    model.SetNormalsIndependentFlat();
    const auto geometryTag = "$cube." + std::to_string( size );
    AddBind( VertexBuffer::Resolve( gdi, geometryTag, model.m_VBVertices ) );
    AddBind( IndexBuffer::Resolve( gdi, geometryTag, model.m_vecOfIndices ) );

    auto pvs = VertexShader::Resolve( gdi, "VertexShader.cso" );
    auto pvsbc = pvs->GetBytecode();
    AddBind( std::move( pvs ) );

    AddBind( PixelShader::Resolve( gdi, "PixelShader.cso" ) );

    AddBind( InputLayout::Resolve( gdi, model.m_VBVertices.GetLayout(), pvsbc ) );
    
    AddBind( Topology::Resolve( gdi, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );

    AddBind( PixelConstantBuffer<PSMaterialConstant>::Resolve( gdi, pmc, 1u ) );

    AddBind( std::make_shared<Bind::TransformCbuf>( gdi, *this ) );

    AddBind( Rasterizer::Resolve( gdi, true ) );
}

Cube::Cube( const Cube& cube )
{
    m_Data = cube.m_Data;
}

Cube& Cube::operator=( const Cube& cube )
{
    this->m_Data = cube.m_Data;
    return *this;
}

void Cube::SetPos( DirectX::XMFLOAT3 pos ) noexcept
{
	m_Data.pos.x += pos.x;
    m_Data.pos.y += pos.y;
    m_Data.pos.z += pos.z;
}

void Cube::SetRotation( f32 roll, f32 pitch, f32 yaw ) noexcept
{
	m_Data.roll += roll;
	m_Data.pitch += pitch;
	m_Data.yaw += yaw;
}

DirectX::XMMATRIX Cube::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw( m_Data.roll, m_Data.pitch, m_Data.yaw ) *
		DirectX::XMMatrixTranslation( m_Data.pos.x, m_Data.pos.y, m_Data.pos.z );
}

const Cube::Data Cube::GetData() noexcept
{
    return m_Data;
}

IndexedTriangleList Cube::MakeIndependent( Dvtx::VertexLayout layout )
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
