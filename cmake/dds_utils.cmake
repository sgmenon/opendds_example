macro(LIST_TO_STRING _string _list)
    set(${_string})
    foreach(_item ${_list})
        set(${_string} "${${_string}} ${_item}")
    endforeach(_item)
endmacro(LIST_TO_STRING)


macro(FILTER_LIST _list _pattern _output)
    set(${_output})
    foreach(_item ${_list})
        if("${_item}" MATCHES ${_pattern})
            set(${_output} ${${_output}} ${_item})
        endif("${_item}" MATCHES ${_pattern})
    endforeach(_item)
endmacro(FILTER_LIST)


include(FindPkgConfig)
macro(GET_PKG_CONFIG_INFO _pkg _required)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(${_pkg}_PKG ${_required} ${_pkg})
        if(${_pkg}_PKG_CFLAGS_OTHER)
            LIST_TO_STRING(${_pkg}_CFLAGS "${${_pkg}_PKG_CFLAGS_OTHER}")
        else(${_pkg}_PKG_CFLAGS_OTHER)
            set(${_pkg}_CFLAGS "")
        endif(${_pkg}_PKG_CFLAGS_OTHER)
        set(${_pkg}_INCLUDE_DIRS ${${_pkg}_PKG_INCLUDE_DIRS})
        set(${_pkg}_LINK_LIBS ${${_pkg}_PKG_LIBRARIES})
        set(${_pkg}_LIBRARY_DIRS ${${_pkg}_PKG_LIBRARY_DIRS})
        if(${_pkg}_PKG_LDFLAGS_OTHER)
            LIST_TO_STRING(${_pkg}_LINK_FLAGS ${${_pkg}_PKG_LDFLAGS_OTHER})
        else(${_pkg}_PKG_LDFLAGS_OTHER)
            set(${_pkg}_LINK_FLAGS "")
        endif(${_pkg}_PKG_LDFLAGS_OTHER)
    else(PKG_CONFIG_FOUND)
        message(STATUS "Could not find pkg-config.")
        message(STATUS
            "You will need to set the following variables manually:")
        message(STATUS "${_pkg}_INCLUDE_DIRS ${_pkg}_CFLAGS_OTHER ${_pkg}_LINK_LIBS ${_pkg}_LIBRARY_DIRS ${_pkg}_LINK_FLAGS")
    endif(PKG_CONFIG_FOUND)
endmacro(GET_PKG_CONFIG_INFO)


macro(APPLY_PKG_CONFIG_DIRS _pkg)
    if(${_pkg}_INCLUDE_DIRS)
        include_directories(${${_pkg}_INCLUDE_DIRS})
    endif(${_pkg}_INCLUDE_DIRS)
    if(${_pkg}_LIBRARY_DIRS)
        link_directories(${${_pkg}_LIBRARY_DIRS})
    endif(${_pkg}_LIBRARY_DIRS)
endmacro(APPLY_PKG_CONFIG_DIRS)


macro(APPLY_PKG_CONFIG_TO_TGTS _pkg)
    if(${_pkg}_LINK_FLAGS)
        foreach(_tgt ${ARGN})
            set_target_properties(${_tgt} PROPERTIES
                LINK_FLAGS "${${_pkg}_LINK_FLAGS}")
        endforeach(_tgt)
    endif(${_pkg}_LINK_FLAGS)
    if(${_pkg}_LINK_LIBS)
        foreach(_tgt ${ARGN})
            target_link_libraries(${_tgt} ${${_pkg}_LINK_LIBS})
        endforeach(_tgt)
    endif(${_pkg}_LINK_LIBS)
endmacro(APPLY_PKG_CONFIG_TO_TGTS)


macro(APPLY_PKG_CONFIG_TO_SRCS _pkg)
    if(${_pkg}_CFLAGS)
        set_source_files_properties(${ARGN}
            PROPERTIES COMPILE_FLAGS "${${_pkg}_CFLAGS}")
    endif(${_pkg}_CFLAGS)
endmacro(APPLY_PKG_CONFIG_TO_SRCS)

macro(IDL_OUTPUTS _idl _dir _result _implResults)
    set(${_result} ${_dir}/${_idl}C.cpp ${_dir}/${_idl}C.h ${_dir}/${_idl}C.inl
        ${_dir}/${_idl}S.cpp ${_dir}/${_idl}S.h ${_idl}/${_idl}S.inl
		${_dir}/${_idl}TypeSupportC.cpp ${_dir}/${_idl}TypeSupportC.h ${_dir}/${_idl}TypeSupportC.inl
		${_dir}/${_idl}TypeSupportS.cpp ${_dir}/${_idl}TypeSupportS.h ${_dir}/${_idl}TypeSupportS.inl)
    set(${_implResults} ${_dir}/${_idl}TypeSupportImpl.cpp ${_dir}/${_idl}TypeSupportImpl.h)
endmacro(IDL_OUTPUTS)


macro(COMPILE_IDL _idl_file)
    message(STATUS "Running IDL Compiler on ${_idl_file}")
    get_filename_component(_idl ${_idl_file} NAME_WE)
    set(_idl_srcs_var ${_idl}_SRCS)
    set(_idl_impl_srcs_var ${_idl}_IMPL_SRCS)
    set(_workingdir ${CMAKE_CURRENT_BINARY_DIR})
    IDL_OUTPUTS(${_idl} ${_workingdir} ${_idl_srcs_var} ${_idl_impl_srcs_var})
	configure_file(${_idl_file} ${_workingdir}/${_idl}.idl COPYONLY)
    message(STATUS "Doing initial compile for ${_idl_file}")
    execute_process(COMMAND $ENV{ACE_ROOT}/bin/tao_idl -o ${_workingdir} ${_idl}.idl
                    COMMAND $ENV{DDS_ROOT}/bin/opendds_idl -o ${_workingdir} ${_idl}.idl
                    WORKING_DIRECTORY ${_workingdir}
                    INPUT_FILE ${_idl}.idl
                    )
    execute_process(COMMAND $ENV{ACE_ROOT}/bin/tao_idl -o ${_workingdir} -I$ENV{DDS_ROOT} ${_idl}TypeSupport.idl
                    WORKING_DIRECTORY ${_workingdir}
                    INPUT_FILE ${_idl}TypeSupport.idl
                    )
endmacro(COMPILE_IDL)

macro(PREPEND var prefix)
   set(listVar "")
   foreach(f ${ARGN})
      list(APPEND listVar "${prefix}${f}")
   endforeach(f)
   set(${var} "${listVar}")
endmacro(PREPEND)

macro(CREATE_COMPILE_IDL_COMMAND _idl_file)
    message(STATUS "Creating IDL compiler command for ${_idl_file}")
    get_filename_component(_idl ${_idl_file} NAME_WE)
    set(_idl_srcs_var ${_idl}_SRCS)
    set(_idl_impl_srcs_var ${_idl}_IMPL_SRCS)
    set(_workingdir ${CMAKE_CURRENT_BINARY_DIR})
    set(CUR_PATH $ENV{PATH})
    IDL_OUTPUTS(${_idl} ${_workingdir} ${_idl_srcs_var} ${_idl_impl_srcs_var})
    set(ALL_IDL_SRCS ${${_idl_srcs_var}} ${${_idl_impl_srcs_var}})
    PREPEND(DDS_INCLUDE_FLAG -I ${DDS_INCLUDE_DIRS})
    add_custom_command(OUTPUT ${ALL_IDL_SRCS} 
        COMMAND ${CMAKE_COMMAND} -E env "PATH=${CUR_PATH}" "DDS_ROOT=${DDS_ROOT}" "ACE_ROOT=${ACE_ROOT}" ${ACE_ROOT}/bin/tao_idl -o ${_workingdir} ${_workingdir}/${_idl}.idl
        COMMAND ${CMAKE_COMMAND} -E env "PATH=${CUR_PATH}" "DDS_ROOT=${DDS_ROOT}" "ACE_ROOT=${ACE_ROOT}" ${DDS_ROOT}/bin/opendds_idl -o ${_workingdir} ${_workingdir}/${_idl}.idl
        COMMAND ${CMAKE_COMMAND} -E env "PATH=${CUR_PATH}" "DDS_ROOT=${DDS_ROOT}" "ACE_ROOT=${ACE_ROOT}" ${ACE_ROOT}/bin/tao_idl -o ${_workingdir} ${DDS_INCLUDE_FLAG} -I${_workingdir} ${_workingdir}/${_idl}TypeSupport.idl
        DEPENDS ${_workingdir}/${_idl}.idl
        COMMENT "Compiling ${_idl}.idl" VERBATIM)
endmacro(CREATE_COMPILE_IDL_COMMAND)


macro(COMPILE_IDL_FILES)
    set(idl_srcs)
    foreach(idl ${ARGN})
        COMPILE_IDL(${idl})
    endforeach(idl)
endmacro(COMPILE_IDL_FILES)

macro(CREATE_IDL_FILES_COMMAND)
    set(idl_srcs)
    foreach(idl ${ARGN})
        CREATE_COMPILE_IDL_COMMAND(${idl})
    endforeach(idl)
endmacro(CREATE_IDL_FILES_COMMAND)