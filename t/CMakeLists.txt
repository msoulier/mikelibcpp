function(create_test NAME)
    add_executable(test_${NAME} ${CMAKE_CURRENT_SOURCE_DIR}/test_${NAME}.cpp)
    target_include_directories(test_${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
    target_link_libraries(test_${NAME} mikecpp ssl crypto curl)
    add_test(NAME test_${NAME}
             COMMAND test_${NAME})
endfunction()

create_test(crypto)
#create_test(logger)
#create_test(mstring)
#create_test(safequeue)

enable_testing()
