#pragma once
#include "../Bindable.h"
#include "BufferResource.h"
#include "../../Renderer/Surface.h"

class Graphics;

namespace Bind
{
	class RenderTarget;

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
		void BindAsBuffer( Graphics& gfx ) noexcept override;
		void BindAsBuffer( Graphics& gfx, BufferResource* renderTarget ) noexcept override;
		void BindAsBuffer( Graphics& gfx, RenderTarget* rt ) noexcept;
		void Clear( Graphics& gfx ) noexcept override;
		Surface ToSurface( Graphics& gfx, bool linearlize = true ) const;
		unsigned int GetWidth() const;
		unsigned int GetHeight() const;
	protected:
		DepthStencil( Graphics& gfx, UINT width, UINT height, bool canBindShaderInput, Usage usage );
		ID3D11DepthStencilView* pDepthStencilView;
		unsigned int width;
		unsigned int height;
	};

	class ShaderInputDepthStencil : public DepthStencil
	{
	public:
		ShaderInputDepthStencil( Graphics& gfx, UINT slot, Usage usage = Usage::DepthStencil );
		ShaderInputDepthStencil( Graphics& gfx, UINT width, UINT height, UINT slot, Usage usage = Usage::DepthStencil );
		void Bind( Graphics& gfx ) noexcept override;
	private:
		UINT slot;
		ID3D11ShaderResourceView* pShaderResourceView;
	};

	class OutputOnlyDepthStencil : public DepthStencil
	{
	public:
		OutputOnlyDepthStencil( Graphics& gfx );
		OutputOnlyDepthStencil( Graphics& gfx, UINT width, UINT height );
		void Bind( Graphics& gfx ) noexcept override;
	};
}