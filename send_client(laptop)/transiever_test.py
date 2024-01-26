import requests

url = 'http://192.168.0.110/handle_post'

# Replace 'your_variable_value' with the actual value of your variable
x = 'hello world'

data = {'message': 'Hello, world!', 'variable_name': x}
response = requests.post(url, data=data)
print(response.text)
