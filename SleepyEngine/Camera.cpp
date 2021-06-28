#include "Camera.h"

Camera::Camera( const wchar_t* name, MatrixType matrixType, ViewSpace viewSpace, DirectX::XMFLOAT3 homePos, f32 homePitch, f32 homeYaw ) noexcept
	:
	m_sName( name ),
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

void Camera::Reset()
{
	m_Pos = m_HomePos;
	m_fPitch = m_fHomePitch;
	m_fYaw = m_fHomeYaw;
}

DirectX::XMMATRIX Camera::GeneratePerpectiveProjectionMatrix( f32 width, f32 height, f32 nearZ, f32 farZ )
{
	return DirectX::XMMatrixPerspectiveLH( width, height, nearZ, farZ );
}

DirectX::XMMATRIX Camera::GenerateOrthographicProjectionMatrix( f32 width, f32 height, f32 nearZ, f32 farZ )
{
	return DirectX::XMMatrixOrthographicLH( width, height, nearZ, farZ );
}
