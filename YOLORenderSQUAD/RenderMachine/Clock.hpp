#ifndef __CLOCK_HPP__
#define __CLOCK_HPP__

#include <chrono>


class Clock
{
public:
	typedef std::chrono::high_resolution_clock		SystemClock_t;
	typedef std::chrono::time_point<SystemClock_t>	TimePoint_t;
	typedef std::chrono::duration<double>			DeltaTime_t;

	Clock() = default;
	Clock(const Clock&) = delete;
	Clock(Clock&&) = delete;
	~Clock() = default;

	void	Start();

	void	Tick();
	double	DeltaTime() { return m_DeltaTime.count() / m_Multiplier; }
	void	SetMultiplier(double _v) { if(m_Multiplier != 0.0) m_Multiplier = _v; }

	auto	operator = (const Clock&) -> Clock& = delete;
	auto	operator = (Clock&&) -> Clock& = delete;

private:
	double			m_Multiplier = 1.0;
	DeltaTime_t		m_DeltaTime;

	TimePoint_t		m_Now;
	TimePoint_t		m_LastTick;

	SystemClock_t	m_SystemClock;

};


#endif /*__CLOCK_HPP__*/