#pragma once
#include "../../ResourceManager/IndexedTriangleList.h"
#include <optional>

class Rectangle
{
public:
	static IndexedTriangleList Make( std::optional<Dvtx::VertexLayout> layout = {} )
	{
		using namespace Dvtx;
		using Type = Dvtx::VertexLayout::ElementType;

		if ( !layout )
		{
			layout = Dvtx::VertexLayout{};
			layout->Append( Type::Position2D );
		}

		constexpr float side = 1.0f / 2.0f;

		VertexBuffer vertices( std::move( *layout ), 6u );
		vertices[0].Attr<Type::Position2D>() = { -side,-side };
		vertices[1].Attr<Type::Position2D>() = { side,-side };
		vertices[2].Attr<Type::Position2D>() = { -side,side };
		vertices[3].Attr<Type::Position2D>() = { side,side };
		vertices[4].Attr<Type::Position2D>() = { side,-side };
		vertices[5].Attr<Type::Position2D>() = { -side,side };

		return{
			std::move( vertices ),{
				0,2,1, 2,3,1,
			}
		};
	}
};