file(GLOB ddsexample_SRC
    "*.h"
    "*.hpp"
    "*.cpp"
)

add_definitions(${DDS_DEFINITIONS})

add_executable(StockQuoter_Subscriber 
    ${ddsexample_SRC}
)

add_dependencies(StockQuoter_Subscriber DDSAdapter)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}" 
                     ${PROJECT_BINARY_DIR})

target_link_libraries(StockQuoter_Subscriber LINK_PUBLIC ${Boost_LIBRARIES} ${DDS_LIBRARIES} DDSAdapter)