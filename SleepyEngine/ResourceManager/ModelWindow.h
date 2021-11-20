#pragma once
#include "../GraphicsDeviceInterface/GraphicsDeviceInterface.h"
#include "../Bindable/Bindables/DynamicConstant.h"
#include <optional>
#include <unordered_map>

class Node;

class ModelWindow // pImpl idiom, only defined in this .cpp
{
public:
	void Show( GraphicsDeviceInterface& gfx, const char* windowName, const Node& root ) noexcept;
	void ApplyParameters() noexcept;
private:
	DirectX::XMMATRIX GetTransform() const noexcept;
	const Dcb::Buffer& GetMaterial() const noexcept;
	bool TransformDirty() const noexcept;
	void ResetTransformDirty() noexcept;
	bool MaterialDirty() const noexcept;
	void ResetMaterialDirty() noexcept;
	bool IsDirty() const noexcept;
private:
	Node* pSelectedNode;
	struct TransformParameters
	{
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};
	std::unordered_map<int, TransformParameters> transforms;
};