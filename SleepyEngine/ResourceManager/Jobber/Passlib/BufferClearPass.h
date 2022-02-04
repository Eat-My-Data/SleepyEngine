#pragma once
#include "../Pass.h"

namespace Rgph
{
	class BufferClearPass : public Pass
	{
	public:
		BufferClearPass( std::string name );
		void Execute( GraphicsDeviceInterface& gfx ) const noexcept override;
	};
}