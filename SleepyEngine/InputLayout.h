#pragma once
#include "Bindable.h"
#include "Vertex.h"

namespace Bind
{
	class InputLayout : public Bindable
	{
	public:
		InputLayout( GraphicsDeviceInterface& gfx,
			Dvtx::VertexLayout layout,
			ID3DBlob* pVertexShaderBytecode );
		void Bind( GraphicsDeviceInterface& gfx ) noexcept override;
		static std::shared_ptr<InputLayout> Resolve( GraphicsDeviceInterface& gfx,
			const Dvtx::VertexLayout& layout, ID3DBlob* pVertexShaderBytecode );
		static std::string GenerateUID( const Dvtx::VertexLayout& layout, ID3DBlob* pVertexShaderBytecode = nullptr );
		std::string GetUID() const noexcept override;
	protected:
		Dvtx::VertexLayout layout;
		ID3D11InputLayout* pInputLayout;
	};
}