#pragma once

class GraphicsDeviceInterface;

class GUIContainer
{
public:
	void Draw( class GraphicsDeviceInterface& gdi  );
	void AddElement( /*Element like text or button*/ );
	void Flush();
private:
	// vector of elements
};