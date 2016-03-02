#ifndef __NODE_HPP__
#define __NODE_HPP__


class Node
{
public:
	Node() = default;
	Node(const Node&) = delete;
	Node(Node&&) = delete;
	~Node() = default;

	auto	operator = (const Node&) -> Node& = delete;
	auto	operator = (Node&&) -> Node& = delete;

private:

};


#endif /*__NODE_HPP__*/
