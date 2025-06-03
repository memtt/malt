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
use std::os::raw::{c_void,c_char,c_double,c_int};
use std::ffi::{CString, CStr};

/**********************************************************/
type MaltReaderHandler = *mut c_void;
type c_bool = bool;
type c_size_t = usize;
type c_ssize_t = isize;

/**********************************************************/
pub struct MaltCReader
{
	handler: MaltReaderHandler,	
}

unsafe impl Send for MaltCReader {}
unsafe impl Sync for MaltCReader {}

/**********************************************************/
extern "C" {
	/*//lifecycle
	fn malt_reader_init(fname: *const c_char) -> *mut c_void;
	fn malt_reader_destroy(reader: *mut c_void);
	fn malt_reader_free_response(response: *const c_char);
	fn malt_reader_free_last_response(reader: *mut c_void);
	//requests
	fn malt_reader_get_summary(reader: *mut c_void) -> *const c_char;
	fn malt_reader_get_summary_v2(reader: *mut c_void) -> *const c_char;
	fn malt_reader_get_flat_profile(reader: *mut c_void) -> *const c_char;*/

	// lifecycle
	fn malt_reader_init(fname: *const c_char, progress_bar: c_bool, auto_delete: c_bool) -> MaltReaderHandler;
	fn malt_reader_fini(reader: MaltReaderHandler);
	fn malt_reader_free_response(response: *const c_char);
	fn malt_reader_free_last_response(reader: MaltReaderHandler);
	// requests
	fn malt_reader_get_proc_map(reader: MaltReaderHandler) -> *const c_char;
	fn malt_reader_get_debug_stack_list(reader: MaltReaderHandler) -> *const c_char;
	fn malt_reader_get_global_variables(reader: MaltReaderHandler) -> *const c_char;
	fn malt_reader_get_summary(reader: MaltReaderHandler) -> *const c_char;
	fn malt_reader_get_summary_v2(reader: MaltReaderHandler) -> *const c_char;
	fn malt_reader_get_file_lines_flat_profile(reader: MaltReaderHandler, file: *const c_char, total: c_bool) -> *const c_char;
	fn malt_reader_get_flat_function_profile(reader: MaltReaderHandler, own: c_bool, total: c_bool) -> *const c_char;
	fn malt_reader_get_proc_map_distr(reader: MaltReaderHandler) -> *const c_char;
	fn malt_reader_get_scatter(reader: MaltReaderHandler) -> *const c_char;
	fn malt_reader_get_size_map(reader: MaltReaderHandler) -> *const c_char;
	fn malt_reader_get_realloc_map(reader: MaltReaderHandler) -> *const c_char;
	fn malt_reader_get_timed_values(reader: MaltReaderHandler) -> *const c_char;
	fn malt_reader_get_filtered_stacks_on_file_line(reader: MaltReaderHandler, file: *const c_char, line: c_size_t) -> *const c_char;
	fn malt_reader_get_filterd_stacks_on_symbol(reader: MaltReaderHandler, func: *const c_char) -> *const c_char;
	fn malt_reader_get_call_stack_next_level(reader: MaltReaderHandler, parentStackId: c_size_t, parentDepth: c_size_t, function: *const c_char, file: *const c_char, line: c_int) -> *const c_char;
	fn malt_reader_get_stacks_mem(reader: MaltReaderHandler) -> *const c_char;
	fn malt_reader_get_stack_info_on_function(reader: MaltReaderHandler, thread_id: c_size_t) -> *const c_char;
	fn malt_reader_get_call_tree(reader: MaltReaderHandler, nodeId: c_ssize_t, depth: c_ssize_t, height: c_ssize_t, min_cost: c_double, func: *const c_char, metric: *const c_char, is_ratio: c_bool) -> *const c_char;
	fn malt_reader_is_source_file(reader: MaltReaderHandler, path: *const c_char) -> c_bool;
}

/**********************************************************/
impl MaltCReader
{
	pub fn new(fname: &str) -> MaltCReader {
		let cfname = CString::new(fname).expect("ok");
		unsafe {
			let handler = malt_reader_init(cfname.into_raw(), true, false);
			return MaltCReader{handler: handler};
		}
	}

	pub fn get_proc_map(&self) -> String {
		let c_str = unsafe { malt_reader_get_proc_map(self.handler) };
		let cc_str = unsafe {CStr::from_ptr(c_str)};
		let res = cc_str.to_str().unwrap().to_string();
		unsafe {malt_reader_free_response(c_str)};
		return res;
	}

	pub fn get_debug_stack_list(&self) -> String {
		let c_str = unsafe { malt_reader_get_debug_stack_list(self.handler) };
		let cc_str = unsafe {CStr::from_ptr(c_str)};
		let res = cc_str.to_str().unwrap().to_string();
		unsafe {malt_reader_free_response(c_str)};
		return res;
	}

	pub fn get_global_variables(&self) -> String {
		let c_str = unsafe { malt_reader_get_global_variables(self.handler) };
		let cc_str = unsafe {CStr::from_ptr(c_str)};
		let res = cc_str.to_str().unwrap().to_string();
		unsafe {malt_reader_free_response(c_str)};
		return res;
	}

	pub fn get_summary(&self) -> String {
		let c_str = unsafe { malt_reader_get_summary(self.handler) };
		let cc_str = unsafe {CStr::from_ptr(c_str)};
		let res = cc_str.to_str().unwrap().to_string();
		unsafe {malt_reader_free_response(c_str)};
		return res;
	}

	pub fn get_summary_v2(&self) -> String {
		let c_str = unsafe { malt_reader_get_summary_v2(self.handler) };
		let cc_str = unsafe {CStr::from_ptr(c_str)};
		let res = cc_str.to_str().unwrap().to_string();
		unsafe {malt_reader_free_response(c_str)};
		return res;
	}

	pub fn get_file_lines_flat_profile(&self, file: &str, total: bool) -> String {
		let cfile = CString::new(file).expect("Convert to CSstring");
		let c_str = unsafe { malt_reader_get_file_lines_flat_profile(self.handler, cfile.into_raw(), total) };
		let cc_str = unsafe {CStr::from_ptr(c_str)};
		let res = cc_str.to_str().unwrap().to_string();
		unsafe {malt_reader_free_response(c_str)};
		return res;
	}

	pub fn get_flat_function_profile(&self, own: bool, total: bool) -> String {
		let c_str = unsafe { malt_reader_get_flat_function_profile(self.handler, own, total) };
		let cc_str = unsafe {CStr::from_ptr(c_str)};
		let res = cc_str.to_str().unwrap().to_string();
		unsafe {malt_reader_free_response(c_str)};
		return res;
	}

	pub fn get_proc_map_distre(&self) -> String {
		let c_str = unsafe { malt_reader_get_proc_map_distr(self.handler) };
		let cc_str = unsafe {CStr::from_ptr(c_str)};
		let res = cc_str.to_str().unwrap().to_string();
		unsafe {malt_reader_free_response(c_str)};
		return res;
	}

	pub fn get_scatter(&self) -> String {
		let c_str = unsafe { malt_reader_get_scatter(self.handler) };
		let cc_str = unsafe {CStr::from_ptr(c_str)};
		let res = cc_str.to_str().unwrap().to_string();
		unsafe {malt_reader_free_response(c_str)};
		return res;
	}

	pub fn get_size_map(&self) -> String {
		let c_str = unsafe { malt_reader_get_size_map(self.handler) };
		let cc_str = unsafe {CStr::from_ptr(c_str)};
		let res = cc_str.to_str().unwrap().to_string();
		unsafe {malt_reader_free_response(c_str)};
		return res;
	}

	pub fn get_realloc_map(&self) -> String {
		let c_str = unsafe { malt_reader_get_realloc_map(self.handler) };
		let cc_str = unsafe {CStr::from_ptr(c_str)};
		let res = cc_str.to_str().unwrap().to_string();
		unsafe {malt_reader_free_response(c_str)};
		return res;
	}

	pub fn get_timed_values(&self) -> String {
		let c_str = unsafe { malt_reader_get_timed_values(self.handler) };
		let cc_str = unsafe {CStr::from_ptr(c_str)};
		let res = cc_str.to_str().unwrap().to_string();
		unsafe {malt_reader_free_response(c_str)};
		return res;
	}

	pub fn get_filtered_stacks_on_file_line(&self, file: &str, line: usize) -> String {
		let cfile = CString::new(file).expect("Convert to CSstring");
		let c_str = unsafe { malt_reader_get_filtered_stacks_on_file_line(self.handler, cfile.into_raw(), line) };
		let cc_str = unsafe {CStr::from_ptr(c_str)};
		let res = cc_str.to_str().unwrap().to_string();
		unsafe {malt_reader_free_response(c_str)};
		return res;
	}

	pub fn get_filterd_stacks_on_symbol(&self, func: &str) -> String {
		let cfunc = CString::new(func).expect("Convert to CSstring");
		let c_str = unsafe { malt_reader_get_filterd_stacks_on_symbol(self.handler, cfunc.into_raw()) };
		let cc_str = unsafe {CStr::from_ptr(c_str)};
		let res = cc_str.to_str().unwrap().to_string();
		unsafe {malt_reader_free_response(c_str)};
		return res;
	}

	pub fn get_call_stack_next_level(&self, parentStackId: c_size_t, parentDepth: c_size_t, function: &str, file: &str, line: c_int) -> String {
		let cfunction = CString::new(function).expect("Convert to CSstring");
		let cfile = CString::new(file).expect("Convert to CSstring");
		let c_str = unsafe { malt_reader_get_call_stack_next_level(self.handler, parentStackId, parentDepth, cfunction.into_raw(), cfile.into_raw(), line) };
		let cc_str = unsafe {CStr::from_ptr(c_str)};
		let res = cc_str.to_str().unwrap().to_string();
		unsafe {malt_reader_free_response(c_str)};
		return res;
	}

	pub fn get_stacks_mem(&self) -> String {
		let c_str = unsafe { malt_reader_get_stacks_mem(self.handler) };
		let cc_str = unsafe {CStr::from_ptr(c_str)};
		let res = cc_str.to_str().unwrap().to_string();
		unsafe {malt_reader_free_response(c_str)};
		return res;
	}

	pub fn get_stack_info_on_function(&self, thread_id: usize) -> String {
		let c_str = unsafe { malt_reader_get_stack_info_on_function(self.handler, thread_id) };
		let cc_str = unsafe {CStr::from_ptr(c_str)};
		let res = cc_str.to_str().unwrap().to_string();
		unsafe {malt_reader_free_response(c_str)};
		return res;
	}

	pub fn get_call_tree(&self, nodeId: isize, depth: isize, height: isize, min_cost: f64, func: &str, metric: &str, is_ratio: bool) -> String {
		let cfunc = CString::new(func).expect("Convert to CSstring");
		let cmetric = CString::new(metric).expect("Convert to CSstring");
		let c_str = unsafe { malt_reader_get_call_tree(self.handler, nodeId, depth, height, min_cost, cfunc.into_raw(), cmetric.into_raw(), is_ratio) };
		let cc_str = unsafe {CStr::from_ptr(c_str)};
		let res = cc_str.to_str().unwrap().to_string();
		unsafe {malt_reader_free_response(c_str)};
		return res;
	}

	pub fn is_source_file(&self, file: &str) -> bool {
		let cfile = CString::new(file).expect("Convert to CSstring");
		unsafe { return malt_reader_is_source_file(self.handler, cfile.into_raw()) };
	}
}

impl Drop for MaltCReader
{
	fn drop(&mut self)
	{
		unsafe{
			malt_reader_fini(self.handler);
		}
	}
}

#[cfg(test)]
mod tests {
	use actix_web::body::MessageBody;
use pretty_assertions::assert_eq;

use super::*;
	use std::fs;

	#[test]
	fn test_get_summary()
	{
		//load json
		let path = "./src/test-data/malt-format-example.json";
		let reader = MaltCReader::new(path);
		let result = reader.get_summary();
		let sol: &'static str = "{\"globalStats\":{\"count\":40,\"cumulAllocs\":1126064,\"internalMemory\":5,\"largestStack\":0,\"maxChunkSize\":1048576,\"minChunkSize\":16,\"physicalMem\":6852608,\"requestedMem\":1122438,\"segments\":37808,\"virtualMem\":13164544}}";
		assert_eq!(result, sol);
	}
}
