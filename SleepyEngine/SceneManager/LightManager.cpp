#include "LightManager.h"
#include "../Libraries/imgui/backends/imgui_impl_dx11.h"
#include "../Libraries/imgui/backends/imgui_impl_win32.h"

void LightManager::Initialize( GraphicsDeviceInterface& gdi )
{
	m_pGDI = &gdi;
	m_pDirectionalLight = new DirectionalLight( gdi );
	m_pSpotLight = new SpotLight( gdi, 2.0f  );
	m_vecOfPointLights.push_back( new PointLight( gdi, 20 ) );
	m_vecOfPointLights.push_back( new PointLight( gdi, 20 ) );

	m_pLightIndex = new Bind::PixelConstantBuffer<LightIndex>{ gdi, 9 };
	m_pPixelStructuredBuffer = new Bind::PixelStructuredBuffer<DirectionalLight::DirectionalLightData>{ gdi, 5u };
	m_pPixelStructuredBuffer2 = new Bind::PixelStructuredBuffer<SpotLight::SpotLightData>{ gdi, 10u };
	m_pPixelArrStructuredBuffer = new Bind::PixelArrStructuredBuffer<PointLight::PointLightData>{ gdi, 6u };

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
	ID3D11Texture2D* pTexture;
	ID3D11Texture2D* pTexture2;

	gdi.GetDevice()->CreateTexture2D( &textureDesc, nullptr, &pTexture );
	gdi.GetDevice()->CreateTexture2D( &textureDesc, nullptr, &pTexture2 );

	// create the resource view on the texture
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	gdi.GetDevice()->CreateShaderResourceView( pTexture, &srvDesc, &pTextureView );
	gdi.GetDevice()->CreateShaderResourceView( pTexture2, &srvDesc, &pTextureView2 );

	// make depth buffer resources for capturing shadow map
	for ( u32 face = 0; face < 6; face++ )
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
		gdi.GetDevice()->CreateDepthStencilView( pTexture, &descView, &tempDSV );
		depthBuffers[face] = std::move( tempDSV );
		ID3D11DepthStencilView* tempDSV2 = {};
		gdi.GetDevice()->CreateDepthStencilView( pTexture2, &descView, &tempDSV2 );
		depthBuffers2[face] = std::move( tempDSV2 );
	}

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
	m_pPixelStructuredBuffer->Update( *m_pGDI, m_pDirectionalLight->m_StructuredBufferData );
	m_pPixelStructuredBuffer->Bind( *m_pGDI );

	m_pSpotLight->Update( *m_pGDI, camPos );
	m_pPixelStructuredBuffer2->Update( *m_pGDI, m_pSpotLight->m_StructuredBufferData );
	m_pPixelStructuredBuffer2->Bind( *m_pGDI );

	PointLight::PointLightData* bufferData = new PointLight::PointLightData[2];
	for ( u32 i = 0; i < m_vecOfPointLights.size(); i++ )
	{
		//m_vecOfPointLights[i]->Translate( { 0.0f,0.0f,0.0f } );
		m_vecOfPointLights[i]->Update( m_pGDI->GetViewMatrix(), m_pGDI->GetProjMatrix(), camPos );
		bufferData[i] = m_vecOfPointLights[i]->m_StructuredBufferData;
	}
	m_pPixelArrStructuredBuffer->Update( *m_pGDI, bufferData );
	m_pPixelArrStructuredBuffer->Bind( *m_pGDI );

	m_LightIndexes.numPointLights = (float)m_vecOfPointLights.size();
	m_pLightIndex->Update( *m_pGDI, m_LightIndexes );
	m_pLightIndex->Bind( *m_pGDI );

	m_pGDI->GetContext()->PSSetShaderResources( 7u, 1u, &pTextureView );
	m_pGDI->GetContext()->PSSetShaderResources( 8u, 1u, &pTextureView2 );
	m_pGDI->GetContext()->PSSetShaderResources( 11u, 1u, m_pGDI->GetShadowResource2() );

	delete[] bufferData;
}

void LightManager::Draw()
{
	const float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_pGDI->GetContext()->OMSetBlendState( m_pGDI->GetBlendState(), blendFactor, 0xffffffff );
	m_pGDI->GetContext()->OMSetRenderTargets( 1, m_pGDI->GetTarget(), *m_pGDI->GetDSV_ReadOnly() );
	m_pGDI->GetContext()->OMSetDepthStencilState( m_pGDI->GetLightDSS(), 1u );
	m_pGDI->GetContext()->PSSetShaderResources( 0, 3, m_pGDI->GetShaderResources() );
	m_pGDI->GetContext()->PSSetShaderResources( 3, 1, m_pGDI->GetDepthResource() );
	m_pGDI->GetContext()->PSSetShaderResources( 4, 1, m_pGDI->GetShadowResource() );
	m_pDirectionalLight->Draw( *m_pGDI );

	m_pGDI->GetContext()->OMSetBlendState( m_pGDI->GetBlendState(), blendFactor, 0xffffffff );

	for ( u32 i = 0; i < m_vecOfPointLights.size(); i++ )
	{
		m_LightIndexes.index = (float)i;
		m_pLightIndex->Update( *m_pGDI, m_LightIndexes );
		m_pLightIndex->Bind( *m_pGDI );
		m_vecOfPointLights[i]->Draw( * m_pGDI );
	}

	m_pSpotLight->Draw( *m_pGDI );
}

void LightManager::DrawControlPanel()
{
	ImGui::Text( "Spot Light" );
	ImGui::ColorEdit3( "Spot Color", &m_pSpotLight->m_StructuredBufferData.color.x );
	ImGui::SliderFloat( "Spot X", &m_pSpotLight->m_StructuredBufferData.pos.x, -80.0f, 80.0f );
	ImGui::SliderFloat( "Spot Y", &m_pSpotLight->m_StructuredBufferData.pos.y, -80.0f, 80.0f );
	ImGui::SliderFloat( "Spot Z", &m_pSpotLight->m_StructuredBufferData.pos.z, -80.0f, 80.0f );
	ImGui::Text( "Spot Orientation" );
	ImGui::SliderAngle( "Spot Pitch", &m_pSpotLight->GetCamera().m_fPitch, 0.995f * -180.0f, 0.995f * 180.0f );
	ImGui::SliderAngle( "Spot Yaw", &m_pSpotLight->GetCamera().m_fYaw, 0.995f * -180.0f, 0.995f * 180.0f );
	ImGui::Text( "Directional Light" );
	ImGui::ColorEdit3( "Color", &m_pDirectionalLight->m_StructuredBufferData.color.x );
	ImGui::Text( "Orientation" );
	ImGui::SliderAngle( "Pitch", &m_pDirectionalLight->GetCamera().m_fPitch, 0.995f * -90.0f, 0.995f * 90.0f );
	ImGui::SliderAngle( "Yaw", &m_pDirectionalLight->GetCamera().m_fYaw, -180.0f, 180.0f );
	ImGui::Text( "Point Light #1" );
	ImGui::ColorEdit3( "Color1", &m_vecOfPointLights[0]->m_StructuredBufferData.diffuseColor.x );
	ImGui::SliderFloat( "X1", &m_vecOfPointLights[0]->m_StructuredBufferData.pos.x, -80.0f, 80.0f );
	ImGui::SliderFloat( "Y1", &m_vecOfPointLights[0]->m_StructuredBufferData.pos.y, -80.0f, 80.0f );
	ImGui::SliderFloat( "Z1", &m_vecOfPointLights[0]->m_StructuredBufferData.pos.z, -80.0f, 80.0f );
	ImGui::Text( "Point Light #2" );
	ImGui::ColorEdit3( "Color2", &m_vecOfPointLights[1]->m_StructuredBufferData.diffuseColor.x );
	ImGui::SliderFloat( "X2", &m_vecOfPointLights[1]->m_StructuredBufferData.pos.x, -80.0f, 80.0f );
	ImGui::SliderFloat( "Y2", &m_vecOfPointLights[1]->m_StructuredBufferData.pos.y, -80.0f, 80.0f );
	ImGui::SliderFloat( "Z2", &m_vecOfPointLights[1]->m_StructuredBufferData.pos.z, -80.0f, 80.0f );
}

void LightManager::RenderLightGeometry()
{
	for ( u32 i = 0; i < m_vecOfPointLights.size(); i++ )
	{
		m_LightIndexes.index = (float)i;
		m_pLightIndex->Update( *m_pGDI, m_LightIndexes );
		m_pLightIndex->Bind( *m_pGDI );
		m_vecOfPointLights[i]->m_SolidSphere->Draw( *m_pGDI );
	}

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
	const auto pos = DirectX::XMLoadFloat3( &m_vecOfPointLights[0]->m_StructuredBufferData.pos );
	const auto pos2 = DirectX::XMLoadFloat3( &m_vecOfPointLights[1]->m_StructuredBufferData.pos );

	m_pGDI->SetProjMatrix( DirectX::XMLoadFloat4x4( &projection ) );
	for ( u32 i = 0; i < 6; i++ )                                                                                                                                                                                                                                                                                                     
	{
		m_pGDI->GetContext()->ClearDepthStencilView( depthBuffers[i], D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u );
		m_pGDI->GetContext()->OMSetRenderTargets( 0, nullptr, depthBuffers[i] );
		const auto lookAt = DirectX::XMVectorAdd( pos, DirectX::XMLoadFloat3( &cameraDirections[i] ) );
		m_pGDI->SetViewMatrix( DirectX::XMMatrixLookAtLH( pos, lookAt, DirectX::XMLoadFloat3( &cameraUps[i] ) ) );
		model.Draw( *m_pGDI, true );
		m_pGDI->GetContext()->ClearDepthStencilView( depthBuffers2[i], D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u );
		m_pGDI->GetContext()->OMSetRenderTargets( 0, nullptr, depthBuffers2[i] );
		const auto lookAt2 = DirectX::XMVectorAdd( pos2, DirectX::XMLoadFloat3( &cameraDirections[i] ) );
		m_pGDI->SetViewMatrix( DirectX::XMMatrixLookAtLH( pos2, lookAt2, DirectX::XMLoadFloat3( &cameraUps[i] ) ) );
		model.Draw( *m_pGDI, true );
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