- ECU-PC 정보 교환
    - practice.loop_practice_1() : ECU -> PC로 500ms 마다 "Hello!!!"를 전송
    - practice.loop_practice_2() : ECU -> PC로 100ms 마다 1씩 값을 증가하여 전송
    - main.Can_RxIsrHandler1() : CAN 메시지를 수신해 ID와 데이터 값에 따라 보드의 LED를 제어

- ECU-ECU 간 정보 교환
    - practice.loop_practice_3_sender() : CAN ID 0x100, "Hello!!!" 메시지 전송
    - main.isr_rx_can() : CAN ID 0x100을 수신하면 CAN ID 0x200, "RX_OK!!" 메시지 전송

- CAN ID 필터링 
    - Can_SetFilterMask(v1, v2) : v1부터 v2 메시지만 수신하도록 설정

- CAN DB 설계
    - practice.loop_practice_4() : 가변저항 값을 변경하여, ADC 값과 전압 값을 출력
    - practice.loop_practice_5() : 광량을 조절하여, 광 센서 ADC 값과 전압 값을 출력
    - practice.loop_practice_6() : 광 센서, 가변저항 값을 전송

- 진단 통신 설계
    - main.Can_RxIsrHandler2() : UDS 진단 요청을 수신해 DID에 따라 가변저항 또는 광센서 값을 읽고, 응답 메시지를 전송

- main.Can_RxIsrHandler3() : 레이저 센서의 거리에 따라 아래와 같이 제어
    - 거리 1.5m 이상 - LED 1 ON
    - 거리 0.3m 이하 - LED 2 ON

- practice.loop_practice_7() : CAN ID 0x150과 0x7FF 메시지를 각각 100ms 간격으로 10회씩 번갈아 전송
- main.Can_RxIsrHandler4() : 수신 CAN ID가 0x150이면 LED1을 토글, 0x7FF이면 LED2를 토글

- practice.loop_practice_8() : CAN FD 버스 실습에서 전송하는 CAN FD ID 데이터에 ToF 센서 데이터를 추가하여 전송 