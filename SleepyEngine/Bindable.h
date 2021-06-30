#pragma once
#include "GraphicsDeviceInterface.h"
#include <string>

namespace Bind
{
	class Bindable
	{
	public:
		virtual void Bind( GraphicsDeviceInterface& gfx ) noexcept = 0;
		virtual std::string GetUID() const noexcept
		{
			assert( false );
			return "";
		}
		virtual ~Bindable() = default;
	protected:
		static ID3D11DeviceContext* GetContext( GraphicsDeviceInterface& gfx ) noexcept;
		static ID3D11Device* GetDevice( GraphicsDeviceInterface& gfx ) noexcept;
	};
}
