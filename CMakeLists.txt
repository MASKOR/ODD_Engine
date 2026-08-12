cmake_minimum_required(VERSION 3.16)
project(odd_engine)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
include(FetchContent)

find_package(pybind11 REQUIRED)
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

# cparse, built from source rather than the checked-in .o files (those are
# non-PIC and cannot go into a shared module).
#
# This MUST stay an OBJECT library. builtin-features.cpp contains nothing but
# static Startup objects whose constructors register every cparse operator; it
# exports no symbol anyone references. As a member of a static archive the
# linker drops it, no operators get registered, and parsing "-3.0" fails at
# runtime with "Invalid operator: -". Consumers add $<TARGET_OBJECTS:cparse> so
# the objects land on the link line directly, as the old .o files did.
add_library(cparse OBJECT
	include/cparse/builtin-features.cpp
	include/cparse/containers.cpp
	include/cparse/functions.cpp
	include/cparse/packToken.cpp
	include/cparse/shunting-yard.cpp
)
set_target_properties(cparse PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(cparse PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/cparse)

add_library(ODDEngine STATIC
	src/ODDEngineParser.cpp
	src/ODDEngineInference.cpp
	src/Ontology.cpp
	src/Database.cpp
	src/Expression.cpp
	src/Guardrail.cpp
	src/ODDEngineStructureHelper.cpp
)
set_target_properties(ODDEngine PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(ODDEngine PUBLIC ${REDLAND_INCLUDE_DIRS})
target_link_libraries(ODDEngine PUBLIC
	yaml-cpp::yaml-cpp
	${REDLAND_LIBRARIES}
)

pybind11_add_module(oddengine src/bindings.cpp $<TARGET_OBJECTS:cparse>)
target_link_libraries(oddengine PRIVATE ODDEngine cparse)

# puts oddengine*.so at the root of the wheel so `import oddengine` works
install(TARGETS oddengine LIBRARY DESTINATION .)


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
	$<TARGET_OBJECTS:cparse>
)


target_link_libraries( ${PROJECT_NAME}
	ODDEngine 
	${REDLAND_LIBRARIES} 
	${CORE_SHUNTING_YARD_LIB}
)
