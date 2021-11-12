#include "Cube.h"
#include "../Bindable/BindableCommon.h"
#include "../Bindable/Bindables/ConstantBuffers.h"
#include <sstream>
#include <memory>

Cube::Cube( GraphicsDeviceInterface& gdi, Data data, f32 size )
    :
    m_Data( data )
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Cube::MakeIndependentTextured();
	model.Transform( dx::XMMatrixScaling( size, size, size ) );
	model.SetNormalsIndependentFlat();
	const auto geometryTag = "$cube." + std::to_string( size );
	pVertices = VertexBuffer::Resolve( gdi, geometryTag, model.m_VBVertices );
	pIndices = IndexBuffer::Resolve( gdi, geometryTag, model.m_vecOfIndices );
	pTopology = Topology::Resolve( gdi, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	{
		Technique standard;
		{
			Step only( 0 );

			only.AddBindable( Texture::Resolve( gdi, "Models\\brick_wall\\brick_wall_diffuse.jpg" ) );
			only.AddBindable( Sampler::Resolve( gdi ) );

			auto pvs = VertexShader::Resolve( gdi, "./Shaders/Bin/PhongVS.cso" );
			auto pvsbc = pvs->GetBytecode();
			only.AddBindable( std::move( pvs ) );

			only.AddBindable( PixelShader::Resolve( gdi, "./Shaders/Bin/PhongPS.cso" ) );

			only.AddBindable( PixelConstantBuffer<PSMaterialConstant>::Resolve( gdi, pmc, 1u ) );

			only.AddBindable( InputLayout::Resolve( gdi, model.m_VBVertices.GetLayout(), pvsbc ) );

			only.AddBindable( std::make_shared<TransformCbuf>( gdi ) );

			standard.AddStep( std::move( only ) );
		}
		AddTechnique( std::move( standard ) );
	}

	{
		Technique outline;
		{
			Step mask( 1 );

			auto pvs = VertexShader::Resolve( gdi, "./Shaders/Bin/SolidVS.cso" );
			auto pvsbc = pvs->GetBytecode();
			mask.AddBindable( std::move( pvs ) );

			// TODO: better sub-layout generation tech for future consideration maybe
			mask.AddBindable( InputLayout::Resolve( gdi, model.m_VBVertices.GetLayout(), pvsbc ) );

			mask.AddBindable( std::make_shared<TransformCbuf>( gdi ) );

			// TODO: might need to specify rasterizer when doubled-sided models start being used

			outline.AddStep( std::move( mask ) );
		}
		{
			Step draw( 2 );

			auto pvs = VertexShader::Resolve( gdi, "./Shaders/Bin/SolidVS.cso" );
			auto pvsbc = pvs->GetBytecode();
			draw.AddBindable( std::move( pvs ) );

			// this can be pass-constant
			draw.AddBindable( PixelShader::Resolve( gdi, "./Shaders/Bin/SolidPS.cso" ) );

			// TODO: better sub-layout generation tech for future consideration maybe
			draw.AddBindable( InputLayout::Resolve( gdi, model.m_VBVertices.GetLayout(), pvsbc ) );

			// quick and dirty... nicer solution maybe takes a lamba... we'll see :)
			class TransformCbufScaling : public TransformCbuf
			{
			public:
				using TransformCbuf::TransformCbuf;
				void Bind( GraphicsDeviceInterface& gdi ) noexcept override
				{
					const auto scale = dx::XMMatrixScaling( 1.04f, 1.04f, 1.04f );
					auto xf = GetTransforms( gdi );
					xf.m_ModelViewMatrix = xf.m_ModelViewMatrix * scale;
					xf.m_ModelViewProjMatrix = xf.m_ModelViewProjMatrix * scale;
					UpdateBindImpl( gdi, xf );
				}
			};
			draw.AddBindable( std::make_shared<TransformCbufScaling>( gdi ) );

			// TODO: might need to specify rasterizer when doubled-sided models start being used

			outline.AddStep( std::move( draw ) );
		}
		AddTechnique( std::move( outline ) );
	}
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

    VertexBuffer m_VBVertices( std::move( layout ), 24u );
    m_VBVertices[0].Attr<Type::Position3D>() = { -side,-side,-side };// 0 near side
    m_VBVertices[1].Attr<Type::Position3D>() = { side,-side,-side };// 1
    m_VBVertices[2].Attr<Type::Position3D>() = { -side,side,-side };// 2
    m_VBVertices[3].Attr<Type::Position3D>() = { side,side,-side };// 3
    m_VBVertices[4].Attr<Type::Position3D>() = { -side,-side,side };// 4 far side
    m_VBVertices[5].Attr<Type::Position3D>() = { side,-side,side };// 5
    m_VBVertices[6].Attr<Type::Position3D>() = { -side,side,side };// 6
    m_VBVertices[7].Attr<Type::Position3D>() = { side,side,side };// 7
    m_VBVertices[8].Attr<Type::Position3D>() = { -side,-side,-side };// 8 left side
    m_VBVertices[9].Attr<Type::Position3D>() = { -side,side,-side };// 9
    m_VBVertices[10].Attr<Type::Position3D>() = { -side,-side,side };// 10
    m_VBVertices[11].Attr<Type::Position3D>() = { -side,side,side };// 11
    m_VBVertices[12].Attr<Type::Position3D>() = { side,-side,-side };// 12 right side
    m_VBVertices[13].Attr<Type::Position3D>() = { side,side,-side };// 13
    m_VBVertices[14].Attr<Type::Position3D>() = { side,-side,side };// 14
    m_VBVertices[15].Attr<Type::Position3D>() = { side,side,side };// 15
    m_VBVertices[16].Attr<Type::Position3D>() = { -side,-side,-side };// 16 bottom side
    m_VBVertices[17].Attr<Type::Position3D>() = { side,-side,-side };// 17
    m_VBVertices[18].Attr<Type::Position3D>() = { -side,-side,side };// 18
    m_VBVertices[19].Attr<Type::Position3D>() = { side,-side,side };// 19
    m_VBVertices[20].Attr<Type::Position3D>() = { -side,side,-side };// 20 top side
    m_VBVertices[21].Attr<Type::Position3D>() = { side,side,-side };// 21
    m_VBVertices[22].Attr<Type::Position3D>() = { -side,side,side };// 22
    m_VBVertices[23].Attr<Type::Position3D>() = { side,side,side };// 23

    return{
        std::move( m_VBVertices ),{
            0,2, 1,    2,3,1,
            4,5, 7,    4,7,6,
            8,10, 9,  10,11,9,
            12,13,15, 12,15,14,
            16,17,18, 18,17,19,
            20,23,21, 20,22,23
        }
    };
}

IndexedTriangleList Cube::MakeIndependentTextured()
{
	using namespace Dvtx;
	using Type = Dvtx::VertexLayout::ElementType;

	auto itl = MakeIndependent( std::move( VertexLayout{}
		.Append( Type::Position3D )
		.Append( Type::Normal )
		.Append( Type::Texture2D )
	) );

	itl.m_VBVertices[0].Attr<Type::Texture2D>() = { 0.0f,0.0f };
	itl.m_VBVertices[1].Attr<Type::Texture2D>() = { 1.0f,0.0f };
	itl.m_VBVertices[2].Attr<Type::Texture2D>() = { 0.0f,1.0f };
	itl.m_VBVertices[3].Attr<Type::Texture2D>() = { 1.0f,1.0f };
	itl.m_VBVertices[4].Attr<Type::Texture2D>() = { 0.0f,0.0f };
	itl.m_VBVertices[5].Attr<Type::Texture2D>() = { 1.0f,0.0f };
	itl.m_VBVertices[6].Attr<Type::Texture2D>() = { 0.0f,1.0f };
	itl.m_VBVertices[7].Attr<Type::Texture2D>() = { 1.0f,1.0f };
	itl.m_VBVertices[8].Attr<Type::Texture2D>() = { 0.0f,0.0f };
	itl.m_VBVertices[9].Attr<Type::Texture2D>() = { 1.0f,0.0f };
	itl.m_VBVertices[10].Attr<Type::Texture2D>() = { 0.0f,1.0f };
	itl.m_VBVertices[11].Attr<Type::Texture2D>() = { 1.0f,1.0f };
	itl.m_VBVertices[12].Attr<Type::Texture2D>() = { 0.0f,0.0f };
	itl.m_VBVertices[13].Attr<Type::Texture2D>() = { 1.0f,0.0f };
	itl.m_VBVertices[14].Attr<Type::Texture2D>() = { 0.0f,1.0f };
	itl.m_VBVertices[15].Attr<Type::Texture2D>() = { 1.0f,1.0f };
	itl.m_VBVertices[16].Attr<Type::Texture2D>() = { 0.0f,0.0f };
	itl.m_VBVertices[17].Attr<Type::Texture2D>() = { 1.0f,0.0f };
	itl.m_VBVertices[18].Attr<Type::Texture2D>() = { 0.0f,1.0f };
	itl.m_VBVertices[19].Attr<Type::Texture2D>() = { 1.0f,1.0f };
	itl.m_VBVertices[20].Attr<Type::Texture2D>() = { 0.0f,0.0f };
	itl.m_VBVertices[21].Attr<Type::Texture2D>() = { 1.0f,0.0f };
	itl.m_VBVertices[22].Attr<Type::Texture2D>() = { 0.0f,1.0f };
	itl.m_VBVertices[23].Attr<Type::Texture2D>() = { 1.0f,1.0f };

	return itl;
}