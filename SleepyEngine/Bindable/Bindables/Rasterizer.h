#pragma once
#include "../Bindable.h"
#include <array>

namespace Bind
{
	class Rasterizer : public Bindable
	{
	public:
		Rasterizer( Graphics& gdi, bool twoSided );
		void Bind( Graphics& gdi ) noexcept override;
		static std::shared_ptr<Rasterizer> Resolve( Graphics& gdi, bool twoSided );
		static std::string GenerateUID( bool twoSided );
		std::string GetUID() const noexcept override;
	protected:
		ID3D11RasterizerState* m_pRasterizer;
		bool m_bTwoSided;
	};
}
