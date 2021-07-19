#pragma once
#include "ChooseRenderTechnique.h"
#include "../SceneManager/Camera.h"
#include "DirectXMath.h"
#include "../Utilities/NumericDataTypes.h"
#include "UI_ID.h"
#include <vector>

class GraphicsDeviceInterface;
class SceneManager;
class Mouse;

class GUIManager
{
public:
	void Initialize( class GraphicsDeviceInterface& gdi, class SceneManager& sceneManager, class Mouse& mouse );
	bool IsInitialized() noexcept;
	void Update();
	void Draw();
private:
	std::vector<GUIElement*> m_vecOfGUIElements;
private:
	struct GUITransform
	{
		DirectX::XMMATRIX guiTransform;
	} m_GUITransformCbuf;
	Camera m_GUITransformCamera = { L"GUI Transfrom", MatrixType::Orthographic, ViewSpace( 640.0f, 360.0f, -1.0f, 1.0f ), DirectX::XMFLOAT3{ 0.0f, 0.0f, -1.0f } };
private:
	GraphicsDeviceInterface* m_pGDI = nullptr;
	SceneManager* m_pSceneManager = nullptr;
	Mouse* m_pMouse;
private:
	UI_ID* m_pHot = nullptr;
	UI_ID* m_pActive = nullptr;
private:
	bool m_bIsInitialized = false;
};