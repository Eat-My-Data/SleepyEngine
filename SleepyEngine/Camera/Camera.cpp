#include "Camera.h"
#include "../Libraries/imgui/imgui.h"
#include "../Utilities/ChiliMath.h"
#include "../Graphics/Graphics.h"

namespace dx = DirectX;

Camera::Camera( Graphics& gfx, std::string name, DirectX::XMFLOAT3 homePos, float homePitch, float homeYaw ) noexcept
	:
	name( std::move( name ) ),
	homePos( homePos ),
	homePitch( homePitch ),
	homeYaw( homeYaw ),
	proj( gfx, 1.0f, 9.0f / 16.0f, 0.5f, 400.0f ),
	indicator( gfx )
{
	Reset();
	indicator.SetPos( pos );
	indicator.SetRotation( { pitch,yaw,0.0f } );
	proj.SetPos( pos );
	proj.SetRotation( { pitch,yaw,0.0f } );
}

void Camera::BindToGraphics( Graphics& gfx ) const
{
	gfx.SetCamera( GetMatrix() );
	gfx.SetProjection( proj.GetMatrix() );
}

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	using namespace dx;

	const dx::XMVECTOR forwardBaseVector = XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f );
	// apply the camera rotations to a base vector
	const auto lookVector = XMVector3Transform( forwardBaseVector,
		XMMatrixRotationRollPitchYaw( pitch, yaw, 0.0f )
	);
	// generate camera transform (applied to all objects to arrange them relative
	// to camera position/orientation in world) from cam position and direction
	// camera "top" always faces towards +Y (cannot do a barrel roll)
	const auto camPosition = XMLoadFloat3( &pos );
	const auto camTarget = camPosition + lookVector;
	return XMMatrixLookAtLH( camPosition, camTarget, XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f ) );
}

void Camera::SpawnControlWidgets( Graphics& gfx ) noexcept
{
	ImGui::Text( "Position" );
	ImGui::SliderFloat( "X", &pos.x, -80.0f, 80.0f, "%.1f" );
	ImGui::SliderFloat( "Y", &pos.y, -80.0f, 80.0f, "%.1f" );
	ImGui::SliderFloat( "Z", &pos.z, -80.0f, 80.0f, "%.1f" );
	ImGui::Text( "Orientation" );
	ImGui::SliderAngle( "Pitch", &pitch, 0.995f * -90.0f, 0.995f * 90.0f );
	ImGui::SliderAngle( "Yaw", &yaw, -180.0f, 180.0f );
	if ( ImGui::Button( "Reset" ) )
	{
		Reset();
		indicator.SetPos( pos );
		indicator.SetRotation( { pitch,yaw,0.0f } );
	}
	proj.RenderWidgets( gfx );
}

void Camera::Reset() noexcept
{
	pos = homePos;
	pitch = homePitch;
	yaw = homeYaw;
}

void Camera::Rotate( float dx, float dy ) noexcept
{
	yaw = wrap_angle( yaw + dx * rotationSpeed );
	pitch = std::clamp( pitch + dy * rotationSpeed, 0.995f * -PI / 2.0f, 0.995f * PI / 2.0f );
	const dx::XMFLOAT3 angles = { pitch,yaw,0.0f };
	indicator.SetRotation( angles );
	proj.SetRotation( angles );
}

void Camera::Translate( DirectX::XMFLOAT3 translation ) noexcept
{
	dx::XMStoreFloat3( &translation, dx::XMVector3Transform(
		dx::XMLoadFloat3( &translation ),
		dx::XMMatrixRotationRollPitchYaw( pitch, yaw, 0.0f ) *
		dx::XMMatrixScaling( travelSpeed, travelSpeed, travelSpeed )
	) );
	pos = {
		pos.x + translation.x,
		pos.y + translation.y,
		pos.z + translation.z
	};
	indicator.SetPos( pos );
	proj.SetPos( pos );
}

DirectX::XMFLOAT3 Camera::GetPos() const noexcept
{
	return pos;
}

const std::string& Camera::GetName() const noexcept
{
	return name;
}

void Camera::LinkTechniques( Rgph::RenderGraph& rg )
{
	indicator.LinkTechniques( rg );
	proj.LinkTechniques( rg );
}

void Camera::Submit() const
{
	indicator.Submit();
	proj.Submit();
}