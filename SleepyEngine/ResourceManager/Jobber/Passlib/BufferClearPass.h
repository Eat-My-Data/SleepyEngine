#pragma once
#include "../Pass.h"

namespace Bind
{
	class BufferResource;
}

namespace Rgph
{
	class BufferClearPass : public Pass
	{
	public:
		BufferClearPass( std::string name );
		void Execute( GraphicsDeviceInterface& gfx ) const noexcept override;
	private:
		std::shared_ptr<Bind::BufferResource> buffer;
	};
}