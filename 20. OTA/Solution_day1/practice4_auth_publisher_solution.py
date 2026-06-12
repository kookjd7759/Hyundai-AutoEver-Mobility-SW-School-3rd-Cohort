import getpass

import paho.mqtt.client as mqtt


def on_connect(client, userdata, flags, reasonCode):
    if reasonCode == 0:
        print('Connected OK')
    else:
        print('Connection failed, reason code:', reasonCode)


def on_disconnect(client, userdata, rc):
    print('Disconnected, RC:', rc)


def on_publish(client, userdata, mid):
    print('Published message, MID:', mid)


def main():
    print('=== Practice Auth Publisher Solution ===')
    broker_ip = input('Broker IP [localhost]: ').strip() or 'localhost'
    port_text = input('Broker Port [1883]: ').strip() or '1883'
    topic = input('Publish Topic: ').strip()
    if not topic:
        print('Publish topic is required.')
        return

    message = input('Message to publish [Hello! Auth test message.]: ').strip() or 'Hello! Auth test message.'

    username = input('MQTT username: ').strip()
    password = None
    if username:
        password = getpass.getpass('MQTT password: ')

    try:
        port = int(port_text)
    except ValueError:
        print('Port must be a number. Using 1883.')
        port = 1883

    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_disconnect = on_disconnect
    client.on_publish = on_publish

    if not username:
        print('MQTT username is required.')
        return

    client.username_pw_set(username, password)

    try:
        client.connect(broker_ip, port)
        client.loop_start()
        publish_info = client.publish(topic, message, qos=1)
        publish_info.wait_for_publish()
        client.loop_stop()
        client.disconnect()
        print(f'Sent auth message to topic: {topic}')
    except Exception as exc:
        print('Publish error:', exc)


if __name__ == '__main__':
    main()
