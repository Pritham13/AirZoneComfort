graph LR
    subgraph Sensor_Data_Acquisition
        A[DHT11 Sensor] --> B((ESP32))
        C[BMP280 Sensor] --> B
    end
    B --> D((ESP32 Microcontroller))
    D --> E[Data Processing]
    E --> F[WiFi Connectivity]
    F --> G((WiFi Communication and Computational Unit))
    G --> H[Machine Learning Model]
    H --> I[Fan Speed Prediction]
    I --> G
    G --> J[Feedback to ESP32]
    J --> D
    D --> K[Fan Speed Control]
    K --> L((Fan))
