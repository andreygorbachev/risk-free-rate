project("risk-free-rate")

add_library(${PROJECT_NAME} INTERFACE
  resets.h
)

target_include_directories(${PROJECT_NAME} INTERFACE .)
