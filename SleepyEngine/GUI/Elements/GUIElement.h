#pragma once
#include "../../Utilities/NumericDataTypes.h"

class GUIElement
{
public:
	virtual void Draw() = 0;
	virtual ~GUIElement() = default;
public:
	u32 m_iHeight;
	u32 m_iWidth;
};