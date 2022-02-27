#pragma once
#include "../Bindable.h"
#include <array>
#include <optional>

namespace Bind
{
	class Blender : public Bindable
	{
	public:
		Blender( Graphics& gdi, bool blending, std::optional<float> factor = {} );
		void Bind( Graphics& gdi ) noexcept override;
		void SetFactor( float factor ) noexcept;
		float GetFactor() const noexcept;
		static std::shared_ptr<Blender> Resolve( Graphics& gdi, bool blending, std::optional<float> factor = {} );
		static std::string GenerateUID( bool blending, std::optional<float> factor );
		std::string GetUID() const noexcept override;
	protected:
		ID3D11BlendState* pBlender;
		bool blending;
		std::optional<std::array<float, 4>> factors;
	};
}
