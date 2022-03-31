#include "TestCube.h"
#include "../Geometry/Cube.h"
#include "../Bindable/BindableCommon.h"
#include "../Bindable/Bindables/ConstantBuffersEx.h"
#include "../Bindable/Bindables/TransformCbufScaling.h"
#include "../Libraries/imgui/imgui.h"
#include "../Bindable/Bindables/DynamicConstant.h"
#include "../Renderer/Jobber/TechniqueProbe.h"
#include "../Renderer/Channels.h"

TestCube::TestCube( Graphics& gfx,float size )
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Cube::MakeIndependentTextured();
	model.Transform( dx::XMMatrixScaling( size,size,size ) );
	model.SetNormalsIndependentFlat();
	const auto geometryTag = "$cube." + std::to_string( size );
	pVertices = VertexBuffer::Resolve( gfx,geometryTag,model.m_VBVertices );
	pIndices = IndexBuffer::Resolve( gfx,geometryTag,model.m_vecOfIndices );
	pTopology = Topology::Resolve( gfx,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	
	auto tcb = std::make_shared<TransformCbuf>( gfx );

	{
		Technique shade("Phong", Chan::main );
		{
			Step only( "lambertian" );

			only.AddBindable( Texture::Resolve( gfx,"Models\\brick_wall\\brick_wall_diffuse.jpg" ) );
			only.AddBindable( Sampler::Resolve( gfx ) );

			auto pvs = VertexShader::Resolve( gfx,"./Shaders/Bin/ShadowTest_VS.cso" );
			only.AddBindable( InputLayout::Resolve( gfx,model.m_VBVertices.GetLayout(),*pvs ) );
			only.AddBindable( std::move( pvs ) );

			only.AddBindable( PixelShader::Resolve( gfx,"./Shaders/Bin/ShadowTest_PS.cso" ) );
			
			Dcb::RawLayout lay;
			lay.Add<Dcb::Float3>( "specularColor" );
			lay.Add<Dcb::Float>( "specularWeight" );
			lay.Add<Dcb::Float>( "specularGloss" );
			auto buf = Dcb::Buffer( std::move( lay ) );
			buf["specularColor"] = dx::XMFLOAT3{ 1.0f,1.0f,1.0f };
			buf["specularWeight"] = 0.1f;
			buf["specularGloss"] = 20.0f;
			only.AddBindable( std::make_shared<Bind::CachingPixelConstantBufferEx>( gfx,buf,1u ) );


			only.AddBindable( Rasterizer::Resolve( gfx,false ) );

			only.AddBindable( tcb );

			shade.AddStep( std::move( only ) );
		}
		AddTechnique( std::move( shade ) );
	}
	//{
	//	Technique outline("Outline", Chan::main );
	//	{
	//		Step mask( "outlineMask" );

	//		// TODO: better sub-layout generation tech for future consideration maybe
	//		mask.AddBindable( InputLayout::Resolve( gfx,model.m_VBVertices.GetLayout(),*VertexShader::Resolve( gfx,"./Shaders/Bin/Solid_VS.cso" ) ) );

	//		mask.AddBindable( std::move( tcb ) );

	//		// TODO: might need to specify rasterizer when doubled-sided models start being used

	//		outline.AddStep( std::move( mask ) );
	//	}
	//	{
	//		Step draw( "outlineDraw" );

	//		Dcb::RawLayout lay;
	//		lay.Add<Dcb::Float4>( "color" );
	//		auto buf = Dcb::Buffer( std::move( lay ) );
	//		buf["color"] = DirectX::XMFLOAT4{ 1.0f,0.4f,0.4f,1.0f };
	//		draw.AddBindable( std::make_shared<Bind::CachingPixelConstantBufferEx>( gfx,buf,1u ) );

	//		// TODO: better sub-layout generation tech for future consideration maybe
	//		draw.AddBindable( InputLayout::Resolve( gfx,model.m_VBVertices.GetLayout(),*VertexShader::Resolve( gfx,"./Shaders/Bin/Solid_VS.cso" ) ) );
	//		
	//		draw.AddBindable( std::make_shared<TransformCbuf>( gfx ) );

	//		// TODO: might need to specify rasterizer when doubled-sided models start being used

	//		outline.AddStep( std::move( draw ) );
	//	}
	//	AddTechnique( std::move( outline ) );
	//}
	// shadow map technique
	{
		Technique map{ "ShadowMap",Chan::shadow,true };
		{
			Step draw( "shadowMap" );

			// TODO: better sub-layout generation tech for future consideration maybe
			draw.AddBindable( InputLayout::Resolve( gfx, model.m_VBVertices.GetLayout(), *VertexShader::Resolve( gfx, "./Shaders/Bin/Solid_VS.cso" ) ) );

			draw.AddBindable( std::make_shared<TransformCbuf>( gfx ) );

			// TODO: might need to specify rasterizer when doubled-sided models start being used

			map.AddStep( std::move( draw ) );
		}
		AddTechnique( std::move( map ) );
	}
}

void TestCube::SetPos( DirectX::XMFLOAT3 pos ) noexcept
{
	this->pos = pos;
}

void TestCube::SetRotation( float roll,float pitch,float yaw ) noexcept
{
	this->roll = roll;
	this->pitch = pitch;
	this->yaw = yaw;
}

DirectX::XMMATRIX TestCube::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw( roll,pitch,yaw ) *
		DirectX::XMMatrixTranslation( pos.x,pos.y,pos.z );
}

void TestCube::SpawnControlWindow( Graphics& gfx,const char* name ) noexcept
{
	if( ImGui::Begin( name ) )
	{
		ImGui::Text( "Position" );
		ImGui::SliderFloat( "X",&pos.x,-80.0f,80.0f,"%.1f" );
		ImGui::SliderFloat( "Y",&pos.y,-80.0f,80.0f,"%.1f" );
		ImGui::SliderFloat( "Z",&pos.z,-80.0f,80.0f,"%.1f" );
		ImGui::Text( "Orientation" );
		ImGui::SliderAngle( "Roll",&roll,-180.0f,180.0f );
		ImGui::SliderAngle( "Pitch",&pitch,-180.0f,180.0f );
		ImGui::SliderAngle( "Yaw",&yaw,-180.0f,180.0f );

		class Probe : public TechniqueProbe
		{
		public:
			void OnSetTechnique() override
			{
				using namespace std::string_literals;
				ImGui::TextColored( { 0.4f,1.0f,0.6f,1.0f },pTech->GetName().c_str() );
				bool active = pTech->IsActive();
				ImGui::Checkbox( ("Tech Active##"s + std::to_string( techIdx )).c_str(),&active );
				pTech->SetActiveState( active );
			}
			bool OnVisitBuffer( Dcb::Buffer& buf ) override
			{
				namespace dx = DirectX;
				float dirty = false;
				const auto dcheck = [&dirty]( bool changed ) {dirty = dirty || changed;};
				auto tag = [tagScratch = std::string{},tagString = "##" + std::to_string( bufIdx )]
				( const char* label ) mutable
				{
					tagScratch = label + tagString;
					return tagScratch.c_str();
				};

				if( auto v = buf["scale"]; v.Exists() )
				{
					dcheck( ImGui::SliderFloat( tag( "Scale" ),&v,1.0f,2.0f,"%.3f",3.5f ) );
				}
				if( auto v = buf["color"]; v.Exists() )
				{
					dcheck( ImGui::ColorPicker4( tag( "Color" ),reinterpret_cast<float*>(&static_cast<dx::XMFLOAT4&>(v)) ) );
				}
				if( auto v = buf["specularIntensity"]; v.Exists() )
				{
					dcheck( ImGui::SliderFloat( tag( "Spec. Intens." ),&v,0.0f,1.0f ) );
				}
				if( auto v = buf["specularPower"]; v.Exists() )
				{
					dcheck( ImGui::SliderFloat( tag( "Glossiness" ),&v,1.0f,100.0f,"%.1f",1.5f ) );
				}
				return dirty;
			}
		} probe;

		Accept( probe );
	}
	ImGui::End();
}
