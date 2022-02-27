#pragma once
#include "RenderGraph.h"

class Graphics;

namespace Rgph
{
	class ScanOutlineRenderGraph : public RenderGraph
	{
	public:
		ScanOutlineRenderGraph( Graphics& gfx );
	};
}