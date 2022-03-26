#include "DirectionalLight.h"
#include "../Bindable/BindableCommon.h"
#include "../Bindable/Bindables/Sampler.h"
#include "../Bindable/Bindables/Blender.h"
#include <algorithm>
#include "../Libraries/imgui/imgui.h"

DirectionalLight::DirectionalLight( Graphics& gfx )
	:
	mesh( gfx ),
	cbuf( gfx, 5 )
{
	pCamera = std::make_shared<Camera>( gfx, "Directional Light", homePos, homePitch, homeYaw, true );

	home = {
		{ 0.0f, -1.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f },
		{ 0.7f,0.7f,0.7f },
		1.0f,
		{ 0.0f, 0.0f }, // padding
		{ pCamera->GetMatrix() * pCamera->GetProjection() }
	};

	Reset();
}

void DirectionalLight::SpawnControlWindow() noexcept
{
	if ( ImGui::Begin( "Directional Light" ) )
	{
		bool dirtyPos = false;
		const auto d = [&dirtyPos]( bool dirty ) {dirtyPos = dirtyPos || dirty; };

		ImGui::SliderAngle( "Pitch", &pCamera->pitch, 0.0f, 0.995f * 90.0f * 2.0f );;
		ImGui::SliderAngle( "Yaw", &pCamera->yaw, 0.995f * -180.0f, 0.995f * 180.0f );

		ImGui::SliderFloat( "Intensity", &cbData.intensity, 0.0f, 2.0f );

		ImGui::Text( "Color/Ambience" );
		ImGui::ColorEdit3( "Diffuse Color", &cbData.color.x );
		ImGui::ColorEdit3( "Ambience", &cbData.ambient.x );

		if ( ImGui::Button( "Reset" ) )
		{
			Reset();
		}
	}
	ImGui::End();
}

void DirectionalLight::Reset() noexcept
{
	cbData = home;
	pCamera->SetPos( homePos );
	pCamera->pitch = homePitch;
	pCamera->yaw = homeYaw;
}

void DirectionalLight::Submit( size_t channels ) const noexcept( !IS_DEBUG )
{
	mesh.Submit( channels );
}

void DirectionalLight::Bind( Graphics& gfx, DirectX::FXMMATRIX view ) const noexcept
{
	auto dataCopy = cbData;
	dataCopy.lightDirection = { pCamera->GetLookAt().m128_f32[0], pCamera->GetLookAt().m128_f32[1], pCamera->GetLookAt().m128_f32[2] };
	cbuf.Update( gfx, dataCopy );
	cbuf.Bind( gfx );
}

void DirectionalLight::LinkTechniques( Rgph::RenderGraph& rg )
{
	mesh.LinkTechniques( rg );
}

std::shared_ptr<Camera> DirectionalLight::ShareCamera() const noexcept
{
	return pCamera;
}

void DirectionalLight::ToggleRenderTechnique( Graphics& gfx, const std::string& renderTechnique )
{
	mesh.ToggleRenderTechnique( gfx, renderTechnique );
}