#pragma once
#include "../Pass.h"
#include "../PassInput.h"
#include "../PassOutput.h"

class BufferClearPass : public Pass
{
public:
	BufferClearPass( std::string name );
	void Execute( GraphicsDeviceInterface& gfx ) const noexcept override;
};