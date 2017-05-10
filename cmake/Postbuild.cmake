include(ShaderFiles.cmake)

############################ REMOVE_SHADERS ########################
foreach(_file ${shaders})
    file(REMOVE "${_file}.out")
endforeach()