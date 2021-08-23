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
	virtual bool Begin( char* title ) = 0;
	virtual void Text( char* sentence ) = 0;
	virtual bool Button( char* sentence ) = 0;
	virtual void End() = 0;
private:
	virtual bool Inside( std::pair<u32, u32> mousePos ) = 0;
protected:
	f32 m_iX;
	f32 m_iY;
	f32 m_iWidth;
	f32 m_iHeight;
};