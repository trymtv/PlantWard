import paho.mqtt.client as mqtt
from plantward import settings
from plantwardsite.models import Measurement
import json

def on_connect(mqtt_client, userdata, flags, rc):
    if rc == 0:
        print('Mqtt client connected successfully')
        mqtt_client.subscribe('sensor/values')
    else:
        print('Bad connection. Code:', rc)

def on_message(mqtt_client, userdata, msg):
    try:
        message_body = json.loads(msg.payload)
        new_measurement = Measurement(**message_body)
        new_measurement.save()
    except Exception:
        print("[ERROR] Failed parsing mqtt message!!!")
        pass

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(
    host=settings.MQTT_SERVER,
    port=settings.MQTT_PORT,
    keepalive=settings.MQTT_KEEPALIVE
)
client.loop_start()