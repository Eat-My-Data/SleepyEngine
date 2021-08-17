#pragma once
#include "Form.h"

class TextBox : public Form
{
public:
	TextBox( f32 offset_x, f32 offset_y, f32 width, f32 height );
public:
	void Draw( class GraphicsDeviceInterface& gdi ) override;
};