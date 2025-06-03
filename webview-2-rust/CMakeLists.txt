############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.4
#    DATE     : 10/2024
#    LICENSE  : CeCILL-C
#    FILE     : src/lib/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2014 - 2020
#    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
############################################################

############################################################
if (CMAKE_BUILD_TYPE STREQUAL "Release")
	set(RUST_BUILD_OPTION "--release")
	set(RUST_BUILD_DIR "target/release")
elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfos")
	set(RUST_BUILD_OPTION "--release")
	set(RUST_BUILD_DIR "target/release")
elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(RUST_BUILD_OPTION "")
	set(RUST_BUILD_DIR "target/debug")
else()
	set(RUST_BUILD_OPTION "")
	set(RUST_BUILD_DIR "target/debug")
endif ()

add_custom_target(
	malt-webview-2-rust
	ALL
	SOURCES build.rs
	        Cargo.toml
			Cargo.lock
			src/c.rs
			src/c/malt.rs
	COMMAND LD_LIBRARY_PATH="${CMAKE_BINARY_DIR}/src/reader/libreader"
	        CARGO_TARGET_DIR="${CMAKE_CURRENT_BINARY_DIR}/target"
			CMAKE_BINARY_DIR="${CMAKE_BINARY_DIR}"
	        cargo build ${RUST_BUILD_OPTION}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${RUST_BUILD_DIR}/malt-webview-new DESTINATION ${CMAKE_INSTALL_BINDIR})