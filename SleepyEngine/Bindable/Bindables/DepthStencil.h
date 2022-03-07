#pragma once
#include "../Bindable.h"
#include "BufferResource.h"
#include "../../Renderer/Surface.h"

class Graphics;

namespace Bind
{
	class RenderTarget;
	class GBufferRenderTargets;

	class DepthStencil : public Bindable, public BufferResource
	{
		friend RenderTarget;
	public:
		enum class Usage
		{
			DepthStencil,
			ShadowDepth,
		};
	public:
		void BindAsBuffer( Graphics& gfx ) noxnd  override;
		void BindAsBuffer( Graphics& gfx, BufferResource* renderTarget ) noxnd  override;
		void BindAsBuffer( Graphics& gfx, RenderTarget* rt ) noxnd;
		void BindAsBuffer( Graphics& gfx, GBufferRenderTargets* rt ) noxnd;
		void Clear( Graphics& gfx ) noxnd  override;
		Surface ToSurface( Graphics& gfx, bool linearlize = true ) const;
		void Dumpy( Graphics& gfx, const std::string& path ) const;
		unsigned int GetWidth() const;
		unsigned int GetHeight() const;
	private:
		std::pair<Microsoft::WRL::ComPtr<ID3D11Texture2D>, D3D11_TEXTURE2D_DESC> MakeStaging( Graphics& gfx ) const;
	protected:
		DepthStencil( Graphics& gfx, Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture, UINT face );
		DepthStencil( Graphics& gfx, UINT width, UINT height, bool canBindShaderInput, Usage usage );
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView;
		unsigned int width;
		unsigned int height;
	};

	class ShaderInputDepthStencil : public DepthStencil
	{
	public:
		ShaderInputDepthStencil( Graphics& gfx, UINT slot, Usage usage = Usage::DepthStencil );
		ShaderInputDepthStencil( Graphics& gfx, UINT width, UINT height, UINT slot, Usage usage = Usage::DepthStencil );
		void Bind( Graphics& gfx ) noxnd  override;
		void Bind( Graphics& gfx, UINT slot ) noxnd;
	private:
		UINT slot;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView;
	};

	class OutputOnlyDepthStencil : public DepthStencil
	{
	public:
		OutputOnlyDepthStencil( Graphics& gfx, Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture, UINT face );
		OutputOnlyDepthStencil( Graphics& gfx );
		OutputOnlyDepthStencil( Graphics& gfx, UINT width, UINT height );
		void Bind( Graphics& gfx ) noxnd  override;
	};
}