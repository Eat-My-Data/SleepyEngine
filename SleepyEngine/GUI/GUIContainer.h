#pragma once
#include "./Elements/GUIElement.h"
#include "Elements\Title.h"

#include <vector>

class GraphicsDeviceInterface;

class GUIContainer
{
public:
	static void AddElement( GUIElement* element );
	static void Draw( class GraphicsDeviceInterface& gdi  );
private:
	static std::vector<GUIElement*> m_vecOfElements;
};