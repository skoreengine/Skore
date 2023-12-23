add_subdirectory(Thirdparty/GLFW)

string(TIMESTAMP TODAY "%Y%m%d")
add_compile_definitions(SK_VERSION="Pre-Alpha.0.1.${TODAY}")


file(GLOB_RECURSE SKORE_RUNTIME_SOURCES Source/*.hpp Source/*.cpp Source/*.h Source/*.c)

file(GLOB_RECURSE SKORE_RUNTIME_THIRDPARTY_SOURCES
				Thirdparty/glad/src/glad.c
)

add_library(SkoreRuntime SHARED ${SKORE_RUNTIME_SOURCES} ${SKORE_RUNTIME_THIRDPARTY_SOURCES})

target_include_directories(SkoreRuntime PUBLIC Source)
target_include_directories(SkoreRuntime PRIVATE Thirdparty/glad/include)

target_link_libraries(SkoreRuntime PRIVATE glfw)

#tests

file(GLOB_RECURSE SKORE_RUNTIME_TESTS_SOURCES Tests/*.hpp Tests/*.cpp Tests/*.h Tests/*.c)
add_executable(SkoreRuntimeTests ${SKORE_RUNTIME_TESTS_SOURCES})
target_link_libraries(SkoreRuntimeTests PUBLIC SkoreRuntime)
target_include_directories(SkoreRuntimeTests PUBLIC Thirdparty/doctest)
target_include_directories(SkoreRuntimeTests PUBLIC Tests)