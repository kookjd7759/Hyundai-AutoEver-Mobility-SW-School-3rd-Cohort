- memory_addr_test.c - Infineon AURIX TC275에서 코드/데이터/BSS/Heap/Stack 영역의 실제 메모리 주소를 확인하여 메모리 배치 구조를 분석한 실습
- boot-sequence.c - AURIX TC275에서 ROM/Init Data/BSS와 Heap(malloc), Stack(재귀 로컬변수) 주소를 기록해 실제 메모리 레이아웃을 확인한 실습
- compiler_test.c - AURIX TC275에서 함수/전역/루프 코드의 최적화 동작을 비교하기 위해 결과를 전역 변수로 저장하는 실습  
<br>
- morning-before.c: 1ms/10ms 주기 스케줄링으로 속도·상태를 표시하고 SW1 홀드로 크루즈 목표속도를 제어하는 기본 구현  
- morning-optimized.c: FND 표시를 버퍼링(10ms 계산→1ms 출력)하고 홀드타임을 정수화해 주기 태스크 연산량을 줄인 최적화 버전  
<br>
- afternoon-before.c: 스위치 이벤트/상태 전환을 진단 로그에 기록하고 FND 표시 및 CRC8 검증을 수행하는 기본 구현  
- afternoon-optimized.c: 7세그 룩업테이블·비트마스크·CRC 루프 최적화(hoist/inline/unroll)로 분기와 반복 오버헤드를 줄인 최적화 버전  