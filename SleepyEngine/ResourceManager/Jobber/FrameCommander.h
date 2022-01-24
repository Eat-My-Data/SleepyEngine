#pragma once
#include <array>
#include <optional>
#include "Pass.h"
#include "../../Bindable/Bindables/RenderTarget.h"
#include "../../Bindable/Bindables/DepthStencil.h"
#include "../../Bindable/BindableCommon.h"
#include "../../GraphicsDeviceInterface/GraphicsDeviceInterface.h"
#include "Job.h"
#include "BlurPack.h"

namespace Bind
{
	class VertexBuffer;
	class IndexBuffer;
	class VertexShader;
	class InputLayout;
	class Sampler;
	class Sampler;
	class Blender;
}

class FrameCommander
{
public:
	FrameCommander( GraphicsDeviceInterface& gfx );
	void Accept( Job job, size_t target ) noexcept;
	void Execute( GraphicsDeviceInterface& gfx ) noexcept;
	void Reset() noexcept;
	void ShowWindows( GraphicsDeviceInterface& gfx );
private:
	std::array<Pass, 3> passes;
	int downFactor = 1;
	DepthStencil ds;
	std::optional<RenderTarget> rt1;
	std::optional<RenderTarget> rt2;
	BlurPack blur;
	std::shared_ptr<Bind::VertexBuffer> pVbFull;
	std::shared_ptr<Bind::IndexBuffer> pIbFull;
	std::shared_ptr<Bind::VertexShader> pVsFull;
	std::shared_ptr<Bind::InputLayout> pLayoutFull;
	std::shared_ptr<Bind::Sampler> pSamplerFullPoint;
	std::shared_ptr<Bind::Sampler> pSamplerFullBilin;
	std::shared_ptr<Bind::Blender> pBlenderMerge;
};