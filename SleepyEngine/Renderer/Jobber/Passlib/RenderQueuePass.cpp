#include "RenderQueuePass.h"

namespace Rgph
{
	void RenderQueuePass::Accept( Job job ) noexcept
	{
		// failed here now
		jobs.push_back( job );
	}

	void RenderQueuePass::Execute( Graphics& gfx ) const noxnd
	{
		BindAll( gfx );

		for ( const auto& j : jobs )
		{
			j.Execute( gfx );
		}
	}

	void RenderQueuePass::Reset() noexcept
	{
		jobs.clear();
	}
}