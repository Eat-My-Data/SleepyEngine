#pragma once
#include "GUIElement.h"
#include "../Bindable/Bindable.h"
#include "DirectXMath.h"

class ChooseRenderTechnique : public GUIElement
{
public:
	ChooseRenderTechnique( u32 x, u32 y, u32 width, u32 height );
public:
	void Draw( class GraphicsDeviceInterface& gdi ) override;
	void Interact( class SceneManager& sceneManager ) override;
private:
	bool isForwardRender = false;
private:
	struct ColorConst
	{
		DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	} color;
};