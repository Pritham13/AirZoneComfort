import numpy as np
import pandas as pd
import tensorflow as tf

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

manual_values = {
    'month': 6,  
    'day': 15,   
    'time': 12,  
    'humidity': 80,  
    'tempC': 40   
}

predicted_fan_speed = predict_fan_speed(manual_values)
print("Predicted Fan Speed Class:", predicted_fan_speed)
