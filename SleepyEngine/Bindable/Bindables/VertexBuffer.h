#pragma once
#include "../Bindable.h"
#include "../../Renderer/Model/Vertex.h"
#include <memory>

namespace Bind
{
	class VertexBuffer : public Bindable
	{
	public:
		VertexBuffer( Graphics& gdi, const std::string& tag, const Dvtx::VertexBuffer& vbuf );
		VertexBuffer( Graphics& gdi, const Dvtx::VertexBuffer& vbuf );
		void Bind( Graphics& gdi ) noexcept override;
		const Dvtx::VertexLayout& GetLayout() const noexcept;
		static std::shared_ptr<VertexBuffer> Resolve( Graphics& gdi, const std::string& tag,
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
