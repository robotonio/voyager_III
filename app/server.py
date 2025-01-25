from fastapi import FastAPI, Request
from fastapi.responses import HTMLResponse
from sse_starlette.sse import EventSourceResponse
import json,os
from fastapi.middleware.cors import CORSMiddleware


class Hotspot:
    def __init__(self, longitude: float, latitude: float, confidence: float):
        self.longitude = longitude
        self.latitude = latitude
        self.confidence = confidence

class Server:
    def __init__(self):
        self.hotspots = []
        self.clients = set()

    async def create_server(self):
        app = FastAPI()
        app.add_middleware(
            CORSMiddleware,
            allow_origins=["*"],
            allow_methods=["*"],
            allow_headers=["*"],
        )
        @app.get("/home", response_class=HTMLResponse)
        def serve_home():
            html_path = os.path.join(os.path.abspath(os.getcwd()), 'app', 'index.html')
            
            with open(html_path, "r", encoding="utf-8") as file:
                html_content = file.read()
            
            return HTMLResponse(content=html_content)
        # SSE Endpoint
        @app.get("/api/hotspots/get")
        async def get_hotspots(request: Request):
            async def event_generator():
                client = request.client
                self.clients.add(client)
                try:
                    # Send an initial message
                    yield f"data: {json.dumps([hotspot.__dict__ for hotspot in self.hotspots])}\n\n"
                    while True:
                        if await request.is_disconnected():
                            break
                finally:
                    self.clients.discard(client)

            return EventSourceResponse(event_generator())
        
        @app.get('/api/hotspots/add')    
        def add_hotspots(self, longitude: float, latitude: float, confidence: float):
            new_hotspot = Hotspot(longitude, latitude, confidence)
            self.hotspots.append(new_hotspot)
            for client in self.clients:
                client.send(f"data: {json.dumps(new_hotspot.__dict__)}\n\n")
        return app