#pragma once
#include "./Elements/GUIElement.h"
#include "./Elements/Title.h"
#include "./Elements/TextBox.h"
#include <vector>

class GraphicsDeviceInterface;

class GUIContainer
{
public:
	void AddElement( GUIElement* element );
	void Draw( class GraphicsDeviceInterface& gdi  );
private:
	void AdjustContainer( GUIElement* element );
	std::vector<GUIElement*> m_vecOfElements;
	f32 m_iContainerWidth = 0;
	f32 m_iContainerHeight = 0;
};