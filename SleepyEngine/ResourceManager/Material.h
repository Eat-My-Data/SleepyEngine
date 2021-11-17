#pragma once
#include "../GraphicsDeviceInterface/GraphicsDeviceInterface.h"
#include "../Bindable/BindableCommon.h"
#include <vector>
#include <filesystem>
#include "./Jobber/Technique.h"

struct aiMaterial;
struct aiMesh;

class Material
{
public:
	Material( GraphicsDeviceInterface& gfx, const aiMaterial& material, const std::filesystem::path& path ) noexcept;
	Dvtx::VertexBuffer ExtractVertices( const aiMesh& mesh ) const noexcept;
	std::vector<unsigned short> ExtractIndices( const aiMesh& mesh ) const noexcept;
	std::shared_ptr<Bind::VertexBuffer> MakeVertexBindable( GraphicsDeviceInterface& gfx, const aiMesh& mesh ) const noexcept;
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