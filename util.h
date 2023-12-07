#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <boost/beast.hpp>

static inline void fail(boost::beast::error_code ec, char const* what) {
  std::cerr << what << ": " << ec.message() << "\n";
}

#endif
