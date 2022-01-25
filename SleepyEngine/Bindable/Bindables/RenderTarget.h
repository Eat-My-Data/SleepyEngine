#pragma once
#include "../Bindable.h"
#include "BufferResource.h"
#include <array>

class GraphicsDeviceInterface;

namespace Bind
{
	class DepthStencil;

	class RenderTarget : public Bindable, public BufferResource
	{
	public:
		void BindAsBuffer( GraphicsDeviceInterface& gfx ) noexcept override;
		void BindAsBuffer( GraphicsDeviceInterface& gfx, BufferResource* depthStencil ) noexcept override;
		void BindAsBuffer( GraphicsDeviceInterface& gfx, DepthStencil* depthStencil ) noexcept;
		void Clear( GraphicsDeviceInterface& gfx ) noexcept override;
		void Clear( GraphicsDeviceInterface& gfx, const std::array<float, 4>& color ) noexcept;
		UINT GetWidth() const noexcept;
		UINT GetHeight() const noexcept;
	private:
		void BindAsBuffer( GraphicsDeviceInterface& gfx, ID3D11DepthStencilView* pDepthStencilView ) noexcept;
	protected:
		RenderTarget( GraphicsDeviceInterface& gfx, ID3D11Texture2D* pTexture );
		RenderTarget( GraphicsDeviceInterface& gfx, UINT width, UINT height );
		UINT width;
		UINT height;
		ID3D11RenderTargetView* pTargetView;
	};

	class ShaderInputRenderTarget : public RenderTarget
	{
	public:
		ShaderInputRenderTarget( GraphicsDeviceInterface& gfx, UINT width, UINT height, UINT slot );
		void Bind( GraphicsDeviceInterface& gfx ) noexcept override;
	private:
		UINT slot;
		ID3D11ShaderResourceView* pShaderResourceView;
	};

	// RT for Graphics to create RenderTarget for the back buffer
	class OutputOnlyRenderTarget : public RenderTarget
	{
		friend GraphicsDeviceInterface;
	public:
		void Bind( GraphicsDeviceInterface& gfx ) noexcept override;
	private:
		OutputOnlyRenderTarget( GraphicsDeviceInterface& gfx, ID3D11Texture2D* pTexture );
	};
}