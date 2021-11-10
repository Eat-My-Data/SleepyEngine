#pragma once
#include "../GraphicsDeviceInterface/GraphicsDeviceInterface.h"
#include <DirectXMath.h>
#include <vector>
#include <memory>
#include "../ResourceManager/Jobber/Technique.h"

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
	Drawable( const Drawable& ) = delete;
	void AddTechnique( Technique tech_in ) noexcept;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Draw( GraphicsDeviceInterface& gdi ) const noexcept;
	void DrawDepth( GraphicsDeviceInterface& gdi ) const noexcept;
	void Submit( class FrameCommander& frame ) const noexcept;
	void Bind( GraphicsDeviceInterface& gfx ) const noexcept;
	UINT GetIndexCount() const noexcept;
	virtual ~Drawable();	template<class T>
	T* QueryBindable() noexcept
	{
		for ( auto& pb : binds )
		{
			if ( auto pt = dynamic_cast<T*>( pb.get() ) )
			{
				return pt;
			}
		}
		return nullptr;
	}
protected:
	void AddBind( std::shared_ptr<Bind::Bindable> bind ) noexcept;
	const Bind::IndexBuffer* pIndexBuffer = nullptr;
	std::shared_ptr<Bind::IndexBuffer> pIndices;
	std::shared_ptr<Bind::VertexBuffer> pVertices;
	std::shared_ptr<Bind::Topology> pTopology;
	std::vector<Technique> techniques;
	std::vector<std::shared_ptr<Bind::Bindable>> binds;
};
