#pragma once
#include "ConstantBuffers.h"
#include "Drawable.h"
#include <DirectXMath.h>

namespace Bind
{
	class TransformCbuf : public Bindable
	{
	protected:
		struct Transforms
		{
			DirectX::XMMATRIX model;
			DirectX::XMMATRIX modelView;
			DirectX::XMMATRIX modelViewProj;
		};
	public:
		TransformCbuf( GraphicsDeviceInterface& gfx, const Drawable& parent, UINT slot = 0u );
		void Bind( GraphicsDeviceInterface& gfx ) noexcept override;
	protected:
		void UpdateBindImpl( GraphicsDeviceInterface& gfx, const Transforms& tf ) noexcept;
		Transforms GetTransforms( GraphicsDeviceInterface& gfx ) noexcept;
	private:
		static std::unique_ptr<VertexConstantBuffer<Transforms>> pVcbuf;
		const Drawable& parent;
	};
}
