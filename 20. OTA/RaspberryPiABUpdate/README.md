# Raspberry Pi A/B Update

`ab_manager.py`를 실행하면 활성 Slot을 자동으로 띄우고 CAN 업데이트를
기다립니다. 업데이트가 끝나면 새 Slot을 다시 띄운 뒤 다음 업데이트를
계속 기다립니다.

## 파일 구성

- `ab_manager.py`: A/B Slot 부팅 및 업데이트
- `active_slot.txt`: 현재 활성 Slot (`A` 또는 `B`)
- `slots/slot_a`, `slots/slot_b`: Slot별 `cluster.py` 또는 `slot.txt`

## 실행

```bash
python ab_manager.py
```

ECU 종류와 CAN 설정은 `ab_manager.py` 상단의 `ECU_NAME`, `CAN_CHANNEL`,
`CAN_BITRATE` 값을 수정합니다.

Central Gateway의 START, DATA, END Message를 받아 다음 순서로 처리합니다.

1. START Message에서 파일 크기를 확인합니다.
2. DATA Message의 내용을 메모리에 모읍니다.
3. END Message에서 실제 수신 크기를 검사합니다.
4. 정상 파일만 비활성 Slot에 저장합니다.
5. 저장 성공 후 `active_slot.txt`를 새 Slot으로 변경합니다.
6. 새 활성 Slot을 띄우고 다음 업데이트를 기다립니다.

CAN 업데이트 기능에는 `python-can`이 필요합니다.
