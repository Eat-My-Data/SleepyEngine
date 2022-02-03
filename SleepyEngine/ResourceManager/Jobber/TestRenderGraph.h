#pragma once
#include "RenderGraph.h"

class GraphicsDeviceInterface;

class TestRenderGraph : public RenderGraph
{
public:
	TestRenderGraph( GraphicsDeviceInterface& gfx );
};