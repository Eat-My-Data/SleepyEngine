#include "Mesh.h"
#include "../Libraries/imgui/imgui.h"
#include "Surface.h"
#include <unordered_map>
#include <sstream>
#include <filesystem>
#include "../Utilities/SleepyXM.h"
#include "../Bindable/Bindables/DynamicConstant.h"
#include "../Bindable/Bindables/ConstantBuffersEx.h"
#include "../ResourceManager/LayoutCodex.h"
#include "../Bindable/Bindables/Stencil.h"

namespace dx = DirectX;


// Mesh
Mesh::Mesh( GraphicsDeviceInterface& gfx, const Material& mat, const aiMesh& mesh ) noexcept
	:
	Drawable( gfx, mat, mesh )
{}

void Mesh::Submit( FrameCommander& frame, dx::FXMMATRIX accumulatedTranform ) const noexcept
{
	dx::XMStoreFloat4x4( &transform, accumulatedTranform );
	Drawable::Submit( frame );
}

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4( &transform );
}