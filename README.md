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

---

</details> <br>
  
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

---

</details> <br>
  
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

---

</details> <br>
  
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

---

</details> <br>

### 05\. 자동차 소프트웨어 개발 프로세스 <sub>(2026.01.22 ~ 2026.01.28)</sub>  
> **📝 학습 내용**  
> 자동차 소프트웨어 개발 전 과정을 V-Model 기반으로 이해하고, A-SPICE·ISO 26262 표준에 따른 요구사항·설계·테스트·형상관리까지 체계적으로 학습  

<details>
  <summary>📚 상세 학습 내용 보기</summary>
  
---

**자동차 SW 개발 프로세스**
- [자동차의 전장화와 소프트웨어 시스템의 증가] - 현재 자동차 산업은 SDV(Software Defined Vehicle)로 전환하며 소프트웨어가 차량의 전 방위적인 혁신을 일으키는 가장 중요한 요소가 되고 있음
- [글로벌 차동차 OEM 업체들의 품질 요구사항] - 프로세스 품질(Process Quality)과 제품 품질(Product Quality)의 두 가지 관점으로 구분되어 평가됨
- [소프트웨어의 정의와 특징] - 소스 코드 프로그램을 포함하여 프로세스를 통해 만들어진 모든 산출물(요구사항 명세서, 아키텍처 설계서, 소스코드 프로그램, 테스트 결과서)을 말하며, 구조가 눈에 보이지 않는 비가시성(Invisibility), 비선형적인 구조(Non-Linearity), 마모되지 않고 계속되어 변형(Does not wear out but change), 사람 중심의 작업(Human intensive)이라는 특징이 있음
- [소프트웨어 공학 개요] - 재사용 및 생산성 향상/소프트웨어 품질 향상/개발 및 유지보수 비용 절감/일정 준수를 목표로 개발, 운용, 유지보수 및 폐기에 대한 체계적인 공학적 접근 방법
- [소프트웨어 개발 프로세스와 생명 주기]
  - 프로세스: 고객의 요구사항을 만족하는 제품을 만들기 위한 절차/방법, 도구/장비, 인력의 통합
  - 소프트웨어 개발 프로세스: 소프트웨어 개발에 필요한 절차/방법, 도구/장비, 인력의 통합
  - 소프트웨어 개발 생명주기: 소프트웨어를 어떻게 개발할 것인가에 대해 정의한 최상위 수준의 프로세스(주먹구구식, 폭포수, 원형, 나선형 모델 등등)
- [V-Model] - 개발 생명주기의 각 단계와 그에 상응하는 소프트웨어 테스트 단계를 매핑한 모델
- **[A-SPICE]** - A-SPICE는 자동차 소프트웨어 개발 프로세스를 평가하기 위한 국제 표준으로, 조직의 프로세스 성숙도를 Capability Level 0~5로 구분하여 평가한다

  | Level   | 설명                                  |
  |:-------:|---------------------------------------|
  | Level 0 | 프로세스가 정의되어 있지 않음          |
  | Level 1 | 프로세스가 수행되고 있음 (비관리 상태) |
  | Level 2 | 계획되고 관리됨 (문서화 및 추적 가능)  |
  | Level 3 | 조직 차원의 표준 프로세스로 정립됨     |
  | Level 4 | 정량적 데이터 기반으로 관리됨          |
  | Level 5 | 지속적 개선 체계가 확립됨              |  
  
  실무에서는 일반적으로 Level 2 이상을 요구하는 경우가 많다.
    
  A-SPICE는 개발 활동을 여러 프로세스 영역(Process Area)으로 나누어 정의한다.
  그 중 핵심 영역은 SYS (System Level) 과 SWE (Software Level) 이다.
  
  **SYS – System Level**  - 시스템 단위(ECU 포함)의 상위 개발 단계에 해당한다.  
  | 코드   | 의미                     | 설명 |
  |--------|--------------------------|------|
  | SYS.1  | 시스템 요구사항 분석     | 이해관계자 요구사항을 시스템 요구사항으로 정의 |
  | SYS.2  | 시스템 아키텍처 설계     | 시스템 구성 요소 및 인터페이스 정의 |
  | SYS.3  | 시스템 통합 및 테스트    | 구성 요소 통합 후 시스템 수준 시험 |
  | SYS.4  | 시스템 검증              | 요구사항 충족 여부 검증 |

  **SWE – Software Level (임베디드 개발 핵심 영역)** - 임베디드 소프트웨어 개발자가 직접 수행하는 핵심 프로세스 영역
  | 코드   | 의미                         | 설명 |
  |--------|------------------------------|------|
  | SWE.1  | 소프트웨어 요구사항 분석     | 시스템 요구사항을 SW 요구사항으로 구체화 |
  | SWE.2  | 소프트웨어 아키텍처 설계     | SW 구성 요소 및 구조 정의 |
  | SWE.3  | 상세 설계 및 구현            | 모듈 설계 및 코딩 |
  | SWE.4  | 단위 테스트                  | 개별 모듈 기능 검증 |
  | SWE.5  | 소프트웨어 통합 테스트       | 모듈 통합 후 기능 시험 |
  | SWE.6  | 소프트웨어 검증              | 요구사항 대비 검증 |

  
- **[ISO 26262]** - 자동차 전기·전자(E/E) 시스템의 기능 안전(Functional Safety)을 위한 국제 표준.  
  브레이크, 조향, 에어백, ADAS 등 생명과 직결된 시스템이 포함된 자동차 환경에서 소프트웨어 오류는 인명 사고로 이어질 수 있으므로, ISO 26262는 **고장이 발생하더라도 위험을 허용 가능한 수준으로 감소시키는 것**을 목표로 한다.
  
  ISO 26262 핵심 개념  
  | 구분 | 내용 |
  |------|------|
  | 목적 | 자동차 전기/전자 시스템의 기능 안전 확보 |
  | 적용 대상 | 차량 내 E/E 시스템 및 소프트웨어 |
  | 핵심 개념 | 고장 발생 시에도 안전 상태 유지 (Fail-Safe) |
  | 위험 분석 | HARA(Hazard Analysis and Risk Assessment) 수행 |
  | 안전 등급 | ASIL A ~ D (위험도 기반 등급 체계) |

  ASIL (Automotive Safety Integrity Level) 등급 체계  
  | 등급 | 의미 |
  |------|------|
  | QM     | 일반 품질 관리 수준 (안전 요구 없음) |
  | ASIL A | 낮은 위험 수준 |
  | ASIL B | 중간 위험 수준 |
  | ASIL C | 높은 위험 수준 |
  | ASIL D | 최고 위험 수준 (가장 엄격한 안전 요구) |


  - [소프트웨어 설계] - 프로그램을 구현하기 전에 소프트웨어를 구성하는 요소와 구조를 정의해 구현의 기반을 만드는 활동
    - 소프트웨어 아키텍처 설계: 상위 수준에서 소프트웨어 구성요소들 간의 관계로 구성된 전체적인 구조를 설계하는 활동
    - 소프트웨어 상세 설계: 아키텍처 설계에서 도출된 소프트웨어 구성 요소(컴포넌트, 모듈)들의 내부 데이터와 알고리즘 로직 등을 설계하는 활동
  - [소프트웨어 구현 및 통합]
    - 소프트웨어 구현: 개별 소프트웨어 단위를 실행 가능한 형태로 구현하고 이를 문서화 하는 활동으로 프로그래밍이라고도 함
    - 소프트웨어 통합: 개발된 단위 모듈 소프트웨어들을 통합 계획에 따라 통합하여 완전한 소프트웨어 구조를 개발하는 활동(빅뱅, 하향식, 상향식, 지속적 통합 등)
  - [소프트웨어 검증 및 확인] - 제품이 올바르게 만들어지고 있는가, 올바른 제품을 만들고 있는가
  - [소프트웨어 테스팅] - 정상 조건 및 비정상 조건(결함이나 버그) 사이의 차이점을 발견하기 위하여 소프트웨어 항목을 분석하고, 분석된 항목의 특성을 평가하는 프로세스(단위, 통합, 시스템, 인스 테스팅)
  - [소프트웨어 형상 관리] - 형상 항목을 식별하고, 변경을 통제 및 처리 상태를 모니터링함으로써 요구사항에 부합하는지 확인하는 활동  
    
---

</details> <br>
  
### 06\. 임베디드 소프트웨어 개발 기초 <sub>(2026.01.29 ~ 2026.02.02)</sub>  
> **📝 학습 내용**  
> LED·버튼·부저·DC 모터·센서(DHT) 제어를 중심으로 GPIO, PWM, ADC, 시리얼 통신 및 AVR 레지스터 직접 제어를 실습하며 임베디드 소프트웨어 개발 기초를 학습    

<details>
  <summary>📚 상세 학습 내용 보기</summary>
  
---

**이론**  
1. 아두이노
2. 오픈소스 하드웨어
3. Arduino IDE - 스케치
4. 아두이노 우노 보드와 두에 보드
5. 아두이노 시뮬레이터 TinkerCAD와 Fritzing
6. 부저(Buzzer)
7. AVR
8. ATTiny85
9. ATMega328P datasheet 및 주변기기, 인스트럭션
10. ATMega128
11. S/W 도규: AVR 컴파일러의 종류
12. 트러블슈팅, write가 안될때, debugWIRE 활성화
13. ATMega328P GPIO 레지스터 및 설정
14. DDRB, PORTB  

**실습**  
1. 아두이노 개발 환경 구축 및 Hello, Arduino Uno !
2. 아두이노 외부 LED 1개 점등
3. 아두이노 외부 LED 여러개 점등
4. 아두이노 스위치 1개 입력 받기
5. 아두이노 스위치 여러개 입력 받기
6. 아두이노 내부 풀업 사용해보기
7. 스위치를 입력받아 LED의 패턴 바꿔보기  

    <table align="left"> <tr> <td align="center">
          <img src="https://github.com/user-attachments/assets/20f3b36f-ede5-4cc4-ad24-5c4a98f1a4dd" width="220"/><br/>
          <sub><b>LED 점등 응용 실습</b></sub> </td> </tr> </table> <br clear="left"/>  

8. 아두이노 시리얼 통신 소개 및 Hello, Arudion Serial
9. 시리얼 통신 연습: Write 및 정수 값 출력
10. 시리얼 통신으로 입력받아 LED 점등하기
11. Dummy 센서값을 시리얼 통신으로 출력
12. 아두이노 시리얼 플롵 사용해보기
13. UART 데이터 프레임 분석
14. 아두이노 문자열 다루기
15. 아날로그 출력을 이용한 LED 밝기 조절
16. 아날로그-디지털 변환기 첫번째 예제 Hello, ADC!!
17. 가변저항을 ADC로 읽기
18. 가변 저항으로 온도 센서 VB100을 모의
19. 아두이노 타이머 사용
20. 아두이노 SimpleTimer
21. 아두이노 우노 외부 인터럽트
22. 아두이노 DC 모터 구동  

    <table align="left"> <tr> <td align="center">
          <img src="https://github.com/user-attachments/assets/b1bcd98f-2e67-4135-a045-061ffb89cd7f" width="220"/><br/>
          <sub><b>DC 모터 제어 실습</b></sub> </td> </tr> </table> <br clear="left"/>

23. Hello, Buzzer !!
24. 도레미파솔라시도 연주
25. 아두이노 tone 함수
26. 음표를 데이터형으로 만들어보기
27. GPIO 입출력
28. 핀체인지 인터럽트
29. ATMega328P: 인터럽트
30. ATMega328P Hello, UART !!
31. ATMega328P 타이머를 이용하여 1ms 출력
32. ADC로 가변저항 입력 받기  

---

</details> <br>

### 07\. 임베디드 C 프로그래밍 기초 <sub>(2026.02.03 ~ 2026.02.09)</sub>  
> **📝 학습 내용**  
> STM32 기반 임베디드 시스템 개발 전반을 이해하고, GPIO·HAL·인터럽트·통신(UART/I2C/SPI)·아날로그 입력·타이머·PWM·센서 제어까지 이론과 실습으로 익히는 과정.

 <details>
  <summary>📚 상세 학습 내용 보기</summary>

---
  
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

  - **[임베디드 시스템 HW 구조]** - 메인 프로세서, 입력 센서, 출력 엑츄에이

  - **[USART]** - 마이크로컨트롤러가 외부 장치와 직렬 방식으로 데이터를 주고받기 위한 통신 모듈


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

  <table align="left"> <tr> <td align="center">
        <img src="https://github.com/user-attachments/assets/470e5dd0-8b5d-445b-bff8-2e681a3c48ac" width="220"/><br/>
        <sub><b>외부 LED Bliking, 내부 LED toggle</b></sub> </td> </tr> </table> <br clear="left"/>

  **5. User Button 실습**  
  User Button을 이용해 내부/외부 LED 제어, 진동감지센서 연동

  **6. UART 실습**  
  UART를 이용한 PC와 Device간 Serial Communication, TX/RX 실습,  UART TX를 이용한 Printf함수 만들기  
  
  **7. 아날로그 센서 실습**  
  LM35(Analog Input) 온도센서 소개 및 연결, LM35 센서 연결 및 온도 읽기

  **8. Clock  실습**  
  Clock Config, 1초 인터럽트 타이머를 이용한 LED Blinking, PWM을 이용한 LED 밝기 제어, 1us counter 만들기, 초음파 센서를 이용한 거리 재기  
  
---

</details> <br>

### 08\. 임베디드 리눅스 시스템 프로그래밍 <sub>(2026.02.10 ~ 2026.02.13)</sub>  
> **📝 학습 내용**  
> 라즈베리파이 기반 리눅스 환경에서 시스템 구조와 동작 원리를 이해하고, C 프로그래밍과 네트워크 통신을 통해 임베디드 리눅스 시스템 전반을 학습  

<details>
  <summary>📚 상세 학습 내용 보기</summary>
  
---  

  **이론**  
  **1. 리눅스 기초 사용법 익히기**  
  파일 및 폴더 다루기, vi 에디터 사용법, 기본 C 프로그래밍, 셸 사용하기, 파일권한관리, 프로세스 제어  

  **2. 임베디드 리눅스의 이해**  
  리눅스 커널의 구조 이해  

  **3. 소켓 프로그래밍**  
  TCP/IP 프로토콜의 기본 개념 이해, IP주소와 port 번호, 소켓 관련 구조체와 함수 이해

  **실습**  
  **1. 라즈베리파이 리눅스 설치**  
  Raspberry Pi Imager download, terminal 기반의 실습 환경 구성  
  
  **2. 임베디드 리눅스 GPIO 프로그래밍**  
  리눅스의 Memory Mapped IO를 사용하여 GPIO 프로그래밍  
  
  **3. 디바이스 드라이버 개발**  
  디바이스 드라이버의 구조에 대해 이해, GPIO  디바이스 드라이버를 작성하여 구동

  <table align="left"> <tr> <td align="center">
        <img src="https://github.com/user-attachments/assets/a4922e16-afa4-4f3b-84df-63922619603e" width="220"/><br/>
        <sub><b>7 Segment 드라이버 개발</b></sub> </td> </tr> </table> <br clear="left"/>

  **4. 소켓 프로그래밍**  
  소켓을 이용한 통신 프로그램 작성

---  

</details> <br>

### 09\. 임베디드 MCU 프로그래밍 <sub>(2026.02.19 ~ 2026.02.25)</sub>  
> **📝 학습 내용**  
> Infineon AURIX TC275 기반 Bare-metal 프로그래밍을 수행하며, 레지스터 직접 제어부터 iLLD 활용, 인터럽트·타이머·SPI·ADC·PWM·스케줄러 구현까지 차량용 MCU 실전 제어 흐름을 단계적으로 학습  

<details>
  <summary>📚 상세 학습 내용 보기</summary>

---  

  **1. 개발 환경 구축 (AURIX TC275)**  
  - [Board] : ShieldBuddy TC275 (Hitex)  
  - [IDE] : AURIX Development Studio (ADS)  
  - [Architecture] : TriCore (CPU0 사용)  
    
  - AURIX Development Studio 설치 및 Debug 환경 설정, CPU0 기반 프로젝트 생성.  
  - Build -> Debug -> Register/Assembly 확인  
  -  Datasheet vs User Manual 차이 이해  

  **2. GPIO 직접 레지스터 제어**  
  - LED 출력 제어 - IOCR 레지스터 분석, Base Address 계산, Output Register 세팅, Memory-mapped I/O 직접 접근  
  - SW 입력 제어 - Pull-up 회로 이해, Pn_IN 레지스터로 입력값 읽기, 입력값 기반 LED 제어  

  **3. 외부 인터럽트**  
  - Polling vs Interrupt 비교 - Polling: 순차 처리 -> 지연 발생 가능, Interrupt: 우선순위 기반 즉시 처리  
  - ERU 구성 요소 - ERS (External Request Selection), ETL (Event Trigger Logic), OGU (Output Gating Unit), Interrupt Router (IR)  
  - 설정 흐름 - EICR 설정 (Falling Edge), IGCR 설정, SCUERU 설정 (CPU0 지정), ISR 등록  

  **4. Internal Timer Interrupt (STM)**  
  - Timer 구조 이해 - Crystal -> PLL -> 200MHz, STM 모듈 사용, Compare Match 기반 Interrupt 발생
  - iLLD 사용 이유 - 레지스터 직접 제어 부담 감소, 가독성 향상, 개발 시간 단축
  - 1초 LED Blink 구현 - STM Compare 값 설정, ISR에서 Toggle  

  **6. SPI를 활용한 4-FND 제어**  
  - 통신 이론 - Serial vs Parallel, 동기 vs 비동기, Full duplex vs Half duplex  
  - SPI 구성 - MOSI / MISO, SCK, SS, CPOL / CPHA  
  - TM74HC595 기반 FND 제어 - Bit shift 전송, SCLK, RCLK, DIO 직접 GPIO 제어, 7-segment 배열 정의  

  <table align="left"> <tr> <td align="center">
        <img src="https://github.com/user-attachments/assets/f63962b2-64d8-43c3-b568-bdfc31e1a787" width="220"/><br/>
        <sub><b>Timer 묘사</b></sub> </td> </tr> </table> <br clear="left"/>

  **5. Non-Preemptive Task Scheduler**  
  - 1ms/10ms/100ms/1000ms 단위로 작업을 분리하거나, Multi-tasking 구조 구현하기 위해 필요  

  <table align="left"> <tr> <td align="center">
        <img src="https://github.com/user-attachments/assets/67eac0cc-83aa-40c4-aa59-5af1e738d3aa" width="220"/><br/>
        <sub><b>스톱워치 시스템 구현</b></sub> </td> </tr> </table> <br clear="left"/>

  **7. ADC (Analog to Digital Converter)**  
  - 12bit 해상도 - 0 ~ 4095 값 변환, Group4 Channel7 사용  
  - 변환 흐름 - ADC 초기화 -> Conversion 요청 -> 결과 획득 (RES 레지스터)  

  **8. PWM (GTM-TOM 기반)**  
  - PWM - Duty Cycle 제어, 평균값 기반 아날로그 효과  
  - GTM 구조 - GTM → TOM → Output Pin, Fxclk 설정, Period & Duty 설정
  
---

</details> <br>

### 10\. 임베디드 C 프로그래밍 코드 최적화 기법 <sub>(2026.02.26 ~ 2026.03.04)</sub>  
> **📝 학습 내용**  
>   임베디드 시스템의 메모리 구조와 TC275 부트 시퀀스를 이해하고, 컴파일러 최적화와 데이터 처리 및 흐름 제어 기법과 ROM/RAM 사용량과 실행 시간을 줄이기 위한 임베디드 C 코드 최적화 방법, 그리고 MISRA-C/BARR-C 기반의 안정적인 코딩 가이드라인을 학습하였다.

<details>
  <summary>📚 상세 학습 내용 보기</summary>

---  

  **1. 프로그램 실행을 위한 메모리 구조**  
  - 메모리 구조 : <sub>High memory</sub> | 스택 | 힙 | 데이터 | 코드 | <sub>Low memory</sub>
    - 코드 영역 : 실행할 프로그램의 코드가 저장
    - 데이터 영역 : 전역 변수와 정적 변수가 저장 (프로그램 시작과 함께 할당 → 종료되면 소멸)
    - 힙 영역 : 사용자가 직접 관리할 수 있는 영역으로 메모리 공간이 동적으로 할당 및 해제, 낮은 메모리 주소로부터 높은 주소 방향으로 할당, 메모리 엑세스가 느림
    - 스택 영역 : 지역 변수와 매개변수가 저장되는 영역으로 높은 메모리 주소부터 낮은 주소의 방향으로 할당, 메모리 엑세스가 빠름  
    
  - ROM, RAM
    - RAM : CPU와 HDD 사이에 위치하여 자주 쓰는 데이터를 저장해 고속 처리 (Static RAM, Dynamic RAM)
    - ROM : 전원이 꺼져도 데이터가 유지되는 읽기 전용 비휘발성 메모리. (MASK ROM, Programmable ROM, etc.)
  
  **2. TC275 부트시퀀스**  
  - Linker : 불완전한 object 파일들을 합쳐 모든 코드와 데이터를 포함하는 새로운 object 파일을 생성해 내는 도구
  - Locate : 메모리에서 실행 가능하도록 코드와 데이터를 배치하여 최종 바이너리 이미지를 생성하는 도구

  **3. TC275 코드 프로파일링**
  - Disassembly를 활용한 코드 성능 파악

  **4. 컴파일러 최적화**
  - 코드 최적화 : 주어진 코드에 대해 동등한 의미를 가지면서 실행시간 및 메모리를 줄이는 것  
    - 핍홀 최적화 - 몇 개의 연속적인 명령어를 하나의 명령어나 더 짧은 명령어로 변환  
    - 지역 최적화 - 부분적인 관점에서 일련의 비효율적인 코드를 구분해내고 좀 더 효율적인 코드로 만드는 방법  
    - 루프 최적화 - 한 루프 안에서의 최적화 기법  
    - 전역 최적화 - 한 프로시저 내에서 일련의 비효율적인 코드를 구분해내고 좀 더효율적인 코드로 만드는 방법  
    - 프로시저 간 최적화 - 한 프로시저의 한계를 넘어 전체 프로그램에 적용되는 최적화  
    - 기계 종속적 최적화 - 기계의 특성에 따라 달라질 수 있는 성능을 최적화  
  - 기본 블록과 흐름 그래프 : 최적화 기법을 수행하기 위한 방법 중 일부  
    - 기본 블록 - 지역 최적화의 기본 단위
    - 흐름 그래프 - 기본 블록의 집합에 제어 흐름에 관한 정보를 추가하여 만든 유향 그래프  
  
  **5. 임베디드 C코드 최적화**
  - ROM 최적화 : 프로그램 코드, 상수, 초기화된 전역 변수와 정적 변수 최적화 즉, 코드와 상수를 줄이는 것
  - RAM 최적화 : 초기화되지 않은 전역변수와 정적 변수, 지역변수, 함수의 인자 및 함수 호출시 발생하는 context 최적화 즉, Stack의 사용량을 줄이는 것
  - 실행시간 최적화 : 인라인 함수 사용, 참조 테이블 및 인라인 어셈블리어 활용, 전역 변수 사용을 통한 함수 호출 오버헤드 회피, 폴링방식 및 정수 연산 활용  

  **6. Data Handling (데이터 처리)**
  - Data Types Usage : 올바른 Data Type을 사용하여, 불필요한 변수 크기로 인한 처리시간 증가를 예방  
  - Avoid Type Coversion : 처리를 위해 동일한 타입의 변수를 사용하여, 타입 변환으로 인한 처리시간 증가를 예방  
  - Signed & Unsigned의 구분  
    - Unsigned = 몫/나머지, loop cointer, array, indexing
    - Signed = 사칙 연산
  - Floats & Doubles : Float의 최대 값 = 0x7F7F FFFF, Double의 최대 값 = 0x7F7F FFFF FFFF FFFF, 불필요한 유형 변환이나 혼동을 피하기 위해 숫자 값 뒤에 문자 'f'를 지정
  - Constant : 데이터를 상수로 정의해 ROM 공간에 할당
  - Volatile : 컴파일러가 변수에 대한 최적화를 수행하지 못하도록 금지
  - Data Alignment - Arrangement & Paking : 메모리 structure를 고려한 변수를 선언
  - Pass bt Reference : 매개변수 수가 많을수록 각 함수 호출에서 push 및 pop 동작이 많기 때문에 오버헤드를 줄이기 위해 structure 참조를 파라미터로 전달하는 것이 더욱 효율적  
  - Initialization : Memory allocation 방법을 적절히 사용하여 RAM 사용량을 줄이는 것이 가능
  - Return 값 : 함수의 Return 값은 레지스터에 저장되며, Return data가 의도된 용도로 사용되지 않는다면 낭비가 됨
  - 비트 플래그 사용 : 상태를 나타내기 위한 플래그로 비트 연산(비트 플래그)을 사용  

  **7. Flow Control Handling (흐름제어 처리)**
  - if VS switch : 하나의 var를 받아 control flow를 짤 때에는 switch가 효율적, if는 값에 따라 시간이 달라지지만 switch는 lookup table 방식이기 때문
  - Inline : 컴파일러가 함수에 대한 모든 호출을 함수 코드의 복사본으로 대체하여 함수 호출과 관련한 오버헤드가 제거 (큰 함수를 인라인 할 경우 실행 파일의 크기가 너무 커짐)
  - Loop Hoisting : 컴퓨팅 시간 절약을 위해 후프 외부로 계산 이동
  - Loop overhead : MCU에는 양수에서 0으로 카운트다운 할 때 잘 작동, "미리 정의된 숫자"보다 "0"을 감지하는 것이 더 쉬움
  
  **8. Other Handling (기타 처리)**
  - Use of operators : 기존 연산자 적극 활용
  - Replacing, Integer Division with Multiplication : 정수 나눗셈은 모든 정수 연살 중 가장 느리므로, 식에 여러 나눗셈이 있는 경우 정수 나눗셈을 곱셈으로 변경
  - Use of Fomula : 곱셈을 최대한 피하고 덧셈으로 치환
  - Inline assembly : 어셈블리 명령들을 inline 함수로 작성
  - 부동소수점 연산 제거
  - 조건문 최적화

  **9. 신뢰성 있는 코드를 위한 가이드 라인**
  - [MISRA-C] : Moter Industry Software Reliablility Association, 자동차 산업 소프트웨어 신뢰성 협회로, 자동차 임베디드 소프트웨어의 결함을 줄이기 위해 탄생했지만 많은 개발자 스타일 가이드 라인으로 사용됨
  - [BARR-C] : Barr 그룹에서 발표한 자동차 임베디드 C코딩 표준으로 유지보수성과 이식성 향상이 목적
  - Line Widths : 프로그램 내 모든 줄의 너비는 최대 80자
  - Keywords to Avoid : auto/register 사용 불가, goto, continue 사용 지향
  - Braces : if, else, while 등의 코드 블록은 항상 중괄호로 둘러싸기 및 중괄호는 단독 줄에 표시
  - Parentheses : 수식은 명확성을 위해 괄효 사용
  - Common Abberviations : 약자는 되도록 '약어표'를 참조
  - Casts : 각 형변환에는 가능한 값 범위에서 적절한 동작을 보장하는 방법을 설명하는 주석이 포함되어야 함
  - Keywords to Frequent
    - static - 모듈 외부에 표시될 필요가 없는 모든 함수와 변수를 선언하는데 활용
    - const - 초기화 후 변경해서는 안되는 변수 선언, 수정해서는 안되는 call-by-reference 함수 매개변수 정의, 수정해서는 안되는 struct 또는 union의 필드 정의에 활용
    - volatile - interrupt service routune에서 엑세스할 수 있는 전역 변수 선언, 두 개 이상의 thread에서 엑세스할 수 있는 전역 변수 선언, memory-mapped I/O 주변 레지스터 세트에 대한 포인터 선언, delay loop counter 선언에 활용
  - Acceptable Formats : 일시적인 코드 주석 처리 안됨 (조건부 컴파일 기능을 활용하기)  
  - Locations and Content : 주석은 완전한 문장과 같은 들여쓰기 수준으로 모든 가정을 설명해야 함
    - WARNING - 이 코드를 변경하는데 위험이 있음을 경고
    - NOTE - chunk의 why에 대한 설명적 주석을 제공
    - TODO - 코드가 아직 작성 중임을 나타냄
  - Space : 대입/이항연산자 앞뒤 공백, 단항연산자는 피연산자쪽에 공백 없이, 삼항 연산자 구성 문자 앞뒤 공백 등
  - Alignment : 일련 선언 내 변수 이름은 첫 글자를 정렬, struct 및 union의 이름은 첫 글자를 정렬
  - Blank Lines : 한 줄의 코드에 두 개 이상의 문장이 포함되면 안되며 코드 블록의 앞뒤는 공백이 있어야 함
  - Indentation : 각 들여쓰기는 4자의 배수, 가독성 있게 들여쓰기 해야 함
  - Tabs : 탭 문자는 소스 코드 파일 내에 나타나지 않아야 함
  - Naming Conventions : 모든 모듈 이름은 소문자/숫자/밑줄로만 구성, 모든 모듈 이름의 처음 8자는 고유해야 함
  - Header Files : 각 소스 파일에는 정확히 하나의 헤더파일이 있어야 하며 헤더 파일은 항상 동일한 경로와 이름을 가져야 함, 각 헤더 파일에는 다중 포함을 방지하는 전처리기 가드가 포함되어야 함
  - File Templates : 헤더 파일과 소스 파일에 대한 템플릿 세트는 프로젝트 수준에서 유지 관리 되어야 함
  - Naming Conventions : structur, union, enum 등의 data type의 이름은 소문자와 밑줄로만 구성해야 하며, typedef를 통해 이름을 지정해야 함
  - Fixed-Width Integers : integer 값의 width가 중요한 경우 fixed width data types 중 하나를 사용해야 함
  - Signed and Unsigned Intergers : Bit-fiels는 signed integer types 내에 정의되어서는 안됨, 비트 연산자는 signed interger 연산에 사용해서는 안됨
  - Floating Point : floating point 상수와 변수 대신 가급적 Fixed-point math를 사용하는 것이 대안이 될 수 있음
  - Structuers and Unions : 컴파일러가 외부와 통신하는데 사용되는 struct 또는 unoin 내에 padding bytes가 삽입되지 않도록 주의, 컴파일러가 bit-fields 내 bits의 정해진 순서를 변경하지 않도록 적절한 주의
  - Booleans : bool 키워드로 선언, Non-boolean 값은 관계형 연산사를 사용해 boolean으로 변환해야 함
  - Naming Conventions : Procedure의 이름은 기존 라이브러리의 키워드 및 함수 이름에 겹칠 수 없으며, 함수 이름에 대문자를 포함하면 안됨, 매크로 이름에 소문자를 포함하면 안됨
  - Functions : 각 함수 길이는 100줄 이내로 제한하기 위해 노력해야 함, 종료 지점은 하나가 바람직, 모든 private functions 는 static으로 선언해야 함
  - Function-Like Macros : 동일한 동작을 수행하는 함수를 작성할 수 있는 경우 매개 변수화된 매크로를 사용하면 안됨
  - Threads of Execution : 실행 threads를 캡슐화하는 모든 함수의 이름의 끝은 "_thread" 로 지정
  - Interrupt Service Routines : ISRs은 일반적인 함수가 아니며 #pragma 또는 컴파일 전용 키워드를 통해 ISR임을 알려야 함, 관련 함수의 이름은 "_isr"로 끝나야 함
  - Naming Conventions : pointer 변수는 'p'로 시작해야 함, pointer-to-pointer는 'pp'로 시작해야 함, 전역 변수는 'g'로 시작해야 함, 3자 이상 31자 이내로 선언해야 함
  - Initialization : 모든 변수는 사용 전 초기화, 지역변수는 함수 맨 위에 모두 정의하는 것보다 필요할 때마다 정의하는 것이 좋음
  - Variable Declarations : 쉼표 연산자(,)는 변수 선언 내 사용 불가
  - Conditional Statements : if 및 else if 절 중 가장 짧은(코드행이) 항목은 먼저 배치, if 또는 else if 조건문 안에서 변수 할당을 하지 말아야 함
  - Switch Statements : 각 case는 코드 블록의 내용이 아닌 관련 case와 일치하도록 들여쓰기, 다음 case로 넘어가도록 설계된 경우 break가 없는 이유를 주석으로 달아야 함
  - Loops : Magic number를 초기값으로 사용하거나 while, do-while 등의 endpoint test에 사용하면 안됨, 무한 루프는 (;;)에 대한 제어 표현식을 통해 구현해야 함
  - Jumps : goto 문은 사용 제한
  - Equivalence Tests : 상수에 대해 변수의 동일성을 평가할 때 상수는 항상 등호 연산자 (==)의 왼쪽에 배치

--- 

</details> <br>

### 11\. 소프트웨어 요구사항 분석 및 설계 <sub>(2026.03.05 ~ 2026.03.11)</sub>  
> **📝 학습 내용**  
>   

<details>
  <summary>📚 상세 학습 내용 보기</summary>

---

  **1. 소프트웨어 개발과 요구공학**  
  - 요구사항 : 시스템이 수행할 작업 및 제공하는 서비스나 작동에 대한 제한 사항에 대한 설명, 이해관계자들이 제공될 시스템에 요구하는 기능과 제약사항을 정의 즉, _고객이 실제로 원하는 것_
  - 요구사항 관리
    - [정의] - 시스템 개발 과정에서 이해관계자가 요구한 기능·제약사항을 수집, 정리, 추적, 변경 관리하여 일관되게 유지하는 활동
    - [목적] - 고객이 실제로 원하는 기능과 제약을 정확히 반영하고 변경을 통제해 프로젝트 품질과 성공 가능성을 높이기 위함
  - 소프트웨어 개발 : 기술적 문제보다 인력과 요구사항 관리적 측면이 중요
    - [성공 원인] - 효과적인 경영 지원, 이해관계자들의 능동적 참여, 명확한 요구사항
    - [실패 원인] - 이해관계자들의 참여 부족, 의사소통 불명확, 불완전한 요구사항 정의
  - 소프트웨어 결함 : 불완전하고 불명확한 요구사항은 SW 결함을 발생 -> 결함 수정은 생명주기 후반에서 더 많은 노력을 요구 -> 요구사항 결함에 따른 SW 재작업 발생 -> 재작업 비용과 시간 추가
  - 명확한 요구사항 정의와 설계로 결함을 제거, 요구 사항 관리에 노력을 투자하는 것이 중요
  - 요구 공학
    - [정의] - 사용자 요구사항과 시스템 제약사항을 이해관계자와의 합의를 통해 추출, 분석, 명세, 검증하고 요구사항 변경을 관리하는 체계적인 공학적 활동
    - [특징]
      1. 이해관계자 중심성(사용자, 고객, 개발자 등 다양한 이해관계자의 요구를 반영하여 정의)
      2. 반복적·점진적 과정(요구사항은 한 번에 완성되지 않고 반복적으로 수정·보완됨)
      3. 의사소통 중심 활동(고객과 개발자 간 지속적인 협의와 합의 과정이 중요)
      4. 문서화 중심(요구사항을 명세서(SRS) 형태로 명확하게 문서화해야 함)
      5. 변경 관리 필요(개발 과정 중 요구사항이 변경될 수 있으므로 체계적인 변경 관리가 필요)
      6. 검증 가능성(요구사항은 테스트나 검증이 가능하도록 명확하게 정의되어야 함)
    - [프로세스] - 도출 -> 분석 -> 명세 -> 검증 -> 관리
    - [6 Questions]
      1. Who - 누가 시스템을 사용할 것인가?
      2. What - 시스템이 무엇을 해야 하는가?
      3. Why - 왜 이 시스템이 필요한가?
      4. Where - 시스템이 어디에서 사용되는가?
      5. When - 시스템이 언제 동작해야 하는가?
      6. How - 시스템이 어떻게 동작해야 하는가?
      
  **2. 비즈니스 중심 요구사항 추출**  
  - 요구사항 
    - [정의] - 이해관계자(고객, 사용자 등)로부터 시스템이 수행해야 할 기능과 제약사항을 찾아내고 수집하는 과정  
    - [계층] - 비즈니스 요구사항 - 사용자 요구사항 - 시스템 요구사항  
    - [원칙]
      1. 이해관계자 중심 - 모든 이해관계자의 요구를 균형 있게 반영
      2. 명확성 (Clarity) - 요구사항은 모호하지 않게 작성
      3. 완전성 (Completeness) - 필요한 요구사항이 빠짐없이 포함
      4. 일관성 (Consistency) - 서로 충돌하는 요구사항이 없어야 함
      5. 검증 가능성 (Verifiability) - 테스트로 확인 가능해야 함

---

</details> <br>

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

