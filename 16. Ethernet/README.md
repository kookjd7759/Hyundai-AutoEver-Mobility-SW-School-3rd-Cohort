- main.Loop_1() : UART로 수신받은 데이터를 다시 전송하여 입력한 값이 출력되도록 함
- main.Loop_2() : UART로 '0', '1' 그 외의 문자를 구분하여 출력

- BSW/Driver/can.c - void Can_RxIsrHandler() : CAN 수신 인터럽트 
- BSW/Driver/can.c - void Can_SendMsg(), void Can_RecvMsg() : CAN 송수신 함수

- main.Loop_3() : 100ms 마다 CAN 메시지 전송

- LwIP 초기화 함수
    - BSW/Driver/geth_Iwip.c - Ifx_Lwip_init()
    - Libraries/Ethernet/Iwip/src/init.c - Iwip_init()
- 이더넷 모듈 초기화 함수
    - Libraries/Ethernet/Iwip/port/src/netif.c
    
- main.Loop_4() : 10ms 마다 Ethernet 메시지 전송

- Libraries/Ethernet/Iwip/port/src/Ifx_Lwip.c : LwIP 네트워크 설정
- examples/tcp_echo.c : TCP 초기화 함수
- examples/udp_echo.c : UDP 초기화 함수

- can_eth_recv() : tcp 수신 콜백 함수

- tcp_doip_init : DoIP 초기화 및 13400 포트 설정