#include "LightManager.h"

void LightManager::Initialize( GraphicsDeviceInterface& gdi )
{
	m_pGDI = &gdi;
	m_pDirectionalLight = new DirectionalLight( gdi );
	m_vecOfPointLights.push_back( new PointLight( gdi, 20 ) );
	m_vecOfPointLights.push_back( new PointLight( gdi, 20 ) );

	m_pPixelStructuredBuffer = new Bind::PixelStructuredBuffer<DirectionalLight::DirectionalLightData>{ gdi, 5u };
	m_pPixelArrStructuredBuffer = new Bind::PixelArrStructuredBuffer<PointLight::PointLightData>{ gdi, 6u };
}

void LightManager::UpdateBuffers( DirectX::XMFLOAT3 camPos )
{
	m_pDirectionalLight->Update( *m_pGDI, camPos );
	m_pPixelStructuredBuffer->Update( *m_pGDI, m_pDirectionalLight->m_StructuredBufferData );
	m_pPixelStructuredBuffer->Bind( *m_pGDI );

	PointLight::PointLightData* bufferData = new PointLight::PointLightData[2];
	for ( u32 i = 0; i < m_vecOfPointLights.size(); i++ )
	{
		m_vecOfPointLights[i]->Update( m_pGDI->GetViewMatrix(), m_pGDI->GetProjMatrix(), camPos );
		bufferData[i] = m_vecOfPointLights[i]->m_StructuredBufferData;
	}
	m_pPixelArrStructuredBuffer->Update( *m_pGDI, bufferData );
	m_pPixelArrStructuredBuffer->Bind( *m_pGDI );
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
		m_vecOfPointLights[i]->Draw( *m_pGDI );
	}
}

void LightManager::RenderSolidSpheres()
{
	for ( u32 i = 0; i < m_vecOfPointLights.size(); i++ )
	{
		m_vecOfPointLights[i]->m_SolidSphere->Draw( *m_pGDI );
	}
}

void LightManager::PrepareDepthFromLight()
{
	m_pGDI->SetViewMatrix( m_pDirectionalLight->GetViewMatrix() );
	m_pGDI->SetProjMatrix( m_pDirectionalLight->GetProjectionMatrix() );
	m_pGDI->GetContext()->OMSetDepthStencilState( m_pGDI->GetBufferDSS(), 1u );
	m_pGDI->GetContext()->OMSetRenderTargets( 0, nullptr, *m_pGDI->GetShadowDSV() );
}

void LightManager::SelectLight( const u32 index )
{
	m_iSelectedLight = index;
}

void LightManager::TranslatePointLight( DirectX::XMFLOAT3 translation )
{
	m_vecOfPointLights[m_iSelectedLight]->Translate( translation );
}

void LightManager::TranslateDirectionalLight( DirectX::XMFLOAT3 translation )
{
	m_pDirectionalLight->Translate( translation );
}

void LightManager::RotateDirectionalLight( const f32 dx, const f32 dy )
{
	m_pDirectionalLight->Rotate( dx, dy );
}