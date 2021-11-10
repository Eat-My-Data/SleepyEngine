#pragma once

class Job
{
public:
	Job( const class Step* pStep, const class Drawable* pDrawable );
	void Execute( class GraphicsDeviceInterface& gfx ) const noexcept;
private:
	const class Drawable* pDrawable;
	const class Step* pStep;
};