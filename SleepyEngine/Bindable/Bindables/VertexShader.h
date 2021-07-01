#pragma once
#include "../Bindable.h"

namespace Bind
{
	class VertexShader : public Bindable
	{
	public:
		VertexShader( GraphicsDeviceInterface& gdi, const std::string& path );
		void Bind( GraphicsDeviceInterface& gdi ) noexcept override;
		ID3DBlob* GetBytecode() const noexcept;
		static std::shared_ptr<VertexShader> Resolve( GraphicsDeviceInterface& gdi, const std::string& path );
		static std::string GenerateUID( const std::string& path );
		std::string GetUID() const noexcept override;
	protected:
		std::string m_sPath;
		ID3DBlob* m_pBytecodeBlob;
		ID3D11VertexShader* m_pVertexShader;
	};
}
