cmake_minimum_required(VERSION 4.0)
project(MTPorgramResistorProject)

# 设置C++标准
set(CMAKE_CXX_STANDARD 20)

# 包含MTDCPower子目录
add_subdirectory(MTPorgramResistor)

# 如果测试需要单独启用，可以在这里包含tests目录，或者单独在tests中构建。
# 这里我们选择在顶层中包含tests，但也可以让tests独立构建。
# 注意：我们通常希望测试在构建库之后构建，所以add_subdirectory顺序很重要。
#enable_testing()
#add_subdirectory(tests)