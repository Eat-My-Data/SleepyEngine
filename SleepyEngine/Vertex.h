#pragma once
#include <vector>
#include <type_traits>
#include "./GraphicsDeviceInterface/GraphicsDeviceInterface.h"
#include "Color.h"
#include <string>

namespace Dvtx
{
	class VertexLayout
	{
	public:
		enum ElementType
		{
			Position2D,
			Position3D,
			Texture2D,
			Normal,
			Tangent,
			Bitangent,
			Float3Color,
			Float4Color,
			BGRAColor,
			Count,
		};
		template<ElementType> struct Map;
		template<> struct Map<Position2D>
		{
			using SysType = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic = "Position";
			static constexpr const char* code = "P2";
		};
		template<> struct Map<Position3D>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Position";
			static constexpr const char* code = "P3";
		};
		template<> struct Map<Texture2D>
		{
			using SysType = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic = "Texcoord";
			static constexpr const char* code = "T2";
		};
		template<> struct Map<Normal>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Normal";
			static constexpr const char* code = "N";
		};
		template<> struct Map<Tangent>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Tangent";
			static constexpr const char* code = "Nt";
		};
		template<> struct Map<Bitangent>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Bitangent";
			static constexpr const char* code = "Nb";
		};
		template<> struct Map<Float3Color>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "C3";
		};
		template<> struct Map<Float4Color>
		{
			using SysType = DirectX::XMFLOAT4;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "C4";
		};
		template<> struct Map<BGRAColor>
		{
			using SysType = ::BGRAColor;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "C8";
		};

		class Element
		{
		public:
			Element( ElementType type, size_t offset );
			size_t GetOffsetAfter() const noexcept;
			size_t GetOffset() const;
			size_t Size() const noexcept;
			static constexpr size_t SizeOf( ElementType type ) noexcept;
			ElementType GetType() const noexcept;
			D3D11_INPUT_ELEMENT_DESC GetDesc() const noexcept;
			const char* GetCode() const noexcept;
		private:
			template<ElementType type>
			static constexpr D3D11_INPUT_ELEMENT_DESC GenerateDesc( size_t offset ) noexcept
			{
				return { Map<type>::semantic,0,Map<type>::dxgiFormat,0,(UINT)offset,D3D11_INPUT_PER_VERTEX_DATA,0 };
			}
		private:
			ElementType m_ElementType;
			size_t m_iOffset;
		};
	public:
		template<ElementType Type>
		const Element& Resolve() const noexcept
		{
			for ( auto& e : m_vecOfElements )
			{
				if ( e.GetType() == Type )
				{
					return e;
				}
			}
			assert( "Could not resolve element type" && false );
			return m_vecOfElements.front();
		}
		const Element& ResolveByIndex( size_t i ) const noexcept;
		VertexLayout& Append( ElementType type ) noexcept;
		size_t Size() const noexcept;
		size_t GetElementCount() const noexcept;
		std::vector<D3D11_INPUT_ELEMENT_DESC> GetD3DLayout() const noexcept;
		std::string GetCode() const noexcept;
	private:
		std::vector<Element> m_vecOfElements;
	};

	class Vertex
	{
		friend class VertexBuffer;
	public:
		template<VertexLayout::ElementType Type>
		auto& Attr() noexcept
		{
			auto pAttribute = m_pData + m_VertexLayout.Resolve<Type>().GetOffset();
			return *reinterpret_cast<typename VertexLayout::Map<Type>::SysType*>( pAttribute );
		}
		template<typename T>
		void SetAttributeByIndex( size_t i, T&& val ) noexcept
		{
			const auto& element = m_VertexLayout.ResolveByIndex( i );
			auto pAttribute = m_pData + element.GetOffset();
			switch ( element.GetType() )
			{
			case VertexLayout::Position2D:
				SetAttribute<VertexLayout::Position2D>( pAttribute, std::forward<T>( val ) );
				break;
			case VertexLayout::Position3D:
				SetAttribute<VertexLayout::Position3D>( pAttribute, std::forward<T>( val ) );
				break;
			case VertexLayout::Texture2D:
				SetAttribute<VertexLayout::Texture2D>( pAttribute, std::forward<T>( val ) );
				break;
			case VertexLayout::Normal:
				SetAttribute<VertexLayout::Normal>( pAttribute, std::forward<T>( val ) );
				break;
			case VertexLayout::Tangent:
				SetAttribute<VertexLayout::Tangent>( pAttribute, std::forward<T>( val ) );
				break;
			case VertexLayout::Bitangent:
				SetAttribute<VertexLayout::Bitangent>( pAttribute, std::forward<T>( val ) );
				break;
			case VertexLayout::Float3Color:
				SetAttribute<VertexLayout::Float3Color>( pAttribute, std::forward<T>( val ) );
				break;
			case VertexLayout::Float4Color:
				SetAttribute<VertexLayout::Float4Color>( pAttribute, std::forward<T>( val ) );
				break;
			case VertexLayout::BGRAColor:
				SetAttribute<VertexLayout::BGRAColor>( pAttribute, std::forward<T>( val ) );
				break;
			default:
				assert( "Bad element type" && false );
			}
		}
	protected:
		Vertex( char* pData, const VertexLayout& layout ) noexcept;
	private:
		// enables parameter pack setting of multiple parameters by element index
		template<typename First, typename ...Rest>
		void SetAttributeByIndex( size_t i, First&& first, Rest&&... rest ) noexcept
		{
			SetAttributeByIndex( i, std::forward<First>( first ) );
			SetAttributeByIndex( i + 1, std::forward<Rest>( rest )... );
		}
		// helper to reduce code duplication in SetAttributeByIndex
		template<VertexLayout::ElementType DestLayoutType, typename SrcType>
		void SetAttribute( char* pAttribute, SrcType&& val ) noexcept
		{
			using Dest = typename VertexLayout::Map<DestLayoutType>::SysType;
			if constexpr ( std::is_assignable<Dest, SrcType>::value )
			{
				*reinterpret_cast<Dest*>( pAttribute ) = val;
			}
			else
			{
				assert( "Parameter attribute type mismatch" && false );
			}
		}
	private:
		char* m_pData = nullptr;
		const VertexLayout& m_VertexLayout;
	};

	class ConstVertex
	{
	public:
		ConstVertex( const Vertex& v ) noexcept;
		template<VertexLayout::ElementType Type>
		const auto& Attr() const noexcept
		{
			return const_cast<Vertex&>( vertex ).Attr<Type>();
		}
	private:
		Vertex vertex;
	};

	class VertexBuffer
	{
	public:
		VertexBuffer( VertexLayout layout, size_t size = 0u ) noexcept;
		const char* GetData() const noexcept;
		const VertexLayout& GetLayout() const noexcept;
		void Resize( size_t newSize ) noexcept;
		size_t Size() const noexcept;
		size_t SizeBytes() const noexcept;
		template<typename ...Params>
		void EmplaceBack( Params&&... params ) noexcept
		{
			assert( sizeof...( params ) == m_VertexLayout.GetElementCount() && "Param count doesn't match number of vertex elements" );
			m_vecOfChar.resize( m_vecOfChar.size() + m_VertexLayout.Size() );
			Back().SetAttributeByIndex( 0u, std::forward<Params>( params )... );
		}
		Vertex Back() noexcept;
		Vertex Front() noexcept;
		Vertex operator[]( size_t i ) noexcept;
		ConstVertex Back() const noexcept;
		ConstVertex Front() const noexcept;
		ConstVertex operator[]( size_t i ) const noexcept;
	private:
		std::vector<char> m_vecOfChar;
		VertexLayout m_VertexLayout;
	};
}