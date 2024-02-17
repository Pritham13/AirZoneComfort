import warnings
with warnings.catch_warnings():
    warnings.filterwarnings("ignore", category=FutureWarning)
    import tensorflow as tf

import numpy as np
import pandas as pd
import datetime
from sklearn.preprocessing import MinMaxScaler
import torch
from torch.utils.data import TensorDataset, DataLoader
import torch.nn as nn
import torch.nn.functional as F
from sklearn.model_selection import train_test_split
import asyncio
import websockets 

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

            await asyncio.sleep(10)  

async def main():
    # server_uri = "ws://192.168.12.163/ws"
    server_uri = "ws://192.168.0.110/ws"
    
    while True:
        await receive_and_send_data(server_uri)
        await asyncio.sleep(10)

if __name__ == "__main__":
    asyncio.run(main())
