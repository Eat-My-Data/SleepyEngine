#pragma once
#include <array>
#include "../../Bindable/BindableCommon.h"
#include "../../GraphicsDeviceInterface/GraphicsDeviceInterface.h"
#include "Job.h"
#include "Pass.h"

class FrameCommander
{
public:
	void Accept( Job job, size_t target ) noexcept
	{
		passes[target].Accept( job );
	}
	void Execute( GraphicsDeviceInterface& gfx ) const noexcept
	{
		using namespace Bind;
		// normally this would be a loop with each pass defining it setup / etc.
		// and later on it would be a complex graph with parallel execution contingent
		// on input / output requirements

		// main phong lighting pass
		//Stencil::Resolve( gfx, Stencil::Mode::Off )->Bind( gfx );
		passes[0].Execute( gfx );
		// outline masking pass
		//Stencil::Resolve( gfx, Stencil::Mode::Write )->Bind( gfx );
		//NullPixelShader::Resolve( gfx )->Bind( gfx );
		passes[1].Execute( gfx );
		// outline drawing pass
		//Stencil::Resolve( gfx, Stencil::Mode::Mask )->Bind( gfx );
		//struct SolidColorBuffer
		//{
		//	DirectX::XMFLOAT4 color = { 1.0f,0.4f,0.4f,1.0f };
		//} scb;
		//PixelConstantBuffer<SolidColorBuffer>::Resolve( gfx, scb, 1u )->Bind( gfx );
		passes[2].Execute( gfx );
	}
	void Reset() noexcept
	{
		for ( auto& p : passes )
		{
			p.Reset();
		}
	}
private:
	std::array<Pass, 3> passes;
};