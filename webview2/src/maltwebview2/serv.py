import os
import json

from fastapi import FastAPI
from fastapi.responses import RedirectResponse, Response
from fastapi.staticfiles import StaticFiles
from starlette.responses import FileResponse 
from starlette.responses import StreamingResponse 
import starlette.status as status
from starlette.middleware.gzip import GZipMiddleware
from pydantic import BaseModel

from .apizmq import MaltProfileRequest

class PostStacksItem(BaseModel):
    file: str | None = None
    func: str | None = None
    line: str | None = None

class PostSourceFile(BaseModel):
    path: str

class PostGetCallStackNextLevelFilter(BaseModel):
    function: str = ""
    file: str = ""
    line: int = -1

class PostGetCallStackNextLevel(BaseModel):
    parentStackId: int
    parentStackDepth: int
    filter: PostGetCallStackNextLevelFilter

class PostGetFileInfos(BaseModel):
    file: str

app = FastAPI()
#app.add_middleware(GZipMiddleware, minimum_size=1000)

# create the reader
malt_reader = MaltProfileRequest(os.environ['MALT_WEBVIEW2_SOCKET'])

#@app.get("/api/index")
#def read_root():
#    return {"Hello": os.environ['MALT_WEBVIEW2_FILE']}

#@app.get("/bundle.js")
#async def read_index():
#    return FileResponse('./typescript/dist/bundle.js')

@app.get("/")
async def main():
    # Redirect to /docs (relative URL)
    return RedirectResponse(url="app/index.html", status_code=status.HTTP_302_FOUND)

app.mount("/app/", StaticFiles(directory="./client-files/app"), name="app")

@app.get("/summary.json")
async def get_api_summary():
    return Response(content=malt_reader.get_summary(), media_type="application/json")

@app.get("/data/summary.json")
async def get_api_summary_v2():
    return Response(content=malt_reader.get_summary_v2(), media_type="application/json")

@app.get("/flat.json")
async def get_api_get_flat():
    return Response(content=malt_reader.get_flat_profile(), media_type="application/json")

@app.post("/stacks.json")
async def post_api_get_stacks(item: PostStacksItem):
    if item.func is not None:
        resp = malt_reader.getFilterdStacksOnSymbol(item.func)
        json.loads(resp)
        return Response(content=resp, media_type="application/json")
    elif item.file is not None and item.line is not None:
        return Response(content=malt_reader.getFilterdStacksOnSymbol(item.file, item.line), media_type="application/json")
    else:
        raise Exception("Invalid parametters, should get {func} of {file & line} !")

@app.post("/source-file")
async def post_api_get_stacks(item: PostSourceFile):
    if os.path.exists(item.path):
        return FileResponse(item.path)
    else:
        raise Exception(f"File not found : {item.path}")

@app.post("/file-infos.json")
async def post_api_file_infos(item: PostGetFileInfos):
    return Response(content=malt_reader.getFileLinesFlatProfile(item.file, False), media_type="application/json")

@app.post("/call-stack-next-level.json")
async def post_api_call_stack_next_level(item: PostGetCallStackNextLevel):
    filter = {"function": item.filter.function, "file": item.filter.file, "line": item.filter.line}
    return Response(content=malt_reader.get_call_stack_next_level(item.parentStackId, item.parentStackDepth, filter), media_type="application/json")
