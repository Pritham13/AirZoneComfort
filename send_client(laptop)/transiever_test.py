import asyncio
import websockets

async def receive_and_send_data(uri):
    async with websockets.connect(uri) as websocket:
        while True:
            # Receive data from the server
            received_data = await websocket.recv()
            print(f"Received data: {received_data}")

            # Process the received data (Replace this with your processing logic)
            processed_data = received_data.upper()

            # Send the processed data back to the server
            await websocket.send(processed_data)
            print(f"Sent processed data: {processed_data}")

            # Wait for 10 minutes before receiving data again
            await asyncio.sleep(3600)  # 600 seconds = 10 minutes

async def main():
    # Replace 'ws://your.esp32.server.ip:port' with the actual WebSocket server URI
    server_uri = "ws://your.esp32.server.ip:port"
    
    await receive_and_send_data(server_uri)

if __name__ == "__main__":
    asyncio.run(main())
