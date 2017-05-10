namespace shader
{
#if FILEEXT == VERTEX_EXT
    namespace vertex{
#define EXTENSION .vp
#elif FILEEXT == GEOMETRY_EXT
    namespace geometry{
#define EXTENSION .geom
#elif FILEEXT == FRAGMENT_EXT
    namespace fragment{
#define EXTENSION .fp
#else
#define SKIP_FILE
#endif

#ifndef SKIP_FILE
    struct BOOST_PP_CAT(FILENAME, Shader)
    {
        inline static std::string getCode()
        {
#define INCLUDESTRING BOOST_PP_CAT(Shaders/,BOOST_PP_CAT(FILENAME,EXTENSION))
#ifdef _DEBUG
            return readFile(BOOST_PP_STRINGIZE(INCLUDESTRING));
#else
#define RESULTSTRING1 )
            return std::string(
#include BOOST_PP_STRINGIZE(BOOST_PP_CAT(../,BOOST_PP_CAT(INCLUDESTRING,.out))
                );
#endif
#undef INCLUDESTRING
        }
    };

#undef EXTENSION
}//namespace
#endif

#undef SKIP_FILE
}