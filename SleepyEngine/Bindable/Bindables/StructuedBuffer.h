#pragma once
#include "../Bindable.h"
#include "../BindableCodex.h"

namespace Bind
{
	template<typename C>
	class StructuredBuffer : public Bindable
	{
	public:
		void Update( GraphicsDeviceInterface& gdi, const C& consts )
		{
			D3D11_MAPPED_SUBRESOURCE msr;
			GetContext( gdi )->Map(
				m_pConstantBuffer, 0u,
				D3D11_MAP_WRITE_DISCARD, 0u,
				&msr
			);
			memcpy( msr.pData, &consts, sizeof( consts ) );
			GetContext( gdi )->Unmap( m_pConstantBuffer, 0u );
		}
		StructuredBuffer( GraphicsDeviceInterface& gdi, const C& consts, UINT slot = 0u )
			:
			m_iSlot( slot )
		{
			D3D11_BUFFER_DESC cbd;
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = 0u;
			cbd.ByteWidth = sizeof( C );
			cbd.StructureByteStride = 0u;

			D3D11_SUBRESOURCE_DATA csd = {};
			csd.pSysMem = &consts;
			GetDevice( gdi )->CreateBuffer( &cbd, &csd, &m_pConstantBuffer );
		}
		StructuredBuffer( GraphicsDeviceInterface& gdi, UINT slot = 0u )
			:
			m_iSlot( slot )
		{
			D3D11_BUFFER_DESC cbd;
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = 0u;
			cbd.ByteWidth = sizeof( C );
			cbd.StructureByteStride = 0u;
			GetDevice( gdi )->CreateBuffer( &cbd, nullptr, &m_pConstantBuffer );
		}
	protected:
		ID3D11Buffer* m_pConstantBuffer;
		UINT m_iSlot;
	};
	template<typename C>
	class PixelStructuredBuffer: public ConstantBuffer<C>
	{
		using StructuredBuffer<C>::m_pStructuredBuffer;
		using StructuredBuffer<C>::m_iSlot;
		using Bindable::GetContext;
	public:
		using StructuredBuffer<C>::StructuredBuffer;
		void Bind( GraphicsDeviceInterface& gdi ) noexcept override
		{
			GetContext( gdi )->PSSetStructuredBuffers( m_iSlot, 1u, &m_pStructuredBuffer );
		}
		static std::shared_ptr<PixelStructuredBuffer> Resolve( GraphicsDeviceInterface& gdi, const C& consts, UINT slot = 0 )
		{
			return Codex::Resolve<PixelStructuredBuffer>( gdi, consts, slot );
		}
		static std::shared_ptr<PixelStructuredBuffer> Resolve( GraphicsDeviceInterface& gdi, UINT slot = 0 )
		{
			return Codex::Resolve<PixelStructuredBuffer>( gdi, slot );
		}
		static std::string GenerateUID( const C&, UINT slot )
		{
			return GenerateUID( slot );
		}
		static std::string GenerateUID( UINT slot = 0 )
		{
			return typeid( PixelStructuredBuffer ).name();
		}
		std::string GetUID() const noexcept override
		{
			return GenerateUID( m_iSlot );
		}
	};
}