file(
  GLOB_RECURSE
  slog_src
  *.cc
  ../include/slog/*
)

add_library(
  slog_static
  STATIC
  ${slog_src}
)

add_library(
  slog
  SHARED
  ${slog_src}
)

install (TARGETS slog
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib
         RUNTIME DESTINATION bin)

install (TARGETS slog_static
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib
         RUNTIME DESTINATION bin)
