#pragma once
#include "../Bindable.h"
#include "DepthStencil.h"

class Surface;

namespace Bind
{
	class Texture : public Bindable
	{
	public:
		Texture( Graphics& gdi, const std::string& path, UINT slot = 0 );
		void Bind( Graphics& gdi ) noexcept override;
		static std::shared_ptr<Texture> Resolve( Graphics& gdi, const std::string& path, UINT slot = 0 );
		static std::string GenerateUID( const std::string& path, UINT slot = 0 );
		std::string GetUID() const noexcept override;
		bool HasAlpha() const noexcept;
	private:
		static UINT CalculateNumberOfMipLevels( UINT width, UINT height ) noexcept;
	private:
		unsigned int slot;
	protected:
		bool hasAlpha = false;
		std::string path;
		ID3D11ShaderResourceView* pTextureView;
	};

	class DepthTexture : public Bindable
	{
	public:
		DepthTexture( Graphics& gdi, UINT size, UINT slot = 0 );
		void Bind( Graphics& gdi ) noxnd override;
		bool HasAlpha() const noexcept;
		std::shared_ptr<OutputOnlyDepthStencil> GetDepthBuffer() const;
	private:
		static UINT CalculateNumberOfMipLevels( UINT width, UINT height ) noexcept;
	private:
		unsigned int slot;
	protected:
		bool hasAlpha = false;
		ID3D11ShaderResourceView* pTextureView;
		std::shared_ptr<OutputOnlyDepthStencil> depthBuffer;
		UINT size;
	};
}
