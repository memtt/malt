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
mod c;

use actix_web::{get, post, web, App, HttpResponse, HttpServer, Responder};
use actix_web::{http::header::ContentType};
use actix_files;
use c::malt::MaltCReader;
use std::sync::Mutex;
use serde::Deserialize;
use std::fs;

#[derive(Deserialize)]
struct PostStacksItem {
	file: String,
	func: String,
	line: isize,
}

impl Default for PostStacksItem {
	fn default() -> Self {
		PostStacksItem {
			file: "".to_string(),
			func: "".to_string(),
			line: -1
		}
	}
}

#[derive(Deserialize)]
struct PostGetCallStackNextLevelFilter
{
	function: Option<String>,
	file: Option<String>,
	line: Option<isize>,
}

impl Default for PostGetCallStackNextLevelFilter {
	fn default() -> Self {
		PostGetCallStackNextLevelFilter {
			function: Some("".to_string()),
			file: Some("".to_string()),
			line: Some(-1)
		}
	}
}

#[derive(Deserialize)]
struct PostGetCallStackNextLevel
{
	parentStackId: usize,
	parentStackDepth: usize,
	filter: PostGetCallStackNextLevelFilter,
}

impl Default for PostGetCallStackNextLevel {
	fn default() -> Self {
		PostGetCallStackNextLevel {
			parentStackId: 0,
			parentStackDepth: 0,
			filter: PostGetCallStackNextLevelFilter::default()
		}
	}
}

#[derive(Deserialize)]
struct SourceFile
{
	path: String
}

impl Default for SourceFile {
	fn default() -> Self {
		SourceFile {
			path: "".to_string()
		}
	}
}

#[derive(Deserialize)]
struct GetSourceFile
{
	file: String
}

impl Default for GetSourceFile {
	fn default() -> Self {
		GetSourceFile {
			file: "".to_string()
		}
	}
}

#[get("/summary.json")]
async fn summary_v1(data: web::Data<MaltCReader>) -> impl Responder
{
	HttpResponse::Ok()
		.content_type(ContentType::json())
		.body(data.get_summary())
}

#[get("/data/summary.json")]
async fn summary_v2(data: web::Data<MaltCReader>) -> impl Responder
{
	HttpResponse::Ok()
		.content_type(ContentType::json())
		.body(data.get_summary_v2())
}

#[get("/flat.json")]
async fn flat_profile(data: web::Data<MaltCReader>) -> impl Responder
{
	HttpResponse::Ok()
		.content_type(ContentType::json())
		.body(data.get_flat_function_profile(true, true))
}

#[post("/stacks.json")]
async fn stacks(data: web::Data<MaltCReader>, item: web::Query<PostStacksItem>) -> impl Responder
{
	if item.file.is_empty() == false && item.line != -1 {
    	HttpResponse::Ok()
			.content_type(ContentType::json())
			.body(data.get_filtered_stacks_on_file_line(&item.file, item.line as usize))
	} else {
		HttpResponse::Ok()
			.content_type(ContentType::json())
			.body(data.get_filterd_stacks_on_symbol(&item.func))
	}
}

#[post("/source-file")]
async fn source_file(data: web::Data<MaltCReader>, file: web::Json<SourceFile>) -> impl Responder
{
	//if data.is_source_file(&file.path) {
	if fs::exists(&file.path).unwrap() {
		let content = fs::read_to_string(&file.path).expect("Something went wrong");
		HttpResponse::Ok()
			.content_type(ContentType::plaintext())
			.body(content)
	} else {
		HttpResponse::Ok()
			.content_type(ContentType::plaintext())
			.body(format!("File not found in profile : {}", &file.path))
	}
}

#[post("/file-infos.json")]
async fn file_infos(data: web::Data<MaltCReader>, file: web::Json<GetSourceFile>) -> impl Responder
{
	HttpResponse::Ok()
		.content_type(ContentType::json())
		.body(data.get_file_lines_flat_profile(&file.file, true))
}

#[post("/call-stack-next-level.json")]
async fn call_stack_next_level(data: web::Data<MaltCReader>, item: web::Json<PostGetCallStackNextLevel>) -> impl Responder
{
	let function = item.filter.function.clone().unwrap_or("".to_string());
	let file = item.filter.file.clone().unwrap_or("".to_string());
	let line = item.filter.line.unwrap_or(-1);

	HttpResponse::Ok()
		.content_type(ContentType::json())
		.body(data.get_call_stack_next_level(item.parentStackId, item.parentStackDepth, &function, &file, line as i32))
}

#[get("/timed.json")]
async fn timed(data: web::Data<MaltCReader>) -> impl Responder
{
	HttpResponse::Ok()
		.content_type(ContentType::json())
		.body(data.get_timed_values())
}

#[actix_web::main]
async fn main() -> std::io::Result<()> {
	// Note: web::Data created _outside_ HttpServer::new closure
	//let mut counter = web::Data::new(MaltCReader::new("./src/test-data/malt-format-example.json"));
	let reader = web::Data::new(MaltCReader::new("/home/sebv/Projects/inria/malt/build/malt-simple-case-finstr-linked-34827.json"));

	HttpServer::new(move || {
		App::new()
			.app_data(reader.clone())
			.service(summary_v1)
			.service(summary_v2)
			.service(flat_profile)
			.service(stacks)
			.service(source_file)
			.service(file_infos)
			.service(call_stack_next_level)
			.service(timed)
			.service(web::redirect("/", "/app/index.html"))
			.service(actix_files::Files::new("/app", "/home/sebv/Projects/inria/malt/src/webview/client-files/app").show_files_listing())
	})
	.bind(("127.0.0.1", 8080))?
	.bind_uds("/tmp/tmp.sock")?
	.run()
	.await
}
