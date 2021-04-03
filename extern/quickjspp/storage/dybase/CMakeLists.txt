cmake_minimum_required(VERSION 3.19)
include(GNUInstallDirs)
project(
	storagelib
	LANGUAGES C
)

set(sources ${sources}
src/btree.cpp
src/database.cpp
src/dybase.cpp
src/file.cpp
src/pagepool.cpp

)
set(headers ${headers}
src/btree.h
src/buffer.h
src/database.h
src/file.h
src/hashtab.h
src/pagepool.h
src/stdtp.h
src/sync.h
include/dybase.h
)

add_library(
	${PROJECT_NAME}
	STATIC
	${headers}
	${sources}
)
source_group(
	TREE
		${CMAKE_CURRENT_SOURCE_DIR}
	FILES
		${headers}
		${sources}
)
target_include_directories(
	${PROJECT_NAME}
	PUBLIC
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
		"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
add_compile_definitions(
_CRT_SECURE_NO_WARNINGS
)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})