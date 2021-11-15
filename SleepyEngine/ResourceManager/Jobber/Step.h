#pragma once
#include <vector>
#include <memory>
#include "../../Bindable/Bindable.h"
#include "../../GraphicsDeviceInterface/GraphicsDeviceInterface.h"
#include "TechniqueProbe.h"

class Step
{
public:
	Step( size_t targetPass_in )
		:
		targetPass{ targetPass_in }
	{}
	void AddBindable( std::shared_ptr<Bind::Bindable> bind_in ) noexcept
	{
		bindables.push_back( std::move( bind_in ) );
	}
	void Submit( class FrameCommander& frame, const class Drawable& drawable ) const;
	void Bind( GraphicsDeviceInterface& gfx ) const
	{
		for ( const auto& b : bindables )
		{
			b->Bind( gfx );
		}
	}
	void InitializeParentReferences( const class Drawable& parent ) noexcept;
	void Accept( TechniqueProbe& probe )
	{
		probe.SetStep( this );
		for ( auto& pb : bindables )
		{
			pb->Accept( probe );
		}
	}
private:
	size_t targetPass;
	std::vector<std::shared_ptr<Bind::Bindable>> bindables;
};