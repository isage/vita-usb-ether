cmake_minimum_required(VERSION 3.12)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

set(SHORT_NAME usb_ether)
project(${SHORT_NAME})
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11")

add_executable(ether-enabler
  ether-enabler.c
)

set_target_properties(ether-enabler
  PROPERTIES LINK_FLAGS
  -nostdlib
)

target_link_libraries(ether-enabler
  SceDebugForDriver_stub
  SceKernelSuspendForDriver_stub
  SceModulemgrForDriver_stub
  SceSysmemForDriver_stub
  SceSysclibForDriver_stub
  SceSysrootForDriver_stub
  SceUsbServForDriver_stub
  SceSblACMgrForDriver_stub
  taihenForKernel_stub
  taihenModuleUtils_stub
)

vita_create_self(ether-enabler.skprx ether-enabler
  CONFIG ${CMAKE_SOURCE_DIR}/ether-enabler.yml
  UNSAFE
)
