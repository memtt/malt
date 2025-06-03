/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tools/Backtrace.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
fn main() {
	let cmake_binary_dir = std::env::var("CMAKE_BINARY_DIR").unwrap();
	println!("cargo:rustc-link-lib=malt-profile");
	println!("cargo:rustc-link-search={}/src/reader/libreader/", cmake_binary_dir);
}
