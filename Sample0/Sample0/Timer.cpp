#include "Common.h"
#include "Timer.h"

double Timer::_secondsPerCount = 0.;
__int64 Timer::_deltaTime = 0;
__int64 Timer::_fromBeginning = 0;
__int64 Timer::_lastTime = 0; 

void Timer::Setup()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&_lastTime);
	__int64 countsPerSecounds;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSecounds);
	_secondsPerCount = 1. / (double)countsPerSecounds;

	_deltaTime = _fromBeginning = 0;
}

void Timer::Tick()
{
	__int64 tmp_time;
	QueryPerformanceCounter((LARGE_INTEGER*)&tmp_time);
	_deltaTime = tmp_time - _lastTime;
	_lastTime = tmp_time;
	_fromBeginning += _deltaTime;
}

float Timer::GetDeltaTime()
{
	return _deltaTime * _secondsPerCount;
}

float Timer::GetTimeFromBeginning()
{
	return _fromBeginning * _secondsPerCount;
}