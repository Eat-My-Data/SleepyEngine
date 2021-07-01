#pragma once
#include "../Bindable.h"
#include <array>

namespace Bind
{
	class Rasterizer : public Bindable
	{
	public:
		Rasterizer( GraphicsDeviceInterface& gdi, bool twoSided );
		void Bind( GraphicsDeviceInterface& gdi ) noexcept override;
		static std::shared_ptr<Rasterizer> Resolve( GraphicsDeviceInterface& gdi, bool twoSided );
		static std::string GenerateUID( bool twoSided );
		std::string GetUID() const noexcept override;
	protected:
		ID3D11RasterizerState* m_pRasterizer;
		bool m_bTwoSided;
	};
}
