project("risk-free-rate")

add_library(${PROJECT_NAME} INTERFACE
  time_series.h
  resets.h
  compounded_index.h
  compounded_rate.h
)

target_include_directories(${PROJECT_NAME} INTERFACE .)
