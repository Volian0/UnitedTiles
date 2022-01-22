cmake_minimum_required(VERSION 3.22.1)
project(UnitedTiles)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include_directories("include")
file(GLOB UnitedTilesSRC CONFIGURE_DEPENDS "src/*.h" "src/*.cpp" "src/tiles/*.cpp" "src/tiles/*.h" "src/ui/*.cpp" "src/ui/.*h")

add_executable(UnitedTiles ${UnitedTilesSRC})

#find_package(SDL2)

INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf>=2.0.0)

target_include_directories(${PROJECT_NAME} PRIVATE
        include ${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE
        ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES})

set(CURL_LIBRARY "-lcurl")
find_package(CURL REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_DL_LIBS} Threads::Threads ${CURL_LIBRARIES})

file(COPY res/ DESTINATION res/)