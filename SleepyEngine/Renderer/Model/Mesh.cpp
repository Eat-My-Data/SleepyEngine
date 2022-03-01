#include "Mesh.h"
#include "../../Libraries/imgui/imgui.h"
#include "../Surface.h"
#include <unordered_map>
#include <sstream>
#include <filesystem>
#include "../../Utilities/ChiliXM.h"
#include "../../Bindable/Bindables/DynamicConstant.h"
#include "../../Bindable/Bindables/ConstantBuffersEx.h"
#include "../../Renderer/Model/LayoutCodex.h"
#include "../../Bindable/Bindables/Stencil.h"

namespace dx = DirectX;


// Mesh
Mesh::Mesh( Graphics& gfx, const Material& mat, const aiMesh& mesh, float scale ) noexcept
	:
	Drawable( gfx, mat, mesh, scale )
{}

void Mesh::Submit( size_t channels, dx::FXMMATRIX accumulatedTranform ) const noexcept
{
	dx::XMStoreFloat4x4( &transform, accumulatedTranform );
	Drawable::Submit( channels );
}

void Mesh::ToggleRenderTechnique( Graphics& gfx, const std::string& renderTechnique )
{
	Drawable::ToggleRenderTechnique( gfx, renderTechnique );
}

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4( &transform );
}