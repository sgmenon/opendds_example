file(GLOB_RECURSE Original_IDL_Files
  "${CMAKE_SOURCE_DIR}/IDL/*.idl"
)
CREATE_IDL_FILES_COMMAND(${Original_IDL_Files})
file(GLOB_RECURSE Local_IDL_Files
  "${CMAKE_CURRENT_BINARY_DIR}/*.idl"
)
file(GLOB_RECURSE in1_files
  "${CMAKE_CURRENT_BINARY_DIR}/*.cpp"
  "${CMAKE_CURRENT_BINARY_DIR}/*.hpp"
  "${CMAKE_CURRENT_BINARY_DIR}/*.h"
)
if (NOT in1_files)
    #create an initial version of these files
    COMPILE_IDL_FILES(${Original_IDL_Files})
    file(GLOB_RECURSE in1_files
      "${CMAKE_CURRENT_BINARY_DIR}/*.cpp"
      "${CMAKE_CURRENT_BINARY_DIR}/*.hpp"
      "${CMAKE_CURRENT_BINARY_DIR}/*.h"
    )
endif()
add_custom_target(CompileIDLFiles ALL
    DEPENDS ${ALL_IDL_SRCS})
add_definitions(${DDS_DEFINITIONS})
add_library(idl_generated_lib STATIC 
    ${in1_files}
)
add_dependencies(idl_generated_lib CompileIDLFiles)

include_directories("${CMAKE_CURRENT_BINARY_DIR}" 
                     ${PROJECT_BINARY_DIR})

target_link_libraries(idl_generated_lib LINK_PUBLIC ${DDS_LIBRARIES})

target_include_directories(idl_generated_lib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<INSTALL_INTERFACE:include> 
)