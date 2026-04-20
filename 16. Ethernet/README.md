- main.Loop_1() : UART로 수신받은 데이터를 다시 전송하여 입력한 값이 출력되도록 함
- main.Loop_2() : UART로 '0', '1' 그 외의 문자를 구분하여 출력

- BSW/Driver/can.c - void Can_RxIsrHandler() : CAN 수신 인터럽트 
- BSW/Driver/can.c - void Can_SendMsg(), void Can_RecvMsg() : CAN 송수신 함수

- main.Loop_3() : 100ms 마다 CAN 메시지 전송