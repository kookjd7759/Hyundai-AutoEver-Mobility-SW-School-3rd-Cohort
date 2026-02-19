  <img
    src="https://i.namu.wiki/i/2qIUz7F5QHjrgQAnDWkImlqD7cXjMpjWNEfVm8QXsESVl9UjVX4A72cTaqaqh_VuA_vD0cECe9l_alyQPxmelQ9jbmZQ1RBLcf7misVcHd3pka1jZkQAxnKFqcdRYNl41M5eBFEbgfwsBu31DGKu-w.svg"
    width="120"
    align="left"
    hspace="20"
  />
  <div>
    <strong>Hyundai AutoEver Mobility SW School, 3rd Cohort (현대오토에버 모빌리티 SW 스쿨 3기) </strong><br/>
    <em>&nbsp;Embedded Software, 6th Cohort (임베디드 SW 6기) </em><br/>
    📅 Dec 2025 – Jun 2026 (Expected)
  </div>
  <br clear="left"/>
</div>

---

해당 **Hyundai AutoEver Mobility SW School, 3rd Cohort (현대오토에버 모빌리티 SW 스쿨 3기)** 과정은,  
🚗 현대자동차그룹의 SDV(Software Defined Vehicle) 및 모빌리티 소프트웨어 인재 양성을 목표로 한 실무 중심 교육 프로그램이다.

본 과정은 C/C++ 기반 임베디드 프로그래밍을 중심으로,  
자동차 전장 시스템 이해, 차량용 소프트웨어 개발 프로세스(A-SPICE, ISO 26262), 임베디드 Linux·MCU 개발, AUTOSAR, 차량 통신(Ethernet/RTOS), OTA,
그리고 자율주행 기능 구현까지 차량용 임베디드 소프트웨어 개발 전 주기를 단계적으로 학습하도록 구성되어 있다.

이 저장소는 해당 교육 과정 동안 수행한 실습, 과제, 프로젝트 결과물을 정리한 것으로,  
임베디드 시스템 설계부터 구현·최적화·테스팅에 이르는 실무 역량을 증명하는 것을 목표로 한다.  

## Course <sub>(2025.12.18 ~2026.06.29)</sub>  
### 01\. 미래 모빌리티 트렌드 <sub>(2025.12.18 ~ 2025.12.22)</sub>  
> **📝 학습 내용**  
> OT와 팀 빌딩, 미래 모빌리티 트렌드 리서치를 통한 모빌리티 혁신 아이디어 기획 및 팀 발표  

<details>
  <summary>📚 상세 학습 내용 보기</summary>
  
---

**1. 자동차 산업의 변화**  
- [자동차의 플랫폼화] - 자동차는 단순한 이동수단을 넘어, 다양한 센서와 통신기술을 기반으로 클라우드와 연결되는 소프트웨어 중심의 지능형 주행 플랫폼으로 변화하고 있다.  
- [CASE] - 미래 자동차 산업은 차량 간·인프라 간 연결(Connected), 공유·서비스 중심 이용 구조(Shared/Service), AI 기반 자율주행(Autonomous), 전기차 중심 전동화(Electric)로 재편되고 있다.  
이러한 변화로 자동차 산업은 연결성, 자율주행, 공유·서비스화, 전동화를 중심으로 성장하고 있으며, 시장 구조 역시 그 방향으로 전환되고 있다.  

**2. 모빌리티 비즈니스의 변환**  
모빌리티 비즈니스는 차량 판매 중심의 제조 산업에서 벗어나, 자율주행·전동화·플랫폼 기반 서비스 중심 산업으로 전환되고 있다.
소유에서 이용으로 패러다임이 변화하며 로보택시, 차량 인포테인먼트, V2G 에너지 서비스 등 새로운 수익 모델이 창출되고 있다.
결국 자동차는 단순한 이동수단이 아니라, 데이터와 네트워크를 기반으로 다양한 서비스를 제공하는 모빌리티 플랫폼으로 진화하고 있다.  

**3. 패러다임 변환 주요기술**  
- [HEV 하이브리드] - 내연기관과 전기모터를 함께 사용해 연비를 향상시키고 배출가스를 줄이는 차량으로, 외부 충전 없이 주행 중 배터리를 충전한다.  
- [EV 전기차] - 배터리에 저장된 전기에너지만으로 모터를 구동하는 차량으로, 배출가스가 없고 외부 전력 충전을 통해 운행한다.  
- [PHEV 플러그인 하이브리드] - 내연기관과 전기모터를 함께 사용하지만 외부 충전이 가능하며, 일정 거리까지는 전기만으로 주행할 수 있다.  
- [FCEV 수소전기차] - 수소와 산소의 화학반응을 통해 전기를 생산해 모터를 구동하는 차량으로, 배출물은 물(H₂O)만 발생한다.  
- [UAM (Urban Air Mobility) 도심 항공 모빌리티] - 도심 내 교통 혼잡을 해결하기 위해 전기 기반 수직이착륙기(eVTOL)를 활용해 사람과 화물을 운송하는 차세대 항공 모빌리티 시스템이다.  
- [하이퍼루프 (Hyperloop)] - 진공에 가까운 튜브 내부에서 캡슐형 열차를 자기부상 및 전자기 추진 방식으로 이동시켜 초고속으로 사람과 화물을 운송하는 차세대 교통 시스템이다.  

**4. SDV와 차량용 OS**  
- [SDV (Software Defined Vehicle)] - 차량의 기능과 성능이 하드웨어가 아니라 소프트웨어에 의해 정의되고, 업데이트를 통해 지속적으로 확장·개선되는 차량이다.  
- [차량용 OS (Automotive Operating System)] - 차량 내 ECU와 애플리케이션을 제어·관리하기 위해 사용되는 운영체제로, 하드웨어와 소프트웨어 사이를 연결하는 핵심 플랫폼이다.  

**5. 모빌리티 서비스 현황과 방향성**  
모빌리티 서비스는 단순한 교통수단 제공을 넘어, ICT 기반의 스마트 기술을 활용해 이동을 통합·최적화하는 서비스 개념으로 확장되고 있다.
기존 교통체계는 혼잡, 안전·환경 문제, 비효율성 등의 한계를 가지고 있으며, 이를 해결하기 위해 공유경제 확산, 플랫폼화, 데이터 기반 운영이 강화되고 있다.
특히 자율주행 기술 발전과 함께 로보셔틀, 무인 운송 서비스 등 스마트 모빌리티가 새로운 혁신 방향으로 제시되고 있다.  

**6. 한국자동차 산업의 현황과 대응전략**  
글로벌 탄소중립과 전기차·SDV 확산에 대응하기 위해 한국 자동차 산업은 친환경차 보급 확대와 충전·수소 인프라 확충에 집중하고 있다.
배터리 안전성 강화, 공급망 안정화, 핵심 소재·차세대 배터리 기술 개발을 통해 산업 경쟁력을 높이려는 전략을 추진 중이다.
또한 자율주행과 스마트 모빌리티 기술 고도화를 통해 미래 모빌리티 시장 선점을 목표로 하고 있다.  

<table align="left"> <tr> <td align="center">
      <img src="https://github.com/user-attachments/assets/e56df490-c14d-4822-a232-f5083c5033f6" width="220"/><br/>
      <sub><b>트렌드 리서치 결과 발표</b></sub> </td> </tr> </table> <br clear="left"/>

**7. Human-Centered Design in Mobility**  
인간 중심의 관점에서 소프트웨어를 활용해 모빌리티 문제를 해결하는 아이디어를 기획·발표하였으며, 제한된 시간 안에 몰입과 협업을 통해 이를 시각화된 결과물로 도출하는 팀 프로젝트 활동을 수행하였다.  

<table align="left"> <tr> <td align="center">
      <img src="https://github.com/user-attachments/assets/466de70e-10e8-4e7b-9278-03e7dded7dfb" width="220"/><br/>
      <sub><b>모빌리티 산업 아이디어 소개</b></sub> </td> </tr> </table> <br clear="left"/>

</details>
  
### 02\. C/C++ 프로그래밍 <sub>(2025.12.23 ~ 2026.01.08)</sub>  
> **📝 학습 내용**  
> C/C++ 프로그래밍 기초 학습  

<details>
  <summary>📚 상세 학습 내용 보기</summary>
  
---

**[ C ]**  
- C언어 소개  
- 비주얼 스튜디오 소개 및 설치  
- Hello, C!! - Hello C!! 출력 실습과 함께 C언어의 구조에 대해 대략적으로 알아봄  
- printf(), scanf(), getch(), getche(), getchar() 함수 이해  
- 자료형과 파생 자료형  
- 묵시적 형변환과 명시적 형변환  
- 연산자와 비트 연산자, sizeof() 연산자  
- C의 bool  
- for, if, while 문법  
- 배열  
- 함수  
- 기억 부류(Storage Class) - auto(지역 변수), extern(전역 변수), static(정적 변수), register(레지스터 변수)  
- 변수  
- 매크로  
- 포인터, 배열 포인터 - swap 함수 만들기 실습 및 포인터 이해  
- 문자열  
- 구조체  

**[ C++ ]**  
- C++ 언어 소개  
- 함수 오버로딩, 매개변수 기본값 설정  
- 인라인 함수  
- 네임스페이스  
- C++ 에서의 bool  
- 값에 의한 반환, 참조에 의한 반환  
- this 포인터  
- 메모리 동적 할당  
- 클래스, 정보은닉과 캡슐화  
- 접근 제어 지정자 - private, protected, public  
- 상수 멤버 함수  
- 클래스의 배열  
- 생성자와 소멸자  
- friend, static, const, mutable  
- 상속과 다형성  
- 가상함수  
- 연산자 오버로딩  
- string class  
- 템블릿, 클래스 템플릿  
- STL  
- 컨테이너  
- 예외 처리 - try-catch  
- 형변환 연산자  

</details>
  
### 03\. 모던 C++ <sub>(2026.01.09 ~ 2026.01.15)</sub>  
> **📝 학습 내용**  
> 모던 C++ 프로그래밍 기초 학습  

<details>
  <summary>📚 상세 학습 내용 보기</summary>
  
---

**[ Modern C++ ]**  
- 모던 C++ 소개  
- 형 변환
- 구조화된 바인딩  
- enum 클래스
- 유니폼 초기화
- 이동 생성자와 이동 대입 연산자
- 범위 기반 for 루프
- constexpr function and consteval function
- 자동 타입 추론 - auto, decltype
- 람다 표현식
- friend, static, mutable  

</details>
  
### 04\. 자동차 구조 및 전장시스템 이해 <sub>(2026.01.16 ~ 2026.01.21)</sub>  
> **📝 학습 내용**  
> 자동차의 바디·섀시 구조와 주요 전장 시스템의 구성 원리를 이해하고, 실제 차량 분해·조립 실습을 통해 제동·구동·에어백·스마트키 시스템의 작동 구조를 체계적으로 학습.  

<details>
  <summary>📚 상세 학습 내용 보기</summary>
  
---

**이론**  
- 바디 - 차체, 의장 부품, 전장 부품  
- 섀시 - 주행장치(현가장치, 조향장치, 제동장치), 구동장치(전륜, 후륜, 사륜, 엔진, 클러치, 변속기 자재이음, 종감속기어, 차동기어, 휠/타이어)  

**실습**  
**1. 내연 기관 바디 전장 전기 장치 실습**  
- [실습 차량] - 기아 K5(JF)  
- [내용] - 프런트 범퍼, 리어 범퍼, 전조등 분리, 방향 지시등, 리어 콤비네이션 램프, 전방 안개등, 퓨즈, 릴레이 점검, 번호판등 탈거 및 재장착  

<table align="left"> <tr> <td align="center">
      <img src="https://github.com/user-attachments/assets/ec2f1768-ec88-49bd-b9be-d0c01d3207c7" width="220"/><br/>
      <sub><b>프런트 범퍼 탈거 실습 중</b></sub> </td> </tr> </table> <br clear="left"/>

**2. 동력전달/섀시 구조 및 전기 장치 실습**  
- [실습 차량] - 현대 싼타페 DM  
- [내용] - 타이어, 브레이크 장치 탈거 및 재장착, 변속 시스템, 동력 전달 구조, 차동 장치, 타이어와 휠의 구조, 현가 장치, 승차 감각, 조향 장치, 제동 장치 실습  
  ABS (Anti-lock Braking System) - 급제동 시 바퀴가 잠기는 것을 방지해 조향 능력을 유지하도록 돕는 제동 보조 시스템.  
  BAS (Brake Assist System) - 긴급 상황에서 운전자의 급제동을 감지해 제동력을 자동으로 증폭시키는 시스템.  
  EBD (Electronic Brakeforce Distribution) - 차량 하중과 주행 상태에 따라 앞뒤 바퀴의 제동력을 전자적으로 배분하는 시스템.  
  TCS (Traction Control System) - 가속 시 바퀴의 헛돌림을 감지해 구동력을 제어하는 미끄럼 방지 시스템.  
  ESP (Electronic Stability Program) - 차량이 미끄러지거나 차체가 불안정할 때 제동력과 엔진 출력을 제어해 주행 안정성을 유지하는 시스템.

<table align="left"> <tr> <td align="center">
      <img src="https://github.com/user-attachments/assets/654f503e-2c14-41e4-9eb8-4687d2742901" width="220"/><br/>
      <sub><b>브레이크 장치 제거 실습 중</b></sub> </td> </tr> </table>

<table align="left"> <tr> <td align="center">
      <img src="https://github.com/user-attachments/assets/201781e3-2899-44a2-b1ea-d3ee1eeefcc7" width="220"/><br/>
      <sub><b>전체 부품 탈거 후</b></sub> </td> </tr> </table> <br clear="left"/>

**3. 에어백 및 스마트 키 전장시스템 실습**  
- [실습 차량] - 그랜저 하이브리드 2014년식
- [내용] - 도어 트림, 사이드미러, 윈도우 모터, 스피커, 아웃 사이드 핸들, 시트벨트 및 프리텐셔너, 에어백 컴퓨터, 요-레이트센서, 스마트키 안테나, ECM 리어 뷰 미러, 오버 헤드 콘솔 램프, 계기판, 흡기호스 및 에어클리터, 맵센서, 캠 포지션 센서 탈거 및 재장착

<table align="left"> <tr> <td align="center">
      <img src="https://github.com/user-attachments/assets/9ae86738-6400-4b90-8a0b-070efc864dde" width="220"/><br/>
      <sub><b>프런트 범퍼 탈거 실습</b></sub> </td> </tr> </table>

<table align="left"> <tr> <td align="center">
      <img src="https://github.com/user-attachments/assets/7899641c-ad0a-42da-9572-2913f077b7c9" width="220"/><br/>
      <sub><b>시트 탈거 후</b></sub> </td> </tr> </table> <br clear="left"/>

</details>

### 05\. 자동차 소프트웨어 개발 프로세스 <sub>(2026.01.22 ~ 2026.01.28)</sub>  
> **📝 학습 내용**  
> 자동차 소프트웨어 개발 전 과정을 V-Model 기반으로 이해하고, A-SPICE·ISO 26262 표준에 따른 요구사항·설계·테스트·형상관리까지 체계적으로 학습  

<details>
  <summary>📚 상세 학습 내용 보기</summary>
  
---
 


</details>
  
### 06\. 임베디드 소프트웨어 개발 기초 <sub>(2026.01.29 ~ 2026.02.02)</sub>  
> **📝 학습 내용**  
> LED·버튼·부저·DC 모터·센서(DHT) 제어를 중심으로 GPIO, PWM, ADC, 시리얼 통신 및 AVR 레지스터 직접 제어를 실습하며 임베디드 소프트웨어 개발 기초를 학습    

<table align="left"> <tr> <td align="center">
      <img src="https://github.com/user-attachments/assets/20f3b36f-ede5-4cc4-ad24-5c4a98f1a4dd" width="220"/><br/>
      <sub><b>LED 점등 응용 실습</b></sub> </td> </tr> </table> <br clear="left"/>

<table align="left"> <tr> <td align="center">
      <img src="https://github.com/user-attachments/assets/b1bcd98f-2e67-4135-a045-061ffb89cd7f" width="220"/><br/>
      <sub><b>DC 모터 제어 실습</b></sub> </td> </tr> </table> <br clear="left"/>


### 07\. 임베디드 C 프로그래밍 기초 <sub>(2026.02.03 ~ 2026.02.09)</sub>  
> **📝 학습 내용**  
> STM32 기반 임베디드 시스템 개발 전반을 이해하고, GPIO·HAL·인터럽트·통신(UART/I2C/SPI)·아날로그 입력·타이머·PWM·센서 제어까지 이론과 실습으로 익히는 과정.

 <details>
  <summary>📚 상세 학습 내용 보기</summary>

  **이론**  
  - **[임베디드 시스템]** - 특정 기기에 MCU와 SW를 내장하여, 미리 정해진 고유한 기능만을 수행하도록 설계된 전용 컴퓨터 시스템
    - 특징 : 단일/특정 작업만 수행, 하드웨어와 펌웨어가 밀접하게 결합, 제한된 resource, 실시간성
    - 구조 : 입력 센서 - Main Processor - 출력 엑츄에이터  
  
  - **[CPU/GPU/MPU/MCU]**
    - CPU = Central Processing Unit  
    - GPU = Graphic Processing Unit  
    - MPU = Micro Processing Unit  
    - MCU = Micro Controller Unit : CPU + 메모리 + 인터페이스, 단독 동작이 가능하도록 만들어짐  

  - **[개발용 MCU 보드 종류]** - 초보용 MCU(아두이노, 라즈베리), 상용 MCU(ST(Nucleo 시리즈), 노르딕, 인피니언)  
  
  - **[Volatile keyword]** - 해당 변수를 최적화에서 제외하여 항상 메모리에 접근하도록 컴파일  
    - 사용법 : volatile data_type variable_name  
    - 필요성 : MIMO, 멀티 쓰레드 환경, 인터럽트 서비스 루틴  
  
  - **[MCU Bring-up]** - 전원이 인가된 MCU가 “의도한 소프트웨어를 실행할 수 있는 최소한의 정상 상태”까지 올라오는 과정  

  - **[I/O Access 방법]** - Special Instruction I/O, Memory-mapped I/O, Direct memory access (DMA)  

  - **[HAL]**
    - Basic HAL - 특정 MCU 아키텍처에 맞춰 하드웨어 기능을 직접 추상화한 기본 수준의 HAL (GPIO, SYSTICK, NVIC, RCC, FLASH etc)  
    - Generic HAL - 하드웨어 독립적인 공통 인터페이스를 정의하여 다양한 플랫폼에서 동일한 상위 코드를 가능하게 하는 HAL  

  - **[컴파일과 링킹]**
    - 컴파일(Compile) - 소스코드(.c)를 기계어 오브젝트 파일(.o)로 번역하는 과정
    - 링킹(Linking) - 여러 오브젝트 파일과 라이브러리를 연결해 실행 파일을 만드는 과정
    - 링커(Linker) - 오브젝트 파일들을 연결하고 주소를 배치해 최종 실행 파일을 만드는 프로그램  

  - **[GPIO]** - 마이크로컨트롤러의 핀을 입력 또는 출력으로 설정해 외부 장치와 디지털 신호를 주고받는 범용 입출력 인터페이스  

  - **[Pull up과 Pull down 저항]**
    - Pull up 저항 - 입력 핀이 아무것도 연결되지 않았을 때 기본값을 HIGH(1)로 유지하게 해주는 저항
    - Pull down 저항 - 입력 핀이 떠 있지 않도록 기본값을 LOW(0)로 유지하게 해주는 저항
    - 위 둘은 입력 핀이 아무것도 연결되지 않아 값이 불안정하게 변하는 floating 상태를 방지하기 위해 기본 전압(0 또는 1)을 안정적으로 정해주기 위해 사용한다.  

  - **[HAL]** - TM32의 레지스터를 직접 다루지 않고도 주변장치(GPIO, UART, SPI 등)를 쉽게 제어할 수 있도록 만든 하드웨어 추상화 라이브러리  

  **실습**  
  - [Board] - STM32 Nucleo-L073RZ  
  - [development enviroment] - Windows, Keil MDK_ARM, STM32CubeMX  
  
  **1. 개발 환경 설치**  
  MDK-ARM 설치 및 Licensing, STM32CubeMX 설치  

  **2. 첫번째 프로젝트**  
  개발 환경 설정, 변수 및 제어문, 컴파일과 디버깅, 최적화 옵션 및 Volatile 키워드, Data Type 크기, 포인터, 비트 연산 실습  
  
  **3. LED Blinking**  
  포인터, define, MACRO 비트 연산자, Counting 함수 생성 및 활용, structure, LED 제어 및 PIN Map 작성 실습  

  **4. HAL 사용하기**  
  STM32L0 Source Download, HAL Driver를 이용한 LED 제어, NUCLEO BSP(Board Support, Package)를 이용한 LED 제어  
  


</details>
  

### 08\. 임베디드 리눅스 시스템 프로그래밍 <sub>(2026.02.10 ~ 2026.02.13)</sub>  
> **📝 학습 내용**  
> 라즈베리파이 기반 리눅스 환경에서 시스템 구조와 동작 원리를 이해하고, C 프로그래밍과 네트워크 통신을 통해 임베디드 리눅스 시스템 전반을 학습  

### 09\. 임베디드 MCU 프로그래밍 <sub>(2026.02.19 ~ 2026.02.25)</sub>  
> **📝 학습 내용**  
>   

### 10\. 임베디드 C 프로그래밍 코드 최적화 기법 <sub>(2026.02.26 ~ 2026.03.04)</sub>  
> **📝 학습 내용**  
>   

### 11\. 소프트웨어 요구사항 분석 및 설계 <sub>(2026.03.05 ~ 2026.03.11)</sub>  
> **📝 학습 내용**  
>   

### 12\. 자동차 사이버보안 및 시큐어코딩 이해 <sub>(2026.03.12 ~ 2026.03.18)</sub>  
> **📝 학습 내용**  
>   

### 13\. AUTOSAR 기본 <sub>(2026.03.19 ~ 2026.03.24)</sub>  
> **📝 학습 내용**  
>   

### 14\. 차량용 임베디드 SW 개발 프로젝트 <sub>(2026.03.25 ~ 2026.04.08)</sub>  
> **📝 학습 내용**  
>   

### 15\. 차량용 이더넷 통신 이해 <sub>(2026.04.09 ~ 2026.04.15)</sub>  
> **📝 학습 내용**  
>   

### 16\. 차량용 통신시스템 <sub>(2026.04.16 ~ 2026.04.22)</sub>  
> **📝 학습 내용**  
>   

### 17\. 차량용 신시간 운영체제 기반 프로그래밍 <sub>(2026.04.23 ~ 2026.04.28)</sub>  
> **📝 학습 내용**  
>   

### 18\. 차량용 통신시스템 구현 프로젝트 <sub>(2026.04.29 ~ 2026.06.01)</sub>  
> **📝 학습 내용**  
>   

### 19\. 소프트웨어 테스팅 <sub>(2026.06.02 ~ 2026.06.08)</sub>  
> **📝 학습 내용**  
>   

### 20\. OTA <sub>(2026.06.09 ~ 2026.06.15)</sub>  
> **📝 학습 내용**  
>   

### 21\. 자율주행 기능 구현 프로젝트 <sub>(2026.06.16 ~ 2026.06.29)</sub>  
> **📝 학습 내용**  
>   

