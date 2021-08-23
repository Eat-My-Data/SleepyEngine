#pragma once
#include "./GUIElement.h"

class Text : public GUIElement
{
public:
	Text( f32 offset_x, f32 offset_y, f32 width, f32 height );
public:
	void Draw( class GraphicsDeviceInterface& gdi ) override;
};