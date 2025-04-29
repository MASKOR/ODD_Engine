cmake_minimum_required(VERSION 3.16)
project(odd_engine)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
include(FetchContent)

find_package(PkgConfig REQUIRED)
pkg_check_modules(REDLAND REQUIRED redland)

include_directories(${REDLAND_INCLUDE_DIRS})
link_directories(${REDLAND_LIBRARY_DIRS})

FetchContent_Declare(
        yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
)
FetchContent_GetProperties(yaml-cpp)

if(NOT yaml-cpp_POPULATED)
    message(STATUS "Fetching yaml-cpp...")
    FetchContent_Populate(yaml-cpp)
    add_subdirectory(${yaml-cpp_SOURCE_DIR} ${yaml-cpp_BINARY_DIR})
endif()

add_library(ODDEngine STATIC 
	src/ODDEngineParser.cpp 
	src/ODDEngineInference.cpp 
	src/Ontology.cpp 
	src/Database.cpp 
	src/Expression.cpp 
	src/Guardrail.cpp
	
	include/cparse/builtin-features.o
	include/cparse/core-shunting-yard.o
)

target_include_directories(ODDEngine PUBLIC ${REDLAND_INCLUDE_DIRS})
target_link_libraries(ODDEngine PUBLIC 
	yaml-cpp::yaml-cpp 
	${REDLAND_LIBRARIES}
)


# Include-Verzeichnisse für die Bibliothek
target_include_directories(ODDEngine PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

# Installationsregeln
install(TARGETS ODDEngine yaml-cpp
        EXPORT ODDEngineTargets
        ARCHIVE DESTINATION lib)

install(DIRECTORY include/
        DESTINATION include)

install(EXPORT ODDEngineTargets
        FILE ODDEngineConfig.cmake
        NAMESPACE ODDEngine::
        DESTINATION lib/cmake/ODDEngine)


# For Testing
include_directories(include/cparse)

add_executable(${PROJECT_NAME} 
	src/main.cpp
	include/cparse/builtin-features.o
	include/cparse/core-shunting-yard.o
)


target_link_libraries( ${PROJECT_NAME}
	ODDEngine 
	${REDLAND_LIBRARIES} 
	${CORE_SHUNTING_YARD_LIB}
)
