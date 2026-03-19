- gpio_button_select.c - 폴링 방식으로 버튼 입력을 읽어 조건에 따라 서로 다른 LED를 제어하는 GPIO 입력/출력 실습
- gpio_button_omr.c - OMR(Set/Reset) 레지스터를 활용하여 버튼 입력에 따라 LED를 전환하는 GPIO 제어 실습
- button_toggle_polling.c - 버튼 입력을 폴링으로 감지하고 버튼 해제 대기 로직을 포함해 LED를 토글하는 실습
- button_toggle_fast.c - 디바운스 없이 버튼 입력 시 OMR을 이용해 LED를 빠르게 토글하는 동작 확인 실습
- eru_led_on.c - ERU(External Request Unit) 인터럽트를 설정하고 ISR에서 LED를 제어하는 외부 인터럽트 기초 실습
- eru_dual_toggle.c - 두 개의 버튼을 ERU 인터럽트로 구성하여 각각의 LED를 개별적으로 토글하는 인터럽트 실습
- eru_blink_state.c - ERU 인터럽트와 상태 머신을 결합하여 버튼 입력에 따라 LED 점멸 동작을 전환하는 상태 기반 제어 실습
- stm_traffic_pause.c - STM 타이머 기반 3상 신호등에 버튼 입력으로 일시정지/재개 기능을 추가한 상태머신 구현  
- stm_led_traffic.c - STM 타이머 주기 인터럽트를 활용해 RED/BLUE/BLINK 3상 LED를 상태머신으로 순환 제어하는 타이머 기반 제어 실습
- stm_fnd_stopwatch.c - STM 타이머 틱과 ERU 버튼 인터럽트를 결합하여 4자리 FND 스톱워치를 구현한 상태 기반 카운터 실습
- Driver_Stm.c / Driver_Stm.h - STM0 Compare Interrupt를 이용해 1ms Tick 기반 1/10/100/1000ms 소프트웨어 스케줄링 플래그를 생성하는 타이머 드라이버 구현  -
- blink_scheduler - 1ms 기반 소프트웨어 스케줄러를 활용해 100ms·1000ms 주기 LED 토글을 구현하며 주기 태스크 구조를 검증한 실습
- eru_stopwatch - ERU 외부 인터럽트와 상태머신(FSM)을 적용해 FND 기반 스톱워치를 제어하는 인터럽트 중심 구조 실습
- mux_stopwatch - 1ms 멀티플렉싱 FND 구동과 10ms 디바운싱 입력 처리를 결합한 논블로킹 스톱워치 시스템 구현
- Driver_Adc.c / Driver_Adc.h - AURIX VADC Group4의 Autoscan 기능을 설정하고 CH7 변환을 시작/대기(VF) 후 결과(RESULT)를 읽어오는 ADC 드라이버 실습
- Driver_Adc.c / Driver_Adc.h - AURIX VADC Group4 Autoscan을 구성해 CH6·CH7을 주기적으로 변환하고 VF 플래그 폴링으로 결과를 읽어오는 ADC 드라이버 구현
- adc-light-control.c - STM 주기 스케줄러 기반으로 VADC(CH6·CH7) 값을 필터링하여 RGB 색상 제어, 조도 히스테리시스 제어 및 FND 멀티플렉싱 표시를 구현한 통합 제어 실습
- GTM_TOM_PWM.c - AURIX GTM TOM PWM 초기화/듀티 제어를 구현하고, 주파수(Period) 변경으로 부저 음계 생성 + 듀티로 LED 페이드(ADC 기반 포함)까지 제공하는 PWM 드라이버
- StmScheduler_ButtonBuzzer.c - STM 기반 1/10/100/1000ms 태스크 스케줄링으로 버튼 입력에 따라 부저 음 전환 실습
- StmScheduler_BabySharkPlayer.c - TM 1ms 주기 타이머로 멜로디 배열(“Baby Shark”)을 재생하고, 버튼으로 재생 및 정지
<br>  

- [Final Mini project], VehicleControlSystem.c - AURIX 기반 Multi-rate 스케줄링(1~1000ms)과 상태 머신을 적용해 ADC 입력, ERU 인터럽트, FND 표시, LED/부저 경고 로직을 통합 구현한 차량 제어 실습 코드  
  
    해당 프로젝트는 단순 GPIO 제어를 넘어, 주기 기반 소프트웨어 스케줄링 구조를 설계하고 각 주기별 역할을 분리하여 실시간 제어 시스템을 구성하는 데 목적을 두었다. 1ms 주기에서는 FND 동적 스캔을 수행하고, 10ms/100ms 주기에서는 상태 전이 및 센서 입력 처리를 담당하도록 계층적으로 구성하였다.  

    또한 STATE_NORMAL, STATE_CRUISE, STATE_EMERGENCY의 상태 머신 구조를 통해 제어 우선순위를 명확히 정의하였으며, Emergency 모드가 항상 최우선으로 동작하도록 설계하였다. 조도 센서에는 히스테리시스 로직을 적용하여 노이즈에 의한 오동작을 방지하였고, 과속 및 비상 상황에서는 LED 및 부저를 활용한 경고 시스템을 구현하였다.