# The tests are based on the GTEST framework
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(
        ${PROJECT_NAME}_test
        emd_wrap_test.cpp
        cost_matrix_test.cpp
)

target_link_libraries(
        ${PROJECT_NAME}_test
        PRIVATE
        gtest_main
        ${PROJECT_NAME}
)

# Enable CMAKE's test runner to discover tests
include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}_test)