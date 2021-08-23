#pragma once
#include "./GUIElement.h"

class Title : public GUIElement
{
public:
	Title( f32 offset_x, f32 offset_y, f32 width, f32 height );
	void Draw() override;
};