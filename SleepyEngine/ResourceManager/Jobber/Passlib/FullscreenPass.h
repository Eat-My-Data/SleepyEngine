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

class FullscreenPass : public BindingPass
{
public:
	FullscreenPass( const std::string name,GraphicsDeviceInterface& gfx ) noexcept;
	void Execute( GraphicsDeviceInterface& gfx ) const noexcept override;
};
