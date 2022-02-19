#pragma once
#include "../Bindable.h"
#include "BufferResource.h"

class GraphicsDeviceInterface;

namespace Bind
{
	class RenderTarget;

	class DepthStencil : public Bindable, public BufferResource
	{
		friend RenderTarget;
	public:
		void BindAsBuffer( GraphicsDeviceInterface& gfx ) noexcept override;
		void BindAsBuffer( GraphicsDeviceInterface& gfx, BufferResource* renderTarget ) noexcept override;
		void BindAsBuffer( GraphicsDeviceInterface& gfx, RenderTarget* rt ) noexcept;
		void Clear( GraphicsDeviceInterface& gfx ) noexcept override;
	protected:
		DepthStencil( GraphicsDeviceInterface& gfx, UINT width, UINT height, bool canBindShaderInput );
		ID3D11DepthStencilView* pDepthStencilView;
	};

	class ShaderInputDepthStencil : public DepthStencil
	{
	public:
		ShaderInputDepthStencil( GraphicsDeviceInterface& gfx, UINT slot );
		ShaderInputDepthStencil( GraphicsDeviceInterface& gfx, UINT width, UINT height, UINT slot );
		void Bind( GraphicsDeviceInterface& gfx ) noexcept override;
	private:
		UINT slot;
		ID3D11ShaderResourceView* pShaderResourceView;
	};

	class OutputOnlyDepthStencil : public DepthStencil
	{
	public:
		OutputOnlyDepthStencil( GraphicsDeviceInterface& gfx );
		OutputOnlyDepthStencil( GraphicsDeviceInterface& gfx, UINT width, UINT height );
		void Bind( GraphicsDeviceInterface& gfx ) noexcept override;
	};
}