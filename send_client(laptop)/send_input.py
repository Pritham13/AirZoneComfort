import asyncio
import websockets

# WebSocket server URL
ws_url = "ws://192.168.0.101/ws"

# Function to handle incoming messages
async def message_handler(websocket):
    async for message in websocket:
        print("Received data:", message)

# Function to send series of data and receive response for each data value
async def send_data(websocket):
    data_to_send = ["data1", "data2", "data3"]  # List of data values to send
    for data in data_to_send:
        await websocket.send(data)
        print("Data sent to server:", data)
        response = await websocket.recv()
        print("Received response from server:", response)

# Main function
async def main():
    async with websockets.connect(ws_url) as websocket:
        # Start a task to handle incoming messages
        message_task = asyncio.create_task(message_handler(websocket))

        # Send series of data and receive response for each data value
        await send_data(websocket)

# Run the main function
asyncio.run(main())
