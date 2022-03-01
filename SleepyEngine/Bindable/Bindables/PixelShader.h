#pragma once
#include "../Bindable.h"

namespace Bind
{
	class PixelShader : public Bindable
	{
	public:
		PixelShader( Graphics& gdi, const std::string& path );
		void Bind( Graphics& gdi ) noexcept override;
		static std::shared_ptr<PixelShader> Resolve( Graphics& gdi, const std::string& path );
		static std::string GenerateUID( const std::string& path );
		std::string GetUID() const noexcept override;
		const std::string GetPath() const noexcept;
	protected:
		std::string m_sPath;
		ID3D11PixelShader* m_pPixelShader;
	};
}
