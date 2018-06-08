file(GLOB ddsadapter_SRC
    "*.h"
    "*.hpp"
    "*.cpp"
)

add_definitions(${DDS_DEFINITIONS})

add_library(DDSAdapter STATIC
    ${ddsadapter_SRC}
)

add_dependencies(DDSAdapter idl_generated_lib)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}" 
                     ${PROJECT_BINARY_DIR})

target_link_libraries(DDSAdapter LINK_PUBLIC ${Boost_LIBRARIES} ${DDS_LIBRARIES} idl_generated_lib)

target_include_directories(DDSAdapter PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include> 
)