#pragma once
#include "../Utilities/NumericDataTypes.h"

class GraphicsDeviceInterface;
class SceneManager;

class GUIElement
{
public:
	GUIElement( u32 x, u32 y, u32 width, u32 height );
public:
	virtual void Draw( class GraphicsDeviceInterface& gdi ) = 0;
	virtual void Interact( class SceneManager& sceneManager ) = 0;
private:
	u32 m_iX;
	u32 m_iY;
	u32 m_iWidth;
	u32 m_iHeight;
};