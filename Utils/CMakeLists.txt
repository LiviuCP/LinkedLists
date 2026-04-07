project(Utils LANGUAGES C)

add_library(${PROJECT_NAME} ${LIB_TYPE}
    codeutils.c
    testobjects.c
    bitoperations.c
)
