include(ShaderFiles.cmake)

############################ ADD_SHADERS ########################
function(convert_shader_file _infile _outfile)
    file(READ ${_infile} shaderFileInput)
    if (shaderFileInput)#not empty file
        file(WRITE ${_outfile} "\"" )
        foreach(_text ${shaderFileInput})
            string(REGEX REPLACE "\n" "\\\\n" _text ${_text})
            file(APPEND ${_outfile} "${_text};\\n" )
        endforeach()
        file(APPEND ${_outfile} "\"")
    endif()
endfunction(convert_shader_file)

foreach(_file ${shaders})
    convert_shader_file("${_file}" "${_file}.out")
endforeach()