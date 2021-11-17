#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "ModelWindow.h"
#include "Node.h"
#include "Mesh.h"

namespace dx = DirectX;

Model::Model( GraphicsDeviceInterface& gfx, const std::string& pathString, const float scale )
	:
	pWindow( std::make_unique<ModelWindow>() )
{
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile( pathString.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);

	if ( pScene == nullptr )
	{
		throw std::exception();
	}

	for ( size_t i = 0; i < pScene->mNumMeshes; i++ )
	{
		meshPtrs.push_back( ParseMesh( gfx, *pScene->mMeshes[i], pScene->mMaterials, pathString, scale ) );
	}

	int nextId = 0;
	pRoot = ParseNode( nextId, *pScene->mRootNode );
}

void Model::Submit( FrameCommander& frame ) const noexcept
{
	// I'm still not happy about updating parameters (i.e. mutating a bindable GPU state
	// which is part of a mesh which is part of a node which is part of the model that is
	// const in this call) Can probably do this elsewhere
	pWindow->ApplyParameters();
	pRoot->Submit( frame, dx::XMMatrixIdentity() );
}

void Model::ShowWindow( GraphicsDeviceInterface& gfx, const char* windowName ) noexcept
{
	pWindow->Show( gfx, windowName, *pRoot );
}

void Model::SetRootTransform( DirectX::FXMMATRIX tf ) noexcept
{
	pRoot->SetAppliedTransform( tf );
}

Model::~Model() noexcept
{}

std::unique_ptr<Mesh> Model::ParseMesh( GraphicsDeviceInterface& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials, const std::filesystem::path& path, float scale )
{
	return {};
}

std::unique_ptr<Node> Model::ParseNode( int& nextId, const aiNode& node ) noexcept
{
	namespace dx = DirectX;
	const auto transform = dx::XMMatrixTranspose( dx::XMLoadFloat4x4(
		reinterpret_cast<const dx::XMFLOAT4X4*>( &node.mTransformation )
	) );

	std::vector<Mesh*> curMeshPtrs;
	curMeshPtrs.reserve( node.mNumMeshes );
	for ( size_t i = 0; i < node.mNumMeshes; i++ )
	{
		const auto meshIdx = node.mMeshes[i];
		curMeshPtrs.push_back( meshPtrs.at( meshIdx ).get() );
	}

	auto pNode = std::make_unique<Node>( nextId++, node.mName.C_Str(), std::move( curMeshPtrs ), transform );
	for ( size_t i = 0; i < node.mNumChildren; i++ )
	{
		pNode->AddChild( ParseNode( nextId, *node.mChildren[i] ) );
	}

	return pNode;
}
