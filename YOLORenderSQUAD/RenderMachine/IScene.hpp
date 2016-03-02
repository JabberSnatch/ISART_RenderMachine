#ifndef __ISCENE_HPP__
#define __ISCENE_HPP__


class IScene
{
public:
	IScene() = default;
	IScene(const IScene&) = delete;
	IScene(IScene&&) = delete;
	virtual ~IScene() = default;

	virtual void	Render() = 0;
	virtual void	Shutdown() = 0;

	auto	operator = (const IScene&) -> IScene& = delete;
	auto	operator = (IScene&&) -> IScene& = delete;
};


#endif /*__ISCENE_HPP__*/