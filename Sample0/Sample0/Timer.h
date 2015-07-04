#pragma once

class Timer
{
private:
	static double _secondsPerCount;
	static __int64 _deltaTime;
	static __int64 _fromBeginning;
	static __int64 _lastTime;

public:
	static void Setup();
	static void Tick();
	static float GetDeltaTime();
	static float GetTimeFromBeginning();
};