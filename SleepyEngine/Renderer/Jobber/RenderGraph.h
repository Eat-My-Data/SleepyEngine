#pragma once
#include <string>
#include <vector>
#include <memory>

class Graphics;

namespace Bind
{
	class RenderTarget;
	class DepthStencil;
}

namespace Rgph
{
	class Pass;
	class RenderQueuePass;
	class Source;
	class Sink;

	class RenderGraph
	{
	public:
		RenderGraph( Graphics& gfx );
		~RenderGraph();
		void Execute( Graphics& gfx ) noexcept;
		void Reset() noexcept;
		RenderQueuePass& GetRenderQueue( const std::string& passName );
	protected:
		void SetSinkTarget( const std::string& sinkName,const std::string& target );
		void AddGlobalSource( std::unique_ptr<Source> );
		void AddGlobalSink( std::unique_ptr<Sink> );
		void Finalize();
		void AppendPass( std::unique_ptr<Pass> pass );
	private:
		void LinkSinks( Pass& pass );
		void LinkGlobalSinks();
	private:
		std::vector<std::unique_ptr<Pass>> passes;
		std::vector<std::unique_ptr<Source>> globalSources;
		std::vector<std::unique_ptr<Sink>> globalSinks;
		std::shared_ptr<Bind::RenderTarget> backBufferTarget;
		std::shared_ptr<Bind::DepthStencil> masterDepth;
		bool finalized = false;
	};
}