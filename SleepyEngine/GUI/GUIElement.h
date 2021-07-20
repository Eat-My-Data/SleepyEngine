#pragma once
#include "../Utilities/NumericDataTypes.h"
#include <numeric>
#include "UI_ID.h"

class GraphicsDeviceInterface;
class SceneManager;
class Mouse;

class GUIElement
{
public:
	GUIElement( f32 x, f32 y, f32 width, f32 height );
public:
	virtual bool DoElement( UI_ID& active, UI_ID& hot, Mouse& mouse ) = 0;
	virtual void Draw( class GraphicsDeviceInterface& gdi ) = 0;
	virtual void Interact( class SceneManager& sceneManager ) = 0;
private:
	virtual bool Inside( std::pair<u32, u32> mousePos ) = 0;
protected:
	f32 m_iX;
	f32 m_iY;
	f32 m_iWidth;
	f32 m_iHeight;
};