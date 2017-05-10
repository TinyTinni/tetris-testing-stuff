# author: Matthias Moeller

#groups sources passed to the function according to the directory of the source tree
#therefore, the CMAKE_CURRENT_SOURCE_DIR will be stripped and the rest relative path will be set as group
function(group_sources)
	foreach(f ${ARGN})
		get_filename_component(abs_path ${f} PATH)
		source_group("" FILES ${f})
		if(abs_path) # if  it is not in the CMAKE_ROOT_SOURCE_DIR
		#strip of the cmake source dir
			string(REPLACE ${CMAKE_CURRENT_SOURCE_DIR} "" rel_path ${abs_path})
			if (rel_path)
				string(REPLACE "/" "\\" group ${rel_path})
				source_group(${group} FILES ${f})
			endif(rel_path)
		endif(abs_path)
	endforeach(f)
endfunction(group_sources)


# Groups all sources of the specified target according to the dir tree
# this will affect especially the VS Filters
function(group_target_sources target)
	get_target_property(s ${target} SOURCES)
	group_sources(${s})
endfunction(group_target_sources)