project(aoc2020)

macro(aoc_app NAME)
    add_executable(${NAME}
            ${NAME}/main.cpp
    )

    target_include_directories(${NAME}
            ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
endmacro()

aoc_app(day1)
