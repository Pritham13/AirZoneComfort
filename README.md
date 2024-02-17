# Smart HVAC System Project Overview

## Project Description
The Smart HVAC system is designed to adjust fan speed based on real-time data such as temperature and humidity using a custom-built machine learning model. The system consists of hardware components for data collection and display, as well as software components for data transmission and fan speed control.

Note: The clinet file is client.py and the ESP32 file is final_server.ino both are under the folder names final
## Hardware Components
- **BMP180**: Used for measuring temperature and barometric pressure.
- **DHT11**: Used for measuring temperature and humidity.
- **ESP32**: Microcontroller used for data collection, data transmission, and fan speed control.
- **OLED Screen**: Displaying real-time data including fan speed, temperature, and humidity.

## Software Components
- **Custom Machine Learning Model**: Utilized to predict fan speed based on input data (temperature, humidity, time).
- **WebSocket Server (ESP32)**: Facilitates communication between the ESP32 and client (laptop).
- **WebSocket Client (Laptop)**: Connects to the WebSocket server on the ESP32 to receive data and send commands.
- **Core Management**: Data collection and display run on Core 0, while WebSocket server runs on Core 1 of the ESP32.

## Machine Learning Model Architecture
The machine learning model used in the Smart HVAC system is implemented using TensorFlow and Keras. It consists of three dense layers:
- **Input Layer**: Dense layer with 100 neurons and LeakyReLU activation function, receiving input data with a shape of (5,) representing month, day, time, humidity, and temperature.
- **Hidden Layer**: Dense layer with 100 neurons and LeakyReLU activation function.
- **Output Layer**: Dense layer with 6 neurons and softmax activation function for multi-class classification, predicting the fan speed categories.

## Future Scalability
To ensure future scalability and adaptability, the Smart HVAC system can explore the following options:

### Cloud Computing Options
- **Amazon Web Services (AWS)**: Utilize services like AWS IoT Core for data ingestion and AWS Lambda for serverless computing.
- **Microsoft Azure**: Leverage Azure IoT Hub for device management and Azure Functions for event-driven processing.
- **Google Cloud Platform (GCP)**: Utilize Google Cloud IoT Core for device connectivity and Google Cloud Functions for serverless computing.

### Multiplexing Options
- **Time Division Multiplexing (TDM)**: Allocate time slots for different data sources to transmit their data, optimizing bandwidth usage.
- **Frequency Division Multiplexing (FDM)**: Allocate different frequency bands for simultaneous data transmission, reducing interference and improving throughput.
- **Code Division Multiplexing (CDM)**: Assign unique codes to different data sources for simultaneous transmission, enhancing data security and reliability.

## Steps Involved

### 1. Data Collection
   - Read temperature and humidity data from BMP180 and DHT11 sensors.
   - Collect real-time data including current time, humidity, and temperature.
   - Display the collected data on the OLED screen.
   - This Runs on Core 0 of the ESP32
### 2. Data Transmission
   - ESP32 sets up a WebSocket server.
   - Laptop (client) connects to the WebSocket server.
   - ESP32 sends temperature and humidity data to the laptop through the WebSocket connection.
   - This runs on Core 1 of the ESP32

### 3. Machine Learning Model
   - Laptop receives temperature and humidity data from ESP32.
   - Data is processed through the custom machine learning model.
   - The machine learning model predicts the appropriate fan speed based on the input data.

### 4. Fan Speed Control
   - Laptop sends the predicted fan speed back to the ESP32 through the WebSocket connection.
   - ESP32 adjusts the fan speed accordingly based on the received command.

## Conclusion
The Smart HVAC system effectively integrates hardware components for data collection and display with software components for data transmission and fan speed control. The custom machine learning model enhances the system's capability to dynamically adjust fan speed based on real-time environmental conditions, optimizing energy efficiency and user comfort.
