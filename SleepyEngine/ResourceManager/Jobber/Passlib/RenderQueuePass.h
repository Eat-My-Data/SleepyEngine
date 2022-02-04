#pragma once
#include "BindingPass.h"
#include "../Job.h"
#include <vector>

namespace Rgph
{
	class RenderQueuePass : public BindingPass
	{
	public:
		using BindingPass::BindingPass;
		void Accept( Job job ) noexcept;
		void Execute( GraphicsDeviceInterface& gfx ) const noexcept override;
		void Reset() noexcept override;
	private:
		std::vector<Job> jobs;
	};
}