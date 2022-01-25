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
	void Submit() const noexcept;
	//void ShowWindow( GraphicsDeviceInterface& gfx, const char* windowName = nullptr ) noexcept;
	void SetRootTransform( DirectX::FXMMATRIX tf ) noexcept;
	void Accept( class ModelProbe& probe );
	~Model() noexcept;
private:
	std::unique_ptr<Node> ParseNode( int& nextId, const aiNode& node, float scale ) noexcept;
private:
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
	//std::unique_ptr<ModelWindow> pWindow;
};