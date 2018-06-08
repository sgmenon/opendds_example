file(GLOB ddsexample_SRC
    "*.h"
    "*.hpp"
    "*.cpp"
)

add_definitions(${DDS_DEFINITIONS})

add_executable(StockQuoter_Publisher 
    ${ddsexample_SRC}
)

add_dependencies(StockQuoter_Publisher DDSAdapter)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}" 
                     ${PROJECT_BINARY_DIR})

target_link_libraries(StockQuoter_Publisher LINK_PUBLIC ${Boost_LIBRARIES} ${DDS_LIBRARIES} DDSAdapter)