import random
import time
import math
import datetime

from flask import render_template
from flask import request
from markupsafe import escape
from flask import Flask, request, jsonify
from flask_mqtt import Mqtt


app = Flask(__name__)

# region MQTT
# Configure Flask-MQTT extension
app.config["MQTT_BROKER_URL"] = "192.168.0.29"
app.config["MQTT_BROKER_PORT"] = 1885
app.config[
    "MQTT_USERNAME"
] = ""  # Set this item when you need to verify username and password
app.config[
    "MQTT_PASSWORD"
] = ""  # Set this item when you need to verify username and password
app.config["MQTT_KEEPALIVE"] = 5  # Set KeepAlive time in seconds
app.config["MQTT_TLS_ENABLED"] = False  # If your server supports TLS, set it True
topic_led = "blink_led"
topic_heartbeat = "my_topic"
topic_temperature = "sensor/temp"
topic_gps = "sensor/gps"

esp_data = dict(led=0, tempC=0, tempF=0, tempK=0, lat=42, lng=-8.5, vel=0)

mqtt_client = Mqtt(app)

# Write connect callback function
@mqtt_client.on_connect()
def handle_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected successfully")
        mqtt_client.subscribe(topic_heartbeat)  # subscribe topic
        mqtt_client.subscribe(topic_led)
        mqtt_client.subscribe(topic_temperature)
        mqtt_client.subscribe(topic_gps)
    else:
        print("Bad connection. Code:", rc)


# Write message callback function
@mqtt_client.on_message()
def handle_mqtt_message(client, userdata, message):
    data = dict(topic=message.topic, payload=message.payload.decode())

    print("Received message on topic: {topic} with payload: {payload}".format(**data))
    
    # Call to the parse function
    parse_data(data)


# Create message publish API
@app.route("/publish", methods=["POST"])
def publish_message():
    request_data = request.get_json()
    publish_result = mqtt_client.publish(request_data["topic"], request_data["msg"])
    return jsonify({"code": publish_result[0]})

# endregion

@app.route("/")
def framework(name=None):
    return render_template("framework.html", name=name)

@app.route("/<name>")
def Tab(name):
    variable = math.pi
    print(f"|The value of PI is: {variable: }")
    return render_template("index.html", name=name)

@app.route("/mqtt/data/")
def get_data():
    return {"led": esp_data["led"], "tempC": esp_data["tempC"], "tempF": esp_data["tempF"], "tempK": esp_data["tempK"], "lat": esp_data["lat"], "lng": esp_data["lng"], "vel": esp_data["vel"]}

#region Data parsing function
def parse_data(data):
    if data["topic"] == topic_led:
        if esp_data["led"] == 0:
            esp_data["led"] = 1
        else:
            esp_data["led"] = 0

    elif data["topic"] == topic_temperature:
        temp = data["payload"].split(",")
        esp_data["tempC"] = temp[0]
        esp_data["tempF"] = temp[1]
        esp_data["tempK"] = temp[2]

    elif data["topic"] == topic_gps:
        gps = data["payload"].split(",")
        esp_data["lat"] = gps[0]
        esp_data["lng"] = gps[1]
        esp_data["vel"] = gps[2]

#endregion

@app.route('/send', methods=['POST'])
def send():
  blink = request.get_json()['blink']
  publish_result = mqtt_client.publish("blink_led", "2")
  return 'Variable received'