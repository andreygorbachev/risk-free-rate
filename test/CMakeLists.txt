project(risk-free-rate-test)
#project(test)

include(FetchContent)
FetchContent_Declare(
  rapidcsv
  GIT_REPOSITORY https://github.com/d99kris/rapidcsv.git
  GIT_TAG        v8.75
)
FetchContent_MakeAvailable(rapidcsv)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)
FetchContent_MakeAvailable(googletest)

add_executable(${PROJECT_NAME}
  compounded_rate.cpp
  sonia.cpp
  sofr.cpp
  eurostr.cpp
  saron.cpp
  setup.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  risk-free-rate
  Calendar::calendar
  CouponSchedule::coupon-schedule
  Reset::reset
  rapidcsv
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data
)
