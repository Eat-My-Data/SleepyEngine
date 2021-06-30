#pragma once
#include "Bindable.h"
#include "Vertex.h"

namespace Bind
{
	class VertexBuffer : public Bindable
	{
	public:
		VertexBuffer( GraphicsDeviceInterface& gfx, const std::string& tag, const Dvtx::VertexBuffer& vbuf );
		VertexBuffer( GraphicsDeviceInterface& gfx, const Dvtx::VertexBuffer& vbuf );
		void Bind( GraphicsDeviceInterface& gfx ) noexcept override;
		static std::shared_ptr<VertexBuffer> Resolve( GraphicsDeviceInterface& gfx, const std::string& tag,
			const Dvtx::VertexBuffer& vbuf );
		template<typename...Ignore>
		static std::string GenerateUID( const std::string& tag, Ignore&&...ignore )
		{
			return GenerateUID_( tag );
		}
		std::string GetUID() const noexcept override;
	private:
		static std::string GenerateUID_( const std::string& tag );
	protected:
		std::string tag;
		UINT stride;
		ID3D11Buffer* pVertexBuffer;
	};
}
