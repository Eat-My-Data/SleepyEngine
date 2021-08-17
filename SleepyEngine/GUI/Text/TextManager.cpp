#include "TextManager.h"
#include "../../GraphicsDeviceInterface/GraphicsDeviceInterface.h"
#include "../../SceneManager/SceneManager.h"
#include ".././Forms/Rectangle.h"
#include "../../Bindable/BindableCommon.h"
#include "../../Win32/Mouse.h"
#include <fstream>

TextManager::TextManager( GraphicsDeviceInterface& gdi )
{
	m_pGDI = &gdi;

	std::ifstream fin;
	int i;
	char temp;

	// Create the font spacing buffer.
	m_pFont = new FontType[95];
	if ( !m_pFont )
	{
		throw std::exception();
	}

	// Read in the font size and spacing between chars.
	fin.open( "IndexFile.txt" );
	if ( fin.fail() )
	{
		throw std::exception();
	}

	// Read in the 95 used ascii characters for text.
	for ( i = 0; i < 95; i++ )
	{
		fin.get( temp );
		while ( temp != ' ' )
		{
			fin.get( temp );
		}
		fin.get( temp );
		while ( temp != ' ' )
		{
			fin.get( temp );
		}

		fin >> m_pFont[i].left;
		fin >> m_pFont[i].right;
		fin >> m_pFont[i].size;
	}

	// Close the file.
	fin.close();

	m_pTex = new Bind::Texture( gdi, "font_01.jpg" );
}

void TextManager::Draw( char* text, float x, float y )
{
	//VertexType* vertices;
	//unsigned long* indices;
	//D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	//D3D11_SUBRESOURCE_DATA vertexData, indexData;
	//HRESULT result;
	//int i;


	//// Create a new sentence object.
	//*sentence = new SentenceType;
	//if ( !*sentence )
	//{
	//	return false;
	//}

	//// Initialize the sentence buffers to null.
	//( *sentence )->vertexBuffer = 0;
	//( *sentence )->indexBuffer = 0;

	//// Set the maximum length of the sentence.
	//( *sentence )->maxLength = maxLength;

	//// Set the number of vertices in the vertex array.
	//( *sentence )->vertexCount = 6 * maxLength;

	//// Set the number of indexes in the index array.
	//( *sentence )->indexCount = ( *sentence )->vertexCount;

	//// Create the vertex array.
	//vertices = new VertexType[( *sentence )->vertexCount];
	//if ( !vertices )
	//{
	//	return false;
	//}

	//// Create the index array.
	//indices = new unsigned long[( *sentence )->indexCount];
	//if ( !indices )
	//{
	//	return false;
	//}

	//// Initialize vertex array to zeros at first.
	//memset( vertices, 0, ( sizeof( VertexType ) * ( *sentence )->vertexCount ) );

	//// Initialize the index array.
	//for ( i = 0; i < ( *sentence )->indexCount; i++ )
	//{
	//	indices[i] = i;
	//}

	//	// Set up the description of the dynamic vertex buffer.
	//	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	//vertexBufferDesc.ByteWidth = sizeof( VertexType ) * ( *sentence )->vertexCount;
	//vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//vertexBufferDesc.MiscFlags = 0;
	//vertexBufferDesc.StructureByteStride = 0;

	//// Give the subresource structure a pointer to the vertex data.
	//vertexData.pSysMem = vertices;
	//vertexData.SysMemPitch = 0;
	//vertexData.SysMemSlicePitch = 0;

	//// Create the vertex buffer.
	//result = device->CreateBuffer( &vertexBufferDesc, &vertexData, &( *sentence )->vertexBuffer );
	//if ( FAILED( result ) )
	//{
	//	return false;
	//}
	//The index buffer is setup as a normal static buffer since the contents will never need to change.

	//	// Set up the description of the static index buffer.
	//	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//indexBufferDesc.ByteWidth = sizeof( unsigned long ) * ( *sentence )->indexCount;
	//indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//indexBufferDesc.CPUAccessFlags = 0;
	//indexBufferDesc.MiscFlags = 0;
	//indexBufferDesc.StructureByteStride = 0;

	//// Give the subresource structure a pointer to the index data.
	//indexData.pSysMem = indices;
	//indexData.SysMemPitch = 0;
	//indexData.SysMemSlicePitch = 0;

	//// Create the index buffer.
	//result = device->CreateBuffer( &indexBufferDesc, &indexData, &( *sentence )->indexBuffer );
	//if ( FAILED( result ) )
	//{
	//	return false;
	//}

	//// Release the vertex array as it is no longer needed.
	//delete[] vertices;
	//vertices = 0;

	//// Release the index array as it is no longer needed.
	//delete[] indices;
	//indices = 0;

	//return true;
}
