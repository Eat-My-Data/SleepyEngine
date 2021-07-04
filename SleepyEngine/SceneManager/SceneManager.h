#pragma once
#include "../GraphicsDeviceInterface/GraphicsDeviceInterface.h"
#include "Camera.h"
#include "../Utilities/SleepyMath.h"
#include "../ResourceManager/Mesh.h"
#include "../Drawable/DirectionalLight.h"

enum class RenderTechnique
{
	Uninitialized,
	Forward,
	Deferred
};

class SceneManager
{
public:
	SceneManager() = default;
	~SceneManager();
public:
	void Initialize( GraphicsDeviceInterface& gdi, GraphicsAPI api );
	bool IsInitialzed() noexcept;
	void SetRenderTechnique( RenderTechnique renderTechnique ) noexcept;
public:
	void Draw();
	void Update( f32 dt );
private:
	void ForwardRender();
	void DeferredRender();
private:
	GraphicsAPI m_GraphicsAPI = GraphicsAPI::Uninitialized;
	GraphicsDeviceInterface* m_pGDI = nullptr;
	Camera m_Camera = { L"Character Camera", MatrixType::Perspective, ViewSpace( 1.0f, 9.0f / 16.0f, 0.5f, 400.0f ), DirectX::XMFLOAT3{ -13.5f, 10.0f, 3.5f }, 0.0f, PI / 2.0f };
	RenderTechnique m_RenderTechnique = RenderTechnique::Uninitialized;
private:
	DirectionalLight* m_pDirectionalLight;
	std::vector<Model*> m_vecOfModels;
};