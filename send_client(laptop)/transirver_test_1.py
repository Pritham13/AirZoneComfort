import asyncio
import websockets

async def receive_messages():
    uri = "ws://192.168.0.109/ws"
    async with websockets.connect(uri) as websocket:
        print("Connected to ESP32 WebSocket server")
        while True:
            message = await websocket.recv()
            print(f"Received message: {message}")

asyncio.get_event_loop().run_until_complete(receive_messages())
