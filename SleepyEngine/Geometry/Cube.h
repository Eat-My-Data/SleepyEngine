#pragma once

#include <optional>
#include "../Renderer/Model/Vertex.h"
#include "../Renderer/Model/IndexedTriangleList.h"
#include <DirectXMath.h>
#include "../Utilities/ChiliMath.h"
#include <array>
#include <optional>

class Cube
{
public:
	static IndexedTriangleList Make( std::optional<Dvtx::VertexLayout> layout = {} )
	{
		using namespace Dvtx;
		using Type = Dvtx::VertexLayout::ElementType;

		if ( !layout )
		{
			layout = Dvtx::VertexLayout{};
			layout->Append( Type::Position3D );
		}

		constexpr float side = 1.0f / 2.0f;

		Dvtx::VertexBuffer vertices( std::move( *layout ), 8u );
		vertices[0].Attr<Type::Position3D>() = { -side,-side,-side };
		vertices[1].Attr<Type::Position3D>() = { side,-side,-side };
		vertices[2].Attr<Type::Position3D>() = { -side,side,-side };
		vertices[3].Attr<Type::Position3D>() = { side,side,-side };
		vertices[4].Attr<Type::Position3D>() = { -side,-side,side };
		vertices[5].Attr<Type::Position3D>() = { side,-side,side };
		vertices[6].Attr<Type::Position3D>() = { -side,side,side };
		vertices[7].Attr<Type::Position3D>() = { side,side,side };

		return{
			std::move( vertices ),{
				0,2,1, 2,3,1,
				1,3,5, 3,7,5,
				2,6,3, 3,6,7,
				4,5,7, 4,7,6,
				0,4,2, 2,4,6,
				0,1,4, 1,5,4
			}
		};
	}
	static IndexedTriangleList MakeIndependent( Dvtx::VertexLayout layout )
	{
		using namespace Dvtx;
		using Type = Dvtx::VertexLayout::ElementType;

		constexpr float side = 1.0f / 2.0f;

		Dvtx::VertexBuffer m_VBVertices( std::move( layout ),24u );
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
	static IndexedTriangleList MakeIndependentTextured()
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
};