/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 06/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/webview-2-rust/build.rs
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
fn main() {
	let cmake_binary_dir = std::env::var("CMAKE_BINARY_DIR").unwrap();
	println!("cargo:rustc-link-lib=malt-profile");
	println!("cargo:rustc-link-search={}/src/reader/libreader/", cmake_binary_dir);
}
