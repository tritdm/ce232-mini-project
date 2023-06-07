# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/mike/esp/esp-idf/components/bootloader/subproject"
  "/home/mike/Documents/tkhtn_kd/mini_project/esp32_source/build/bootloader"
  "/home/mike/Documents/tkhtn_kd/mini_project/esp32_source/build/bootloader-prefix"
  "/home/mike/Documents/tkhtn_kd/mini_project/esp32_source/build/bootloader-prefix/tmp"
  "/home/mike/Documents/tkhtn_kd/mini_project/esp32_source/build/bootloader-prefix/src/bootloader-stamp"
  "/home/mike/Documents/tkhtn_kd/mini_project/esp32_source/build/bootloader-prefix/src"
  "/home/mike/Documents/tkhtn_kd/mini_project/esp32_source/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/mike/Documents/tkhtn_kd/mini_project/esp32_source/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/mike/Documents/tkhtn_kd/mini_project/esp32_source/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
