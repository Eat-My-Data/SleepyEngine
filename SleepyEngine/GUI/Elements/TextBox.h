#pragma once
#include "./GUIElement.h"

class TextBox : public GUIElement
{
public:
	TextBox( char* text );
	void Draw( GraphicsDeviceInterface& gdi ) override;
};