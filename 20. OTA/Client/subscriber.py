import paho.mqtt.client as mqtt

TOPIC = '<topic>' #Message 수신 주제를 정의하는 변수
BROKER_IP = "<broker ip>" #Message publish를 중개해주는 MQTT Broker의 주소
BROKER_PORT = "<broker port>" #연결하려는 MQTT Broker의 open port

#MQTT broker에 연결 시 연결 성공 여부 확인을 위한 콜백 함수 작성 (print 내 내용 수정을 통해 원하는 메시지가 커널에 출력되도록 할 수 있음)
def on_connect(client, userdata, flags, reasonCode):
    if reasonCode == 0:
        print("Connected successfully.")
        client.subscribe(TOPIC)
    else:
        print(f"Failed to connect, return code {reasonCode}")

#MQTT broker에 연결 종료 시 결과 확인을 위한 콜백 함수 작성
def on_disconnect(client, userdata, flags, rc = 0):
    print(str(rc) + '/')

#MQTT broker로 부터 메시지 수신 시 메시지 처리 명령을 위한 콜백 함수 작성
def on_message(client, userdata, msg):
    try:
        payload = msg.payload.decode('utf-8')        

        print('Receive a message: ', payload)

    except Exception as e:
        print(f"Error: {e}")

def topic_subscribe():
    #MQTT client 선언 및 class function에 작성한 콜백 함수 연동
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_disconnect = on_disconnect
    client.on_message = on_message

    #MQTT broker를 통해 지정 Topic 구독 및 메시지 수신 대기상태로 전환
    try:
        client.connect(BROKER_IP, BROKER_PORT, keepalive = 60)
        client.loop_forever()
    except Exception as e:
        print(f"Connection error {e}")

#해당 코드를 직접 실행할 때만 하기 코드가 동작하도록 작성!
if __name__ == "__main__":
    topic_subscribe()
