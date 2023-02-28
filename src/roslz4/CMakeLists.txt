set_source_files_properties(
    lz4s.c
    PROPERTIES COMPILE_FLAGS "-Wno-sign-compare")

set_source_files_properties(
    lz4s.c xxhash.c
    PROPERTIES COMPILE_DEFINITIONS "XXH_NAMESPACE=ROSLZ4_")

target_sources(${PROJECT_NAME} PRIVATE
  lz4s.c
  xxhash.c
)