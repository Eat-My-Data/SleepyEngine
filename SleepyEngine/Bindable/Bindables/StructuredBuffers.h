#pragma once
#include "../Bindable.h"
#include "../BindableCodex.h"

namespace Bind
{
	template<typename C>
	class StructuredBuffers : public Bindable
	{
	public:
		void Update( GraphicsDeviceInterface& gdi, const C& consts )
		{
			D3D11_MAPPED_SUBRESOURCE msr;
			GetContext( gdi )->Map(
				m_pStructuredBuffer, 0u,
				D3D11_MAP_WRITE_DISCARD, 0u,
				&msr
			);
			memcpy( msr.pData, &consts, sizeof( consts ) );
			GetContext( gdi )->Unmap( m_pStructuredBuffer, 0u );
		}
		StructuredBuffers( GraphicsDeviceInterface& gdi, const C& consts, UINT slot = 0u )
			:
			m_iSlot( slot )
		{
			D3D11_BUFFER_DESC cbd = {};
			cbd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			cbd.ByteWidth = sizeof( consts );
			cbd.StructureByteStride = sizeof( consts );

			D3D11_SUBRESOURCE_DATA csd = {};
			csd.pSysMem = &consts;
			GetDevice( gdi )->CreateBuffer( &cbd, &csd, &m_pStructuredBuffer );
		}
		StructuredBuffers( GraphicsDeviceInterface& gdi, UINT slot = 0u )
			:
			m_iSlot( slot )
		{
			D3D11_BUFFER_DESC cbd = {};
			cbd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			cbd.ByteWidth = sizeof( C );
			cbd.StructureByteStride = sizeof( C );
			GetDevice( gdi )->CreateBuffer( &cbd, nullptr, &m_pStructuredBuffer );
		}
	protected:
		ID3D11Buffer* m_pStructuredBuffer;
		UINT m_iSlot;
	};
	template<typename C>
	class PixelStructuredBuffer: public StructuredBuffers<C>
	{
		using StructuredBuffers<C>::m_pStructuredBuffer;
		using StructuredBuffers<C>::m_iSlot;
		using Bindable::GetContext;
	public:
		using StructuredBuffers<C>::StructuredBuffers;
		void Bind( GraphicsDeviceInterface& gdi ) noexcept override
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = DXGI_FORMAT_UNKNOWN;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			srvDesc.Buffer.FirstElement = 0;
			srvDesc.Buffer.NumElements = 1;

			ID3D11ShaderResourceView* pStructBufSRV;
			gdi.GetDevice()->CreateShaderResourceView( m_pStructuredBuffer, &srvDesc, &pStructBufSRV );
			GetContext( gdi )->PSSetShaderResources( m_iSlot, 1u, &pStructBufSRV );
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

	template<typename C>
	class PixelArrStructuredBuffer : public Bindable
	{
		using Bindable::GetContext;
	public:
		void Update( GraphicsDeviceInterface& gdi, const C* consts )
		{
			D3D11_MAPPED_SUBRESOURCE msr;
			GetContext( gdi )->Map(
				m_pStructuredBuffer, 0u,
				D3D11_MAP_WRITE_DISCARD, 0u,
				&msr
			);
			memcpy( msr.pData, consts, sizeof( C ) * 2 );
			GetContext( gdi )->Unmap( m_pStructuredBuffer, 0u );
		}
		PixelArrStructuredBuffer( GraphicsDeviceInterface& gdi, const C* consts, UINT slot = 0u )
			:
			m_iSlot( slot )
		{
			D3D11_BUFFER_DESC cbd = {};
			cbd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			cbd.ByteWidth = sizeof( C ) * 2;
			cbd.StructureByteStride = sizeof( C );

			D3D11_SUBRESOURCE_DATA csd = {};
			csd.pSysMem = consts;
			GetDevice( gdi )->CreateBuffer( &cbd, &csd, &m_pStructuredBuffer );
		}
		PixelArrStructuredBuffer( GraphicsDeviceInterface& gdi, UINT slot = 0u )
			:
			m_iSlot( slot )
		{
			D3D11_BUFFER_DESC cbd = {};
			cbd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			cbd.ByteWidth = sizeof( C ) * 2;
			cbd.StructureByteStride = sizeof( C );
			GetDevice( gdi )->CreateBuffer( &cbd, nullptr, &m_pStructuredBuffer );
		}
		void Bind( GraphicsDeviceInterface& gdi ) noexcept override
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = DXGI_FORMAT_UNKNOWN;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			srvDesc.Buffer.FirstElement = 0;
			srvDesc.Buffer.NumElements = 1;

			ID3D11ShaderResourceView* pStructBufSRV;
			gdi.GetDevice()->CreateShaderResourceView( m_pStructuredBuffer, &srvDesc, &pStructBufSRV );
			GetContext( gdi )->PSSetShaderResources( m_iSlot, 1u, &pStructBufSRV );
		}
		static std::shared_ptr<PixelArrStructuredBuffer> Resolve( GraphicsDeviceInterface& gdi, const C* consts, UINT slot = 0 )
		{
			return Codex::Resolve<PixelArrStructuredBuffer>( gdi, consts, slot );
		}
		static std::shared_ptr<PixelArrStructuredBuffer> Resolve( GraphicsDeviceInterface& gdi, UINT slot = 0 )
		{
			return Codex::Resolve<PixelArrStructuredBuffer>( gdi, slot );
		}
		static std::string GenerateUID( const C&, UINT slot )
		{
			return GenerateUID( slot );
		}
		static std::string GenerateUID( UINT slot = 0 )
		{
			return typeid( PixelArrStructuredBuffer ).name();
		}
		std::string GetUID() const noexcept override
		{
			return GenerateUID( m_iSlot );
		}
	protected:
		ID3D11Buffer* m_pStructuredBuffer;
		UINT m_iSlot;
	};
}