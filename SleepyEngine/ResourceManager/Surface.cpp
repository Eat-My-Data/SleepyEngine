#include "Surface.h"
#include "../Win32/Win32Window.h"
#include <algorithm>
#include <cassert>
#include <sstream>
#include <filesystem>
#include <exception>
#include "../Utilities/SleepyUtil.h"

Surface::Surface( unsigned int width, unsigned int height )
{
	HRESULT hr = scratch.Initialize2D(
		format,
		width, height, 1u, 1u
	);
	if ( FAILED( hr ) )
	{
		throw std::exception();
	}
}

void Surface::Clear( Color fillValue ) noexcept
{
	const auto width = GetWidth();
	const auto height = GetHeight();
	auto& imgData = *scratch.GetImage( 0, 0, 0 );
	for ( size_t y = 0u; y < height; y++ )
	{
		auto rowStart = reinterpret_cast<Color*>( imgData.pixels + imgData.rowPitch * y );
		std::fill( rowStart, rowStart + imgData.width, fillValue );
	}
}

void Surface::PutPixel( unsigned int x, unsigned int y, Color c ) noexcept
{
	assert( x >= 0 );
	assert( y >= 0 );
	assert( x < GetWidth() );
	assert( y < GetHeight() );
	auto& imgData = *scratch.GetImage( 0, 0, 0 );
	reinterpret_cast<Color*>( &imgData.pixels[y * imgData.rowPitch] )[x] = c;
}

Surface::Color Surface::GetPixel( unsigned int x, unsigned int y ) const noexcept
{
	assert( x >= 0 );
	assert( y >= 0 );
	assert( x < GetWidth() );
	assert( y < GetHeight() );
	auto& imgData = *scratch.GetImage( 0, 0, 0 );
	return reinterpret_cast<Color*>( &imgData.pixels[y * imgData.rowPitch] )[x];
}

unsigned int Surface::GetWidth() const noexcept
{
	return (unsigned int)scratch.GetMetadata().width;
}

unsigned int Surface::GetHeight() const noexcept
{
	return (unsigned int)scratch.GetMetadata().height;
}

Surface::Color* Surface::GetBufferPtr() noexcept
{
	return reinterpret_cast<Color*>( scratch.GetPixels() );
}

const Surface::Color* Surface::GetBufferPtr() const noexcept
{
	return const_cast<Surface*>( this )->GetBufferPtr();
}

const Surface::Color* Surface::GetBufferPtrConst() const noexcept
{
	return const_cast<Surface*>( this )->GetBufferPtr();
}

Surface Surface::FromFile( const std::string& name )
{
	DirectX::ScratchImage scratch;
	HRESULT hr = DirectX::LoadFromWICFile( ToWide( name ).c_str(), DirectX::WIC_FLAGS_NONE, nullptr, scratch );

	if ( FAILED( hr ) )
	{
		throw std::exception();
	}

	if ( scratch.GetImage( 0, 0, 0 )->format != format )
	{
		DirectX::ScratchImage converted;
		hr = DirectX::Convert(
			*scratch.GetImage( 0, 0, 0 ),
			format,
			DirectX::TEX_FILTER_DEFAULT,
			DirectX::TEX_THRESHOLD_DEFAULT,
			converted
		);

		if ( FAILED( hr ) )
		{
			throw std::exception();
		}

		return Surface( std::move( converted ) );
	}

	return Surface( std::move( scratch ) );
}

void Surface::Save( const std::string& filename ) const
{
	const auto GetCodecID = []( const std::string& filename ) {
		const std::filesystem::path path = filename;
		const auto ext = path.extension().string();
		if ( ext == ".png" )
		{
			return DirectX::WIC_CODEC_PNG;
		}
		else if ( ext == ".jpg" )
		{
			return DirectX::WIC_CODEC_JPEG;
		}
		else if ( ext == ".bmp" )
		{
			return DirectX::WIC_CODEC_BMP;
		}
		throw std::exception();
	};

	HRESULT hr = DirectX::SaveToWICFile(
		*scratch.GetImage( 0, 0, 0 ),
		DirectX::WIC_FLAGS_NONE,
		GetWICCodec( GetCodecID( filename ) ),
		ToWide( filename ).c_str()
	);
	if ( FAILED( hr ) )
	{
		throw std::exception();
	}
}

bool Surface::AlphaLoaded() const noexcept
{
	return !scratch.IsAlphaAllOpaque();
}

Surface::Surface( DirectX::ScratchImage scratch ) noexcept
	:
	scratch( std::move( scratch ) )
{}