import requests

url = 'http://10.25.16.126/handle_post'

# Replace 'your_variable_value' with the actual value of your variable
x = 'hello world'

data = {'message': 'Hello, world!', 'variable_name': x}
response = requests.post(url, data=data)
print(response.text)
