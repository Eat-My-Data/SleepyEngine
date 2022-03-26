#pragma once
#include <DirectXMath.h>
#include <string>
#include "Projection.h"
#include "../Drawable/CameraIndicator.h"
#include "../Bindable/Bindables/ConstantBuffers.h"

class Graphics;
namespace Rgph
{
	class RenderGraph;
}

struct WorldPosition
{
	DirectX::XMMATRIX projInvMatrix;
	DirectX::XMMATRIX viewInvMatrix;
	DirectX::XMFLOAT3 camPos;
	float padding;
};

class Camera
{
public:
	Camera( Graphics& gfx, std::string name, DirectX::XMFLOAT3 homePos = { 0.0f,0.0f,0.0f }, float homePitch = 0.0f, float homeYaw = 0.0f, bool tethered = false ) noexcept;
	void BindToGraphics( Graphics& gfx ) const;
	DirectX::XMMATRIX GetMatrix() const noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;
	void SpawnControlWidgets( Graphics& gfx ) noexcept;
	void Reset( Graphics& gfx ) noexcept;
	void Rotate( float dx, float dy ) noexcept;
	void Translate( DirectX::XMFLOAT3 translation ) noexcept;
	DirectX::XMFLOAT3 GetPos() const noexcept;
	void SetPos( const DirectX::XMFLOAT3& pos ) noexcept;
	const std::string& GetName() const noexcept;
	void LinkTechniques( Rgph::RenderGraph& rg );
	void Submit( size_t channel ) const;
	DirectX::XMVECTOR GetLookAt() const noexcept;
	DirectX::XMVECTOR GetUpVec() const noexcept;
public:
	float pitch;
	float yaw;
private:
	bool tethered;
	std::string name;
	DirectX::XMFLOAT3 homePos;
	float homePitch;
	float homeYaw;
	DirectX::XMFLOAT3 pos;
	static constexpr float travelSpeed = 12.0f;
	static constexpr float rotationSpeed = 0.004f;
	bool enableCameraIndicator = false;
	bool enableFrustumIndicator = false;
	Projection proj;
	CameraIndicator indicator;
	mutable WorldPosition cbufData;
	std::shared_ptr<Bind::PixelConstantBuffer<WorldPosition>> worldPositionCBuf;

};