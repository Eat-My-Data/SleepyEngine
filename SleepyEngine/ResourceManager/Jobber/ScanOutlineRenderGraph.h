#pragma once
#include "RenderGraph.h"

class GraphicsDeviceInterface;

class ScanOutlineRenderGraph : public RenderGraph
{
public:
	ScanOutlineRenderGraph( GraphicsDeviceInterface& gfx );
};