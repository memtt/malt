############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 10/2025
#    LICENSE  : CeCILL-C
#    FILE     : src/reader/webview-2-rust/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
if (CMAKE_BUILD_TYPE STREQUAL "Release")
	set(RUST_BUILD_OPTION "--release")
	set(RUST_BUILD_DIR "target/release")
elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
	set(RUST_BUILD_OPTION "--release")
	set(RUST_BUILD_DIR "target/release")
elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(RUST_BUILD_OPTION "")
	set(RUST_BUILD_DIR "target/debug")
else()
	set(RUST_BUILD_OPTION "")
	set(RUST_BUILD_DIR "target/debug")
endif ()

############################################################
add_custom_target(
	malt-webview-2-rust
	ALL
	SOURCES build.rs
	        Cargo.toml
			#Cargo.lock
			src/c.rs
			src/c/malt.rs
	COMMAND LD_LIBRARY_PATH="${CMAKE_BINARY_DIR}/src/reader/libreader"
	        CARGO_TARGET_DIR="${CMAKE_CURRENT_BINARY_DIR}/target"
			CMAKE_BINARY_DIR="${CMAKE_BINARY_DIR}"
	        ${CARGO_BINARY} build ${RUST_BUILD_OPTION}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_dependencies(malt-webview-2-rust malt-reader)

############################################################
#add_test(
#	NAME TestWebview2Rust
#	COMMAND bash -c "LD_LIBRARY_PATH='${CMAKE_BINARY_DIR}/src/reader/libreader' CARGO_TARGET_DIR='${CMAKE_CURRENT_BINARY_DIR}/target' CMAKE_BINARY_DIR='${CMAKE_BINARY_DIR}' ${CARGO_BINARY} test"
#	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#)

############################################################
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${RUST_BUILD_DIR}/malt-webview-rust DESTINATION ${CMAKE_INSTALL_BINDIR} RENAME malt-webview)
