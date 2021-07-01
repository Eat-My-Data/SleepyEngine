#pragma once
#include "GraphicsDeviceInterface.h"
#include "Camera.h"
#include "SleepyMath.h"
#include "Cube.h"

class SceneManager
{
public:
	SceneManager() = default;;
	~SceneManager() = default;
public:
	void Initialize( GraphicsDeviceInterface& gdi, GraphicsAPI api );
	bool IsInitialzed() noexcept;
public:
	void Draw();
	void Update( f32 dt );
private:
	GraphicsAPI m_GraphicsAPI = GraphicsAPI::Uninitialized;
	GraphicsDeviceInterface* m_pGDI = nullptr;
	Camera m_Camera = { L"Character Camera", MatrixType::Perspective, ViewSpace( 1.0f, 9.0f / 16.0f, 0.5f, 400.0f ), DirectX::XMFLOAT3{ -13.5f, 6.0f, 3.5f }, 0.0f, PI / 2.0f };
private:
	// Drawables:
	std::vector<Cube*> m_vecOfCubes;
};