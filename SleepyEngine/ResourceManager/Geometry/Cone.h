#pragma once
#include <optional>
#include "../Vertex.h"
#include "../IndexedTriangleList.h"
#include <DirectXMath.h>
#include "../../Utilities/SleepyMath.h"

class Cone
{
public:
	static IndexedTriangleList MakeTesselated( Dvtx::VertexLayout layout, f32 height, f32 stackCount, f32 topRadius, f32 bottomRadius, f32 sliceCount )
	{
		namespace dx = DirectX;

		Dvtx::VertexBuffer vb{ std::move( layout ) };
        f32 stackHeight = height / stackCount;
        f32 radiusStep = ( topRadius - bottomRadius ) / stackCount;
        f32 ringCount = stackCount + 1;

        for ( int i = 0; i < ringCount; i++ ) {
            f32 y = -0.5f * height + i * stackHeight;
            f32 r = bottomRadius + i * radiusStep;
            f32 dTheta = 2.0f * PI / sliceCount;
            for ( f32 j = 0; j <= sliceCount; j++ ) {

                f32 c = dx::XMScalarCos( j * dTheta );
                f32 s = dx::XMScalarSin( j * dTheta );
                vb.EmplaceBack( dx::XMFLOAT3( r * c, y, r * s ) );
            }
        }
        std::vector<unsigned short> indices;
        u32 ringVertexCount = (u32)sliceCount + 1;
        for ( int i = 0; i < stackCount; i++ ) {
            for ( int j = 0; j < sliceCount; j++ ) {
                indices.push_back( i * ringVertexCount + j );
                indices.push_back( ( i + 1 ) * ringVertexCount + j );
                indices.push_back( ( i + 1 ) * ringVertexCount + j + 1 );

                indices.push_back( i * ringVertexCount + j );
                indices.push_back( ( i + 1 ) * ringVertexCount + j + 1 );
                indices.push_back( i * ringVertexCount + j + 1 );
            }
        }

        //BuildCylinderTopCap( topRadius, height, sliceCount, ref ret );
        u32 baseIndex = (u32)vb.Size();
        f32 y = 0.5f * height;
        f32 dTheta = 2.0f * PI / sliceCount;
        for ( int i = 0; i <= sliceCount; i++ ) {
            f32 x = topRadius * dx::XMScalarCos( i * dTheta );
            f32 z = topRadius * dx::XMScalarSin( i * dTheta );
            vb.EmplaceBack( dx::XMFLOAT3( x, y, z ) );
        }
        vb.EmplaceBack( dx::XMFLOAT3(0, y, 0 ) );
        u32 centerIndex =  (u32)vb.Size() - 1;
        for ( int i = 0; i < sliceCount; i++ ) {
            indices.push_back( centerIndex );
            indices.push_back( baseIndex + i + 1 );
            indices.push_back( baseIndex + i );
        }

        //BuildCylinderBottomCap( bottomRadius, height, sliceCount, ref ret );
        u32 baseIndex2 = (u32)vb.Size();
        f32 y2 = -0.5f * height;
        f32 dTheta2 = 2.0f * PI / sliceCount;
        for ( int i = 0; i <= sliceCount; i++ ) {
            f32 x = bottomRadius * dx::XMScalarCos( i * dTheta2 );
            f32 z = bottomRadius * dx::XMScalarSin( i * dTheta2 );
            vb.EmplaceBack( dx::XMFLOAT3( x, y2, z ) );
        }
        vb.EmplaceBack( dx::XMFLOAT3( 0, y2, 0 ) );
        u32 centerIndex2 = (u32)vb.Size() - 1;
        for ( int i = 0; i < sliceCount; i++ ) {
            indices.push_back( centerIndex );
            indices.push_back( baseIndex2 + i );
            indices.push_back( baseIndex2 + i + 1 );
        }
        return { std::move( vb ),std::move( indices ) };
	}

	static IndexedTriangleList Make( std::optional<Dvtx::VertexLayout> layout = std::nullopt )
	{
		using Element = Dvtx::VertexLayout::ElementType;
		if ( !layout )
		{
			layout = Dvtx::VertexLayout{}
                .Append( Element::Position3D );
		}
		return MakeTesselated(std::move(*layout), 2.0f, 24.0f, 0.5f, 1.0f, 24.0f );
	}
};
