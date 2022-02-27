#pragma once
#include "RenderQueuePass.h"
#include "../Job.h"
#include <vector>
#include "../Sink.h"
#include "../Source.h"

#include "../../../Geometry/Cube.h"
#include "../../../Geometry/Sphere.h"

#include "../../../Bindable/Bindables/Stencil.h"
#include "../../../Camera/Camera.h"
#include "../../../Bindable/Bindables/DepthStencil.h"
#include "../../../Bindable/Bindables/ShadowCameraCBuf.h"
#include "../../../Bindable/Bindables/Sampler.h"
#include "../../../Bindable/Bindables/Rasterizer.h"
#include "../../../Bindable/Bindables/DepthStencil.h"
#include "../../../Bindable/Bindables/CubeTexture.h"
#include "../../../Bindable/Bindables/SkyboxTransformCbuf.h"
#include "../../../Bindable/Bindables/VertexBuffer.h"
#include "../../../Bindable/Bindables/IndexBuffer.h"
#include "../../../Bindable/Bindables/Topology.h"
#include "../../../Bindable/Bindables/InputLayout.h"

#include "../../../Libraries/imgui/imgui.h"

class Graphics;

namespace Rgph
{
	class SkyboxPass : public BindingPass
	{
	public:
		SkyboxPass( Graphics& gfx, std::string name )
			:
			BindingPass( std::move( name ) )
		{
			using namespace Bind;
			RegisterSink( DirectBufferSink<RenderTarget>::Make( "renderTarget", renderTarget ) );
			RegisterSink( DirectBufferSink<DepthStencil>::Make( "depthStencil", depthStencil ) );
			AddBind( std::make_shared<CubeTexture>( gfx, "..\\Images\\SpaceBox" ) );
			AddBind( Stencil::Resolve( gfx, Stencil::Mode::DepthFirst ) );
			AddBind( Sampler::Resolve( gfx, Sampler::Type::Bilinear ) );
			AddBind( Rasterizer::Resolve( gfx, true ) );
			AddBind( std::make_shared<SkyboxTransformCbuf>( gfx ) );
			AddBind( PixelShader::Resolve( gfx, "./Shaders/Bin/Skybox_PS.cso" ) );
			AddBind( Topology::Resolve( gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
			{ // geometry-related
				auto pvs = Bind::VertexShader::Resolve( gfx, "./Shaders/Bin/Skybox_VS.cso" );
				{ // cube
					auto model = Cube::Make();
					const auto geometryTag = "$cube_map";
					pCubeVertices = VertexBuffer::Resolve( gfx, geometryTag, std::move( model.m_VBVertices ) );
					pCubeIndices = IndexBuffer::Resolve( gfx, geometryTag, std::move( model.m_vecOfIndices ) );
					cubeCount = (UINT)model.m_vecOfIndices.size();
					// layout is shared between cube and sphere; use cube data to generate
					AddBind( InputLayout::Resolve( gfx, model.m_VBVertices.GetLayout(), *pvs ) );
				}
				{ // sphere
					auto model = Sphere::Make();
					const auto geometryTag = "$sphere_map";
					pSphereVertices = VertexBuffer::Resolve( gfx, geometryTag, std::move( model.m_VBVertices ) );
					pSphereIndices = IndexBuffer::Resolve( gfx, geometryTag, std::move( model.m_vecOfIndices ) );
					sphereCount = (UINT)model.m_vecOfIndices.size();
				}
				AddBind( std::move( pvs ) );
			}
			RegisterSource( DirectBufferSource<RenderTarget>::Make( "renderTarget", renderTarget ) );
			RegisterSource( DirectBufferSource<DepthStencil>::Make( "depthStencil", depthStencil ) );
		}
		void BindMainCamera( const Camera& cam ) noexcept
		{
			pMainCamera = &cam;
		}
		void Execute( Graphics& gfx ) const noxnd override
		{
			assert( pMainCamera );
			UINT indexCount;
			pMainCamera->BindToGraphics( gfx );
			if ( useSphere )
			{
				pSphereVertices->Bind( gfx );
				pSphereIndices->Bind( gfx );
				indexCount = sphereCount;
			}
			else
			{
				pCubeVertices->Bind( gfx );
				pCubeIndices->Bind( gfx );
				indexCount = cubeCount;
			}
			BindAll( gfx );
			gfx.DrawIndexed( indexCount );
		}
		void RenderWindow()
		{
			if ( ImGui::Begin( "Skybox" ) )
			{
				ImGui::Checkbox( "Use sphere", &useSphere );
			}
			ImGui::End();
		}
	private:
		bool useSphere = true;
		const Camera* pMainCamera = nullptr;
		std::shared_ptr<Bind::VertexBuffer> pCubeVertices;
		std::shared_ptr<Bind::IndexBuffer> pCubeIndices;
		UINT cubeCount;
		std::shared_ptr<Bind::VertexBuffer> pSphereVertices;
		std::shared_ptr<Bind::IndexBuffer> pSphereIndices;
		UINT sphereCount;
	};
}