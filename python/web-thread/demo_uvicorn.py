# https://www.jianshu.com/p/c7d277b05004
# sudo dtruss -a python demo_app.py
import uvicorn
import time
async def app(scope, receive, send):
    # 这里是阻塞的的。2个请求同时访问这个接口需要20s
    time.sleep(10)
    assert scope['type'] == 'http'
    await send({
        'type': 'http.response.start',
        'status': 200,
        'headers': [
            [b'content-type', b'text/plain'],
        ]
    })
    await send({
        'type': 'http.response.body',
        'body': b'Hello, world!',
    })

if __name__ == "__main__":
    uvicorn.run("demo_uvicorn:app", host="127.0.0.1", port=5000, log_level="info", workers=1)