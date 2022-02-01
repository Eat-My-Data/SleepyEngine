#include "SceneManager.h"
#include "../GraphicsDeviceInterface/GraphicsDeviceInterface.h"
#include "../Libraries/imgui/backends/imgui_impl_dx11.h"
#include "../Libraries/imgui/backends/imgui_impl_win32.h"
#include "../Utilities/Testing.h"
#include "../ResourceManager/Material.h"
#include "../ResourceManager/Mesh.h"
#include "../Bindable/Bindables/DynamicConstant.h"
#include "../ResourceManager/Jobber/ModelProbe.h"
#include "../ResourceManager/Node.h"
#include "../Utilities/SleepyXM.h"
#include <assimp/Importer.hpp>
#include "../ResourceManager/Jobber/TechniqueProbe.h"
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../ResourceManager/Jobber/Passlib/BufferClearPass.h"
#include "../ResourceManager/Jobber/Passlib/LambertianPass.h"
#include "../ResourceManager/Jobber/Passlib/OutlineDrawingPass.h"
#include "../ResourceManager/Jobber/Passlib/OutlineMaskGenerationPass.h"

SceneManager::~SceneManager()
{
	for ( u32 i = 0; i < m_vecOfModels.size(); i++ )
	{
		delete m_vecOfModels[i];
	}
}

void SceneManager::Initialize( GraphicsDeviceInterface& gdi, GraphicsAPI api )
{
	m_pGDI = &gdi;
	rg = new RenderGraph( *m_pGDI );
	m_GraphicsAPI = api;
	m_pTestCube = new Cube( *m_pGDI, { { 4.0f,0.0f,0.0f }, 0.0f, 0.0f, 0.0f } );
	m_pTestCube2 = new Cube( *m_pGDI, { { 0.0f,4.0f,0.0f }, 0.0f, 0.0f, 0.0f } );
	sponza = new Model( *m_pGDI, "Models\\Sponza\\sponza.obj", 1.0f / 20.0f );

	{
		{
			auto pass = std::make_unique<BufferClearPass>( "clear" );
			pass->SetInputSource( "renderTarget", "$.backbuffer" );
			pass->SetInputSource( "depthStencil", "$.masterDepth" );
			rg->AppendPass( std::move( pass ) );
		}
		{
			auto pass = std::make_unique<LambertianPass>( *m_pGDI, "lambertian" );
			pass->SetInputSource( "renderTarget", "clear.renderTarget" );
			pass->SetInputSource( "depthStencil", "clear.depthStencil" );
			rg->AppendPass( std::move( pass ) );
		}
		{
			auto pass = std::make_unique<OutlineMaskGenerationPass>( *m_pGDI, "outlineMask" );
			pass->SetInputSource( "depthStencil", "lambertian.depthStencil" );
			rg->AppendPass( std::move( pass ) );
		}
		{
			auto pass = std::make_unique<OutlineDrawingPass>( *m_pGDI, "outlineDraw" );
			pass->SetInputSource( "renderTarget", "lambertian.renderTarget" );
			pass->SetInputSource( "depthStencil", "outlineMask.depthStencil" );
			rg->AppendPass( std::move( pass ) );
		}
		rg->SetSinkTarget( "backbuffer", "outlineDraw.renderTarget" );		rg->Finalize();

		m_pTestCube->LinkTechniques( *rg );
		m_pTestCube2->LinkTechniques( *rg );
		//m_LightManager->LinkTechniques( rg );
		sponza->LinkTechniques( *rg );
	}
	m_pCameraBuffer = new Bind::PixelConstantBuffer<CameraData>{ gdi, 6u };
	m_LightManager.Initialize( *m_pGDI );
	ImGui_ImplDX11_Init( m_pGDI->GetDevice(), m_pGDI->GetContext() );
}

bool SceneManager::IsInitialzed() noexcept
{
	return m_GraphicsAPI != GraphicsAPI::Uninitialized;
}

void SceneManager::SetRenderTechnique( RenderTechnique renderTechnique ) noexcept
{
	m_RenderTechnique = renderTechnique;
}

void SceneManager::ToggleImGuiEngabled() noexcept
{
	imguiEnabled = !imguiEnabled;
}

void SceneManager::Draw()
{
	namespace dx = DirectX;

	PrepareFrame();

	m_pGDI->SetViewMatrix( m_Camera.GetViewMatrix() );
	m_pGDI->SetProjMatrix( m_Camera.GetProjectionMatrix() );

	m_LightManager.Submit();
	m_pTestCube->Submit();
	m_pTestCube2->Submit();
	sponza->Submit();
	rg->Execute( *m_pGDI );


	//UpdateCameraBuffer();
	//m_LightManager.UpdateBuffers();

	if ( imguiEnabled )
	{
		// Mesh techniques window
		class TP : public TechniqueProbe
		{
		public:
			void OnSetTechnique() override
			{
				using namespace std::string_literals;
				ImGui::TextColored( { 0.4f,1.0f,0.6f,1.0f }, pTech->GetName().c_str() );
				bool active = pTech->IsActive();
				ImGui::Checkbox( ( "Tech Active##"s + std::to_string( techIdx ) ).c_str(), &active );
				pTech->SetActiveState( active );
			}
			bool OnVisitBuffer( Dcb::Buffer& buf ) override
			{
				namespace dx = DirectX;
				float dirty = false;
				const auto dcheck = [&dirty]( bool changed ) {dirty = dirty || changed; };
				auto tag = [tagScratch = std::string{}, tagString = "##" + std::to_string( bufIdx )]
				( const char* label ) mutable
				{
					tagScratch = label + tagString;
					return tagScratch.c_str();
				};

				if ( auto v = buf["scale"]; v.Exists() )
				{
					dcheck( ImGui::SliderFloat( tag( "Scale" ), &v, 1.0f, 2.0f, "%.3f", 3.5f ) );
				}
				if ( auto v = buf["offset"]; v.Exists() )
				{
					dcheck( ImGui::SliderFloat( tag( "offset" ), &v, 0.0f, 1.0f, "%.3f", 2.5f ) );
				}
				if ( auto v = buf["materialColor"]; v.Exists() )
				{
					dcheck( ImGui::ColorPicker3( tag( "Color" ), reinterpret_cast<float*>( &static_cast<dx::XMFLOAT3&>( v ) ) ) );
				}
				if ( auto v = buf["specularColor"]; v.Exists() )
				{
					dcheck( ImGui::ColorPicker3( tag( "Spec. Color" ), reinterpret_cast<float*>( &static_cast<dx::XMFLOAT3&>( v ) ) ) );
				}
				if ( auto v = buf["specularGloss"]; v.Exists() )
				{
					dcheck( ImGui::SliderFloat( tag( "Glossiness" ), &v, 1.0f, 100.0f, "%.1f", 1.5f ) );
				}
				if ( auto v = buf["specularWeight"]; v.Exists() )
				{
					dcheck( ImGui::SliderFloat( tag( "Spec. Weight" ), &v, 0.0f, 2.0f ) );
				}
				if ( auto v = buf["useSpecularMap"]; v.Exists() )
				{
					dcheck( ImGui::Checkbox( tag( "Spec. Map Enable" ), &v ) );
				}
				if ( auto v = buf["useNormalMap"]; v.Exists() )
				{
					dcheck( ImGui::Checkbox( tag( "Normal Map Enable" ), &v ) );
				}
				if ( auto v = buf["normalMapWeight"]; v.Exists() )
				{
					dcheck( ImGui::SliderFloat( tag( "Normal Map Weight" ), &v, 0.0f, 2.0f ) );
				}
				return dirty;
			}
		};

		class MP : ModelProbe
		{
		public:
			void SpawnWindow( Model& model )
			{
				ImGui::Begin( "Model" );
				ImGui::Columns( 2, nullptr, true );
				model.Accept( *this );

				ImGui::NextColumn();
				if ( pSelectedNode != nullptr )
				{
					bool dirty = false;
					const auto dcheck = [&dirty]( bool changed ) {dirty = dirty || changed; };
					auto& tf = ResolveTransform();
					ImGui::TextColored( { 0.4f,1.0f,0.6f,1.0f }, "Translation" );
					dcheck( ImGui::SliderFloat( "X", &tf.x, -60.f, 60.f ) );
					dcheck( ImGui::SliderFloat( "Y", &tf.y, -60.f, 60.f ) );
					dcheck( ImGui::SliderFloat( "Z", &tf.z, -60.f, 60.f ) );
					ImGui::TextColored( { 0.4f,1.0f,0.6f,1.0f }, "Orientation" );
					dcheck( ImGui::SliderAngle( "X-rotation", &tf.xRot, -180.0f, 180.0f ) );
					dcheck( ImGui::SliderAngle( "Y-rotation", &tf.yRot, -180.0f, 180.0f ) );
					dcheck( ImGui::SliderAngle( "Z-rotation", &tf.zRot, -180.0f, 180.0f ) );
					if ( dirty )
					{
						pSelectedNode->SetAppliedTransform(
							dx::XMMatrixRotationX( tf.xRot ) *
							dx::XMMatrixRotationY( tf.yRot ) *
							dx::XMMatrixRotationZ( tf.zRot ) *
							dx::XMMatrixTranslation( tf.x, tf.y, tf.z )
						);
					}

					TP probe;
					pSelectedNode->Accept( probe );
				}
				ImGui::End();
			}
		protected:
			bool PushNode( Node& node ) override
			{
				// if there is no selected node, set selectedId to an impossible value
				const int selectedId = ( pSelectedNode == nullptr ) ? -1 : pSelectedNode->GetId();
				// build up flags for current node
				const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
					| ( ( node.GetId() == selectedId ) ? ImGuiTreeNodeFlags_Selected : 0 )
					| ( node.HasChildren() ? 0 : ImGuiTreeNodeFlags_Leaf );
				// render this node
				const auto expanded = ImGui::TreeNodeEx(
					(void*)(intptr_t)node.GetId(),
					node_flags, node.GetName().c_str()
				);
				// processing for selecting node
				if ( ImGui::IsItemClicked() )
				{
					pSelectedNode = &node;
				}
				// signal if children should also be recursed
				return expanded;
			}
			void PopNode( Node& node ) override
			{
				ImGui::TreePop();
			}
		private:
			Node* pSelectedNode = nullptr;
			struct TransformParameters
			{
				float xRot = 0.0f;
				float yRot = 0.0f;
				float zRot = 0.0f;
				float x = 0.0f;
				float y = 0.0f;
				float z = 0.0f;
			};
			std::unordered_map<int, TransformParameters> transformParams;
		private:
			TransformParameters& ResolveTransform() noexcept
			{
				const auto id = pSelectedNode->GetId();
				auto i = transformParams.find( id );
				if ( i == transformParams.end() )
				{
					return LoadTransform( id );
				}
				return i->second;
			}
			TransformParameters& LoadTransform( int id ) noexcept
			{
				const auto& applied = pSelectedNode->GetAppliedTransform();
				const auto angles = ExtractEulerAngles( applied );
				const auto translation = ExtractTranslation( applied );
				TransformParameters tp;
				tp.zRot = angles.z;
				tp.xRot = angles.x;
				tp.yRot = angles.y;
				tp.x = translation.x;
				tp.y = translation.y;
				tp.z = translation.z;
				return transformParams.insert( { id,{ tp } } ).first->second;
			}
		};
		static MP modelProbe;

		// imgui windows
		DrawControlPanel();
		modelProbe.SpawnWindow( *sponza );
	}

	rg->Reset();

	// clear shader resources
	ID3D11ShaderResourceView* null[12] = {};
	m_pGDI->GetContext()->PSSetShaderResources( 0, 12, null );
}

void SceneManager::DrawControlPanel()
{
	if ( imguiEnabled && ImGui::Begin( "Control Panel" ) )
	{
		ImGui::Text( "Camera" );
		ImGui::SliderFloat( "Camera X", &m_Camera.GetPosition().x, -80.0f, 80.0f );
		ImGui::SliderFloat( "Camera Y", &m_Camera.GetPosition().y, -80.0f, 80.0f );
		ImGui::SliderFloat( "Camera Z", &m_Camera.GetPosition().z, -80.0f, 80.0f );
		ImGui::Text( "Camera Orientation" );
		ImGui::SliderAngle( "Camera Pitch", &m_Camera.m_fPitch, 0.995f * -90.0f, 0.995f * 90.0f );
		ImGui::SliderAngle( "Camera Yaw", &m_Camera.m_fYaw, -180.0f, 180.0f );
		if ( ImGui::Button( "Toggle Render Technique" ) )
			m_RenderTechnique == RenderTechnique::Deferred ? SetRenderTechnique( RenderTechnique::Forward ) : SetRenderTechnique( RenderTechnique::Deferred );
		ImGui::SameLine();
		ImGui::Text( m_RenderTechnique == RenderTechnique::Deferred ? "Deferred" : "Forward" );
	}
	ImGui::End();

	m_LightManager.DrawControlPanel();
}

void SceneManager::Present()
{
	if ( imguiEnabled )
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData() );
		ImGui::EndFrame();
	}

	m_pGDI->GetSwap()->Present( 1u, 0u );
}

void SceneManager::RotateCamera( const f32 dx, const f32 dy )
{
	m_Camera.Rotate( dx, dy );
}

void SceneManager::TranslateCamera( DirectX::XMFLOAT3 camDelta )
{
	m_Camera.Translate( camDelta );
}

void SceneManager::SetActivePointLight( const u32 index )
{
	m_LightManager.SelectPointLight( index );
}

void SceneManager::TranslatePointLight( DirectX::XMFLOAT3 translation )
{
	m_LightManager.TranslatePointLight( translation );
}

void SceneManager::TranslateDirectionalLight( DirectX::XMFLOAT3 translation )
{
	m_LightManager.TranslateDirectionalLight( translation );
}

void SceneManager::RotateDirectionalLight( const f32 dx, const f32 dy )
{
	m_LightManager.RotateDirectionalLight( dx, dy );
}

void SceneManager::SetActiveSpotLight( const u32 index )
{
	m_LightManager.SelectSpotLight( index );
}

void SceneManager::TranslateSpotLight( DirectX::XMFLOAT3 translation )
{
	m_LightManager.TranslateSpotLight( translation );
}

void SceneManager::RotateSpotLight( const f32 dx, const f32 dy )
{
	m_LightManager.RotateSpotLight( dx, dy );
}

void SceneManager::PrepareFrame()
{
	// imgui begin frame
	if ( imguiEnabled )
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}
	// clearing shader inputs to prevent simultaneous in/out bind carried over from prev frame
	ID3D11ShaderResourceView* const pNullTex = nullptr;
	m_pGDI->GetContext()->PSSetShaderResources( 0, 1, &pNullTex ); // fullscreen input texture
	m_pGDI->GetContext()->PSSetShaderResources( 3, 1, &pNullTex ); // shadow map texture
}

void SceneManager::UpdateCameraBuffer()
{
	// get camera matrix from view matrix
	DirectX::XMVECTOR determinant = DirectX::XMMatrixDeterminant( m_Camera.GetViewMatrix() );
	DirectX::XMMATRIX cameraMatrix = DirectX::XMMatrixInverse( &determinant, m_Camera.GetViewMatrix() );

	// get inverse of the projection matrix
	DirectX::XMVECTOR determinant2 = DirectX::XMMatrixDeterminant( m_Camera.GetProjectionMatrix() );
	DirectX::XMMATRIX projInvMatrix = DirectX::XMMatrixInverse( &determinant2, m_Camera.GetProjectionMatrix() );

	m_CameraCBufferaData.camPos = DirectX::XMFLOAT4( m_Camera.GetPosition().x, m_Camera.GetPosition().y, m_Camera.GetPosition().z, 1.0f);
	m_CameraCBufferaData.viewInvMatrix = cameraMatrix;
	m_CameraCBufferaData.projInvMatrix = projInvMatrix;

	m_pCameraBuffer->Update( *m_pGDI, m_CameraCBufferaData );
	m_pCameraBuffer->Bind( *m_pGDI );
}