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

/**********************************************************/
use actix_web::{get, post, web, App, HttpResponse, HttpServer, Responder};
use actix_web::{http::header::ContentType};
use actix_files;
use c::malt::MaltCReader;
use std::sync::Mutex;
use serde::Deserialize;
use std::fs;
use clap::{Parser, Subcommand};
use std::env;
use std::process;
use serde_json::{Value};
use actix_web::mime::IMAGE_SVG;

/**********************************************************/
#[derive(Deserialize)]
struct StackItem {
	id: usize,
}

impl Default for StackItem {
	fn default() -> Self {
		StackItem {
			id: 0,
		}
	}
}

/**********************************************************/
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

/**********************************************************/
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

/**********************************************************/
#[derive(Deserialize)]
struct PostGetCallStackNextLevel
{
	#[serde(rename="parentStackId")]
	parent_stack_id: usize,
	#[serde(rename="parentStackDepth")]
	parent_stack_depth: usize,
	filter: PostGetCallStackNextLevelFilter,
}

impl Default for PostGetCallStackNextLevel {
	fn default() -> Self {
		PostGetCallStackNextLevel {
			parent_stack_id: 0,
			parent_stack_depth: 0,
			filter: PostGetCallStackNextLevelFilter::default()
		}
	}
}

/**********************************************************/
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

/**********************************************************/
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

/**********************************************************/
#[derive(Deserialize)]
struct CallTreeItem
{
	nodeid: Option<isize>,
	depth: isize,
	height: isize,
	mincost: f64,
	func: Option<String>,
	metric: String,
	isratio: bool,
	format: Option<String>
}

impl Default for CallTreeItem {
	fn default() -> Self {
		CallTreeItem {
			nodeid: Some(0),
			depth: 0,
			height: 0,
			mincost: 0.0,
			func: Some("".to_string()),
			metric: "".to_string(),
			isratio: false,
			format: None,
		}
	}
}

/**********************************************************/
#[get("/summary.json")]
async fn summary_v1(data: web::Data<MaltCReader>) -> impl Responder
{
	HttpResponse::Ok()
		.content_type(ContentType::json())
		.body(data.get_summary())
}

/**********************************************************/
#[get("/data/summary.json")]
async fn summary_v2(data: web::Data<MaltCReader>) -> impl Responder
{
	HttpResponse::Ok()
		.content_type(ContentType::json())
		.body(data.get_summary_v2())
}

/**********************************************************/
#[get("/flat.json")]
async fn flat_profile(data: web::Data<MaltCReader>) -> impl Responder
{
	HttpResponse::Ok()
		.content_type(ContentType::json())
		.body(data.get_flat_function_profile(true, true))
}

/**********************************************************/
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

/**********************************************************/
#[post("/source-file")]
async fn source_file(_data: web::Data<MaltCReader>, file: web::Json<SourceFile>) -> impl Responder
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

/**********************************************************/
#[post("/file-infos.json")]
async fn file_infos(data: web::Data<MaltCReader>, file: web::Json<GetSourceFile>) -> impl Responder
{
	HttpResponse::Ok()
		.content_type(ContentType::json())
		.body(data.get_file_lines_flat_profile(&file.file, true))
}

/**********************************************************/
#[post("/call-stack-next-level.json")]
async fn call_stack_next_level(data: web::Data<MaltCReader>, item: web::Json<PostGetCallStackNextLevel>) -> impl Responder
{
	let function = item.filter.function.clone().unwrap_or("".to_string());
	let file = item.filter.file.clone().unwrap_or("".to_string());
	let line = item.filter.line.unwrap_or(-1);

	HttpResponse::Ok()
		.content_type(ContentType::json())
		.body(data.get_call_stack_next_level(item.parent_stack_id, item.parent_stack_depth, &function, &file, line as i32))
}

/**********************************************************/
#[get("/timed.json")]
async fn timed(data: web::Data<MaltCReader>) -> impl Responder
{
	HttpResponse::Ok()
		.content_type(ContentType::json())
		.body(data.get_timed_values())
}

/**********************************************************/
#[get("/stacks-mem.json")]
async fn stacks_mem(data: web::Data<MaltCReader>) -> impl Responder
{
	HttpResponse::Ok()
		.content_type(ContentType::json())
		.body(data.get_stacks_mem())
}

/**********************************************************/
#[get("/stack.json")]
async fn stack(data: web::Data<MaltCReader>, item: web::Query<StackItem>) -> impl Responder
{
	HttpResponse::Ok()
		.content_type(ContentType::json())
		.body(data.get_stack_info_on_function(item.id))
}

/**********************************************************/
#[get("/size-map.json")]
async fn size_map(data: web::Data<MaltCReader>) -> impl Responder
{
	HttpResponse::Ok()
		.content_type(ContentType::json())
		.body(data.get_size_map())
}

/**********************************************************/
#[get("/scatter.json")]
async fn scatter(data: web::Data<MaltCReader>) -> impl Responder
{
	HttpResponse::Ok()
		.content_type(ContentType::json())
		.body(data.get_scatter())
}

/**********************************************************/
#[get("/realloc-map.json")]
async fn realloc_map(data: web::Data<MaltCReader>) -> impl Responder
{
	HttpResponse::Ok()
		.content_type(ContentType::json())
		.body(data.get_realloc_map())
}

/**********************************************************/
#[get("/global-variables.json")]
async fn global_variables(data: web::Data<MaltCReader>) -> impl Responder
{
	HttpResponse::Ok()
		.content_type(ContentType::json())
		.body(data.get_global_variables())
}

/**********************************************************/
#[post("/calltree")]
async fn calltree(data: web::Data<MaltCReader>, item: web::Json<CallTreeItem>) -> impl Responder
{
	let format = item.format.clone().unwrap_or("".to_string());
	let func = item.func.clone().unwrap_or("".to_string());
	let nodeid = item.nodeid.clone().unwrap_or(-1);
	let res = data.get_call_tree(nodeid, item.depth, item.height, item.mincost, &func, &item.metric, item.isratio);
	let res_json: Value = serde_json::from_str(&res).expect("Unable to parse");
	match format.as_str()
	{
		"svg" => {
			if res_json["svg"].is_string() {
				HttpResponse::Ok()
					.content_type(ContentType(IMAGE_SVG))
					.body(res_json["svg"].as_str().unwrap().to_string())
			} else {
				HttpResponse::Ok()
					.content_type(ContentType::json())
					.body(res_json.to_string())
			}
		},
		"dot" => {
			if res_json["dot"].is_string() {
				HttpResponse::Ok()
					.content_type("text/vnd.graphviz")
					.body(res_json["dot"].as_str().unwrap().to_string())
			} else {
				HttpResponse::Ok()
					.content_type(ContentType::json())
					.body(res_json.to_string())
			}
		},
		_ => {
			HttpResponse::Ok()
				.content_type(ContentType::json())
				.body(res_json.to_string())
		}
	}
}

/**********************************************************/
#[derive(Parser, Debug)]
#[command(version, about, long_about = None)]
struct Cli
{
	file: String
}

/**********************************************************/
fn get_webview_www_path() -> String
{
	let exe_path = env::current_exe().unwrap();
	let bin_path = exe_path.parent().unwrap();
	let prefix = bin_path.parent().unwrap();
	//get prefix
	return format!("{}/share/malt/webview", prefix.display()).to_string();
}

/**********************************************************/
#[actix_web::main]
async fn main() -> std::io::Result<()> {
	//parsing args
	let cli = Cli::parse();	

	//print
	println!("Loading {}...", &cli.file);

	// Note: web::Data created _outside_ HttpServer::new closure
	//let mut counter = web::Data::new(MaltCReader::new("./src/test-data/malt-format-example.json"));
	let reader = web::Data::new(MaltCReader::new(&cli.file));

	//print
	println!( "+-------------------------------------------------------------------------+");
	println!( "|                                                                         |");
	println!( "|                    _ _                    _          _                  |");
	println!(r"|    _ __ ___   __ _| | |_    __      _____| |____   _(_) _____      __   |");
	println!(r"|   | '_ ` _ \ / _` | | __|___\ \ /\ / / _ \ '_ \ \ / / |/ _ \ \ /\ / /   |");
	println!(r"|   | | | | | | (_| | | ||_____\ V  V /  __/ |_) \ V /| |  __/\ V  V /    |");
	println!(r"|   |_| |_| |_|\__,_|_|\__|     \_/\_/ \___|_.__/ \_/ |_|\___| \_/\_/     |");
	println!( "|                                                                         |"); 
	println!( "|                                                                         |"); 
	println!( "|         Starting server listening on http://localhost:8080/             |");
	println!( "|                                                                         |"); 
	println!( "|                  To use from remove you can :                           |");
	println!( "|           user> ssh -L8080:localhost:8080 myserver                      |");
	println!( "|                                                                         |");
	println!( "+-------------------------------------------------------------------------+");

	//build & start server
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
			.service(stacks_mem)
			.service(stack)
			.service(size_map)
			.service(scatter)
			.service(realloc_map)
			.service(global_variables)
			.service(calltree)
			.service(web::redirect("/", "/app/index.html"))
			.service(actix_files::Files::new("/app", format!("{}/client-files/app", get_webview_www_path())).show_files_listing())
	})
	.bind(("127.0.0.1", 8080))?
	.bind_uds("/tmp/tmp.sock")?
	.run()
	.await
}
