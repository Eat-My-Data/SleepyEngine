#pragma once
#include "Bindable.h"
#include "BindableCodex.h"

namespace Bind
{
	template<typename C>
	class ConstantBuffer : public Bindable
	{
	public:
		void Update( GraphicsDeviceInterface& gdi, const C& consts )
		{
			D3D11_MAPPED_SUBRESOURCE msr;
			GetContext( gdi )->Map(
				pConstantBuffer, 0u,
				D3D11_MAP_WRITE_DISCARD, 0u,
				&msr
			);
			memcpy( msr.pData, &consts, sizeof( consts ) );
			GetContext( gdi )->Unmap( pConstantBuffer, 0u );
		}
		ConstantBuffer( GraphicsDeviceInterface& gdi, const C& consts, UINT slot = 0u )
			:
			slot( slot )
		{
			D3D11_BUFFER_DESC cbd;
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = 0u;
			cbd.ByteWidth = sizeof( consts );
			cbd.StructureByteStride = 0u;

			D3D11_SUBRESOURCE_DATA csd = {};
			csd.pSysMem = &consts;
			GetDevice( gdi )->CreateBuffer( &cbd, &csd, &pConstantBuffer );
		}
		ConstantBuffer( GraphicsDeviceInterface& gdi, UINT slot = 0u )
			:
			slot( slot )
		{
			D3D11_BUFFER_DESC cbd;
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = 0u;
			cbd.ByteWidth = sizeof( C );
			cbd.StructureByteStride = 0u;
			GetDevice( gdi )->CreateBuffer( &cbd, nullptr, &pConstantBuffer );
		}
	protected:
		ID3D11Buffer* pConstantBuffer;
		UINT slot;
	};

	template<typename C>
	class VertexConstantBuffer : public ConstantBuffer<C>
	{
		using ConstantBuffer<C>::pConstantBuffer;
		using ConstantBuffer<C>::slot;
		using Bindable::GetContext;
	public:
		using ConstantBuffer<C>::ConstantBuffer;
		void Bind( GraphicsDeviceInterface& gdi ) noexcept override
		{
			GetContext( gdi )->VSSetConstantBuffers( slot, 1u, &pConstantBuffer );
		}
		static std::shared_ptr<VertexConstantBuffer> Resolve( GraphicsDeviceInterface& gdi, const C& consts, UINT slot = 0 )
		{
			return Codex::Resolve<VertexConstantBuffer>( gdi, consts, slot );
		}
		static std::shared_ptr<VertexConstantBuffer> Resolve( GraphicsDeviceInterface& gdi, UINT slot = 0 )
		{
			return Codex::Resolve<VertexConstantBuffer>( gdi, slot );
		}
		static std::string GenerateUID( const C&, UINT slot )
		{
			return GenerateUID( slot );
		}
		static std::string GenerateUID( UINT slot = 0 )
		{
			using namespace std::string_literals;
			return typeid( VertexConstantBuffer ).name() + "#"s + std::to_string( slot );
		}
		std::string GetUID() const noexcept override
		{
			return GenerateUID( slot );
		}
	};

	template<typename C>
	class PixelConstantBuffer : public ConstantBuffer<C>
	{
		using ConstantBuffer<C>::pConstantBuffer;
		using ConstantBuffer<C>::slot;
		using Bindable::GetContext;
	public:
		using ConstantBuffer<C>::ConstantBuffer;
		void Bind( GraphicsDeviceInterface& gdi ) noexcept override
		{
			GetContext( gdi )->PSSetConstantBuffers( slot, 1u, &pConstantBuffer );
		}
		static std::shared_ptr<PixelConstantBuffer> Resolve( GraphicsDeviceInterface& gdi, const C& consts, UINT slot = 0 )
		{
			return Codex::Resolve<PixelConstantBuffer>( gdi, consts, slot );
		}
		static std::shared_ptr<PixelConstantBuffer> Resolve( GraphicsDeviceInterface& gdi, UINT slot = 0 )
		{
			return Codex::Resolve<PixelConstantBuffer>( gdi, slot );
		}
		static std::string GenerateUID( const C&, UINT slot )
		{
			return GenerateUID( slot );
		}
		static std::string GenerateUID( UINT slot = 0 )
		{
			return typeid( PixelConstantBuffer ).name();
		}
		std::string GetUID() const noexcept override
		{
			return GenerateUID( slot );
		}
	};
}