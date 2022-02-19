#include "GraphicsResource.h"
#include <stdexcept>


ID3D11DeviceContext* GraphicsResource::GetContext( GraphicsDeviceInterface& gfx ) noexcept
{
	return gfx.GetContext();
}

ID3D11Device* GraphicsResource::GetDevice( GraphicsDeviceInterface& gfx ) noexcept
{
	return gfx.GetDevice();
}