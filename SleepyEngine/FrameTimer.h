#pragma once
#include <chrono>
#include "NumericDataTypes.h"

class FrameTimer
{
public:
	FrameTimer();
	f32 Mark();
	f32 Peek() const;
private:
	std::chrono::steady_clock::time_point m_Last;
};
