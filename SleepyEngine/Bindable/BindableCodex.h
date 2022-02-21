#pragma once

#include "Bindable.h"
#include "BindableCodex.h"
#include <type_traits>
#include <memory>
#include <unordered_map>

namespace Bind
{
	class Codex
	{
	public:
		template<class T, typename...Params>
		static std::shared_ptr<T> Resolve( Graphics& gdi, Params&&...p ) noexcept
		{
			static_assert( std::is_base_of<Bindable, T>::value, "Can only resolve classes derived from Bindable" );
			return Get().Resolve_<T>( gdi, std::forward<Params>( p )... );
		}
	private:
		template<class T, typename...Params>
		std::shared_ptr<T> Resolve_( Graphics& gdi, Params&&...p ) noexcept
		{
			const auto key = T::GenerateUID( std::forward<Params>( p )... );
			const auto i = m_mapOfBinds.find( key );
			if ( i == m_mapOfBinds.end() )
			{
				auto bind = std::make_shared<T>( gdi, std::forward<Params>( p )... );
				m_mapOfBinds[key] = bind;
				return bind;
			}
			else
			{
				return std::static_pointer_cast<T>( i->second );
			}
		}
		static Codex& Get()
		{
			static Codex codex;
			return codex;
		}
	private:
		std::unordered_map<std::string, std::shared_ptr<Bindable>> m_mapOfBinds;
	};
}
