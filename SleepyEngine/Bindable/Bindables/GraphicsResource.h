#pragma once
#include "../../GraphicsDeviceInterface/GraphicsDeviceInterface.h"

class GraphicsDeviceInterface;

class GraphicsResource
{
protected:
	static ID3D11DeviceContext* GetContext( GraphicsDeviceInterface& gfx ) noexcept;
	static ID3D11Device* GetDevice( GraphicsDeviceInterface& gfx ) noexcept;
};