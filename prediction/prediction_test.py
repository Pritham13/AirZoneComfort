import numpy as np
import pandas as pd
import tensorflow as tf

def predict_fan_speed(manual_values):
    model = tf.keras.models.load_model('model.h5')

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
    'month': 6,  # Example month value
    'day': 15,   # Example day value
    'time': 12,  # Example time value
    'humidity': 80,  # Example humidity value
    'tempC': 25   # Example temperature value
}

predicted_fan_speed = predict_fan_speed(manual_values)
print("Predicted Fan Speed Class:", predicted_fan_speed)
