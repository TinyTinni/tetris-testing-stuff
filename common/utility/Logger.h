#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <ostream>

//Note: Besides of the boost trivial log, this method is not thread safe
namespace util
{
    extern std::ostream trace;
    extern std::ostream debug;
    extern std::ostream info;
    extern std::ostream warning;
    extern std::ostream error;
    extern std::ostream fatal;

} // namespace util
#endif //__LOGGER_H__