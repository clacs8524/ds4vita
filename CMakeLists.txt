cmake_minimum_required(VERSION 2.8)

set(CMAKE_SYSTEM_NAME "Generic")
set(CMAKE_C_COMPILER "arm-vita-eabi-gcc")
set(CMAKE_CXX_COMPILER "arm-vita-eabi-g++")

project(ds4vita)

set(CMAKE_C_FLAGS "-Wall -Wno-error -O3 -std=gnu99")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11 -fno-rtti -fno-exceptions")

include_directories(
)

link_directories(
	${CMAKE_CURRENT_BINARY_DIR}
)

option(RELEASE "Release build" OFF)

if (RELEASE)
	add_definitions(-DRELEASE)
endif(RELEASE)

add_executable(${PROJECT_NAME}.elf
	main.c
	log.c
)

set_source_files_properties(main.c log.c PROPERTIES LANGUAGE C)

target_link_libraries(${PROJECT_NAME}.elf
	gcc
	taihenForKernel_stub
	SceSysclibForDriver_stub
	SceSysmemForDriver_stub
	SceSysmemForKernel_stub
	SceModulemgrForDriver_stub
	SceModulemgrForKernel_stub
	SceThreadmgrForDriver_stub
	SceIofilemgrForDriver_stub
	SceBtForDriver_stub
	SceKernelSuspendForDriver_stub
	SceCtrlForDriver_stub
)

set_target_properties(${PROJECT_NAME}.elf
	PROPERTIES LINK_FLAGS "-nostdlib -Wl,-q"
	COMPILE_FLAGS "-D__VITA_KERNEL__"
)

add_custom_target(${PROJECT_NAME}.skprx ALL
	COMMAND vita-elf-create -n -e ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}.yml ${PROJECT_NAME}.elf ${PROJECT_NAME}.velf
	COMMAND vita-make-fself -c ${PROJECT_NAME}.velf ${PROJECT_NAME}.skprx
)
add_dependencies(${PROJECT_NAME}.skprx ${PROJECT_NAME}.elf)

add_custom_target(send
	COMMAND curl -T ${PROJECT_NAME}.skprx ftp://$(PSVITAIP):1337/ur0:/tai/ds4vita.skprx
	DEPENDS ${PROJECT_NAME}.skprx
)
