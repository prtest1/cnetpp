#cmake_minimum_required(VERSION 3.1)
cmake_minimum_required(VERSION 2.8)
project(cnetpp)

include_directories("${CMAKE_SOURCE_DIR}/src" )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -pthread -std=c++11 -I./src/")

file(GLOB HTTP_HEADER_FILES "src/http/*.h")
file(GLOB HTTP_SOURCE_FILES "src/http/*.cc")
file(GLOB TCP_HEADER_FILES "src/tcp/*.h")
file(GLOB TCP_SOURCE_FILES "src/tcp/*.cc")
file(GLOB BASE_HEADER_FILES "src/base/*.h")
file(GLOB BASE_SOURCE_FILES "src/base/*.cc")
file(GLOB CONCURRENCY_HEADER_FILES "src/concurrency/*.h")
file(GLOB CONCURRENCY_SOURCE_FILES "src/concurrency/*.cc")

set(SOURCE_FILES
    ${BASE_SOURCE_FILES}
    ${CONCURRENCY_SOURCE_FILES}
    ${HTTP_SOURCE_FILES}
    ${TCP_SOURCE_FILES})

# build shared library
add_library(cnetpp SHARED ${SOURCE_FILES})
set_target_properties(cnetpp PROPERTIES VERSION 1.0 SOVERSION 1)

# build static library
add_library(cnetpp_s STATIC ${SOURCE_FILES})
set_target_properties(cnetpp_s PROPERTIES OUTPUT_NAME "cnetpp")

link_directories("${PROJECT_BINARY_DIR}")
set(TEST_SERVER_SOURCE_FILES src/test/server.cc)
add_executable(cnetpp_server_test ${TEST_SERVER_SOURCE_FILES})
target_link_libraries(cnetpp_server_test cnetpp)

set(TEST_CLIENT_SOURCE_FILES src/test/client.cc)
add_executable(cnetpp_client_test ${TEST_CLIENT_SOURCE_FILES})
target_link_libraries(cnetpp_client_test cnetpp)

install(TARGETS cnetpp cnetpp_s
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(FILES ${BASE_HEADER_FILES} DESTINATION include/cnetpp/base)
install(FILES ${CONCURRENCY_HEADER_FILES} DESTINATION include/cnetpp/concurrency)
install(FILES ${HTTP_HEADER_FILES} DESTINATION include/cnetpp/http)
install(FILES ${TCP_HEADER_FILES} DESTINATION include/cnetpp/tcp)

