#include "Vertex.h"

namespace Dvtx
{
	// VertexLayout
	const VertexLayout::Element& VertexLayout::ResolveByIndex( size_t i ) const noexcept
	{
		return m_vecOfElements[i];
	}
	VertexLayout& VertexLayout::Append( ElementType type ) noexcept
	{
		m_vecOfElements.emplace_back( type, Size() );
		return *this;
	}
	size_t VertexLayout::Size() const noexcept
	{
		return m_vecOfElements.empty() ? 0u : m_vecOfElements.back().GetOffsetAfter();
	}
	size_t VertexLayout::GetElementCount() const noexcept
	{
		return m_vecOfElements.size();
	}
	std::vector<D3D11_INPUT_ELEMENT_DESC> VertexLayout::GetD3DLayout() const noexcept
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> desc;
		desc.reserve( GetElementCount() );
		for ( const auto& e : m_vecOfElements )
		{
			desc.push_back( e.GetDesc() );
		}
		return desc;
	}
	std::string VertexLayout::GetCode() const noexcept
	{
		std::string code;
		for ( const auto& e : m_vecOfElements )
		{
			code += e.GetCode();
		}
		return code;
	}


	// VertexLayout::Element
	VertexLayout::Element::Element( ElementType type, size_t offset )
		:
		m_ElementType( type ),
		m_iOffset( offset )
	{}
	size_t VertexLayout::Element::GetOffsetAfter() const noexcept
	{
		return m_iOffset + Size();
	}
	size_t VertexLayout::Element::GetOffset() const
	{
		return m_iOffset;
	}
	size_t VertexLayout::Element::Size() const noexcept
	{
		return SizeOf( m_ElementType );
	}
	constexpr size_t VertexLayout::Element::SizeOf( ElementType type ) noexcept
	{
		switch ( type )
		{
		case Position2D:
			return sizeof( Map<Position2D>::SysType );
		case Position3D:
			return sizeof( Map<Position3D>::SysType );
		case Texture2D:
			return sizeof( Map<Texture2D>::SysType );
		case Normal:
			return sizeof( Map<Normal>::SysType );
		case Tangent:
			return sizeof( Map<Tangent>::SysType );
		case Bitangent:
			return sizeof( Map<Bitangent>::SysType );
		case Float3Color:
			return sizeof( Map<Float3Color>::SysType );
		case Float4Color:
			return sizeof( Map<Float4Color>::SysType );
		case BGRAColor:
			return sizeof( Map<BGRAColor>::SysType );
		}
		assert( "Invalid element type" && false );
		return 0u;
	}
	VertexLayout::ElementType VertexLayout::Element::GetType() const noexcept
	{
		return m_ElementType;
	}
	const char* Dvtx::VertexLayout::Element::GetCode() const noexcept
	{
		switch ( m_ElementType )
		{
		case Position2D:
			return Map<Position2D>::code;
		case Position3D:
			return Map<Position3D>::code;
		case Texture2D:
			return Map<Texture2D>::code;
		case Normal:
			return Map<Normal>::code;
		case Tangent:
			return Map<Tangent>::code;
		case Bitangent:
			return Map<Bitangent>::code;
		case Float3Color:
			return Map<Float3Color>::code;
		case Float4Color:
			return Map<Float4Color>::code;
		case BGRAColor:
			return Map<BGRAColor>::code;
		}
		assert( "Invalid element type" && false );
		return "Invalid";
	}
	D3D11_INPUT_ELEMENT_DESC VertexLayout::Element::GetDesc() const noexcept
	{
		switch ( m_ElementType )
		{
		case Position2D:
			return GenerateDesc<Position2D>( GetOffset() );
		case Position3D:
			return GenerateDesc<Position3D>( GetOffset() );
		case Texture2D:
			return GenerateDesc<Texture2D>( GetOffset() );
		case Normal:
			return GenerateDesc<Normal>( GetOffset() );
		case Tangent:
			return GenerateDesc<Tangent>( GetOffset() );
		case Bitangent:
			return GenerateDesc<Bitangent>( GetOffset() );
		case Float3Color:
			return GenerateDesc<Float3Color>( GetOffset() );
		case Float4Color:
			return GenerateDesc<Float4Color>( GetOffset() );
		case BGRAColor:
			return GenerateDesc<BGRAColor>( GetOffset() );
		}
		assert( "Invalid element type" && false );
		return { "INVALID",0,DXGI_FORMAT_UNKNOWN,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 };
	}


	// Vertex
	Vertex::Vertex( char* pData, const VertexLayout& layout ) noexcept
		:
		m_pData( pData ),
		m_VertexLayout( layout )
	{
		assert( m_pData != nullptr );
	}
	ConstVertex::ConstVertex( const Vertex& v ) noexcept
		:
	vertex( v )
	{}


	// VertexBuffer
	VertexBuffer::VertexBuffer( VertexLayout layout, size_t size ) noexcept
		:
		m_VertexLayout( std::move( layout ) )
	{
		Resize( size );
	}
	void VertexBuffer::Resize( size_t newSize ) noexcept
	{
		const auto size = Size();
		if ( size < newSize )
		{
			m_vecOfChar.resize( m_vecOfChar.size() + m_VertexLayout.Size() * ( newSize - size ) );
		}
	}
	const char* VertexBuffer::GetData() const noexcept
	{
		return m_vecOfChar.data();
	}
	const VertexLayout& VertexBuffer::GetLayout() const noexcept
	{
		return m_VertexLayout;
	}
	size_t VertexBuffer::Size() const noexcept
	{
		return m_vecOfChar.size() /m_VertexLayout.Size();
	}
	size_t VertexBuffer::SizeBytes() const noexcept
	{
		return m_vecOfChar.size();
	}
	Vertex VertexBuffer::Back() noexcept
	{
		assert( m_vecOfChar.size() != 0u );
		return Vertex{ m_vecOfChar.data() + m_vecOfChar.size() - m_VertexLayout.Size(), m_VertexLayout };
	}
	Vertex VertexBuffer::Front() noexcept
	{
		assert( m_vecOfChar.size() != 0u );
		return Vertex{ m_vecOfChar.data(), m_VertexLayout };
	}
	Vertex VertexBuffer::operator[]( size_t i ) noexcept
	{
		assert( i < Size() );
		return Vertex{ m_vecOfChar.data() + m_VertexLayout.Size() * i,m_VertexLayout };
	}
	ConstVertex VertexBuffer::Back() const noexcept
	{
		return const_cast<VertexBuffer*>( this )->Back();
	}
	ConstVertex VertexBuffer::Front() const noexcept
	{
		return const_cast<VertexBuffer*>( this )->Front();
	}
	ConstVertex VertexBuffer::operator[]( size_t i ) const noexcept
	{
		return const_cast<VertexBuffer&>( *this )[i];
	}
}