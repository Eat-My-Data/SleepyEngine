#pragma once
#include "Camera.h"
#include "SleepyMath.h"
class SceneManager
{
public:
	SceneManager();
private:
	Camera m_Camera = { L"Character Camera", MatrixType::Perspective, ViewSpace( 1.0f, 9.0f / 16.0f, 0.5f, 400.0f ), DirectX::XMFLOAT3{ -13.5f, 0.0f, 3.5f }, 0.0f, PI / 2.0f };
};