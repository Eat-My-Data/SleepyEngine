#include "Drawable.h"
#include "../Bindable/BindableCommon.h"
#include "../Renderer/Model/Material.h"
#include <cassert>

using namespace Bind;

void Drawable::Submit( size_t channelFilter ) const noexcept
{
	for ( const auto& tech : techniques )
	{
		if ( tech.IsActive() )
			tech.Submit( *this, channelFilter );
	}
}

Drawable::Drawable( Graphics& gfx, const Material& mat, const aiMesh& mesh, float scale ) noexcept
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

void Drawable::Bind( Graphics& gfx ) const noexcept
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

void Drawable::ToggleRenderTechnique( Graphics& gfx, const std::string& renderTechnique )
{
	for ( auto technique : techniques )
	{
		if ( renderTechnique == "deferred" && technique.GetName() == "deferredLighting" )
		{
			technique.SetActiveState( true );
		}
		else if ( renderTechnique != "deferred" && technique.GetName() == "deferredLighting" )
		{
			technique.SetActiveState( false );
		}
		else
		{
			technique.ToggleRenderTechnique( gfx, renderTechnique );
		}
	}
}

Drawable::~Drawable()
{}