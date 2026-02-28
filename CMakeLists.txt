# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)
set(EXTRA_COMPONENTS_DIRS components/)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

if(FORD)
  add_compile_definitions(CONFIG_SSID="funkbox-ford-")
  add_compile_definitions(CONFIG_PWD="funkbox-ford-pwd-")
else() 
  add_compile_definitions(CONFIG_SSID="funkbox-vectra-")
  add_compile_definitions(CONFIG_PWD="funkbox-vectra-pwd-")
endif()

project(smurf-box)