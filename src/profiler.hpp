#ifndef PROFILER_HPP
#define PROFILER_HPP

#include <iostream>
#include <sstream>
#include <chrono>
#include <string>
#include <iomanip>

struct NOW
{
	auto operator()()
	{
		auto val = (std::chrono::steady_clock::now().time_since_epoch());
		return val;
	}
};


class PROFILER
{

public:

	PROFILER(std::string name) :
		m_start(NOW()()),
		m_name(std::move(name))
	{
		std::ostringstream oss;
		oss << "PROFILER: \"" << m_name << "\" started!" << std::endl;
		std::cout << oss.str();
	}

	~PROFILER()
	{
		double val = std::chrono::duration_cast<std::chrono::nanoseconds>(NOW()() - m_start).count();

		enum UNIT : unsigned                  { NS,   US,   MS,   S };
		const char * unit_names[] =           {"ns", "us", "ms", "s"};

		UNIT current_unit = NS;
		while (val >= 1000.0 && current_unit != S)
		{
			val /= 1000.0;
			current_unit = (UNIT)((uint64_t)current_unit + 1);
		}

		std::ostringstream oss;
		oss << std::setprecision(3) << "PROFILER: \"" << m_name << "\" finished in " << val << unit_names[(uint64_t)current_unit] << std::endl;
		std::cout << oss.str();
	}

private:
	decltype(NOW()()) m_start;
	const std::string m_name;
};

#endif
