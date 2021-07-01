#include "FrameTimer.h"

using namespace std::chrono;

FrameTimer::FrameTimer()
{
	last = steady_clock::now();
}

f32 FrameTimer::Mark()
{
	const auto old = last;
	last = steady_clock::now();
	const duration<f32> frameTime = last - old;
	return frameTime.count();
}

f32 FrameTimer::Peek() const
{
	return duration<f32>( steady_clock::now() - last ).count();
}
