cmake_minimum_required(VERSION 3.6)
project(mstreams)

set(CMAKE_CXX_STANDARD 11 )
set(CMAKE_CXX_FLAGS -fPIC)
set(CMAKE_BUILD_TYPE DEBUG)
file(GLOB SOURCE_FILES
        "src/addon.cc"
        "src/mproducer.h"
        "src/mproducer.cc"
        "src/mconsumer.h"
        "src/mconsumer.cc"
        "src/mstreams.h"
        "src/mstreams.cc")


add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

target_include_directories(${PROJECT_NAME}
        PRIVATE ${CMAKE_JS_INC}
        PRIVATE /opt/mapr/include/librdkafka
        PRIVATE $ENV{HOME}/.nvm/versions/node/v$ENV{NODE_VERSION}/include/node
        PRIVATE ${CMAKE_SOURCE_DIR}/node_modules/node-addon-api)

target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB} rdkafka)
#add_executable(${PROJECT_NAME} ${SOURCE_FILES})