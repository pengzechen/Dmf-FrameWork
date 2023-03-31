import requests
res = requests.post('http://127.0.0.1:80/getsession?aaa=name&hhh=hhh', headers={'Cookie': 'dmfsession=RHkGhqy3Sk'})
print(res.text)
