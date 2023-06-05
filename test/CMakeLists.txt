project(risk-free-rate-test)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.13.0
)
FetchContent_MakeAvailable(googletest)

add_executable(${PROJECT_NAME}
  time_series.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  risk-free-rate
  Calendar::calendar
  CouponSchedule::coupon-schedule
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})