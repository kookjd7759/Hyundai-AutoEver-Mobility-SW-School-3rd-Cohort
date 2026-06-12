import paho.mqtt.client as mqtt

#MQTT broker에 연결 시 연결 성공 여부 확인을 위한 콜백 함수 작성 (print 내 내용 수정을 통해 원하는 메시지가 커널에 출력되도록 할 수 있음)
def on_connect(__,userdata,flags,reasonCode):
    if reasonCode == 0:
        print("connected OK")
    else:
        print("Error: connection failed, Return Code =", reasonCode)

#MQTT broker와 연결 종료 시 결과 확인을 위한 콜백 함수 작성
def on_disconnect(client, userdata, flags, rc=0):
    print('Disconnected, RC:', rc)

#MQTT broker에 message publish 요청의 결과 확인을 위한 콜백 함수 작성
def on_publish(client, userdata, mid):
    print("message published, MID: ", mid)

#MQTT message publish 함수 정의
def message_publish(topic, message, broker_ip, port = 1883):
    #MQTT Client 선언 및 class function에 작성한 콜백 함수 연동
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_disconnect = on_disconnect
    client.on_publish = on_publish

    #Message publish를 위해 broker와 연결 및 publish 요청 시도
    try:
        client.connect(broker_ip, port)
        client.loop_start()
        
        client.publish(topic, message, qos=2)

        client.loop_stop()

        print(f"Success sending message: {message}")
        client.disconnect()
    except Exception as e:
        print("Error: ", e)

#해당 코드를 직적 실행할 때만 message를 보내도록 정의, 모듈로 쓸 경우 이하 코드는 실행 되지 않음!
if __name__ == '__main__':
    TOPIC = '<topic>' #Message 전송 주제를 정의하는 변수
    BROKER_IP = "<broker ip>" #Message publish를 중개해주는 MQTT Broker의 주소
    BROKER_PORT = "<broker port>" #연결하려는 MQTT Broker의 open port
    message_publish(TOPIC, "Hello! I am an update server.", BROKER_IP, BROKER_PORT)
