import asyncio
import websockets

# WebSocket server URL
# WebSocket server URL
ws_url = "ws://192.168.0.110/ws"


# Global variables to store data
data_to_send = "mf"
received_data = ""
new_data_received = False

# Function to handle incoming messages
async def message_handler(websocket):
    global received_data, new_data_received
    async for message in websocket:
        print("Received data:", message)
        received_data = message
        new_data_received = True

# Function to send data
async def send_data(websocket):
    global data_to_send
    await websocket.send(data_to_send)
    print("Data sent to server")
    data_to_send = ""

# Main function
async def main():
    global new_data_received  # Declare as global
    async with websockets.connect(ws_url) as websocket:
        # Start a task to handle incoming messages
        message_task = asyncio.create_task(message_handler(websocket))

        while True:
            # Check if there is data to send
            if data_to_send:
                await send_data(websocket)

            # Check if new data has been received
            if new_data_received:
                # Process received data
                print("Processing received data:", received_data)
                # Reset flag and clear received data
                new_data_received = False
                received_data = ""

            # Your other loop code goes here

            # Delay for stability
            await asyncio.sleep(0.1)

# Run the main function
asyncio.run(main())
