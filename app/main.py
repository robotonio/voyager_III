import asyncio
from server import Server
import uvicorn
app = asyncio.run(Server().create_server())

uvicorn.run(app, host="0.0.0.0", port=3000)
