#pragma once
#include "../../GraphicsDeviceInterface/GraphicsDeviceInterface.h"
#include "Job.h"
#include <vector>

class Pass
{
public:
	void Accept( Job job ) noexcept;
	void Execute( GraphicsDeviceInterface& gfx ) const noexcept;
	void Reset() noexcept;
private:
	std::vector<Job> jobs;
};