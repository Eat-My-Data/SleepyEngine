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
	class PassInput;
	class PassOutput;

	class Pass
	{
	public:
		Pass( std::string name ) noexcept;
		virtual void Execute( GraphicsDeviceInterface& gfx ) const noexcept = 0;
		virtual void Reset() noexcept;
		const std::string& GetName() const noexcept;
		const std::vector<std::unique_ptr<PassInput>>& GetInputs() const;
		PassOutput& GetOutput( const std::string& registeredName ) const;
		PassInput& GetInput( const std::string& registeredName ) const;
		void SetInputSource( const std::string& registeredName, const std::string& target );
		virtual void Finalize();
		virtual ~Pass();
	protected:
		void RegisterInput( std::unique_ptr<PassInput> input );
		void RegisterOutput( std::unique_ptr<PassOutput> output );
		void BindBufferResources( GraphicsDeviceInterface& gfx ) const noexcept;
		std::shared_ptr<Bind::RenderTarget> renderTarget;
		std::shared_ptr<Bind::DepthStencil> depthStencil;
	private:
		std::vector<std::unique_ptr<PassInput>> inputs;
		std::vector<std::unique_ptr<PassOutput>> outputs;
		std::string name;
	};
}