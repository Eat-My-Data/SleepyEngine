#pragma once
#include "../../Bindable/BindableCommon.h"
#include "../../Utilities/SleepyMath.h"
#include "../../Libraries/imgui/imgui.h"

class BlurPack
{
public:
	BlurPack( GraphicsDeviceInterface& gfx, int radius = 5, float sigma = 2.6f )
		:
		shader( gfx, "./Shaders/Bin/Blur_PS.cso" ),
		pcb( gfx, 0u ),
		ccb( gfx, 1u ),
		radius( radius ),
		sigma( sigma ) 
	{
		SetKernel( gfx, radius, sigma );
	}
	void Bind( GraphicsDeviceInterface& gfx ) noexcept
	{
		shader.Bind( gfx );
		pcb.Bind( gfx );
		ccb.Bind( gfx );
	}
	void SetHorizontal( GraphicsDeviceInterface& gfx )
	{
		ccb.Update( gfx, { TRUE } );
	}
	void SetVertical( GraphicsDeviceInterface& gfx )
	{
		ccb.Update( gfx, { FALSE } );
	}
	void ShowWindow( GraphicsDeviceInterface& gfx )
	{
		ImGui::Begin( "Blur" );
		bool radChange = ImGui::SliderInt( "Radius", &radius, 0, 15 );
		bool sigChange = ImGui::SliderFloat( "Sigma", &sigma, 0.1f, 10.0f );
		if ( radChange || sigChange )
		{
			SetKernel( gfx, radius, sigma );
		}
		ImGui::End();
	}
	// nTaps should be 6sigma - 1
	// for more accurate coefs, need to integrate, but meh :/
	void SetKernel( GraphicsDeviceInterface& gfx, int radius, float sigma ) noexcept
	{
		assert( radius <= 15 );
		Kernel k;
		k.nTaps = radius * 2 + 1;
		float sum = 0.0f;
		for ( int i = 0; i < k.nTaps + 1; i++ )
		{
			const auto x = float( i - ( radius) );
			const auto g = gauss( x, sigma );
			sum += g;
			k.coefficients[i].x = g;
		}
		for ( int i = 0; i < k.nTaps; i++ )
		{
			k.coefficients[i].x /= sum;
		}
		// TODO: Figure out why this throws an error if you set radius to 15 but works 1-14
		pcb.Update( gfx, k );
	}
private:
	int radius;
	float sigma;
	struct Kernel
	{
		int nTaps;
		float padding[3];
		DirectX::XMFLOAT4 coefficients[31];
	};
	struct Control
	{
		BOOL horizontal;
		float padding[3];
	};
	Bind::PixelShader shader;
	Bind::PixelConstantBuffer<Kernel> pcb;
	Bind::PixelConstantBuffer<Control> ccb;
};