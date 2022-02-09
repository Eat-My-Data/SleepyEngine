#pragma once
#include <memory>
#include <string>
#include <vector>
#include <array>

class GraphicsDeviceInterface;

namespace Bind
{
	class RenderTarget;
	class DepthStencil;
}

namespace Rgph
{
	class Sink;
	class Source;

	class Pass
	{
	public:
		Pass( std::string name ) noexcept;
		virtual void Execute( GraphicsDeviceInterface& gfx ) const noexcept = 0;
		virtual void Reset() noexcept;
		const std::string& GetName() const noexcept;
		const std::vector<std::unique_ptr<Sink>>& GetSinks() const;
		Source& GetSource( const std::string& registeredName ) const;
		Sink& GetSink( const std::string& registeredName ) const;
		void SetSinkLinkage( const std::string& registeredName, const std::string& target );
		virtual void Finalize();
		virtual ~Pass();
	protected:
		void RegisterSink( std::unique_ptr<Sink> sink );
		void RegisterSource( std::unique_ptr<Source> source );
		void BindBufferResources( GraphicsDeviceInterface& gfx ) const noexcept;
		std::shared_ptr<Bind::RenderTarget> renderTarget;
		std::shared_ptr<Bind::DepthStencil> depthStencil;
	private:
		std::vector<std::unique_ptr<Sink>> sinks;
		std::vector<std::unique_ptr<Source>> sources;
		std::string name;
	};
}