#pragma once
#include <queue>
#include <bitset>
#include <optional>

class Keyboard
{
	friend class Win32Window;
public:
	class Event
	{
	public:
		enum class Type
		{
			Press,
			Release,
		};
	private:
		Type type;
		unsigned char code;
	public:
		Event( Type type, unsigned char code ) noexcept
			:
			type( type ),
			code( code )
		{}
		bool IsPress() const noexcept
		{
			return type == Type::Press;
		}
		bool IsRelease() const noexcept
		{
			return type == Type::Release;
		}
		unsigned char GetCode() const noexcept
		{
			return code;
		}
	};
public:
	Keyboard() = default;
	Keyboard( const Keyboard& ) = delete;
	Keyboard& operator=( const Keyboard& ) = delete;
	// key event stuff
	bool KeyIsPressed( unsigned char keycode ) const noexcept;
	std::optional<Event> ReadKey() noexcept;
	bool KeyIsEmpty() const noexcept;
	void FlushKey() noexcept;
	// char event stuff
	std::optional<char> ReadChar() noexcept;
	bool CharIsEmpty() const noexcept;
	void FlushChar() noexcept;
	void Flush() noexcept;
	// autorepeat control
	void EnableAutorepeat() noexcept;
	void DisableAutorepeat() noexcept;
	bool AutorepeatIsEnabled() const noexcept;
private:
	void OnKeyPressed( unsigned char keycode ) noexcept;
	void OnKeyReleased( unsigned char keycode ) noexcept;
	void OnChar( char character ) noexcept;
	void ClearState() noexcept;
	template<typename T>
	static void TrimBuffer( std::queue<T>& buffer ) noexcept;
private:
	static constexpr unsigned int m_iNumKeys = 256u;
	static constexpr unsigned int m_iBufferSize = 16u;
	bool m_bAutorepeatEnabled = false;
	std::bitset<m_iNumKeys> m_KeyStates;
	std::queue<Event> m_qKeyBuffer;
	std::queue<char> m_qCharBuffer;
};