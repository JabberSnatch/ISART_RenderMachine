#ifndef __UUID_HPP__
#define __UUID_HPP__

#include <boost\uuid\uuid.hpp>
#include <boost\uuid\random_generator.hpp>


class UUID
{
public:
	UUID()
		:m_ID(boost::uuids::random_generator()()) {}
	UUID(const UUID&) = delete;
	UUID(UUID&&) = delete;
	~UUID() = default;

	auto	operator = (const UUID&) -> UUID& = delete;
	auto	operator = (UUID&&) -> UUID& = delete;

private:
	boost::uuids::uuid	m_ID;

};


#endif /*__UUID_HPP__*/