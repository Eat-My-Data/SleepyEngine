#include "Job.h"
#include "Step.h"
#include "../../Drawable/Drawable.h"

namespace Rgph
{
	Job::Job( const Step* pStep, const Drawable* pDrawable )
		:
		pDrawable{ pDrawable },
		pStep{ pStep }
	{}

	void Job::Execute( GraphicsDeviceInterface& gfx ) const noexcept
	{
		pDrawable->Bind( gfx );
		pStep->Bind( gfx );
		gfx.DrawIndexed( pDrawable->GetIndexCount() );
	}
}