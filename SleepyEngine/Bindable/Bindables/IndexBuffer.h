#pragma once
#include "../Bindable.h"
#include "../BindableCodex.h"

namespace Bind
{
	class IndexBuffer : public Bindable
	{
	public:
		IndexBuffer( GraphicsDeviceInterface& gdi, const std::vector<unsigned short>& indices );
		IndexBuffer( GraphicsDeviceInterface& gdi, std::string tag, const std::vector<unsigned short>& indices );
		void Bind( GraphicsDeviceInterface& gdi ) noexcept override;
		UINT GetCount() const noexcept;
		static std::shared_ptr<IndexBuffer> Resolve( GraphicsDeviceInterface& gdi, const std::string& tag,
			const std::vector<unsigned short>& indices );
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
		UINT m_iCount;
		ID3D11Buffer* m_pIndexBuffer;
	};
}
