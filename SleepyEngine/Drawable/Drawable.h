#pragma once
#include "../GraphicsDeviceInterface/GraphicsDeviceInterface.h"
#include <DirectXMath.h>
#include <vector>
#include <memory>
#include "../ResourceManager/Jobber/Technique.h"

class TechniqueProbe;
class Material;
struct aiMesh;

namespace Bind
{
	class IndexBuffer;
	class VertexBuffer;
	class Topology;
	class InputLayout;
}

class Drawable
{
public:
	Drawable() = default;
	Drawable( GraphicsDeviceInterface& gfx, const Material& mat, const aiMesh& mesh ) noexcept;
	Drawable( const Drawable& ) = delete;
	void AddTechnique( Technique tech_in ) noexcept;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Submit( class FrameCommander& frame ) const noexcept;
	void Bind( GraphicsDeviceInterface& gfx ) const noexcept;
	void Accept( TechniqueProbe& probe );
	UINT GetIndexCount() const noexcept;
	virtual ~Drawable();
protected:
	std::shared_ptr<Bind::IndexBuffer> pIndices;
	std::shared_ptr<Bind::VertexBuffer> pVertices;
	std::shared_ptr<Bind::Topology> pTopology;
	std::vector<Technique> techniques;
};
