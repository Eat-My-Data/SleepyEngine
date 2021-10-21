#include "Mouse.h"
#include <Windows.h>

std::pair<u32, u32> Mouse::GetPos() const noexcept
{
	return { m_iX,m_iY };
}

std::optional<Mouse::RawDelta> Mouse::ReadRawDelta() noexcept
{
	if ( m_qRawDeltaBuffer.empty() )
	{
		return std::nullopt;
	}
	const RawDelta d = m_qRawDeltaBuffer.front();
	m_qRawDeltaBuffer.pop();
	return d;
}

u32 Mouse::GetPosX() const noexcept
{
	return m_iX;
}

u32 Mouse::GetPosY() const noexcept
{
	return m_iY;
}

bool Mouse::IsInWindow() const noexcept
{
	return m_bIsInWindow;
}

bool Mouse::LeftIsReleased() const noexcept
{
	for ( auto& mouseEvent : m_qBuffer )
	{
		if ( mouseEvent.GetType() == Event::Type::LRelease )
			return true;
	}
	return false;
}

bool Mouse::LeftIsPressed() const noexcept
{
	return m_bLeftIsPressed;
}

bool Mouse::RightIsPressed() const noexcept
{
	return m_bRightIsPressed;
}

std::optional<Mouse::Event> Mouse::Read() noexcept
{
	if ( m_qBuffer.size() > 0u )
	{
		Mouse::Event e = m_qBuffer.front();
		m_qBuffer.pop_front();
		return e;
	}
	return {};
}

void Mouse::Flush() noexcept
{
	m_qBuffer = std::deque<Event>();
}

void Mouse::EnableRaw() noexcept
{
	m_bRawEnabled = true;
}

void Mouse::DisableRaw() noexcept
{
	m_bRawEnabled = false;
}

bool Mouse::RawEnabled() const noexcept
{
	return m_bRawEnabled;
}

void Mouse::OnMouseMove( u32 newx, u32 newy ) noexcept
{
	m_iX = newx;
	m_iY = newy;

	m_qBuffer.push_back( Mouse::Event( Mouse::Event::Type::Move, *this ) );
	TrimBuffer();
}

void Mouse::OnMouseLeave() noexcept
{
	m_bIsInWindow = false;
	m_qBuffer.push_back( Mouse::Event( Mouse::Event::Type::Leave, *this ) );
	TrimBuffer();
}

void Mouse::OnMouseEnter() noexcept
{
	m_bIsInWindow = true;
	m_qBuffer.push_back( Mouse::Event( Mouse::Event::Type::Enter, *this ) );
	TrimBuffer();
}

void Mouse::OnRawDelta( u32 dx, u32 dy ) noexcept
{
	m_qRawDeltaBuffer.push( { dx,dy } );
	TrimBuffer();
}

void Mouse::OnLeftPressed( u32 m_iX, u32 m_iY ) noexcept
{
	m_bLeftIsPressed = true;

	m_qBuffer.push_back( Mouse::Event( Mouse::Event::Type::LPress, *this ) );
	TrimBuffer();
}

void Mouse::OnLeftReleased( u32 m_iX, u32 m_iY ) noexcept
{
	m_bLeftIsPressed = false;

	m_qBuffer.push_back( Mouse::Event( Mouse::Event::Type::LRelease, *this ) );
	TrimBuffer();
}

void Mouse::OnRightPressed( u32 m_iX, u32 m_iY ) noexcept
{
	m_bRightIsPressed = true;

	m_qBuffer.push_back( Mouse::Event( Mouse::Event::Type::RPress, *this ) );
	TrimBuffer();
}

void Mouse::OnRightReleased( u32 m_iX, u32 m_iY ) noexcept
{
	m_bRightIsPressed = false;

	m_qBuffer.push_back( Mouse::Event( Mouse::Event::Type::RRelease, *this ) );
	TrimBuffer();
}

void Mouse::OnWheelUp( u32 m_iX, u32 m_iY ) noexcept
{
	m_qBuffer.push_back( Mouse::Event( Mouse::Event::Type::WheelUp, *this ) );
	TrimBuffer();
}

void Mouse::OnWheelDown( u32 m_iX, u32 m_iY ) noexcept
{
	m_qBuffer.push_back( Mouse::Event( Mouse::Event::Type::WheelDown, *this ) );
	TrimBuffer();
}

void Mouse::TrimBuffer() noexcept
{
	while ( m_qBuffer.size() > m_iBufferSize )
	{
		m_qBuffer.pop_back();
	}
}

void Mouse::TrimRawInputBuffer() noexcept
{
	while ( m_qRawDeltaBuffer.size() > m_iBufferSize )
	{
		m_qRawDeltaBuffer.pop();
	}
}

void Mouse::OnWheelDelta( u32 m_iX, u32 m_iY, u32 delta ) noexcept
{
	m_iWheelDeltaCarry += delta;
	// generate events for every 120 
	while ( m_iWheelDeltaCarry >= WHEEL_DELTA )
	{
		m_iWheelDeltaCarry -= WHEEL_DELTA;
		OnWheelUp( m_iX, m_iY );
	}
	while ( m_iWheelDeltaCarry <= -WHEEL_DELTA )
	{
		m_iWheelDeltaCarry += WHEEL_DELTA;
		OnWheelDown( m_iX, m_iY );
	}
}