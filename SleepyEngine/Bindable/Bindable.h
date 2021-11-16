#pragma once
#include "../GraphicsDeviceInterface/GraphicsDeviceInterface.h"
#include <string>
#include <memory>

class Drawable;
class TechniqueProbe;

namespace Bind
{
	class Bindable
	{
	public:
		virtual void Bind( GraphicsDeviceInterface& gdi ) noexcept = 0;
		virtual void InitializeParentReference( const Drawable& ) noexcept
		{}
		virtual void Accept( TechniqueProbe& )
		{}
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
	class CloningBindable : public Bindable
	{
	public:
		virtual std::unique_ptr<CloningBindable> Clone() const noexcept = 0;
	};
}
