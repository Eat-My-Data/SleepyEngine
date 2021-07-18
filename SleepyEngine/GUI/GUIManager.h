#pragma once
#include "ChooseRenderTechnique.h"
#include "../SceneManager/Camera.h"
#include "DirectXMath.h"

class GraphicsDeviceInterface;

class GUIManager
{
public:
	void Draw( class GraphicsDeviceInterface& gdi );
private:
	ChooseRenderTechnique m_ChooseRenderTechnique{ 1, 1, 200, 200 };
private:
	struct GUITransform
	{
		DirectX::XMMATRIX guiTransform;
	} m_GUITransformCbuf;
	Camera m_GUITransformCamera = { L"GUI Transfrom", MatrixType::Orthographic, ViewSpace( 640.0f, 360.0f, -1.0f, 1.0f ), DirectX::XMFLOAT3{ 0.0f, 0.0f, -1.0f } };
};