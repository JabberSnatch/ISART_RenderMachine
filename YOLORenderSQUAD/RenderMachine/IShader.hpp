#ifndef __ISHADER_HPP__
#define __ISHADER_HPP__


class IShader
{
public:
	IShader() = default;
	IShader(const IShader&) = delete;
	IShader(IShader&&) = delete;
	virtual ~IShader() = default;

	auto	operator = (const IShader&) -> IShader& = delete;
	auto	operator = (IShader&&) -> IShader& = delete;
};


#endif /*__ISHADER_HPP__*/