#pragma once
#include "../../Utilities/NumericDataTypes.h"

class GUIElement
{
public:
	virtual void Draw() = 0;
	virtual ~GUIElement() = default;
};