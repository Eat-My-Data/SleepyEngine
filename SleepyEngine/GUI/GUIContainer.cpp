#include "GUIContainer.h"

void GUIContainer::AddElement( GUIElement* element )
{
	// TODO: 
	// - Figure out how to automatically adjust all elements
	AdjustContainer( element );
	m_vecOfElements.push_back( std::move( element ) );
}

void GUIContainer::Draw( GraphicsDeviceInterface& gdi )
{

	for ( u32 i = 0; i < m_vecOfElements.size(); i++ )
	{
		m_vecOfElements[i]->Draw( gdi );
	}
	m_vecOfElements.clear();
}

void GUIContainer::AdjustContainer( GUIElement* element )
{
	m_iContainerWidth += element->m_iWidth;
	m_iContainerHeight += element->m_iHeight;
}
