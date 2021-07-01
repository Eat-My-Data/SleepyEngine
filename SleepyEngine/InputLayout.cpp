#include "InputLayout.h"
#include "BindableCodex.h"
#include "Vertex.h"

namespace Bind
{
	InputLayout::InputLayout( GraphicsDeviceInterface& gdi,
		Dvtx::VertexLayout layout_in,
		ID3DBlob* pVertexShaderBytecode )
		:
		m_VertexLayout( std::move( layout_in ) )
	{
		const auto d3dLayout = m_VertexLayout.GetD3DLayout();

		GetDevice( gdi )->CreateInputLayout(
			d3dLayout.data(), (UINT)d3dLayout.size(),
			pVertexShaderBytecode->GetBufferPointer(),
			pVertexShaderBytecode->GetBufferSize(),
			&m_pInputLayout
		);
	}

	void InputLayout::Bind( GraphicsDeviceInterface& gdi ) noexcept
	{
		GetContext( gdi )->IASetInputLayout( m_pInputLayout );
	}
	std::shared_ptr<InputLayout> InputLayout::Resolve( GraphicsDeviceInterface& gdi,
		const Dvtx::VertexLayout& layout, ID3DBlob* pVertexShaderBytecode )
	{
		return Codex::Resolve<InputLayout>( gdi, layout, pVertexShaderBytecode );
	}
	std::string InputLayout::GenerateUID( const Dvtx::VertexLayout& layout, ID3DBlob* pVertexShaderBytecode )
	{
		using namespace std::string_literals;
		return typeid( InputLayout ).name() + "#"s + layout.GetCode();
	}
	std::string InputLayout::GetUID() const noexcept
	{
		return GenerateUID( m_VertexLayout );
	}
}