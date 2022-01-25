#pragma once
#include <vector>
#include <memory>
#include "../../Bindable/Bindable.h"
#include "../../GraphicsDeviceInterface/GraphicsDeviceInterface.h"

class TechniqueProbe;
class Drawable;
class RenderQueuePass;
class RenderGraph;

class Step
{
public:
	Step( std::string targetPassName );
	Step( Step&& ) = default;
	Step( const Step& src ) noexcept;
	Step& operator=( const Step& ) = delete;
	Step& operator=( Step&& ) = delete;
	void AddBindable( std::shared_ptr<Bind::Bindable> bind_in ) noexcept;
	void Submit( const Drawable& drawable ) const;
	void Bind( GraphicsDeviceInterface& gfx ) const;
	void InitializeParentReferences( const Drawable& parent ) noexcept;
	void Accept( TechniqueProbe& probe );
	void Link( RenderGraph& rg );
private:
	size_t targetPass;
	std::vector<std::shared_ptr<Bind::Bindable>> bindables;
	RenderQueuePass* pTargetPass;
	std::string targetPassName;
};