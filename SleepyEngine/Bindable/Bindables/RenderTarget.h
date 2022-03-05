#pragma once
#include "../Bindable.h"
#include "BufferResource.h"
#include <array>
#include <optional>

class Graphics;
class Surface;

namespace Bind
{
	class DepthStencil;

	class RenderTarget : public Bindable, public BufferResource
	{
	public:
		void BindAsBuffer( Graphics& gfx ) noexcept override;
		void BindAsBuffer( Graphics& gfx, BufferResource* depthStencil ) noexcept override;
		void BindAsBuffer( Graphics& gfx, DepthStencil* depthStencil ) noexcept;
		void BindAsBuffer( Graphics& gfx, ID3D11DepthStencilView* pDepthStencilView ) noexcept;
		void Clear( Graphics& gfx ) noexcept override;
		void Clear( Graphics& gfx, const std::array<float, 4>& color ) noexcept;
		UINT GetWidth() const noexcept;
		UINT GetHeight() const noexcept;
		Surface ToSurface( Graphics& gfx ) const;
		void Dumpy( Graphics& gfx, const std::string& path ) const;
	private:
		std::pair<Microsoft::WRL::ComPtr<ID3D11Texture2D>, D3D11_TEXTURE2D_DESC> MakeStaging( Graphics& gfx ) const;
		//void BindAsBuffer( Graphics& gfx, ID3D11DepthStencilView* pDepthStencilView ) noexcept;
	protected:
		RenderTarget( Graphics& gfx, ID3D11Texture2D* pTexture, std::optional<UINT> face );
		RenderTarget( Graphics& gfx, UINT width, UINT height );
		UINT width;
		UINT height;
		ID3D11RenderTargetView* pTargetView;
	};

	class ShaderInputRenderTarget : public RenderTarget
	{
	public:
		ShaderInputRenderTarget( Graphics& gfx, UINT width, UINT height, UINT slot );
		void Bind( Graphics& gfx ) noexcept override;
	private:
		UINT slot;
		ID3D11ShaderResourceView* pShaderResourceView;
	};

	class OutputOnlyRenderTarget : public RenderTarget
	{
	public:
		void Bind( Graphics& gfx ) noexcept override;
		OutputOnlyRenderTarget( Graphics& gfx, ID3D11Texture2D* pTexture, std::optional<UINT> face = {} );
	};

	class GBufferRenderTargets : public RenderTarget
	{
	public:
		GBufferRenderTargets( Graphics& gfx, UINT width, UINT height, UINT slot );
		void Bind( Graphics& gfx ) noexcept override;
		void BindAsBuffer( Graphics& gfx, ID3D11DepthStencilView* pDepthStencilView ) noexcept;
		void Clear( Graphics& gfx, const std::array<float, 4>& color ) noexcept;
		void Clear( Graphics& gfx ) noexcept;
	private:
		UINT slot;
		static const int bufferCount = 3;
		ID3D11RenderTargetView* m_pGBuffers[bufferCount];
		ID3D11Texture2D* m_pTextures[bufferCount];
		ID3D11ShaderResourceView* m_pShaderResources[bufferCount];
	};
}