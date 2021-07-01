#pragma once
#include "GraphicsDeviceInterface.h"
#include <string>
#include <memory>

namespace Bind
{
	class Bindable
	{
	public:
		virtual void Bind( GraphicsDeviceInterface& gdi ) noexcept = 0;
		virtual std::string GetUID() const noexcept
		{
			assert( false );
			return "";
		}
		virtual ~Bindable() = default;
	protected:
		static ID3D11DeviceContext* GetContext( GraphicsDeviceInterface& gdi ) noexcept;
		static ID3D11Device* GetDevice( GraphicsDeviceInterface& gdi ) noexcept;
	};
}
