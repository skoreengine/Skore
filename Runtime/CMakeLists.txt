file(GLOB_RECURSE SKORE_RUNTIME_SOURCES Source/*.hpp Source/*.cpp Source/*.h Source/*.c)
add_library(SkoreRuntime SHARED ${SKORE_RUNTIME_SOURCES})

target_include_directories(SkoreRuntime PUBLIC Source)



#tests

file(GLOB_RECURSE SKORE_RUNTIME_TESTS_SOURCES Tests/*.hpp Tests/*.cpp Tests/*.h Tests/*.c)
add_executable(SkoreRuntimeTests ${SKORE_RUNTIME_TESTS_SOURCES})
target_link_libraries(SkoreRuntimeTests PUBLIC SkoreRuntime)
target_include_directories(SkoreRuntimeTests PUBLIC Thirdparty/doctest)
target_include_directories(SkoreRuntimeTests PUBLIC Tests)