#pragma once
#include <string>
#include <fstream>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>
//per one time include
#define VERTEX_EXT 0
#define GEOMETRY_EXT 1
#define FRAGMENT_EXT 2
namespace shader
{
inline std::string readFile(const std::string &filename)
{
    std::ifstream file(filename);  
    if (!file.is_open())
        throw std::exception("Could not open file");
    file.seekg(0, std::ios::end);
    std::string result(static_cast<size_t>(file.tellg()),'\0');
    file.seekg(0, std::ios::beg);
    file.read(&result[0], result.size());
    return result;
}
}