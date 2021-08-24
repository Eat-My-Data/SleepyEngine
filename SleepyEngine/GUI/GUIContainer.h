#pragma once
#include "./Elements/GUIElement.h"
#include "./Elements/Title.h"
#include "./Elements/TextBox.h"
#include <vector>

class GraphicsDeviceInterface;

class GUIContainer
{
public:
	static void AddElement( GUIElement* element );
	static void Draw( class GraphicsDeviceInterface& gdi  );
private:
	static std::vector<GUIElement*> m_vecOfElements;
private:
	u32 m_iWidth = 20;
	u32 m_iHeight = 100;
};