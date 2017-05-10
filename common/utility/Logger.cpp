#include "Logger.h"

#include <sstream>
#include <vector>
#include <boost/log/trivial.hpp>

//constructs the buffer class for the stream
//buffer class send the stream buffer to boost::log, when stream is flushed
#define CONSTRUCT_BUFFER(NAME) \
class NAME ## _buffer_t : public std::stringbuf\
{\
protected:\
    int sync()\
    {\
        BOOST_LOG_TRIVIAL(NAME) << str();\
        str(std::string());\
        return 0;\
    }\
} NAME ## _buffer;


#define CONSTRUCT_STREAM(NAME) \
    std::ostream util:: NAME (& NAME ## _buffer);


#define CONSTRUCT_BUFFER_AND_STREAM(NAME) \
    CONSTRUCT_BUFFER(NAME)\
    CONSTRUCT_STREAM(NAME)


CONSTRUCT_BUFFER_AND_STREAM(trace);
CONSTRUCT_BUFFER_AND_STREAM(debug);
CONSTRUCT_BUFFER_AND_STREAM(info);
CONSTRUCT_BUFFER_AND_STREAM(warning);
CONSTRUCT_BUFFER_AND_STREAM(error);
CONSTRUCT_BUFFER_AND_STREAM(fatal);
