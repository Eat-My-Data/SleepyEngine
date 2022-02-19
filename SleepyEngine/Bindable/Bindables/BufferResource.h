#pragma once

class GraphicsDeviceInterface;

namespace Bind
{
	class BufferResource
	{
	public:
		virtual ~BufferResource() = default;
		virtual void BindAsBuffer( GraphicsDeviceInterface& ) noexcept = 0;
		virtual void BindAsBuffer( GraphicsDeviceInterface&,BufferResource* ) noexcept = 0;
		virtual void Clear( GraphicsDeviceInterface& ) noexcept = 0;
	};
}