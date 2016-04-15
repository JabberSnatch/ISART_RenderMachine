#include "Clock.hpp"


void
Clock::Start()
{
	m_LastTick = m_SystemClock.now();
	m_Now = m_SystemClock.now();

	m_DeltaTime = m_Now - m_LastTick;
}


void
Clock::Tick()
{
	m_LastTick = m_Now;
	m_Now = m_SystemClock.now();

	m_DeltaTime = m_Now - m_LastTick;
}
