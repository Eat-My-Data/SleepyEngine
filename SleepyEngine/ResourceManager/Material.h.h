#pragma once
#include <assimp/scene.h>
#include "../Bindable/BindableCommon.h"
#include <vector>
#include <filesystem>
#include "./Jobber/Technique.h"

class Material
{
public:
	Material( GraphicsDeviceInterface& gfx, const aiMaterial* pMaterial, const std::filesystem::path& path ) noexcept
	{

	}
	Dvtx::VertexBuffer ExtractVertices( const aiMesh& mesh ) const noexcept
	{
		using Type = Dvtx::VertexLayout::ElementType;
		Dvtx::VertexBuffer buf{ vtxLayout };
		buf.Resize( mesh.mNumVertices );
		if ( vtxLayout.Has<Type::Position3D>() )
		{
			for ( int i = 0; i < mesh.mNumVertices; i++ )
			{
				buf[i].
			}
		}
	}
	std::vector<Technique> GetTechniques() const noexcept
	{
		return techniques;
	}
private:
	Dvtx::VertexLayout vtxLayout;
	std::vector<Technique> techniques;
};