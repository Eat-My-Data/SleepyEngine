#include "VertexBuffer.h"
#include "../BindableCodex.h"

namespace Bind
{
	VertexBuffer::VertexBuffer( GraphicsDeviceInterface& gdi, const Dvtx::VertexBuffer& vbuf )
		:
		VertexBuffer( gdi, "?", vbuf )
	{}
	VertexBuffer::VertexBuffer( GraphicsDeviceInterface& gdi, const std::string& tag, const Dvtx::VertexBuffer& vbuf )
		:
		m_iStride( (UINT)vbuf.GetLayout().Size() ),
		m_sTag( tag )
	{
		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = UINT( vbuf.SizeBytes() );
		bd.StructureByteStride = m_iStride;
		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = vbuf.GetData();
		gdi.GetDevice()->CreateBuffer( &bd, &sd, &m_pVertexBuffer );
	}
	void VertexBuffer::Bind( GraphicsDeviceInterface& gdi ) noexcept
	{
		const UINT offset = 0u;
		GetContext( gdi )->IASetVertexBuffers( 0u, 1u, &m_pVertexBuffer, &m_iStride, &offset );
	}
	std::shared_ptr<VertexBuffer> VertexBuffer::Resolve( GraphicsDeviceInterface& gdi, const std::string& tag,
		const Dvtx::VertexBuffer& vbuf )
	{
		assert( tag != "?" );
		return Codex::Resolve<VertexBuffer>( gdi, tag, vbuf );
	}
	std::string VertexBuffer::GenerateUID_( const std::string& tag )
	{
		using namespace std::string_literals;
		return typeid( VertexBuffer ).name() + "#"s + tag;
	}
	std::string VertexBuffer::GetUID() const noexcept
	{
		return GenerateUID( m_sTag );
	}
}
