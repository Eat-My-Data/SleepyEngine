#pragma once
#include "../../Graphics/Graphics.h"
#include <vector>
#include "../Jobber/Technique.h"
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
	Material( Graphics& gfx, const aiMaterial& material, const std::filesystem::path& path ) noexcept;
	Dvtx::VertexBuffer ExtractVertices( const aiMesh& mesh ) const noexcept;
	std::vector<unsigned short> ExtractIndices( const aiMesh& mesh ) const noexcept;
	std::shared_ptr<Bind::VertexBuffer> MakeVertexBindable( Graphics& gfx, const aiMesh& mesh, float scale = 1.0f ) const noexcept;
	std::shared_ptr<Bind::IndexBuffer> MakeIndexBindable( Graphics& gfx, const aiMesh& mesh ) const noexcept;
	std::vector<Technique> GetTechniques() const noexcept;
private:
	std::string MakeMeshTag( const aiMesh& mesh ) const noexcept;
private:
	Dvtx::VertexLayout vtxLayout;
	std::vector<Technique> techniques;
	std::string modelPath;
	std::string name;
};