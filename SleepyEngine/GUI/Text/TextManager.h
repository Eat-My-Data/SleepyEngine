#pragma once
#include "../GUIElement.h"
#include "../../Bindable/BindableCommon.h"
#include "DirectXMath.h"

class TextManager
{
public:
	TextManager( GraphicsDeviceInterface& gdi );
public:
	void Draw( char* text, float x, float y );
private:
	GraphicsDeviceInterface* m_pGDI = nullptr;
	Bind::Texture* m_pTex;
private:
	struct FontType
	{
		float left, right;
		int size;
	};
	struct VertexType
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texture;
	};
	struct SentenceType
	{
		ID3D11Buffer* vertexBuffer, * indexBuffer;
		int vertexCount, indexCount, maxLength;
		float red, green, blue;
	};
private:
	FontType* m_pFont;
};