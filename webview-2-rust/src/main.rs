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
use actix_files;
use c::malt::MaltCReader;
use std::sync::Mutex;

#[get("/summary.json")]
async fn summary_v1(data: web::Data<MaltCReader>) -> impl Responder {
    HttpResponse::Ok().body(data.get_summary())
}

#[get("/data/summary.json")]
async fn summary_v2(data: web::Data<MaltCReader>) -> impl Responder {
    HttpResponse::Ok().body(data.get_summary_v2())
}

#[get("/flat.json")]
async fn flat_profile(data: web::Data<MaltCReader>) -> impl Responder {
    HttpResponse::Ok().body(data.get_flat_function_profile(true, true))
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
			.service(web::redirect("/", "/app/index.html"))
            .service(actix_files::Files::new("/app", "/home/sebv/Projects/inria/malt/src/webview/client-files/app").show_files_listing())
    })
    .bind(("127.0.0.1", 8080))?
	.bind_uds("/tmp/tmp.sock")?
    .run()
    .await
}
