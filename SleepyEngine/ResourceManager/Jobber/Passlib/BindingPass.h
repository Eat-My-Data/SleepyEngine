#pragma once
#include "../Pass.h"

namespace Bind
{
	class Bindable;
}

class BindingPass : public Pass
{
protected:
	BindingPass( std::string name,std::vector<std::shared_ptr<Bind::Bindable>> binds = {} );
	void AddBind( std::shared_ptr<Bind::Bindable> bind ) noexcept;
	void BindAll( GraphicsDeviceInterface& gfx ) const noexcept;
	void Finalize() override;
private:
	std::vector<std::shared_ptr<Bind::Bindable>> binds;
};