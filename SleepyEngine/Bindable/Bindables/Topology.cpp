#include "Topology.h"
#include "../BindableCodex.h"

namespace Bind
{
	Topology::Topology( GraphicsDeviceInterface& gdi, D3D11_PRIMITIVE_TOPOLOGY type )
		:
		m_TopologyType( type )
	{}
	void Topology::Bind( GraphicsDeviceInterface& gdi ) noexcept
	{
		gdi.GetContext()->IASetPrimitiveTopology( m_TopologyType );
	}
	std::shared_ptr<Topology> Topology::Resolve( GraphicsDeviceInterface& gdi, D3D11_PRIMITIVE_TOPOLOGY type )
	{
		return Codex::Resolve<Topology>( gdi, type );
	}
	std::string Topology::GenerateUID( D3D11_PRIMITIVE_TOPOLOGY type )
	{
		using namespace std::string_literals;
		return typeid( Topology ).name() + "#"s + std::to_string( type );
	}
	std::string Topology::GetUID() const noexcept
	{
		return GenerateUID( m_TopologyType );
	}
}
