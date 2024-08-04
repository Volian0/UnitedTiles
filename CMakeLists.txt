cmake_minimum_required(VERSION 3.23.3)
project(UnitedTiles)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf>=2.0.0)
PKG_SEARCH_MODULE(CURL REQUIRED libcurl)
PKG_SEARCH_MODULE(SDL2MIXER REQUIRED SDL2_mixer>=2.0.0)

if(WIN32)
  set(UT_CXX "-mwindows")
  set(UT_LIBRARIES "-static" ${SDL2MIXER_STATIC_LIBRARIES} ${SDL2_STATIC_LIBRARIES} ${SDL2IMAGE_STATIC_LIBRARIES} ${SDL2TTF_STATIC_LIBRARIES} ${CURL_STATIC_LIBRARIES})
  add_definitions(-DCURL_STATICLIB)
else()
  set(UT_LIBRARIES ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES} ${CURL_LIBRARIES} SDL2_mixer)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra ${UT_CXX}")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#set(THREADS_PREFER_PTHREAD_FLAG ON)
#find_package(Threads REQUIRED)

include_directories("include")
include_directories("src")
file(GLOB UnitedTilesSRC CONFIGURE_DEPENDS "src/*.h" "src/*.cpp" "src/tiles/*.cpp" "src/tiles/*.h" "src/ui/*.cpp" "src/ui/.*h")

add_executable(UnitedTiles ${UnitedTilesSRC})

target_include_directories(${PROJECT_NAME} PRIVATE
        include ${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIRS} ${SDL2_mixer_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE ${UT_LIBRARIES})
file(COPY res/ DESTINATION res/)

project(midi2usong)
add_executable(midi2usong tools/midi2usong/src/main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE
        include ${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE
        ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES})

        project(csv2db)
        add_executable(csv2db tools/csv2db/src/main.cpp)
        
        target_include_directories(${PROJECT_NAME} PRIVATE
                include ${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIRS})
        
        target_link_libraries(${PROJECT_NAME} PRIVATE
        ${UT_LIBRARIES})
        