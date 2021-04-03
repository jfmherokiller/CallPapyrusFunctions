cmake_minimum_required(VERSION 3.19)
include(GNUInstallDirs)
project(
	quickjs
	LANGUAGES C
)
if (MSVC)
	add_compile_definitions(
		_UNICODE
        CONFIG_JSX
        JS_STRICT_NAN_BOXING
        CONFIG_BIGNUM
        CONFIG_STORAGE
	)
	
	set(COMPILE_OPTIONS_DEBUG
		/Zc:inline-	# Keep unreferenced COMDAT
		/JMC	# Just My Code debugging
	)
	
	set(COMPILE_OPTIONS_RELEASE
		/Zc:inline	# Remove unreferenced COMDAT
		/JMC-	# Disable Just My Code debugging
	)

	add_compile_options(
		/Gy	# Enable Function-Level Linking
		/MP	# Build with Multiple Processes
		/Oi	# Generate Intrinsic Functions
		/sdl	# Enable Additional Security Checks
		/Zi	# Debug Information Format
		
		/permissive-	# Standards conformance
		
		/Zc:alignedNew	# C++17 over-aligned allocation
		/Zc:auto	# Deduce Variable Type
		/Zc:char8_t
		/Zc:__cplusplus	# Enable updated __cplusplus macro
		/Zc:externC
		/Zc:externConstexpr	# Enable extern constexpr variables
		/Zc:forScope	# Force Conformance in for Loop Scope
		/Zc:hiddenFriend
		/Zc:implicitNoexcept	# Implicit Exception Specifiers
		/Zc:lambda
		/Zc:noexceptTypes	# C++17 noexcept rules
		/Zc:preprocessor	# Enable preprocessor conformance mode
		/Zc:referenceBinding	# Enforce reference binding rules
		/Zc:rvalueCast	# Enforce type conversion rules
		/Zc:sizedDealloc	# Enable Global Sized Deallocation Functions
		/Zc:strictStrings	# Disable string literal type conversion
		/Zc:ternary	# Enforce conditional operator rules
		/Zc:threadSafeInit	# Thread-safe Local Static Initialization
		/Zc:tlsGuards
		/Zc:trigraphs	# Trigraphs Substitution
		/Zc:wchar_t	# wchar_t Is Native Type
		
		/experimental:external
		/external:anglebrackets
		/external:W0
		
		/W4	# Warning level (all warnings)
		/WX-# Warning level (warnings are errors)
		/wd4146
        /wd4703
        /wd4100
		"$<$<CONFIG:Debug>:${COMPILE_OPTIONS_DEBUG}>"
		"$<$<CONFIG:Release>:${COMPILE_OPTIONS_RELEASE}>"
	)

	set(LINK_OPTIONS_DEBUG
		/INCREMENTAL	# Link Incrementally
		/OPT:NOREF	# Optimizations (keep functions/data never referenced)
		/OPT:NOICF	# Optimizations (prevent identical COMDAT folding)
	)
	
	set(LINK_OPTIONS_RELEASE
		/INCREMENTAL:NO	# Link Incrementally
		/OPT:REF	# Optimizations (eliminate functions/data never referenced)
		/OPT:ICF	# Optimizations (perform identical COMDAT folding)
	)

	add_link_options(
		"$<$<CONFIG:Debug>:${LINK_OPTIONS_DEBUG}>"
		"$<$<CONFIG:Release>:${LINK_OPTIONS_RELEASE}>"
	)
endif()
add_subdirectory("storage/dybase" storagebit)
set(sources ${sources}
cutils.c
libregexp.c
libunicode.c
quickjs.c
quickjs-libc.c
libbf.c
storage/quickjs-storage.c
)
set(headers ${headers}
cutils.h
libregexp.h
libregexp-opcode.h
libunicode.h
libunicode-table.h
list.h
quickjs.h
quickjs-atom.h
quickjs-libc.h
quickjs-opcode.h
quickjs-jsx.h
storage/quickjs-storage.h
)
add_compile_definitions(
_CRT_SECURE_NO_WARNINGS
JS_STRICT_NAN_BOXING
CONFIG_BIGNUM
CONFIG_STORAGE
CONFIG_JSX
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
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
		"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
target_link_libraries(${PROJECT_NAME}
	PUBLIC
    storagelib::storagelib
)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
