#include "Bindable.h"
#include <stdexcept>

namespace Bind
{
	ID3D11DeviceContext* Bindable::GetContext( GraphicsDeviceInterface& gdi ) noexcept
	{
		return gdi.GetContext();
	}

	ID3D11Device* Bindable::GetDevice( GraphicsDeviceInterface& gdi ) noexcept
	{
		return gdi.GetDevice();
	}
}
