- memory_addr_test.c - Infineon AURIX TC275에서 코드/데이터/BSS/Heap/Stack 영역의 실제 메모리 주소를 확인하여 메모리 배치 구조를 분석한 실습
- boot-sequence.c - AURIX TC275에서 ROM/Init Data/BSS와 Heap(malloc), Stack(재귀 로컬변수) 주소를 기록해 실제 메모리 레이아웃을 확인한 실습
- compiler_test.c - AURIX TC275에서 함수/전역/루프 코드의 최적화 동작을 비교하기 위해 결과를 전역 변수로 저장하는 실습  
<br>
- morning-before.c: 1ms/10ms 주기 스케줄링으로 속도·상태를 표시하고 SW1 홀드로 크루즈 목표속도를 제어하는 기본 구현  
- morning-optimized.c: FND 표시를 버퍼링(10ms 계산→1ms 출력)하고 홀드타임을 정수화해 주기 태스크 연산량을 줄인 최적화 버전  
<br>
- afternoon-before.c: 스위치 이벤트/상태 전환을 진단 로그에 기록하고 FND 표시 및 CRC8 검증을 수행하는 기본 구현  
- afternoon-optimized.c: 7세그 룩업테이블·비트마스크·CRC 루프 최적화(hoist/inline/unroll)로 분기와 반복 오버헤드를 줄인 최적화 버전  
<br>
- [mini-proaject], CruiseLightControlSystem.c - AURIX TC275 환경에서 STM Timer 기반 Multi-rate 스케줄링(1~1000ms)과 상태 머신을 적용하여 ADC 센서 입력, PWM 조명 제어, 스위치 인터럽트 처리, FND 표시 및 LED 경고 로직을 통합 구현한 차량 제어 실습 코드  

    본 프로젝트는 STM(System Timer Module)을 이용한 소프트웨어 스케줄러 구조를 설계하여 1ms, 10ms, 100ms, 1000ms 주기의 태스크를 계층적으로 분리하고, 각 주기별 기능을 독립적으로 수행하도록 구성하였다. 1ms 주기에서는 FND 동적 스캔을 수행하고, 10ms 주기에서는 스위치 디바운싱 및 제어 로직 업데이트, 100ms 및 1000ms 주기에서는 시스템 상태 및 표시 값을 갱신하도록 설계하였다.  

    또한 STATE_NORMAL, STATE_CRUISE, STATE_CRUISE_SET, STATE_EMERGENCY의 상태 머신을 통해 차량 속도 제어 및 시스템 동작 우선순위를 명확히 정의하였다. 가변저항 ADC 입력을 이용해 목표 속도를 설정하고, Speed Ramp 알고리즘을 통해 목표 속도까지 점진적으로 속도를 변경하도록 구현하였다.  

    조도 센서 입력값을 기반으로 GTM TOM PWM을 이용한 조명 밝기 제어 기능을 구현하였으며, 센서 노이즈에 의한 급격한 변화 방지를 위해 Low-pass 형태의 smoothing 필터를 적용하였다. 또한 스위치 입력에는 디바운싱과 Short/Long Press 판별 로직을 적용하여 안정적인 사용자 입력 처리를 수행하도록 설계하였다.