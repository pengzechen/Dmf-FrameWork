import requests

for i in range(1000):
	res = requests.post('http://localhost/apptemp/template', 
			headers={'Cookie': 'dmfsession=RHkGhqy3Sk'})
	print(res.text)
