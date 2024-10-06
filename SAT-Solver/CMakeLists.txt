cmake_minimum_required(VERSION 3.10)
project(SAT_Solver)

set(CMAKE_CXX_STANDARD 11)

add_executable(SAT_Solver ./src/SAT_Solver.cpp)

set(INPUT_DIR "${CMAKE_SOURCE_DIR}/CNF_Files")
set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/Results")

file(GLOB CNF_FILES "${INPUT_DIR}/*.cnf")

add_custom_target(process ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_DIR}
)

foreach(CNF_FILE ${CNF_FILES})
    get_filename_component(BASE_NAME ${CNF_FILE} NAME_WE)
    add_custom_command(TARGET process POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Processing ${CNF_FILE}..."
        COMMAND ${CMAKE_BINARY_DIR}/SAT_Solver < ${CNF_FILE} > ${OUTPUT_DIR}/${BASE_NAME}.out
        DEPENDS SAT_Solver
    )
endforeach()

set(PYTHON_VERIFIER "${CMAKE_SOURCE_DIR}/src/verification_SAT.py")

add_custom_target(verify
    COMMAND ${CMAKE_COMMAND} -E echo "Verifying all solutions..."
)

foreach(CNF_FILE ${CNF_FILES})
    get_filename_component(BASE_NAME ${CNF_FILE} NAME_WE)
    set(OUT_FILE "${OUTPUT_DIR}/${BASE_NAME}.out")
    add_custom_command(TARGET verify POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Verifying ${OUT_FILE} with ${CNF_FILE}..."
        COMMAND python3 ${PYTHON_VERIFIER} ${CNF_FILE} ${OUT_FILE}
    )
endforeach()

add_custom_target(clean_all
    COMMAND ${CMAKE_COMMAND} -E echo "Cleaning up..."
    COMMAND ${CMAKE_COMMAND} -E rm -f ${CMAKE_BINARY_DIR}/SAT_Solver
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${OUTPUT_DIR}
    COMMAND ${CMAKE_COMMAND} -E echo "Done."
)