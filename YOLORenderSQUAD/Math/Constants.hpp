#ifndef __CONSTANTS_HPP__
#define __CONSTANTS_HPP__


struct Constants
{
	Constants() = delete;
	Constants(const Constants&) = delete;
	Constants(Constants&&) = delete;
	~Constants() = delete;

	auto	operator = (const Constants&) -> Constants& = delete;
	auto	operator = (Constants&&) -> Constants& = delete;

	static double PI() { return 3.14159265359; }
	static double Deg2Rad() { return (3.14159265359 / 180.); }
	static double Rad2Deg() { return (180. / 3.14159265359); }

};

using MC = Constants;

#endif /*__CONSTANTS_HPP__*/