#pragma once
#include "../GUIElement.h"
#include "../../Bindable/Bindable.h"
#include "DirectXMath.h"

class ChooseRenderTechnique
{
public:
	bool DoElement( class GraphicsDeviceInterface& gdi, UI_ID& active, UI_ID& hot, Mouse& mouse );
	void Draw( class GraphicsDeviceInterface& gdi );
	void Interact( class SceneManager& sceneManager );
private:
	//bool Inside( std::pair<u32, u32> mousePos ) override;
private:
	char* m_sName = (char*)"ChooseRenderTechnique";
	bool isActive = false;
	bool isForwardRender = false;
private:
	struct ColorConst
	{
		DirectX::XMFLOAT4 color = { 0.0f, 1.0f, 1.0f, 1.0f };
	} color;
};