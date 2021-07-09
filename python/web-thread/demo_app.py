# https://www.jianshu.com/p/c7d277b05004
# sudo dtruss -a python demo_app.py
import uvicorn
async def app(scope, receive, send):
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
    uvicorn.run("demo_app:app", host="127.0.0.1", port=5000, log_level="info", workers=3)