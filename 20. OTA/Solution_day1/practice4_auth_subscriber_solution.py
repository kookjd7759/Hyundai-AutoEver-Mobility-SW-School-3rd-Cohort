import getpass

import paho.mqtt.client as mqtt


def on_connect(client, userdata, flags, reasonCode):
    if reasonCode == 0:
        print('Connected successfully.')
        client.subscribe(userdata['topic'])
        print('Subscribed to topic:', userdata['topic'])
    else:
        print('Connection failed, reason code:', reasonCode)


def on_disconnect(client, userdata, rc):
    print('Disconnected, RC:', rc)


def on_message(client, userdata, msg):
    try:
        text = msg.payload.decode('utf-8')
        print('Received message:', text)
    except Exception:
        print('Received binary payload of length', len(msg.payload))


def main():
    print('=== Practice Auth Subscriber Solution ===')
    broker_ip = input('Broker IP [localhost]: ').strip() or 'localhost'
    port_text = input('Broker Port [1883]: ').strip() or '1883'
    topic = input('Subscribe Topic: ').strip()
    if not topic:
        print('Subscribe topic is required.')
        return

    username = input('MQTT username: ').strip()
    if not username:
        print('MQTT username is required.')
        return
    password = getpass.getpass('MQTT password: ')

    try:
        port = int(port_text)
    except ValueError:
        print('Port must be a number. Using 1883.')
        port = 1883

    client = mqtt.Client(userdata={'topic': topic})
    client.on_connect = on_connect
    client.on_disconnect = on_disconnect
    client.on_message = on_message

    client.username_pw_set(username, password)

    try:
        client.connect(broker_ip, port, keepalive=60)
        client.loop_forever()
    except Exception as exc:
        print('Subscribe error:', exc)


if __name__ == '__main__':
    main()
