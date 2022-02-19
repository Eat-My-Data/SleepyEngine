#include "TransformCbufScaling.h"
#include "../../ResourceManager/Jobber/TechniqueProbe.h"

namespace dx = DirectX;

Bind::TransformCbufScaling::TransformCbufScaling( GraphicsDeviceInterface& gfx, float scale )
	:
	TransformCbuf( gfx ),
	buf( MakeLayout() )
{
	buf["scale"] = scale;
}

void Bind::TransformCbufScaling::Accept( TechniqueProbe& probe )
{
	probe.VisitBuffer( buf );
}

void Bind::TransformCbufScaling::Bind( GraphicsDeviceInterface& gfx ) noexcept
{
	const float scale = buf["scale"];
	const auto scaleMatrix = dx::XMMatrixScaling( scale, scale, scale );
	auto xf = GetTransforms( gfx );
	xf.m_ModelViewMatrix = xf.m_ModelViewMatrix * scaleMatrix;
	xf.m_ModelViewProjMatrix = xf.m_ModelViewProjMatrix * scaleMatrix;
	UpdateBindImpl( gfx, xf );
}

std::unique_ptr<Bind::CloningBindable> Bind::TransformCbufScaling::Clone() const noexcept
{
	return std::make_unique<TransformCbufScaling>( *this );
}


Dcb::RawLayout Bind::TransformCbufScaling::MakeLayout()
{
	Dcb::RawLayout layout;
	layout.Add<Dcb::Float>( "scale" );
	return layout;
}