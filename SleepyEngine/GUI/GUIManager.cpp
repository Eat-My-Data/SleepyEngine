#include "GUIManager.h"
#include "../Bindable/Bindables/ConstantBuffers.h"

void GUIManager::Draw( GraphicsDeviceInterface& gdi )
{
	gdi.GetContext()->OMSetRenderTargets( 1u, gdi.GetTarget(), nullptr );
	m_GUITransformCbuf.guiTransform = m_GUITransformCamera.GetViewMatrix();
	Bind::VertexConstantBuffer<GUITransform>::VertexConstantBuffer(gdi, m_GUITransformCbuf).Bind( gdi );
	m_ChooseRenderTechnique.Draw( gdi );
}
