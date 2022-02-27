#pragma once
#include "BindingPass.h"
#include <string>
#include <memory>

namespace Bind
{
	class IndexBuffer;
	class VertexBuffer;
	class VertexShader;
	class InputLayout;
}

namespace Rgph
{
	class FullscreenPass : public BindingPass
	{
	public:
		FullscreenPass( const std::string name, Graphics& gfx ) noexcept;
		void Execute( Graphics& gfx ) const noexcept override;
	};
}