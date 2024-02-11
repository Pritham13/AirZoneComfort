import numpy as np
import pandas as pd
import datetime
import tensorflow as tf
from sklearn.preprocessing import MinMaxScaler
import torch
from torch.utils.data import TensorDataset, DataLoader
import torch.nn as nn
import torch.nn.functional as F
from sklearn.model_selection import train_test_split
import asyncio
import websockets
import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '2' 

def predict_fan_speed(manual_values):
    model = tf.keras.models.load_model('C:/Users/prit4/OneDrive/Desktop/stuff/active_Github_repos/Mini_project/Training/model.h5')
    scaled_features = pd.DataFrame({
        'month': [manual_values['month'] / 12],
        'day': [manual_values['day'] / 31],
        'time': [manual_values['time'] / 24],
        'humidity': [manual_values['humidity'] / 100],
        'tempC': [manual_values['tempC'] / 50]
    })

    predictions = model.predict(scaled_features)

    predicted_class = np.argmax(predictions)

    return predicted_class
async def receive_and_send_data(uri):
    async with websockets.connect(uri) as websocket:
        while True:
            # Receive data from the server
            received_data = await websocket.recv()
            print(f"Received data: {received_data}")
            # print(f"Received data: {received_data}")

            # Process the received data
            # data = received_data.split(',')
            temperature = float(received_data[:4])
            humidity = int(received_data[4:])
            print(f"Temperature: {temperature}, Humidity: {humidity}")
            current_datetime = datetime.datetime.now()

            day = current_datetime.day
            month = current_datetime.month
            hour = current_datetime.hour
            manual_values = {
                            'month': month,  
                            'day': day,   
                            'time': hour,  
                            'humidity': humidity,  
                            'tempC': temperature   
                        }


            # Process the received data (Replace this with your processing logic)
            processed_data = predict_fan_speed(manual_values)

            # Send the processed data back to the server
            await websocket.send(str(processed_data))
            print(f"Sent processed data: {processed_data}")

            # Wait for 10 minutes before receiving data again
            await asyncio.sleep(10)  # 600 seconds = 10 minutes

async def main():
    # Replace 'ws://your.esp32.server.ip:port' with the actual WebSocket server URI
    server_uri = "ws://192.168.0.110/ws"
    
    while True:
        await receive_and_send_data(server_uri)
        await asyncio.sleep(10)

if __name__ == "__main__":
    asyncio.run(main())
