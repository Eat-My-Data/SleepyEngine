#include "CameraContainer.h"
#include "../Libraries/imgui/imgui.h"
#include "Camera.h"
#include "../Graphics/Graphics.h"

void CameraContainer::SpawnWindow()
{
	if ( ImGui::Begin( "Cameras" ) )
	{
		if ( ImGui::BeginCombo( "Active Camera", cameras[selected]->GetName().c_str() ) )
		{
			for ( int i = 0; i < std::size( cameras ); i++ )
			{
				const bool isSelected = i == selected;
				if ( ImGui::Selectable( cameras[i]->GetName().c_str(), isSelected ) )
				{
					selected = i;
				}
			}
			ImGui::EndCombo();
		}

		GetCamera().SpawnControlWidgets();
	}
	ImGui::End();
}

void CameraContainer::Bind( Graphics& gfx )
{
	gfx.SetCamera( GetCamera().GetMatrix() );
}

void CameraContainer::AddCamera( std::unique_ptr<Camera> pCam )
{
	cameras.push_back( std::move( pCam ) );
}

Camera& CameraContainer::GetCamera()
{
	return *cameras[selected];
}

CameraContainer::~CameraContainer()
{}