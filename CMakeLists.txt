cmake_minimum_required(VERSION 3.17)

project(aoc2020)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

macro(aoc_app NAME)
        add_executable(${NAME}
                ${NAME}/main.cpp
        )

        target_include_directories(${NAME}
                PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
        )
endmacro()

aoc_app(day1)
aoc_app(day2)
aoc_app(day3)
aoc_app(day4)
aoc_app(day5)
aoc_app(day6)
aoc_app(day7)
aoc_app(day8)
aoc_app(day9)
aoc_app(day10)
aoc_app(day11)
aoc_app(day12)
aoc_app(day13)
aoc_app(day14)
aoc_app(day15)
aoc_app(day16)
aoc_app(day17)
aoc_app(day18)
aoc_app(day19)
aoc_app(day20)
aoc_app(day21)
aoc_app(day22)
aoc_app(day23)
aoc_app(day24)
aoc_app(day25)
