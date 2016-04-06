#ifndef __BOOST_TOK_HELPER_HPP__
#define __BOOST_TOK_HELPER_HPP__


#include <boost/tokenizer.hpp>

using bSeparator = boost::char_separator<char>;
using bTokenizer = boost::tokenizer<bSeparator>;
using bIterator = boost::token_iterator_generator<bSeparator>::type;


#endif // __BOOST_TOK_HELPER_HPP__
