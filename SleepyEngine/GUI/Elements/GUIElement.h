#pragma once
#include "../../Utilities/NumericDataTypes.h"

class GraphicsDeviceInterface;

class GUIElement
{
public:
	virtual void Draw( class GraphicsDeviceInterface& gdi ) = 0;
	virtual ~GUIElement() = default;
public:
	f32 m_iHeight;
	f32 m_iWidth;
};