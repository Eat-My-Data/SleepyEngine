#pragma once
#include "../GraphicsDeviceInterface/GraphicsDeviceInterface.h"
#include <vector>
#include "./Jobber/Technique.h"
#include "Vertex.h"
#include <filesystem>

struct aiMaterial;
struct aiMesh;

namespace Bind
{
	class VertexBuffer;
	class IndexBuffer;
}

class Material
{
public:
	Material( GraphicsDeviceInterface& gfx, const aiMaterial& material, const std::filesystem::path& path ) noexcept;
	Dvtx::VertexBuffer ExtractVertices( const aiMesh& mesh ) const noexcept;
	std::vector<unsigned short> ExtractIndices( const aiMesh& mesh ) const noexcept;
	std::shared_ptr<Bind::VertexBuffer> MakeVertexBindable( GraphicsDeviceInterface& gfx, const aiMesh& mesh, float scale = 1.0f ) const noexcept;
	std::shared_ptr<Bind::IndexBuffer> MakeIndexBindable( GraphicsDeviceInterface& gfx, const aiMesh& mesh ) const noexcept;
	std::vector<Technique> GetTechniques() const noexcept;
private:
	std::string MakeMeshTag( const aiMesh& mesh ) const noexcept;
private:
	Dvtx::VertexLayout vtxLayout;
	std::vector<Technique> techniques;
	std::string modelPath;
	std::string name;
};