#include "InputLayout.h"
#include "../BindableCodex.h"
#include "../../ResourceManager/Vertex.h"
#include "VertexShader.h"

namespace Bind
{
	InputLayout::InputLayout( Graphics& gdi,
		Dvtx::VertexLayout layout_in,
		const VertexShader& vs )
		:
		m_VertexLayout( std::move( layout_in ) )
	{
		const auto d3dLayout = m_VertexLayout.GetD3DLayout();
		const auto pBytecode = vs.GetBytecode();

		GetDevice( gdi )->CreateInputLayout(
			d3dLayout.data(), (UINT)d3dLayout.size(),
			pBytecode->GetBufferPointer(),
			pBytecode->GetBufferSize(),
			&m_pInputLayout
		);
	}

	void InputLayout::Bind( Graphics& gdi ) noexcept
	{
		GetContext( gdi )->IASetInputLayout( m_pInputLayout );
	}
	const Dvtx::VertexLayout InputLayout::GetLayout() const noexcept
	{
		return m_VertexLayout;
	}
	std::shared_ptr<InputLayout> InputLayout::Resolve( Graphics& gdi,
		const Dvtx::VertexLayout& layout, const VertexShader& vs )
	{
		return Codex::Resolve<InputLayout>( gdi, layout, vs );
	}
	std::string InputLayout::GenerateUID( const Dvtx::VertexLayout& layout, const VertexShader& vs )
	{
		using namespace std::string_literals;
		return typeid( InputLayout ).name() + "#"s + layout.GetCode() + "#"s + vs.GetUID();
	}
	std::string InputLayout::GetUID() const noexcept
	{
		using namespace std::string_literals;
		return typeid( InputLayout ).name() + "#"s + m_VertexLayout.GetCode() + "#"s + vertexShaderUID;
	}
}