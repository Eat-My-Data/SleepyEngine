#pragma once
#include "GUIElement.h"
#include "../Bindable/Bindable.h"
#include "DirectXMath.h"

class ChooseRenderTechnique : public GUIElement
{
public:
	ChooseRenderTechnique( f32 x, f32 y, f32 width, f32 height );
public:
	void Draw( class GraphicsDeviceInterface& gdi ) override;
	void Interact( class SceneManager& sceneManager ) override;
private:
	bool isForwardRender = false;
private:
	struct ColorConst
	{
		DirectX::XMFLOAT4 color = { 0.0f, 1.0f, 1.0f, 1.0f };
	} color;
};