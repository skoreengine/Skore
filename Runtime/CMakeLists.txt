file(GLOB_RECURSE SKORE_RUNTIME_SOURCES Source/*.hpp Source/*.cpp Source/*.h Source/*.c)
add_library(SkoreRuntime SHARED ${SKORE_RUNTIME_SOURCES})

target_include_directories(SkoreRuntime PUBLIC Source)