#pragma once
#include "../GraphicsDeviceInterface/GraphicsDeviceInterface.h"
#include <string>
#include <memory>
#include <filesystem>

class Node;
class Mesh;
class FrameCommander;
class ModelWindow;
struct aiMesh;
struct aiMaterial;
struct aiNode;

class Model
{
public:
	Model( GraphicsDeviceInterface& gfx, const std::string& pathString, float scale = 1.0f );
	void Submit( FrameCommander& frame ) const noexcept;
	//void ShowWindow( GraphicsDeviceInterface& gfx, const char* windowName = nullptr ) noexcept;
	void SetRootTransform( DirectX::FXMMATRIX tf ) noexcept;
	~Model() noexcept;
private:
	std::unique_ptr<Node> ParseNode( int& nextId, const aiNode& node, DirectX::FXMMATRIX additionalTransform ) noexcept;
private:
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
	//std::unique_ptr<ModelWindow> pWindow;
};