# src/CMakeLists.txt

# 搜索当前目录下所有 .cc 文件
aux_source_directory(. MPRPC_SRC)

# 生成共享库 libmprpc.so
add_library(mprpc SHARED ${MPRPC_SRC})

# include 目录
target_include_directories(mprpc PUBLIC
    ${PROJECT_SOURCE_DIR}/src/include
)

# 链接 muduo 和其他依赖
target_link_libraries(mprpc
    mymuduo        # 你自己的 libmymuduo.so
    pthread
)
