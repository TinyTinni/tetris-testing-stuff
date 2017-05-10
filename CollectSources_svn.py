"""Module docstring.

todo
"""

import sys
import subprocess

from pathlib import Path



def list_source_prefix(file,varName):
    file.write("set("+varName+"\n")

def list_source_postfix(file):
    file.write(")\n")

def get_sources_svn(extensions): # returns all sources with the given extension which are under version control of svn
    """returns all version control sources as a list of strings. 
    This function has to call the 'svn' command, so make sure, that svn is in your path
    The pathes are all realative to the current directory
    """
    pathes = subprocess.check_output("svn st -v -q --depth=infinity")
    pathes = pathes.splitlines()
    result = []
    for f in pathes:
        name = f.decode()
        #decode name <ModFlag> <lastRev> <initialCommit> <author> <filename>
        column = name.split(" ")
        name = column[len(column)-1]
        if ( (column[0] != '!') & (column[0] != 'D') & (not Path(name).is_dir()) & (Path(name).suffix in extensions)):
            result.append(name)

    return result

def get_sources(extensions): #
    """returns all sources with one of the given extension of the current dir recursively
    The pathes are all realative to the current directory
    """
    dirs = [Path(".")]
    result = []

    while (len(dirs) > 0):
        path = dirs.pop()
        for subDir in path.iterdir():
            if (subDir.is_dir()):
                dirs.append(subDir)
            elif (extensions.count(subDir.suffix) > 0):
                result.append(subDir.relative_to(".").as_posix())

    return result

def cmake_list_sources(outputfile, varName, extensions, sourceGetter):
    output = open(outputfile, 'w')

    #prefix
    list_source_prefix(output, varName)

    #filenames
    sources = sourceGetter(extensions)
    for s in sources:
        #normalize name
        s = s.replace('\\','/')
        output.write('"'+s+'"\n')

    #postfix
    list_source_postfix(output)



def main(argv):
    #search_fct = get_sources_svn
    #if (argv.count("--local") > 0):
    #    search_fct = get_sources
    #else if (argv.count("--svn")):
    #    search_fct = get_sources_svn
    cmake_list_sources("./SourceFiles.cmake","sources",[".h",".hpp",".cpp",".cc"],get_sources_svn)
    cmake_list_sources("./ShaderFiles.cmake","shaders",[".vp",".geom",".fp",".cg",".cgfx"],get_sources_svn)
    

if __name__ == "__main__":
    main(sys.argv)
