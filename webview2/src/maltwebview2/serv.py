import os

from fastapi import FastAPI
from fastapi.responses import RedirectResponse
from fastapi.staticfiles import StaticFiles
from starlette.responses import FileResponse 
from starlette.responses import StreamingResponse 
import starlette.status as status

app = FastAPI()

#@app.get("/api/index")
#def read_root():
#    return {"Hello": os.environ['MALT_WEBVIEW2_FILE']}

#@app.get("/bundle.js")
#async def read_index():
#    return FileResponse('./typescript/dist/bundle.js')

# load
import ctypes

# Load the shared library into ctypes
libname = os.path.abspath("../../build/src/reader/libreader/libmalt-profile.so")
malt_reader = ctypes.CDLL(libname)
malt_reader.malt_reader_init.restype = ctypes.c_void_p
malt_reader_handler: ctypes.c_void_p = malt_reader.malt_reader_init(ctypes.c_char_p(os.environ['MALT_WEBVIEW2_FILE'].encode()))
print(f"HDANLE = {hex(malt_reader_handler)}")


@app.get("/")
async def main():
    # Redirect to /docs (relative URL)
    return RedirectResponse(url="app/index.html", status_code=status.HTTP_302_FOUND)

app.mount("/app/", StaticFiles(directory="./client-files/app"), name="www")

@app.get("/summary.json")
async def get_api_summary():
    malt_reader.malt_reader_get_summary.restype = ctypes.c_char_p
    str:ctypes.c_char_p = malt_reader.malt_reader_get_summary(ctypes.c_void_p(malt_reader_handler))
    tmp = str.decode('utf-8')
    #malt_reader.malt_reader_free_response(ctypes.cast(str, ctypes.c_void_p))
    return {"Hello": tmp}

@app.get("/data/summary.json")
async def get_api_summary_v2():
    malt_reader.malt_reader_get_summary_v2.restype = ctypes.c_char_p
    str:ctypes.c_char_p = malt_reader.malt_reader_get_summary_v2(ctypes.c_void_p(malt_reader_handler))
    tmp = str.decode('utf-8')
    #malt_reader.malt_reader_free_response(ctypes.cast(str, ctypes.c_void_p))
    return {"Hello": tmp}