#pragma once
#include "../GraphicsDeviceInterface/GraphicsDeviceInterface.h"
#include "Camera.h"
#include "../Utilities/SleepyMath.h"
#include "../ResourceManager/Mesh.h"
#include "./LightManager.h"
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
	void ToggleImGuiEngabled() noexcept;
public:
	void Draw();
	void DrawControlPanel();
	void Present();
	void RotateCamera( const f32 dx, const f32 dy );
	void TranslateCamera( DirectX::XMFLOAT3 camDelta );
public:
	void SetActivePointLight( const u32 index );
	void TranslatePointLight( DirectX::XMFLOAT3 translation );
	void TranslateDirectionalLight( DirectX::XMFLOAT3 translation );
	void RotateDirectionalLight( const f32 dx, const f32 dy );
	void SetActiveSpotLight( const u32 index );
	void TranslateSpotLight( DirectX::XMFLOAT3 translation );
	void RotateSpotLight( const f32 dx, const f32 dy );
private:
	void PrepareFrame();
	void ForwardRender();
	void DeferredRender();
public:
	bool imguiEnabled = true;
private:
	GraphicsAPI m_GraphicsAPI = GraphicsAPI::Uninitialized;
	GraphicsDeviceInterface* m_pGDI = nullptr;
	Camera m_Camera = { L"Character Camera", MatrixType::Perspective, ViewSpace( 1.0f, 9.0f / 16.0f, 0.5f, 400.0f ), DirectX::XMFLOAT3{ -13.5f, 5.0f, 3.5f }, 0.0f, PI / 2.0f };
	LightManager m_LightManager;
	RenderTechnique m_RenderTechnique = RenderTechnique::Uninitialized;
private:
	std::vector<Model*> m_vecOfModels;
	// TODO: 
	// - Resource Manager
	// - Culling
	// - Clean up shader files/minimize everything
	// - Make to have as little buffers as possible
};