import requests
url = 'http://10.25.16.126/handle_post'
data = {'message': 'Hello, world!'}
response = requests.post(url, data=data)
print(response.text)
