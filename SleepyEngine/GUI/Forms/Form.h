#pragma once
#include "../../Utilities/NumericDataTypes.h"
#include <numeric>

class GraphicsDeviceInterface;
class SceneManager;
class Mouse;

class Form
{
public:
	Form( f32 offset_x, f32 offset_y, f32 width, f32 height );
public:
	virtual bool Begin( char* title ) = 0;
	virtual void Text( char* sentence ) = 0;
	virtual void Draw( class GraphicsDeviceInterface& gdi  ) = 0;	
protected:
	f32 m_iX;
	f32 m_iY;
	f32 m_iWidth;
	f32 m_iHeight;
};