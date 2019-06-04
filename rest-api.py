from flask import Flask
from flask import request
from arduino_com import sendcmd


app = Flask(__name__)

@app.route('/cmd', methods = ['POST'])
def cmd():
    '''
    json:
    {
        "command": [
            int(channel 0),
            int(channel 1),
            int(channel 2),
            int(channel 3)
        ]
    }
    '''
    req = request.get_json()
    cmd = req["command"]
    for val in cmd:
        val = int(val)
    print(cmd)
    sendcmd(cmd)
    return "Command sent!", 200

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=80, debug=True)
