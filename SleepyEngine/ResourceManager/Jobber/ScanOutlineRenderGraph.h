#pragma once
#include "RenderGraph.h"

class GraphicsDeviceInterface;

namespace Rgph
{
	class ScanOutlineRenderGraph : public RenderGraph
	{
	public:
		ScanOutlineRenderGraph( GraphicsDeviceInterface& gfx );
	};
}