#pragma once
#include "RenderGraph.h"
#include <memory>
#include "../../Bindable/Bindables/ConstantBuffersEx.h"

class Graphics;
class Camera;

namespace Bind
{
	class Bindable;
	class RenderTarget;
	class ShadowSampler;
	class ShadowRasterizer;
}

namespace Rgph
{
	class DeferredRenderGraph : public RenderGraph
	{
	public:
		DeferredRenderGraph( Graphics& gfx );
		void RenderWindows( Graphics& gfx );
		void DumpShadowMap( Graphics& gfx, const std::string& path );
		void BindMainCamera( Camera& cam );
		void BindPLShadowCamera( Camera& cam );
		void BindSLShadowCamera( Camera& cam );
		void BindDLShadowCamera( Camera& cam );
		void StoreDepth( Graphics& gfx, const std::string& path );
	private:
		void RenderKernelWindow( Graphics& gfx );
		void RenderShadowWindow( Graphics& gfx );
		// private functions
		void SetKernelGauss( int radius, float sigma ) noxnd;
		void SetKernelBox( int radius ) noxnd;
		// private data
		enum class KernelType
		{
			Gauss,
			Box,
		} kernelType = KernelType::Gauss;
		static constexpr int maxRadius = 7;
		int radius = 4;
		float sigma = 2.0f;
		std::shared_ptr<Bind::CachingPixelConstantBufferEx> blurKernel;
		std::shared_ptr<Bind::CachingPixelConstantBufferEx> blurDirection;
	};
	// TODO:
	//	- Figure out if I can use constant buffer data from forward shaders in my deferred shaders
	//	- Maybe can modify values I store depending on what's in the shader but keep calculations to a minumum?
}