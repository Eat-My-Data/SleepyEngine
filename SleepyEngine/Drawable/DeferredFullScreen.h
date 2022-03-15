#pragma once
#include "../Drawable/Drawable.h"

class DeferredFullScreen : public Drawable
{
public:
	DeferredFullScreen( Graphics& gfx );
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
};