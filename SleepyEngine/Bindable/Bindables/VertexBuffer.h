#pragma once
#include "../Bindable.h"
#include "../../ResourceManager/Vertex.h"
#include <memory>

namespace Bind
{
	class VertexBuffer : public Bindable
	{
	public:
		VertexBuffer( GraphicsDeviceInterface& gdi, const std::string& tag, const Dvtx::VertexBuffer& vbuf );
		VertexBuffer( GraphicsDeviceInterface& gdi, const Dvtx::VertexBuffer& vbuf );
		void Bind( GraphicsDeviceInterface& gdi ) noexcept override;
		const Dvtx::VertexLayout& GetLayout() const noexcept;
		static std::shared_ptr<VertexBuffer> Resolve( GraphicsDeviceInterface& gdi, const std::string& tag,
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
		std::string m_sTag;
		UINT m_iStride;
		ID3D11Buffer* m_pVertexBuffer;
		Dvtx::VertexLayout layout;
	};
}
