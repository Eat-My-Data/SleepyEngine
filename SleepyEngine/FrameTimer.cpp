#include "FrameTimer.h"

using namespace std::chrono;

FrameTimer::FrameTimer()
{
	m_Last = steady_clock::now();
}

f32 FrameTimer::Mark()
{
	const auto old = m_Last;
	m_Last = steady_clock::now();
	const duration<f32> frameTime = m_Last - old;
	return frameTime.count();
}

f32 FrameTimer::Peek() const
{
	return duration<f32>( steady_clock::now() - m_Last ).count();
}
