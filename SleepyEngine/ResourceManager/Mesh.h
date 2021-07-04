#pragma once
#include "../Drawable/Drawable.h"
#include "../Bindable/Bindable.h"
#include "../ResourceManager/Vertex.h"
#include <optional>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../Bindable/Bindables/ConstantBuffers.h"
#include <type_traits>
#include <filesystem>
#include "../Bindable/Bindables/Texture.h"
#include "../Bindable/Bindables/Sampler.h"
#include "../Bindable/Bindables/DynamicConstant.h"

class Mesh : public Drawable
{
public:
	Mesh( GraphicsDeviceInterface& gfx, std::vector<std::shared_ptr<Bind::Bindable>> bindPtrs );
	void Draw( GraphicsDeviceInterface& gfx, DirectX::FXMMATRIX accumulatedTransform ) const noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	mutable DirectX::XMFLOAT4X4 transform;
};

class Node
{
	friend class Model;
public:
	Node( int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform ) noexcept;
	void Draw( GraphicsDeviceInterface& gfx, DirectX::FXMMATRIX accumulatedTransform ) const noexcept;
	void SetAppliedTransform( DirectX::FXMMATRIX transform ) noexcept;
	const DirectX::XMFLOAT4X4& GetAppliedTransform() const noexcept;
	int GetId() const noexcept;
	void ShowTree( Node*& pSelectedNode ) const noexcept;
	const Dcb::Buffer* GetMaterialConstants() const noexcept;
	void SetMaterialConstants( const Dcb::Buffer& ) noexcept;
private:
	void AddChild( std::unique_ptr<Node> pChild ) noexcept;
private:
	std::string name;
	int id;
	std::vector<std::unique_ptr<Node>> childPtrs;
	std::vector<Mesh*> meshPtrs;
	DirectX::XMFLOAT4X4 transform;
	DirectX::XMFLOAT4X4 appliedTransform;
};

class Model
{
public:
	Model( GraphicsDeviceInterface& gfx, const std::string& pathString, float scale = 1.0f );
	void Draw( GraphicsDeviceInterface& gfx ) const noexcept;
	void ShowWindow( GraphicsDeviceInterface& gfx, const char* windowName = nullptr ) noexcept;
	void SetRootTransform( DirectX::FXMMATRIX tf ) noexcept;
	~Model() noexcept;
private:
	static std::unique_ptr<Mesh> ParseMesh( GraphicsDeviceInterface& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials, const std::filesystem::path& path, float scale );
	std::unique_ptr<Node> ParseNode( int& nextId, const aiNode& node ) noexcept;
private:
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
	std::unique_ptr<class ModelWindow> pWindow;
};