#pragma once
#include "RenderQueuePass.h"
#include "../Job.h"
#include <vector>
#include "../Sink.h"
#include "../Source.h"

#include "../../../Bindable/Bindables/Stencil.h"
#include "../../../Bindable/Bindables/PixelShader.h"
#include "../../../Bindable/Bindables/VertexShader.h"
#include "../../../Bindable/Bindables/Rasterizer.h"
#include "../../../Bindable/Bindables/RenderTarget.h"
#include "../../../Bindable/Bindables/Blender.h"
#include "../../../Bindable/Bindables/NullPixelShader.h"
#include "../../../Bindable/Bindables/ShadowRasterizer.h"
#include "../../../Bindable/Bindables/CubeTexture.h"
#include "../../../Bindable/Bindables/Texture.h"
#include "../../../Bindable/Bindables/Viewport.h"
#include "../../../Utilities/ChiliMath.h"

class Graphics;

namespace Rgph
{
	class ShadowMappingPass : public RenderQueuePass
	{
	public:
		void BindPLShadowCamera( const Camera& cam ) noexcept
		{
			pPLShadowCamera = &cam;
		}
		void BindSLShadowCamera( const Camera& cam ) noexcept
		{
			pSLShadowCamera = &cam;
		}
		void BindDLShadowCamera( const Camera& cam ) noexcept
		{
			pDLShadowCamera = &cam;
		}
		ShadowMappingPass( Graphics& gfx, std::string name )
			:
			RenderQueuePass( std::move( name ) )
		{
			using namespace Bind; 
			pDepthCube = std::make_shared<DepthCubeTexture>( gfx, size, 3 );
			pSLDepthTexture = std::make_shared<DepthTexture>( gfx, size, 4 ); // TODO: Cleanup resources
			pDLDepthTexture = std::make_shared<DepthTexture>( gfx, size, 5 ); // TODO: Cleanup resources


			AddBind( VertexShader::Resolve( gfx, "./Shaders/Bin/Shadow_VS.cso" ) );
			AddBind( NullPixelShader::Resolve( gfx ) );
			AddBind( Stencil::Resolve( gfx, Stencil::Mode::Off ) );
			AddBind( Blender::Resolve( gfx, false ) );
			AddBind( std::make_shared<Viewport>( gfx, (float)size, (float)size ) );
			AddBind( std::make_shared<Bind::ShadowRasterizer>( gfx, 50, 2.0f, 0.1f ) );
			RegisterSource( DirectBindableSource<Bind::DepthCubeTexture>::Make( "plMap", pDepthCube ) );;
			RegisterSource( DirectBindableSource<Bind::DepthTexture>::Make( "slMap", pSLDepthTexture ) );;
			RegisterSource( DirectBindableSource<Bind::DepthTexture>::Make( "dlMap", pDLDepthTexture ) );;

			DirectX::XMStoreFloat4x4(
				&projection,
				DirectX::XMMatrixPerspectiveFovLH( PI / 2.0f, 1.0f, 0.5f, 100.0f )
			);
			// +x
			DirectX::XMStoreFloat3( &cameraDirections[0], DirectX::XMVectorSet( 1.0f, 0.0f, 0.0f, 0.0f ) );
			DirectX::XMStoreFloat3( &cameraUps[0], DirectX::XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f ) );
			// -x
			DirectX::XMStoreFloat3( &cameraDirections[1], DirectX::XMVectorSet( -1.0f, 0.0f, 0.0f, 0.0f ) );
			DirectX::XMStoreFloat3( &cameraUps[1], DirectX::XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f ) );
			// +y
			DirectX::XMStoreFloat3( &cameraDirections[2], DirectX::XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f ) );
			DirectX::XMStoreFloat3( &cameraUps[2], DirectX::XMVectorSet( 0.0f, 0.0f, -1.0f, 0.0f ) );
			// -y
			DirectX::XMStoreFloat3( &cameraDirections[3], DirectX::XMVectorSet( 0.0f, -1.0f, 0.0f, 0.0f ) );
			DirectX::XMStoreFloat3( &cameraUps[3], DirectX::XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f ) );
			// +z
			DirectX::XMStoreFloat3( &cameraDirections[4], DirectX::XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f ) );
			DirectX::XMStoreFloat3( &cameraUps[4], DirectX::XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f ) );
			// -z
			DirectX::XMStoreFloat3( &cameraDirections[5], DirectX::XMVectorSet( 0.0f, 0.0f, -1.0f, 0.0f ) );
			DirectX::XMStoreFloat3( &cameraUps[5], DirectX::XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f ) );
		
			// set the depth buffer to *something* so that the rg validation doesn't scream
			SetDepthBuffer( pDepthCube->GetDepthBuffer( 0 ) );
		}
		void Execute( Graphics& gfx ) const noxnd override
		{
			using namespace DirectX;

			// point light
			XMFLOAT3 tempPos{ pPLShadowCamera->GetPos() };
			const auto pos = XMLoadFloat3( &tempPos );
			gfx.SetProjection( XMLoadFloat4x4( &projection ) );
			for ( size_t i = 0; i < 6; i++ )
			{
				auto d = pDepthCube->GetDepthBuffer( i );
				d->Clear( gfx );
				SetDepthBuffer( std::move( d ) );
				const auto lookAt = pos + XMLoadFloat3( &cameraDirections[i] );
				gfx.SetCamera( XMMatrixLookAtLH( pos, lookAt, XMLoadFloat3( &cameraUps[i] ) ) );
				RenderQueuePass::Execute( gfx );
			}

			// spot light
			XMFLOAT3 tempPos2{ pSLShadowCamera->GetPos() };
			const auto pos2 = XMLoadFloat3( &tempPos2 );
			gfx.SetProjection( pSLShadowCamera->GetProjection() );
			auto d = pSLDepthTexture->GetDepthBuffer();
			d->Clear( gfx );
			SetDepthBuffer( std::move( d ) );
			gfx.SetCamera( XMMatrixLookAtLH( pos2, pSLShadowCamera->GetLookAt(), pSLShadowCamera->GetUpVec() ) );
			RenderQueuePass::Execute( gfx );

			// directional light
			XMFLOAT3 tempPos3{ pDLShadowCamera->GetPos() };
			const auto pos3 = XMLoadFloat3( &tempPos3 );
			gfx.SetProjection( pDLShadowCamera->GetProjection() );
			auto d2 = pDLDepthTexture->GetDepthBuffer();
			d2->Clear( gfx );
			SetDepthBuffer( std::move( d2 ) );
			gfx.SetCamera( XMMatrixLookAtLH( pos3, pDLShadowCamera->GetLookAt(), pDLShadowCamera->GetUpVec() ) );
			RenderQueuePass::Execute( gfx );
		}
		void DumpShadowMap( Graphics& gfx, const std::string& path ) const
		{
			for ( size_t i = 0; i < 6; i++ )
			{
				auto d = pDepthCube->GetDepthBuffer( i );
				d->ToSurface( gfx ).Save( path + std::to_string( i ) + ".png" );
			}
		}
	private:
		void SetDepthBuffer( std::shared_ptr<Bind::DepthStencil> ds ) const
		{
			const_cast<ShadowMappingPass*>( this )->depthStencil = std::move( ds );
		}
		static constexpr UINT size = 1000;
		const Camera* pPLShadowCamera = nullptr;
		const Camera* pSLShadowCamera = nullptr;
		const Camera* pDLShadowCamera = nullptr;
		std::shared_ptr<Bind::DepthCubeTexture> pDepthCube;
		std::shared_ptr<Bind::DepthTexture> pSLDepthTexture;
		std::shared_ptr<Bind::DepthTexture> pDLDepthTexture;
		DirectX::XMFLOAT4X4 projection;
		std::vector<DirectX::XMFLOAT3> cameraDirections{ 6 };
		std::vector<DirectX::XMFLOAT3> cameraUps{ 6 };
	};
}