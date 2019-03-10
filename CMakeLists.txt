cmake_minimum_required(VERSION 3.12)
set(NAME pertotman)
set(VERSION "")

project("${NAME}")

add_custom_target(assets ALL)
add_custom_command(
    TARGET assets
    COMMAND cp -r ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/bin/assets
)
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${CMAKE_BINARY_DIR}/bin/assets)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL REQUIRED)
include_directories(${SDL_INCLUDE_DIRS})

find_package(SDL_image REQUIRED)
include_directories(${SDL_IMAGE_INCLUDE_DIRS})

find_package(SDL_ttf REQUIRED)
include_directories(${SDL_TTF_INCLUDE_DIRS})

find_package(Freetype REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

# Add all cpp and hpp files
file( GLOB SOURCE ./*.cpp )
file( GLOB HEADERS ./*.hpp )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(MAIN_CPP "Main.cpp")

add_executable(${NAME} ${MAIN_CPP} ${SOURCE} ${HEADERS})

IF(${PEDANTIC_ON})
if(MSVC)
    target_compile_options(${NAME} PRIVATE /W4 /WX)
else(MSVC)
    target_compile_options(${NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
endif(MSVC)
ENDIF()

target_compile_options(${NAME} PRIVATE -g)
target_include_directories(${NAME} PUBLIC)
target_link_libraries(${NAME} ${OPENGL_LIBRARIES} ${SDL_IMAGE_LIBRARIES} ${SDL_TTF_LIBRARIES} ${SDL_LIBRARY} ${FREETYPE_LIBRARIES} ${PNG_LIBRARIES})
