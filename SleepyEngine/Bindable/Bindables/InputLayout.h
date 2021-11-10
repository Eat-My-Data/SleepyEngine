#pragma once
#include "../Bindable.h"
#include "../BindableCodex.h"
#include "../../ResourceManager/Vertex.h"

namespace Bind
{
	class InputLayout : public Bindable
	{
	public:
		InputLayout( GraphicsDeviceInterface& gdi,
			Dvtx::VertexLayout layout,
			ID3DBlob* pVertexShaderBytecode );
		void Bind( GraphicsDeviceInterface& gdi ) noexcept override;
		const Dvtx::VertexLayout GetLayout() const noexcept;
		static std::shared_ptr<InputLayout> Resolve( GraphicsDeviceInterface& gdi,
			const Dvtx::VertexLayout& layout, ID3DBlob* pVertexShaderBytecode );
		static std::string GenerateUID( const Dvtx::VertexLayout& layout, ID3DBlob* pVertexShaderBytecode = nullptr );
		std::string GetUID() const noexcept override;
	protected:
		Dvtx::VertexLayout m_VertexLayout;
		ID3D11InputLayout* m_pInputLayout;
	};
}