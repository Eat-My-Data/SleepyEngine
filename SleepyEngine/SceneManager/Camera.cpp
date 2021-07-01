#include "Camera.h"
#include "../SleepyMath.h"
#include <algorithm>

Camera::Camera( const wchar_t* name, MatrixType matrixType, ViewSpace viewSpace, DirectX::XMFLOAT3 homePos, f32 homePitch, f32 homeYaw ) noexcept
	:
	m_sName( name ),
	m_MatrixType( matrixType ),
	m_HomePos( homePos ),
	m_fHomePitch( homePitch ),
	m_fHomeYaw( homeYaw ),
	m_ViewSpace( viewSpace )
{
	Reset();
}

Camera::~Camera()
{}

DirectX::XMMATRIX Camera::GetViewMatrix()
{
	using namespace DirectX;

	const XMVECTOR forwardBaseVector = XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f );
	// apply the camera rotations to a base vector
	const auto lookVector = XMVector3Transform( forwardBaseVector,
		XMMatrixRotationRollPitchYaw( m_fPitch, m_fYaw, 0.0f )
	);
	// generate camera transform (applied to all objects to arrange them relative
	// to camera position/orientation in world) from cam position and direction
	// camera "top" always faces towards +Y (cannot do a barrel roll)
	const auto camPosition = XMLoadFloat3( &m_Pos );
	const auto camTarget = camPosition + lookVector;
	return XMMatrixLookAtLH( camPosition, camTarget, XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f ) );
}

DirectX::XMMATRIX Camera::GetProjectionMatrix()
{
	if ( m_MatrixType == MatrixType::Perspective )
		return GeneratePerpectiveProjectionMatrix( m_ViewSpace.width, m_ViewSpace.height, m_ViewSpace.nearZ, m_ViewSpace.farZ );
	else
		return GenerateOrthographicProjectionMatrix( m_ViewSpace.width, m_ViewSpace.height, m_ViewSpace.nearZ, m_ViewSpace.farZ );
}

void Camera::Reset() noexcept
{
	m_Pos = m_HomePos;
	m_fPitch = m_fHomePitch;
	m_fYaw = m_fHomeYaw;
}

void Camera::Rotate( f32 dx, f32 dy ) noexcept
{
	m_fYaw = wrap_angle( m_fYaw + dx * m_fRotationSpeed );
	m_fPitch = std::clamp( m_fPitch + dy * m_fRotationSpeed, 0.995f * -PI / 2.0f, 0.995f * PI / 2.0f );
}

void Camera::Translate( DirectX::XMFLOAT3 translation ) noexcept
{
	DirectX::XMStoreFloat3( &translation, DirectX::XMVector3Transform(
		DirectX::XMLoadFloat3( &translation ),
		DirectX::XMMatrixRotationRollPitchYaw( m_fPitch, m_fYaw, 0.0f ) *
		DirectX::XMMatrixScaling( m_fTravelSpeed, m_fTravelSpeed, m_fTravelSpeed )
	) );
	m_Pos = {
		m_Pos.x + translation.x,
		m_Pos.y + translation.y,
		m_Pos.z + translation.z
	};
}

DirectX::XMMATRIX Camera::GeneratePerpectiveProjectionMatrix( f32 width, f32 height, f32 nearZ, f32 farZ )
{
	return DirectX::XMMatrixPerspectiveLH( width, height, nearZ, farZ );
}

DirectX::XMMATRIX Camera::GenerateOrthographicProjectionMatrix( f32 width, f32 height, f32 nearZ, f32 farZ )
{
	return DirectX::XMMatrixOrthographicLH( width, height, nearZ, farZ );
}

