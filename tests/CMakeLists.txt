add_executable(mc_decompress src/main.cpp)

target_link_libraries(mc_decompress PRIVATE MeshCodec)