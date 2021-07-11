from flask import Flask
import time
import os

app = Flask(__name__)

@app.route("/")
def hello_world():
    print("request incomming")
    time.sleep(100)
    return "<p>Hello, World!</p>"

if __name__ == "__main__":
    print("pid"+ str(os.getpid()))
    app.run(host='127.0.0.1', port=9900, threaded=False)
