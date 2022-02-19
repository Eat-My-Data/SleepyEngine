#include "Drawable.h"
#include "../Bindable/BindableCommon.h"
#include "../ResourceManager/Material.h"
#include <cassert>

using namespace Bind;

void Drawable::Submit() const noexcept
{
	for ( const auto& tech : techniques )
	{
		tech.Submit( *this );
	}
}

Drawable::Drawable( GraphicsDeviceInterface& gfx, const Material& mat, const aiMesh& mesh, float scale ) noexcept
{
	pVertices = mat.MakeVertexBindable( gfx, mesh, scale );
	pIndices = mat.MakeIndexBindable( gfx, mesh );
	pTopology = Bind::Topology::Resolve( gfx );

	for ( auto& t : mat.GetTechniques() )
	{
		AddTechnique(  t  );
	}
}

void Drawable::AddTechnique( Technique tech_in ) noexcept
{
	tech_in.InitializeParentReferences( *this );
	techniques.push_back( std::move( tech_in ) );
}

void Drawable::Bind( GraphicsDeviceInterface& gfx ) const noexcept
{
	pTopology->Bind( gfx );
	pIndices->Bind( gfx );
	pVertices->Bind( gfx );
}

void Drawable::Accept( TechniqueProbe& probe )
{
	for ( auto& t : techniques )
	{
		t.Accept( probe );
	}
}

UINT Drawable::GetIndexCount() const noexcept
{
	return pIndices->GetCount();
}

void Drawable::LinkTechniques( Rgph::RenderGraph& rg )
{
	for ( auto& tech : techniques )
	{
		tech.Link( rg );
	}
}

Drawable::~Drawable()
{}