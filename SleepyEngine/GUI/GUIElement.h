#pragma once
#include "../Utilities/NumericDataTypes.h"

class GraphicsDeviceInterface;
class SceneManager;

class GUIElement
{
public:
	GUIElement( f32 x, f32 y, f32 width, f32 height );
public:
	virtual void Draw( class GraphicsDeviceInterface& gdi ) = 0;
	virtual void Interact( class SceneManager& sceneManager ) = 0;
protected:
	f32 m_iX;
	f32 m_iY;
	f32 m_iWidth;
	f32 m_iHeight;
};