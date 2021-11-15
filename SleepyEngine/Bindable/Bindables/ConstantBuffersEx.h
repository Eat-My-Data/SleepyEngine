#pragma once
#include "../Bindable.h"
#include "./DynamicConstant.h"

namespace Bind
{
	class PixelConstantBufferEX : public Bindable
	{
	public:
		void Update( GraphicsDeviceInterface& gfx, const Dcb::Buffer& buf )
		{
			assert( &buf.GetRootLayoutElement() == &GetRootLayoutElement() );

			D3D11_MAPPED_SUBRESOURCE msr;
			GetContext( gfx )->Map(
				(ID3D11Resource*)pConstantBuffer, 0u,
				D3D11_MAP_WRITE_DISCARD, 0u,
				&msr
			);
			memcpy( msr.pData, buf.GetData(), buf.GetSizeInBytes() );
			GetContext( gfx )->Unmap( (ID3D11Resource*)pConstantBuffer, 0u );
		}
		void Bind( GraphicsDeviceInterface& gfx ) noexcept override
		{
			GetContext( gfx )->PSSetConstantBuffers( slot, 1u, &pConstantBuffer );
		}
		virtual const Dcb::LayoutElement& GetRootLayoutElement() const noexcept = 0;
	protected:
		PixelConstantBufferEX( GraphicsDeviceInterface& gfx, const Dcb::LayoutElement& layoutRoot, UINT slot, const Dcb::Buffer* pBuf )
			:
			slot( slot )
		{
			D3D11_BUFFER_DESC cbd;
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = 0u;
			cbd.ByteWidth = (UINT)layoutRoot.GetSizeInBytes();
			cbd.StructureByteStride = 0u;

			if ( pBuf != nullptr )
			{
				D3D11_SUBRESOURCE_DATA csd = {};
				csd.pSysMem = pBuf->GetData();
				GetDevice( gfx )->CreateBuffer( &cbd, &csd, &pConstantBuffer );
			}
			else
			{
				GetDevice( gfx )->CreateBuffer( &cbd, nullptr, &pConstantBuffer );
			}
		}
	private:
		ID3D11Buffer* pConstantBuffer;
		UINT slot;
	};

	class CachingPixelConstantBufferEX : public PixelConstantBufferEX
	{
	public:
		CachingPixelConstantBufferEX( GraphicsDeviceInterface& gfx, const Dcb::CookedLayout& layout, UINT slot )
			:
			PixelConstantBufferEX( gfx, *layout.ShareRoot(), slot, nullptr ),
			buf( Dcb::Buffer( layout ) )
		{}
		CachingPixelConstantBufferEX( GraphicsDeviceInterface& gfx, const Dcb::Buffer& buf, UINT slot )
			:
			PixelConstantBufferEX( gfx, buf.GetRootLayoutElement(), slot, &buf ),
			buf( buf )
		{}
		const Dcb::LayoutElement& GetRootLayoutElement() const noexcept override
		{
			return buf.GetRootLayoutElement();
		}
		const Dcb::Buffer& GetBuffer() const noexcept
		{
			return buf;
		}
		void SetBuffer( const Dcb::Buffer& buf_in )
		{
			buf.CopyFrom( buf_in );
			dirty = true;
		}
		void Bind( GraphicsDeviceInterface& gfx ) noexcept override
		{
			if ( dirty )
			{
				Update( gfx, buf );
				dirty = false;
			}
			PixelConstantBufferEX::Bind( gfx );
		}
		void Accept( TechniqueProbe& probe ) override
		{
			if ( probe.VisitBuffer( buf ) )
			{
				dirty = true;
			}
		}
	private:
		bool dirty = false;
		Dcb::Buffer buf;
	};

	class NocachePixelConstantBufferEX : public PixelConstantBufferEX
	{
	public:
		NocachePixelConstantBufferEX( GraphicsDeviceInterface& gfx, const Dcb::CookedLayout& layout, UINT slot )
			:
			PixelConstantBufferEX( gfx, *layout.ShareRoot(), slot, nullptr ),
			pLayoutRoot( layout.ShareRoot() )
		{}
		NocachePixelConstantBufferEX( GraphicsDeviceInterface& gfx, const Dcb::Buffer& buf, UINT slot )
			:
			PixelConstantBufferEX( gfx, buf.GetRootLayoutElement(), slot, &buf ),
			pLayoutRoot( buf.ShareLayoutRoot() )
		{}
		const Dcb::LayoutElement& GetRootLayoutElement() const noexcept override
		{
			return *pLayoutRoot;
		}
	private:
		std::shared_ptr<Dcb::LayoutElement> pLayoutRoot;
	};
}