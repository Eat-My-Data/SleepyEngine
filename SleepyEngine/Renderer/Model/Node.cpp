#include "Node.h"
#include "Mesh.h"
#include "../Jobber/ModelProbe.h"
#include "../../Libraries/imgui/imgui.h"

namespace dx = DirectX;

Node::Node( int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform_in ) noexcept
	:
	id( id ),
	meshPtrs( std::move( meshPtrs ) ),
	name( name )
{
	dx::XMStoreFloat4x4( &transform, transform_in );
	dx::XMStoreFloat4x4( &appliedTransform, dx::XMMatrixIdentity() );
}

void Node::Submit( size_t channels, DirectX::FXMMATRIX accumulatedTransform ) const noexcept
{
	const auto built =
		dx::XMLoadFloat4x4( &appliedTransform ) *
		dx::XMLoadFloat4x4( &transform ) *
		accumulatedTransform;
	for ( const auto pm : meshPtrs )
	{
		pm->Submit( channels, built );
	}
	for ( const auto& pc : childPtrs )
	{
		pc->Submit( channels, built );
	}
}

void Node::AddChild( std::unique_ptr<Node> pChild ) noexcept
{
	assert( pChild );
	childPtrs.push_back( std::move( pChild ) );
}

//void Node::ShowTree( Node*& pSelectedNode ) const noexcept
//{
//	// if there is no selected node, set selectedId to an impossible value
//	const int selectedId = ( pSelectedNode == nullptr ) ? -1 : pSelectedNode->GetId();
//	// build up flags for current node
//	const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
//		| ( ( GetId() == selectedId ) ? ImGuiTreeNodeFlags_Selected : 0 )
//		| ( ( childPtrs.size() == 0 ) ? ImGuiTreeNodeFlags_Leaf : 0 );
//	// render this node
//	const auto expanded = ImGui::TreeNodeEx(
//		(void*)(intptr_t)GetId(), node_flags, name.c_str()
//	);
//	// processing for selecting node
//	if ( ImGui::IsItemClicked() )
//	{
//		pSelectedNode = const_cast<Node*>( this );
//	}
//	// recursive rendering of open node's children
//	if ( expanded )
//	{
//		for ( const auto& pChild : childPtrs )
//		{
//			pChild->ShowTree( pSelectedNode );
//		}
//		ImGui::TreePop();
//	}
//}

void Node::Accept( ModelProbe& probe )
{
	if ( probe.PushNode( *this ) )
	{
		for ( auto& cp : childPtrs )
		{
			cp->Accept( probe );
		}
		probe.PopNode( *this );
	}
}

void Node::Accept( TechniqueProbe& probe )
{
	for ( auto& mp : meshPtrs )
	{
		mp->Accept( probe );
	}
}

void Node::SetAppliedTransform( DirectX::FXMMATRIX transform ) noexcept
{
	dx::XMStoreFloat4x4( &appliedTransform, transform );
}

const DirectX::XMFLOAT4X4& Node::GetAppliedTransform() const noexcept
{
	return appliedTransform;
}

int Node::GetId() const noexcept
{
	return id;
}