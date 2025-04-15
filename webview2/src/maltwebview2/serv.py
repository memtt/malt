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

@app.get("/")
async def main():
    # Redirect to /docs (relative URL)
    return RedirectResponse(url="app/index.html", status_code=status.HTTP_302_FOUND)

app.mount("/app/", StaticFiles(directory="./client-files/app"), name="www")
