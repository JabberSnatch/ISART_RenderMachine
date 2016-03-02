#ifndef __UUID_HPP__
#define __UUID_HPP__

#include <boost\uuid\uuid.hpp>
#include <boost\uuid\random_generator.hpp>


class UUID
{
public:
	UUID()
		:m_ID(boost::uuids::random_generator()()) {}
	UUID(const UUID&) = default;
	UUID(UUID&&) = delete;
	~UUID() = default;

	bool	operator < (const UUID& _other) const 
	{ return memcmp(m_ID.data, _other.m_ID.data, m_ID.size()) < 0; }

	auto	operator = (const UUID&) -> UUID& = default;
	auto	operator = (UUID&&) -> UUID& = delete;

private:
	boost::uuids::uuid	m_ID;

};


#endif /*__UUID_HPP__*/