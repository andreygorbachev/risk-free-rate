project("risk-free-rate")

add_library(${PROJECT_NAME} INTERFACE
  round.h
  resets.h
  compounded_index.h
  compounded_rate.h
  inverse_modified_following.h
)

target_include_directories(${PROJECT_NAME} INTERFACE .)
