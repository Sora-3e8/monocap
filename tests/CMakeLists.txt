add_executable(monocap-test src/monocap-test.cpp)
target_compile_definitions(monocap-test PRIVATE $<$<CONFIG:Debug>:__DEBUG__>)

# Basic memory bug catcher
target_compile_options(monocap-test PRIVATE -fsanitize=address -fno-omit-frame-pointer)
target_link_options(monocap-test PRIVATE -fsanitize=address)
target_link_libraries(monocap-test PRIVATE monocap)
target_include_directories(monocap-test PRIVATE "${CMAKE_SOURCE_DIR}/include")

#target_link_libraries(example-test PRIVATE library)
add_test(NAME "Initialization test" COMMAND monocap-test --constrTest)
add_test(NAME "Photo test" COMMAND monocap-test --photoTest)
add_custom_command(TARGET monocap-test POST_BUILD COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure)


