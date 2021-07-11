#pragma once
#include "../GraphicsDeviceInterface/GraphicsDeviceInterface.h"
#include "Camera.h"
#include "../Utilities/SleepyMath.h"
#include "../ResourceManager/Mesh.h"
#include "../Drawable/DirectionalLight.h"
#include "../Drawable/PointLight.h"
#include "./RenderTechnique.h"


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
	void Present();
	void RotateCamera( const f32 dx, const f32 dy );
	void TranslateCamera( DirectX::XMFLOAT3 camDelta );
private:
	void ForwardRender();
	void DeferredRender();
private:
	GraphicsAPI m_GraphicsAPI = GraphicsAPI::Uninitialized;
	GraphicsDeviceInterface* m_pGDI = nullptr;
	Camera m_Camera = { L"Character Camera", MatrixType::Perspective, ViewSpace( 1.0f, 9.0f / 16.0f, 0.5f, 400.0f ), DirectX::XMFLOAT3{ -13.5f, 5.0f, 3.5f }, 0.0f, PI / 2.0f };
	Camera m_DirectionalLightOrthoCamera = { L"Directional Light Camera", MatrixType::Orthographic, ViewSpace( 400.0f, 400.0f, 1.0f, 1200.0f ), DirectX::XMFLOAT3{ 0.0f,200.8f,100.0f },  PI / 2.0f, -PI };
	RenderTechnique m_RenderTechnique = RenderTechnique::Uninitialized;
private:
	DirectionalLight* m_pForwardDirectionalLight;
	DirectionalLight* m_pDeferredDirectionalLight;
	PointLight* m_pPointLight;
	std::vector<Model*> m_vecOfModels;
};