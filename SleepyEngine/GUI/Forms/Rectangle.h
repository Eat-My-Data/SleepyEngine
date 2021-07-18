#pragma once
#include "../../ResourceManager/IndexedTriangleList.h"
#include <optional>

class Rectangle
{
public:
	static IndexedTriangleList Make( f32 x, f32 y, f32 width, f32 height, std::optional<Dvtx::VertexLayout> layout = {} )
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
		vertices[0].Attr<Type::Position2D>() = { x,y };
		vertices[1].Attr<Type::Position2D>() = { x + width,y };
		vertices[2].Attr<Type::Position2D>() = { x,y + height };
		vertices[3].Attr<Type::Position2D>() = { x + width,y + height };
		vertices[4].Attr<Type::Position2D>() = { x + width,y };
		vertices[5].Attr<Type::Position2D>() = { x,y + height };

		return{
			std::move( vertices ),{
				0,2,1, 2,3,1,
			}
		};
	}
};