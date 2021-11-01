#include "LightManager.h"
#include "../Libraries/imgui/backends/imgui_impl_dx11.h"
#include "../Libraries/imgui/backends/imgui_impl_win32.h"

void LightManager::Initialize( GraphicsDeviceInterface& gdi )
{
	m_pGDI = &gdi;
	m_pDirectionalLight = new DirectionalLight( gdi );
	m_pSpotLight = new SpotLight( gdi, 10.0f  );
	m_vecOfPointLights.push_back( new PointLight( gdi, 20.0f ) );

	// structured buffers
	// 5, 7, and 9 are respective depth textures
	m_pDirectionalLightBuffer = new Bind::PixelStructuredBuffer<DirectionalLight::DirectionalLightData>{ gdi, 4u };
	// TODO: PixelArrStructuredBuffer is limited to two = BAD
	m_pPointLightBuffer = new Bind::PixelArrStructuredBuffer<PointLight::PointLightData>{ gdi, 6u };
	m_pSpotLightBuffer = new Bind::PixelStructuredBuffer<SpotLight::SpotLightData>{ gdi, 8u };

	// constant buffer
	m_pDefaultLightSettingsBuffer = new Bind::PixelConstantBuffer<DefaultLightSettings>{ gdi, 7u };
	m_pSolidGeometryColorBuffer = new Bind::PixelConstantBuffer<SolidGeometryColor>{ gdi, 8u };
	m_pLightIndex = new Bind::PixelConstantBuffer<LightIndex>{ gdi, 9u };

	InitializePointLightShadowResources();

	DirectX::XMStoreFloat4x4(
		&projection,
		DirectX::XMMatrixPerspectiveFovLH( PI / 2.0f, 1.0f, 0.001f, 200.0f )
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
}

void LightManager::UpdateBuffers( DirectX::XMFLOAT3 camPos )
{
	m_pDirectionalLight->Update( *m_pGDI, camPos );
	m_pDirectionalLightBuffer->Update( *m_pGDI, m_pDirectionalLight->m_StructuredBufferData );
	m_pDirectionalLightBuffer->Bind( *m_pGDI );

	m_pSpotLight->Update( *m_pGDI, camPos );
	m_pSpotLightBuffer->Update( *m_pGDI, m_pSpotLight->m_StructuredBufferData );
	m_pSpotLightBuffer->Bind( *m_pGDI );

	// TODO: Currently limited to two lights = BAD
	PointLight::PointLightData* bufferData = new PointLight::PointLightData[3];
	for ( u32 i = 0; i < m_vecOfPointLights.size(); i++ )
	{
		m_vecOfPointLights[i]->Update();
		bufferData[i] = m_vecOfPointLights[i]->m_StructuredBufferData;
	}
	m_pPointLightBuffer->Update( *m_pGDI, bufferData );
	m_pPointLightBuffer->Bind( *m_pGDI );
	delete [] bufferData;

	m_LightIndexes.numPointLights = (int)m_vecOfPointLights.size();
	m_pLightIndex->Update( *m_pGDI, m_LightIndexes );
	m_pLightIndex->Bind( *m_pGDI );

	m_pDefaultLightSettingsBuffer->Update( *m_pGDI, m_DefaultLightSettings );
	m_pDefaultLightSettingsBuffer->Bind( *m_pGDI );

	m_pGDI->GetContext()->PSSetShaderResources( 11u, (UINT)m_vecOfPointLights.size(), &pTextureView[0] );
	m_pGDI->GetContext()->PSSetShaderResources( 9u, 1u, m_pGDI->GetShadowResource2() );
}

void LightManager::Draw()
{
	const float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_pGDI->GetContext()->OMSetBlendState( m_pGDI->GetBlendState(), blendFactor, 0xffffffff );
	m_pGDI->GetContext()->OMSetRenderTargets( 1, m_pGDI->GetTarget(), *m_pGDI->GetDSV_ReadOnly() );
	m_pGDI->GetContext()->OMSetDepthStencilState( m_pGDI->GetLightDSS(), 1u );
	m_pGDI->GetContext()->PSSetShaderResources( 0, 3, m_pGDI->GetShaderResources() );
	m_pGDI->GetContext()->PSSetShaderResources( 3, 1, m_pGDI->GetDepthResource() );
	m_pGDI->GetContext()->PSSetShaderResources( 5, 1, m_pGDI->GetShadowResource() );
	m_pDirectionalLight->Draw( *m_pGDI );

	m_pGDI->GetContext()->OMSetBlendState( m_pGDI->GetBlendState(), blendFactor, 0xffffffff );

	for ( u32 i = 0; i < m_vecOfPointLights.size(); i++ )
	{
		m_LightIndexes.index = i;
		m_pLightIndex->Update( *m_pGDI, m_LightIndexes );
		m_pLightIndex->Bind( *m_pGDI );
		m_vecOfPointLights[i]->Draw( * m_pGDI );
	}

	m_pGDI->GetContext()->OMSetBlendState( m_pGDI->GetBlendState(), blendFactor, 0xffffffff );

	m_pSpotLight->Draw( *m_pGDI );
}

void LightManager::DrawControlPanel()
{
	if ( ImGui::Begin( "Directional Light Manager" ) )
	{
		m_pDirectionalLight->DrawControlPanel();
	}
	ImGui::End();

	if ( ImGui::Begin( "Point Light Manager" ) )
	{
		if ( ImGui::Button( "Spawn New Light" ) )
		{
			m_vecOfPointLights.push_back( new PointLight( *m_pGDI, 20.0f ) );
			InitializePointLightShadowResources();
		}
		if ( m_vecOfPointLights.size() > 1 )
			ImGui::DragInt( "Current Light", &m_iSelectedPointLight, 1, 0, (int)m_vecOfPointLights.size()-1 );
		m_vecOfPointLights[m_iSelectedPointLight]->DrawControlPanel();
	}
	ImGui::End();

	if ( ImGui::Begin( "Spot Light Manager" ) )
	{
		m_pSpotLight->DrawControlPanel();
	}
	ImGui::End();
}

void LightManager::RenderLightGeometry()
{
	for ( u32 i = 0; i < m_vecOfPointLights.size(); i++ )
	{
		m_LightIndexes.index = i;
		m_pLightIndex->Update( *m_pGDI, m_LightIndexes );
		m_pLightIndex->Bind( *m_pGDI );
		m_SolidGeometryColor.color = m_vecOfPointLights[i]->m_StructuredBufferData.color;
		m_pSolidGeometryColorBuffer->Update( *m_pGDI, m_SolidGeometryColor );
		m_pSolidGeometryColorBuffer->Bind( *m_pGDI );
		m_vecOfPointLights[i]->m_SolidSphere->Draw( *m_pGDI );
	}
	m_SolidGeometryColor.color = m_pSpotLight->m_StructuredBufferData.color;
	m_pSolidGeometryColorBuffer->Update( *m_pGDI, m_SolidGeometryColor );
	m_pSolidGeometryColorBuffer->Bind( *m_pGDI );
	m_pSpotLight->m_pSolidCone->Draw( *m_pGDI );
}

void LightManager::PrepareDepthFromLight()
{
	m_pGDI->SetViewMatrix( m_pDirectionalLight->GetViewMatrix() );
	m_pGDI->SetProjMatrix( m_pDirectionalLight->GetProjectionMatrix() );
	m_pGDI->GetContext()->OMSetDepthStencilState( m_pGDI->GetBufferDSS(), 1u );
	m_pGDI->GetContext()->OMSetRenderTargets( 0, nullptr, *m_pGDI->GetShadowDSV() );
}

void LightManager::RenderPointLightCubeTextures( const Model& model )
{
	m_pGDI->SetProjMatrix( DirectX::XMLoadFloat4x4( &projection ) );
	for ( int index = 0; index < m_vecOfPointLights.size(); index++ )
	{
		const auto pos = DirectX::XMLoadFloat3( &m_vecOfPointLights[index]->m_StructuredBufferData.pos );

		for ( int i = 0; i < 6; i++ )
		{
			m_pGDI->GetContext()->ClearDepthStencilView( depthBuffers[i + ( index * 6 )], D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u );
			m_pGDI->GetContext()->OMSetRenderTargets( 0, nullptr, depthBuffers[i + ( index * 6 )] );
			const auto lookAt = DirectX::XMVectorAdd( pos, DirectX::XMLoadFloat3( &cameraDirections[i] ) );
			m_pGDI->SetViewMatrix( DirectX::XMMatrixLookAtLH( pos, lookAt, DirectX::XMLoadFloat3( &cameraUps[i] ) ) );
			model.Draw( *m_pGDI, true );
		}
	}
}

void LightManager::PrepareDepthFromSpotLight()
{
	m_pGDI->SetViewMatrix( m_pSpotLight->GetViewMatrix() );
	m_pGDI->SetProjMatrix( m_pSpotLight->GetProjectionMatrix() );
	m_pGDI->GetContext()->OMSetDepthStencilState( m_pGDI->GetBufferDSS2(), 1u );
	m_pGDI->GetContext()->OMSetRenderTargets( 0, nullptr, *m_pGDI->GetShadowDSV2() );
}

void LightManager::SelectPointLight( const u32 index )
{
	m_iSelectedPointLight = index;
}

void LightManager::TranslateSpotLight( DirectX::XMFLOAT3 translation )
{
	m_pSpotLight->Translate( translation );
}

void LightManager::RotateSpotLight( const f32 dx, const f32 dy )
{
	m_pSpotLight->Rotate( dx, dy );
}

void LightManager::TranslatePointLight( DirectX::XMFLOAT3 translation )
{
	m_vecOfPointLights[m_iSelectedPointLight]->Translate( translation );
}

void LightManager::SelectSpotLight( const u32 index )
{
	m_iSelectedSpotLight = index;
}

void LightManager::TranslateDirectionalLight( DirectX::XMFLOAT3 translation )
{
	m_pDirectionalLight->Translate( translation );
}

void LightManager::RotateDirectionalLight( const f32 dx, const f32 dy )
{
	m_pDirectionalLight->Rotate( dx, dy );
}

void LightManager::InitializePointLightShadowResources()
{
	// texture descriptor
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = 1024;
	textureDesc.Height = 1024;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 6;
	textureDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	// create the texture resource
	std::vector<ID3D11Texture2D*> pTextures;
	pTextures.resize( m_vecOfPointLights.size() );
	for ( int index = 0; index < m_vecOfPointLights.size(); index++ )
	{
		m_pGDI->GetDevice()->CreateTexture2D( &textureDesc, nullptr, &pTextures[index] );
	}

	// make depth buffer resources for capturing shadow map
	D3D11_TEXCUBE_ARRAY_SRV txCubeArrSrv = {};
	txCubeArrSrv.NumCubes = (UINT)m_vecOfPointLights.size();
	txCubeArrSrv.MipLevels = 1;
	txCubeArrSrv.MostDetailedMip = 0;
	txCubeArrSrv.First2DArrayFace = 0;

	// create the resource view on the texture
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.TextureCubeArray = txCubeArrSrv;
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	pTextureView.resize( m_vecOfPointLights.size() );

	for ( int index = 0; index < m_vecOfPointLights.size(); index++ )
	{
		m_pGDI->GetDevice()->CreateShaderResourceView( pTextures[index], &srvDesc, &pTextureView[index] );
	}

	depthBuffers.resize( m_vecOfPointLights.size() * 6 );

	for ( int index = 0; index < m_vecOfPointLights.size(); index++ )
	{
		for ( int face = 0; face < 6; face++ )
		{
			// create target view of depth stensil texture
			D3D11_DEPTH_STENCIL_VIEW_DESC descView = {};
			descView.Format = DXGI_FORMAT_D32_FLOAT;
			descView.Flags = 0;
			descView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			descView.Texture2DArray.MipSlice = 0;
			descView.Texture2DArray.ArraySize = 1;
			descView.Texture2DArray.FirstArraySlice = face;
			ID3D11DepthStencilView* tempDSV = {};
			m_pGDI->GetDevice()->CreateDepthStencilView( pTextures[index], &descView, &tempDSV );
			depthBuffers[face + (index * 6)] = std::move( tempDSV );
		}
	}
}