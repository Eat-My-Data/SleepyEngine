#pragma once
#include "../Bindable.h"
#include "../BindableCodex.h"
#include "../../ResourceManager/Vertex.h"

namespace Bind
{
	class VertexShader;

	class InputLayout : public Bindable
	{
	public:
		InputLayout( GraphicsDeviceInterface& gdi,
			Dvtx::VertexLayout layout,
			const VertexShader& vs );
		void Bind( GraphicsDeviceInterface& gdi ) noexcept override;
		const Dvtx::VertexLayout GetLayout() const noexcept;
		static std::shared_ptr<InputLayout> Resolve( GraphicsDeviceInterface& gdi,
			const Dvtx::VertexLayout& layout, const VertexShader& vs );
		static std::string GenerateUID( const Dvtx::VertexLayout& layout, const VertexShader& vs );
		std::string GetUID() const noexcept override;
	protected:
		std::string vertexShaderUID;
		Dvtx::VertexLayout m_VertexLayout;
		ID3D11InputLayout* m_pInputLayout;
	};
}