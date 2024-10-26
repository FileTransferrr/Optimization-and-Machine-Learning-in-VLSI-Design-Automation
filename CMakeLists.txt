cmake_minimum_required(VERSION 3.10)
project(Floorplan)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(floorplan ./src/floorplan.cpp)

set(INPUT_FILE "${CMAKE_SOURCE_DIR}/src/input.txt")
set(OUTPUT_FILE "${CMAKE_SOURCE_DIR}/Results/result.txt")

set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/Results")

add_custom_target(process ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_DIR}
    COMMAND ${CMAKE_COMMAND} -E touch ${OUTPUT_FILE}
    COMMAND floorplan < ${INPUT_FILE} > ${OUTPUT_FILE}
    DEPENDS floorplan
)

add_custom_target(clean_all
    COMMAND ${CMAKE_COMMAND} -E echo "Cleaning up..."
    COMMAND ${CMAKE_COMMAND} -E rm -f ${CMAKE_BINARY_DIR}/SAT_Solver
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${OUTPUT_DIR}
    COMMAND ${CMAKE_COMMAND} -E echo "Done."
)