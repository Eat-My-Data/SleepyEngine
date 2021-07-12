#include "Keyboard.h"

bool Keyboard::KeyIsPressed( unsigned char keycode ) const noexcept
{
	return m_KeyStates[keycode];
}

std::optional<Keyboard::Event> Keyboard::ReadKey() noexcept
{
	if ( m_qKeyBuffer.size() > 0u )
	{
		Keyboard::Event e = m_qKeyBuffer.front();
		m_qKeyBuffer.pop();
		return e;
	}
	return {};
}

bool Keyboard::KeyIsEmpty() const noexcept
{
	return m_qKeyBuffer.empty();
}

std::optional<char> Keyboard::ReadChar() noexcept
{
	if ( m_qCharBuffer.size() > 0u )
	{
		unsigned char charcode = m_qCharBuffer.front();
		m_qCharBuffer.pop();
		return charcode;
	}
	return {};
}

bool Keyboard::CharIsEmpty() const noexcept
{
	return m_qCharBuffer.empty();
}

void Keyboard::FlushKey() noexcept
{
	m_qKeyBuffer = std::queue<Event>();
}

void Keyboard::FlushChar() noexcept
{
	m_qCharBuffer = std::queue<char>();
}

void Keyboard::Flush() noexcept
{
	FlushKey();
	FlushChar();
}

void Keyboard::EnableAutorepeat() noexcept
{
	m_bAutorepeatEnabled = true;
}

void Keyboard::DisableAutorepeat() noexcept
{
	m_bAutorepeatEnabled = false;
}

bool Keyboard::AutorepeatIsEnabled() const noexcept
{
	return m_bAutorepeatEnabled;
}

void Keyboard::OnKeyPressed( unsigned char keycode ) noexcept
{
	m_KeyStates[keycode] = true;
	m_qKeyBuffer.push( Keyboard::Event( Keyboard::Event::Type::Press, keycode ) );
	TrimBuffer( m_qKeyBuffer );
}

void Keyboard::OnKeyReleased( unsigned char keycode ) noexcept
{
	m_KeyStates[keycode] = false;
	m_qKeyBuffer.push( Keyboard::Event( Keyboard::Event::Type::Release, keycode ) );
	TrimBuffer( m_qKeyBuffer );
}

void Keyboard::OnChar( char character ) noexcept
{
	m_qCharBuffer.push( character );
	TrimBuffer( m_qCharBuffer );
}

void Keyboard::ClearState() noexcept
{
	m_KeyStates.reset();
}

template<typename T>
void Keyboard::TrimBuffer( std::queue<T>& buffer ) noexcept
{
	while ( buffer.size() > m_iBufferSize )
	{
		buffer.pop();
	}
}