project(Utils LANGUAGES C CXX)

add_library(${PROJECT_NAME} ${LIB_TYPE}
    utils.cpp
    codeutils.c
    testobjects.c
    bitoperations.c
)

target_compile_definitions(${PROJECT_NAME} PRIVATE UTILS_LIBRARY)
