# NBC Assignment 9 — 멀티플레이어 숫자 야구 게임

Unreal Engine 5 기반의 멀티플레이어 숫자 야구 게임입니다.
두 플레이어가 서버에 접속하여 번갈아가며 숫자를 추측하고, 상대방의 정답을 먼저 맞히면 승리합니다.

---

## 게임 규칙

- 서버가 1~9 사이의 중복 없는 3자리 숫자를 비밀번호로 생성합니다.
- 각 플레이어는 턴마다 최대 3회 숫자를 입력할 수 있습니다.
- 입력 결과는 Strike / Ball 형태로 전체 채팅에 공개됩니다.
  - Strike: 숫자와 자릿수가 모두 일치
  - Ball: 숫자는 있지만 자릿수가 다름
- 3Strike = 정답 → 승리, 모든 플레이어 기회 소진 → 무승부 후 리셋

---

## 클래스 구조

| 클래스 | 역할 |
|---|---|
| `BBG_GameModeBase` | 게임 전체 서버 로직 (난수 생성, 판정, 턴 관리) |
| `BBG_GameStateBase` | 복제 상태 관리 (남은 시간, 현재 턴 플레이어) |
| `BBG_PlayerState` | 개인 상태 관리 (이름, 시도 횟수) |
| `BBG_PlayerController` | 로컬 클라이언트 입력 및 위젯 관리 |
| `BBG_ControllerChatComponent` | 채팅/숫자 입력 처리 및 RPC 전송 |
| `BBG_MainWidget` | 채팅창, 타이머, 공지 UI |

---

## 구현 과정

### 1단계 — 게임 기본 구조 설계

구현에 앞서 UE 멀티플레이어의 클래스 역할 분리를 먼저 정리했습니다.

- **GameMode**: 서버에서만 존재. 모든 게임 로직의 권위적 실행 주체.
- **GameState**: 서버가 관리하고 모든 클라이언트에 복제. 공유 상태 저장.
- **PlayerState**: 플레이어별 상태. 서버 + 모든 클라이언트에 복제.
- **PlayerController**: 각 클라이언트의 소유 컨트롤러. 서버와 소유 클라이언트에만 존재.

이 구분을 기준으로 어떤 데이터를 어디에 둘지, 어떤 방식으로 전파할지를 먼저 결정한 뒤 코드 작성에 들어갔습니다.

---

### 2단계 — 단일 플레이어 로직 구현

**난수 생성**

1~9 숫자 배열을 `FMath::RandShuffleArray`로 셔플한 뒤 앞 3자리를 추출합니다.
`FMath::RandInit(FDateTime::Now().GetTicks())`으로 매 게임마다 다른 시드를 보장했습니다.

**입력 유효성 검사**

- 3자리인지 확인
- 각 자릿수가 1~9 범위인지 확인 (0 불가)
- `TSet`을 활용해 중복 숫자를 감지

초기 구현에서 중복 체크 로직이 누락되어 같은 숫자를 반복 입력해도 통과되는 버그가 있었습니다.
`UniqueDigits.Num() != 3` 조건을 추가하여 해결했습니다.

**Strike / Ball 판정**

추측 문자열과 정답 문자열을 자릿수별로 비교하여 카운트를 계산합니다.
초기 구현에서 `IsGuessingNumberCorrect`가 항상 `false`를 반환하는 문제가 있었는데,
함수 내부에서 `bCanPlay = true` 설정이 누락되어 있었던 것이 원인이었습니다.

---

### 3단계 — 멀티플레이 연동

**Replicated vs RPC 선택 기준**

구현 중 "언제 Replicated를 쓰고 언제 RPC를 써야 하는가"를 명확히 정리했습니다.

- **Replicated + OnRep**: 지속되는 상태값 (시도 횟수, 남은 시간, 현재 턴 플레이어 이름)
- **Client RPC**: 특정 플레이어에게만 전달하는 이벤트 (판정 결과, 오류 메시지)
- **NetMulticast RPC**: 모든 플레이어에게 동시에 전달하는 이벤트 (승리/무승부/턴 전환 공지)

**PlayerController 복제 범위**

초기에 `GameState`에서 `UserWidget`에 직접 접근하려 했으나,
GameState는 PlayerController의 위젯에 접근할 수 없다는 것을 확인했습니다.
전체 공지는 `NetMulticast RPC`로, 개인 메시지는 `Client RPC`로 분리하여 해결했습니다.

---

### 4단계 — 채팅 시스템 구현

**채팅과 숫자 입력 분리**

하나의 `EditableTextBox`에서 채팅과 숫자 입력을 모두 처리해야 했습니다.
`NativeOnPreviewKeyDown`에서 Tab 키를 감지하여 `bIsGuessMode` 플래그를 전환하는 방식으로 구현했습니다.
Enter 시 현재 모드에 따라 `ProcessChatMessageString` 또는 `ProcessGuessNumberString`으로 분기합니다.

**채팅 전파 흐름**

```
클라이언트 입력
  → ServerRPCPrintChatMessageString (서버 전달)
  → GameMode::PrintChatMessageToAll
  → 모든 PC에 ClientRPCPrintChatMessageString
  → Widget::AddChatMessage (채팅창 출력)
```

---

### 5단계 — 공지 시스템 구현

초기에 `NotificationText`(UTextBlock)에 직접 텍스트를 대입하는 방식을 시도했으나,
서버에서 클라이언트의 위젯에 직접 접근할 수 없어 동작하지 않았습니다.

이를 두 채널로 분리하여 해결했습니다.

- **오버레이 팝업 (전체)**: `MulticastRPCBroadcastSystemMessage` → `ShowNotification`
- **채팅 시스템 메시지 (개인)**: `ClientRPCPrintSystemMessage` → `Widget::AddSystemMessage`

승리/무승부/턴 전환처럼 모든 플레이어가 알아야 하는 정보는 Multicast 팝업으로,
판정 결과나 오류 메시지처럼 개인에게 전달하는 정보는 Client RPC 채팅으로 처리했습니다.

---

### 6단계 — 도전: 턴 제어 시스템

**턴 흐름 설계**

```
StartTurn → 타이머 시작 (30초) → OnTurnTimerTick (1초 감소)
  ├─ 타이머 만료 → EndTurn (미입력 시 기회 1회 차감)
  └─ 입력 성공 → DoGuessingNumber
       ├─ 정답 → 게임 종료 + ResetGame + StartTurn(0)
       ├─ 3회 소진 → EndTurn
       └─ 기회 남음 → 타이머 유지, 계속 입력 가능

EndTurn → CheckAndHandleDraw → StartTurn(다음 플레이어)
```

**흐름 버그 발견 및 수정**

초기 구현에서 `DoGuessingNumber`의 `else` 분기가 매 입력마다 `EndTurn`을 호출하고 있었습니다.
이로 인해 입력할 때마다 턴이 전환되는 문제가 발생했습니다.
`else if (CurrentGuessCount >= MaxGuessCount)` 조건으로 변경하여
3회 소진 시에만 턴이 종료되도록 수정했습니다.

**bHasPlayedThisTurn**

타이머 만료 시 한 번도 입력하지 않은 플레이어에게 기회 1회를 차감하는 패널티 변수입니다.
`DoGuessingNumber`에서 입력 성공 시 `true`로 설정하고, `EndTurn`에서 `false` 체크 후 리셋합니다.
이 변수가 없으면 3회 소진으로 `EndTurn`이 호출될 때 `EndTurn` 내부에서 기회가 중복 차감되는 문제가 발생합니다.

---

### 7단계 — 도전: 타이머 UI 연동

**델리게이트 기반 UI 갱신**

`GameState`의 `OnRemainingTimeChanged` 델리게이트를 Widget이 `NativeConstruct`에서 바인딩합니다.
서버에서 `RemainingTime`이 변경되면 `OnRep_RemainingTime`이 클라이언트에서 발동되어 UI를 갱신합니다.

```
서버: RemainingTime-- → 복제 → OnRep_RemainingTime
→ OnRemainingTimeChanged.Broadcast → Widget::UpdateTimerText → "PlayerName | 남은 시간: MM:SS"
```

**PlayerArray 인덱스 순서 문제와 해결**

타이머 헤더와 턴 공지에서 현재 턴 플레이어 이름을 표시할 때,
`GameMode`의 `AllPlayerControllers`와 `GameState`의 `PlayerArray`의 인덱스 순서가
클라이언트마다 다를 수 있다는 문제가 발견되었습니다.

클라이언트 접속 시 각자의 `PlayerState`가 로컬에 먼저 등록되면서
`PlayerArray[0]`이 각 클라이언트에서 자기 자신을 가리키는 현상이 원인이었습니다.

인덱스 대신 `CurrentTurnPlayerName`(FString)을 `GameState`에 별도로 복제하여
클라이언트가 이름 자체를 직접 표시하고 비교하도록 해결했습니다.

```cpp
// GameState에 복제
UPROPERTY(ReplicatedUsing=OnRep_CurrentTurnPlayerName)
FString CurrentTurnPlayerName;

// 클라이언트 턴 검증 (인덱스 비교 → 이름 비교)
if (PS->PlayerNameString != GS->CurrentTurnPlayerName) { ... }
```

---

## 추가 구현 기능 (도전)

- **채팅 / 숫자 입력 모드 분리**: Tab 키로 전환, 모드별 메시지 처리 분기
- **이중 공지 시스템**: 오버레이 팝업(Multicast) + 채팅 시스템 메시지 동시 활용
- **이중 턴 검증**: 클라이언트 사전 검증(즉시 피드백) + 서버 재검증(치트 방지)
- **추측 결과 전체 공지**: 한 플레이어의 추측 결과를 모든 플레이어 채팅창에 표시
- **타이머 만료 패널티**: 턴 중 미입력 시 기회 1회 자동 차감