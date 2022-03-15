#pragma once
#include <optional>
#include "../Renderer/Model/Vertex.h"
#include "../Renderer/Model/IndexedTriangleList.h"
#include <DirectXMath.h>
#include "../Utilities/ChiliMath.h"

class FullScreen2T
{
public:
	static IndexedTriangleList Make()
	{
		namespace dx = DirectX;
		Dvtx::VertexLayout lay;
		lay.Append( Dvtx::VertexLayout::Position2D );
		Dvtx::VertexBuffer vb{ lay };
		vb.EmplaceBack( dx::XMFLOAT2{ -1,1 } );
		vb.EmplaceBack( dx::XMFLOAT2{ 1,1 } );
		vb.EmplaceBack( dx::XMFLOAT2{ -1,-1 } );
		vb.EmplaceBack( dx::XMFLOAT2{ 1,-1 } );
		std::vector<unsigned short> indices = { 0,1,2,1,3,2 };
		return { std::move( vb ),std::move( indices ) };
	}
};
