
# Gmt_CMD_Parser

GMT SixAxes Controller TCP/IP Command Parser

---

## 1. Project Overview

`Gmt_CMD_Parser` is the command communication and parsing system running on the Raspberry Pi CM5 for the GMT SixAxes Controller motion system.

The project is designed to provide a layered communication path between an external user/host application and the STM32H755 EtherCAT Master.

The overall system is:

```text
                         End User
                            │
                            │ TCP/IP
                            ▼
                 ┌─────────────────────┐
                 │ Raspberry Pi CM5    │
                 │                     │
                 │ TCP Command Server  │
                 │         │           │
                 │         ▼           │
                 │   Command Parser    │
                 │         │           │
                 │   Command Handler   │
                 └─────────┼───────────┘
                           │
                           │ USB CDC ACM
                           │ Command / Response
                           ▼
                 ┌─────────────────────┐
                 │ STM32H755           │
                 │                     │
                 │ USB Command         │
                 │ Executor            │
                 │         │           │
                 │         ▼           │
                 │ EtherCAT Master     │
                 └─────────────────────┘
```

The STM32H755 also has an independent SPI2 data path to the Raspberry Pi CM5:

```text
                 STM32H755
                     │
                     │ SPI2
                     │
                     ▼
                 Raspberry Pi CM5
                     │
                     ▼
               Trajectory Logger
                     │
                     │ DONE
                     ▼
                Log File
```

The SPI2 data path is an independent real-time data collection channel and is **not part of the TCP Command Parser transport path**.

---

# 2. Main Project Goals

The main goals of this project are:

1. Provide a TCP/IP command interface on the Raspberry Pi CM5.
2. Follow the GMT SixAxes Controller command protocol defined by the official command manual.
3. Receive ASCII commands from an external TCP client.
4. Parse and validate commands and parameters.
5. Determine whether a command is handled locally by the CM5 or must be forwarded to the STM32H755.
6. Communicate with the STM32H755 through USB CDC ACM.
7. Receive command execution results from the STM32H755.
8. Return appropriate responses to the TCP client.
9. Integrate special commands such as `DONE` with the Trajectory Logger.
10. Keep the system modular so that TCP transport, command parsing, packet handling, USB communication, and logging are separated into independent modules.

---

# 3. GMT TCP Command Protocol

According to the GMT SixAxes Controller TCP/IP over EtherCAT Command Manual:

### Command Interface

```text
Protocol : TCP/IP
Port     : 9999
Format   : ASCII
Terminator : \r\n
```

The command interface follows a request-response model.

```text
TCP Client
    │
    │ ASCII Command\r\n
    ▼
GMT Command Server
    │
    │ Command processing
    ▼
Response
    │
    │ ASCII Response\r\n
    ▼
TCP Client
```

The protocol uses human-readable ASCII commands.

Numeric values may be represented in decimal or hexadecimal form, depending on the command specification.

Boolean values are typically represented as:

```text
0
1
```

---

# 4. GMT Routine Status Stream

The GMT protocol also defines an independent routine status stream.

```text
Port : 8888
```

The routine status packet is sent periodically and is separate from the command interface.

According to the current manual information:

```text
Period       : 200 ms
Destination  : localhost:8888
Format       : HEX string
Packet size  : 68 bytes
Encoded size : 136 HEX characters
```

The packet begins with:

```text
>
```

and terminates with:

```text
\r\n
```

The current documented structure includes:

```text
Controller Status    UINT32    4 bytes
Analog Input         UINT16   16 bytes
Main Stage Position  DOUBLE   48 bytes
--------------------------------------
Total                         68 bytes
```

The routine status stream will be handled separately from the command request-response path.

---

# 5. Planned Software Architecture

The project will be divided according to functional responsibility.

The planned structure is:

```text
Gmt_CMD_Parser/
│
├── CMakeLists.txt
├── README.md
├── .gitignore
│
├── include/
│   ├── tcp_server.h
│   ├── command_parser.h
│   ├── command.h
│   ├── response.h
│   ├── packet.h
│   ├── usb_cdc.h
│   ├── command_handler.h
│   └── ...
│
├── src/
│   ├── main.cpp
│   ├── tcp_server.cpp
│   ├── command_parser.cpp
│   ├── command.cpp
│   ├── response.cpp
│   ├── packet.cpp
│   ├── usb_cdc.cpp
│   ├── command_handler.cpp
│   └── ...
│
└── build/
    └── (local build files, not committed to Git)
```

The exact module list will be refined after the complete GMT command manual has been analyzed.

---

# 6. Functional Layering

The intended software layers are:

```text
┌─────────────────────────────────────┐
│          TCP Client / Host          │
└──────────────────┬──────────────────┘
                   │
                   │ TCP :9999
                   ▼
┌─────────────────────────────────────┐
│          TCP Server Layer           │
│          tcp_server.cpp             │
└──────────────────┬──────────────────┘
                   │
                   ▼
┌─────────────────────────────────────┐
│          Command Parser             │
│                                     │
│  Command recognition                │
│  Parameter parsing                  │
│  Parameter validation               │
│  Request validation                 │
└──────────────────┬──────────────────┘
                   │
                   ▼
┌─────────────────────────────────────┐
│          Command Handler            │
│                                     │
│  CM5-local command                  │
│  STM32 command dispatch             │
│  Special command handling           │
└───────────────┬───────────┬─────────┘
                │           │
                │           │
                ▼           ▼
        ┌────────────┐  ┌────────────────┐
        │ CM5 Module │  │ USB CDC ACM    │
        │            │  │                │
        │ e.g. DONE  │  │ CM5 ↔ STM32    │
        └────────────┘  └───────┬────────┘
                                │
                                ▼
                        ┌────────────────┐
                        │ STM32H755      │
                        │ Command        │
                        │ Executor       │
                        └────────────────┘
```

The TCP layer should not contain GMT command-specific logic.

The command parser should not directly contain socket implementation details.

This separation is intentional.

---

# 7. Command Dispatch Design

The final command parser will classify commands into at least two major groups.

### CM5-local commands

Commands that can be processed entirely by the Raspberry Pi CM5.

Example:

```text
DONE
```

`DONE` is expected to have special integration behavior because it marks the completion of a trajectory/logging operation.

### STM32 commands

Commands that require the STM32H755 EtherCAT Master to execute an operation.

The intended flow is:

```text
TCP Client
    │
    │ Command
    ▼
TCP Server
    │
    ▼
Command Parser
    │
    ▼
Command Handler
    │
    ├──────────────► CM5 local processing
    │
    └──────────────► USB CDC ACM
                           │
                           ▼
                       STM32H755
                           │
                           ▼
                     EtherCAT Master
```

The exact command classification will be determined from the complete GMT command manual.

---

# 8. DONE Integration

`DONE` is a special command in the planned system.

The current trajectory logger uses a temporary time-based completion condition.

Current temporary behavior:

```text
4 seconds
   ↓
Trajectory complete
   ↓
Create / finalize trajectory log
```

The final architecture will replace the time-based condition with a valid `DONE` command.

The intended behavior is:

```text
TCP Client
    │
    │ DONE\r\n
    ▼
Command Parser
    │
    ├──────────────► STM32H755
    │
    │
    └──────────────► Trajectory Logger
                          │
                          ▼
                    Finalize/Create log
```

The existing logger architecture should be preserved as much as possible.

The goal is to replace only the completion trigger rather than redesigning the logger.

---

# 9. Independent SPI2 Data Channel

The SPI2 channel is deliberately kept separate from the command channel.

Current validated architecture:

```text
STM32H755 SPI2 Slave
          │
          │ 204 bytes
          │
          ▼
Raspberry Pi CM5 SPI Master
          │
          ▼
Trajectory Logger
```

The SPI2 path has already been extensively tested and is considered an established data collection channel.

Important principle:

> Do not redesign or modify the validated SPI2 architecture as part of the GMT command parser development.

The TCP/USB command path and SPI2 real-time data path are independent.

---

# 10. Development Strategy

Development is performed incrementally.

Each stage should:

1. Make a small change.
2. Compile.
3. Run a focused test.
4. Confirm the result.
5. Establish a checkpoint.
6. Continue to the next stage.

Large unverified changes should be avoided.

The project should maintain stable checkpoints so that development can continue safely across different work sessions.

---

# 11. Development Phases

## Phase 1 — TCP Server Foundation

Goal:

```text
Windows 11 TCP Client
        ⇅
Raspberry Pi CM5 TCP Server
```

Required:

* TCP server socket
* Port 9999
* `bind()`
* `listen()`
* `accept()`
* receive ASCII data
* send ASCII response

### Status: COMPLETE

Validated on 2026-09-01.

---

## Phase 2 — CM5 ↔ STM32H755 USB CDC ACM

Goal:

Establish an independent bidirectional command transport.

```text
CM5
 │
 │ USB CDC ACM
 ▼
STM32H755
 │
 │ Response
 ▼
CM5
```

Initial loopback test:

```text
CM5 → STM32 : TEST\r\n
STM32 → CM5 : TEST_OK\r\n
```

Both sides should print transmit and receive information in their terminals.

### Status: NOT STARTED

---

## Phase 3 — TCP Client / Server Bidirectional Test

Goal:

Confirm that the Windows 11 host can send an ASCII request and receive an ASCII response from the CM5 server.

Test:

```text
Windows → CM5

HELLO\r\n
```

Response:

```text
CM5 → Windows

HELLO_OK\r\n
```

### Status: COMPLETE

Validated on 2026-09-01.

Actual result:

```text
Windows:
HELLO_OK
```

CM5:

```text
[RX] HELLO
[TX] HELLO_OK
```

---

## Phase 4 — Command Parser Foundation

After the TCP and USB communication foundations are stable, implement:

```text
TCP Server
    │
    ▼
Command Parser
    │
    ├── Command recognition
    ├── Parameter parsing
    ├── Parameter validation
    ├── Request validation
    └── Dispatch
```

The parser data structures must be designed from the complete GMT command manual.

Required information to extract from the manual:

* Command format
* Request format
* Response format
* Parameter definitions
* Parameter types
* Valid ranges
* Error format
* Command categories
* CM5-local commands
* STM32 commands
* Special commands
* Response mapping

### Status: NOT STARTED

---

## Phase 5 — STM32 Command Executor

Implement the STM32-side command handling after the USB protocol is established.

Intended path:

```text
TCP Client
    ↓
CM5 TCP Server
    ↓
Command Parser
    ↓
Command Handler
    ↓
USB CDC ACM
    ↓
STM32 Command Executor
    ↓
EtherCAT Master
```

STM32 responses return through the reverse path.

### Status: NOT STARTED

---

## Phase 6 — DONE / Trajectory Logger Integration

Replace the current temporary time-based completion trigger with:

```text
DONE
```

The logger should finalize the trajectory according to the valid command event.

### Status: NOT STARTED

---

## Phase 7 — Complete GMT Command Implementation

After the foundation is verified, implement the GMT command manual systematically.

Commands will be added in groups rather than modifying the entire system at once.

For every command:

```text
Command
   ↓
Parser
   ↓
Parameter validation
   ↓
Dispatch
   ↓
CM5 / STM32
   ↓
Response
   ↓
TCP Client
```

### Status: NOT STARTED

---

## Phase 8 — Final Integration Test

Final target:

```text
                         Windows 11
                        TCP Client
                             │
                             │ TCP/IP :9999
                             ▼
                  ┌──────────────────────┐
                  │ Raspberry Pi CM5     │
                  │                      │
                  │ TCP Server           │
                  │      │               │
                  │      ▼               │
                  │ Command Parser       │
                  │      │               │
                  │ Command Handler      │
                  └──────┼───────────────┘
                         │
                         │ USB CDC ACM
                         ▼
                  ┌──────────────────────┐
                  │ STM32H755            │
                  │ Command Executor     │
                  │        │             │
                  │        ▼             │
                  │ EtherCAT Master      │
                  └──────────────────────┘


                  Independent data path:

                  STM32H755
                       │
                       │ SPI2
                       ▼
                  Raspberry Pi CM5
                       │
                       ▼
                  Trajectory Logger
                       │
                       │ DONE
                       ▼
                    Log File
```

The final integration test must verify:

* TCP connection
* Command reception
* Command parsing
* Parameter validation
* Command dispatch
* CM5-local commands
* CM5 → STM32 USB communication
* STM32 command execution
* STM32 → CM5 response
* CM5 → TCP response
* `DONE` processing
* Trajectory logging
* Independent SPI2 data collection
* Error handling
* Client disconnect/reconnect behavior

---

# 12. Current Project Status

Date:

```text
2026-09-01
```

Current repository state:

```text
Gmt_CMD_Parser/
├── .gitignore
├── CMakeLists.txt
├── include/
│   └── tcp_server.h
├── src/
│   ├── main.cpp
│   └── tcp_server.cpp
└── build/
```

The `build/` directory is ignored by Git.

Current executable:

```text
build/Gmt_CMD_Parser
```

---

# 13. Current TCP Server Implementation Status

The current `TcpServer` supports:

```text
socket()
bind()
listen()
accept()
recv()
send()
```

Current port:

```text
9999
```

Current test protocol:

```text
Client → Server

HELLO\r\n
```

Server response:

```text
Server → Client

HELLO_OK\r\n
```

---

# 14. Verified TCP Test

Windows 11:

```text
IP:
192.168.137.200

Port:
9999
```

The connection was verified using:

```powershell
Test-NetConnection 192.168.137.200 -Port 9999
```

Result:

```text
TcpTestSucceeded : True
```

Bidirectional application-level test was also completed.

CM5 output:

```text
[OK] socket() created. fd=3
[OK] bind() successful. port=9999
[OK] listen() successful.
[WAIT] Waiting for client connection...
[OK] Client connected. fd=4
[OK] TCP server started.
[RUN] Client communication loop started.
[RX] HELLO
[TX] HELLO_OK
```

Windows output:

```text
HELLO_OK
```

Therefore:

```text
Windows 11 TCP Client
        ⇅
CM5 TCP Server :9999
```

is currently **verified working**.

---

# 15. Current Limitations

The current TCP implementation is intentionally a minimal foundation test.

It does not yet provide:

* Persistent client communication loop
* Multiple client support
* Command parsing
* Command validation
* GMT command implementation
* Formal response handling
* Error code handling
* USB CDC ACM transport
* STM32 command executor
* Routine status stream
* `DONE` integration

These are future development stages.

The current implementation should therefore be treated as a **TCP Foundation Checkpoint**, not the final TCP server architecture.

---

# 16. Important Development Principles

### 1. Do not redesign validated subsystems without a specific reason.

In particular, the validated STM32H755 SPI2 data path should remain independent.

### 2. Keep functional modules separated.

Do not place TCP, command parsing, packet handling, USB communication, and logging logic into `main.cpp`.

### 3. Follow the GMT command manual.

The command parser should be designed from the documented protocol rather than inventing a new command format.

### 4. Build incrementally.

Each change should be small and testable.

### 5. Preserve stable checkpoints.

After each major milestone, verify:

```text
compile
run
test
pass
```

before proceeding.

### 6. Keep transport and application layers separate.

TCP transport should not become tightly coupled to GMT command semantics.

### 7. Do not mix the SPI2 data path with the command path.

SPI2 is the real-time EtherCAT data collection channel.

TCP + USB CDC ACM is the command/control channel.

---

# 17. Immediate Next Steps

The next major development stage is:

```text
Phase 2
CM5 ↔ STM32H755 USB CDC ACM
```

The objective is to establish a simple bidirectional loopback:

```text
CM5
 │
 │ TEST\r\n
 ▼
STM32H755
 │
 │ TEST_OK\r\n
 ▼
CM5
```

Both sides must print transmit and receive information.

After USB CDC ACM is verified, the TCP foundation and USB foundation can be connected through the future command dispatch architecture.

The complete GMT command manual should then be used to define the final:

```text
Command
Request
Response
Parameter
Error
Dispatch
```

data structures and command handling architecture.

---

# 18. Git Checkpoint

The first Git checkpoint represents:

```text
Gmt_CMD_Parser
TCP Foundation
```

Checkpoint:

```text
TCP Server :9999
        │
        ├── socket      PASS
        ├── bind        PASS
        ├── listen      PASS
        ├── accept      PASS
        ├── receive     PASS
        └── response    PASS
```

Validated:

```text
Windows 11 ↔ Raspberry Pi CM5
```

Date:

```text
2026-09-01
```

This checkpoint is the stable starting point for the next development stage.
---
## [2026-09-01] 今天完成進度 測試：
- CM5 Terminal 執行 ： herman@RPiCM5:~/Gmt_CMD_Parser/build $ ./Gmt_CMD_Parser
輸出 ： 
```text
========================================
GMT Command Parser
========================================
[OK] socket() created. fd=3
[OK] bind() successful. port=9999
[OK] listen() successful.
[WAIT] Waiting for client connection...
```
- Windows Powershell 執行 ：
```bash
$client = [System.Net.Sockets.TcpClient]::new("192.168.137.200",9999); $stream = $client.GetStream(); $data = [System.Text.Encoding]::ASCII.GetBytes("HELLO`r`n"); $stream.Write($data,0,$data.Length); $buffer = New-Object byte[] 1024; $count = $stream.Read($buffer,0,$buffer.Length); [System.Text.Encoding]::ASCII.GetString($buffer,0,$count); $client.Close()
HELLO_OK
```
- CM5 的輸出會改變為
```text
========================================
GMT Command Parser
========================================
[OK] socket() created. fd=3
[OK] bind() successful. port=9999
[OK] listen() successful.
[WAIT] Waiting for client connection...
[OK] Client connected. fd=4
[OK] TCP server started.
[RUN] Client communication loop started.
[RX] HELLO
[TX] HELLO_OK
```
- 兩邊都出現預期結果，那麼今天我們就可以正式宣布： 雙向 TCP 基礎通道 PASS。
```text
Windows 11
     │
     │ TCP :9999
     │ HELLO\r\n
     ▼
CM5 TCP Server
     │
     │ HELLO_OK\r\n
     ▼
Windows 11
```
- Windows 不需要寫 TCP Client 程式，只需要在 Poershell 執行以下 命令就可以連接 TCP Server，然後觀察是否連線成功。
- 第一步要讓 TCP server 執行 ./Gmt_CMD_Parser 使其處於 Listening 狀態
- 連接測試命令 ： Windows Powershell 端
```bash
Test-NetConnection 192.168.137.200 -Port 9999
```
**今天第一階段的核心目標已經達成，而且是實機雙向驗證成功。**

### ✅ Windows 11 → CM5

Windows：

```text
HELLO\r\n
```

CM5：

```text
[RX] HELLO
```

### ✅ CM5 → Windows 11

CM5：

```text
[TX] HELLO_OK
```

Windows：

```text
HELLO_OK
```

因此目前實際資料路徑是：

```text
┌────────────────┐
│ Windows 11     │
│ TCP Client     │
└───────┬────────┘
        │
        │ TCP :9999
        │ "HELLO\r\n"
        ▼
┌────────────────┐
│ Raspberry Pi   │
│ CM5            │
│ TCP Server     │
└───────┬────────┘
        │
        │ "HELLO_OK\r\n"
        ▼
┌────────────────┐
│ Windows 11     │
│ TCP Client     │
└────────────────┘
```

而且 **9999 是按照你貼出的 GMT Command Manual 規格**，不是我們自行決定的測試 port。

---

## 目前 `Gmt_CMD_Parser` 的第一個 checkpoint

我們現在可以把今天的成果定義為：

```text
Gmt_CMD_Parser
│
├── CMakeLists.txt
│
├── include/
│   └── tcp_server.h
│
├── src/
│   ├── main.cpp
│   └── tcp_server.cpp
│
└── build/
    └── Gmt_CMD_Parser
```

TCP Server 已完成：

```text
socket()   ✓
bind()     ✓ 0.0.0.0:9999
listen()   ✓
accept()   ✓
recv()     ✓
send()     ✓
```

雙向實測：

```text
Client → Server    ✓
Server → Client    ✓
```
![RoadMap](images/CM5命令解析連接圖.png)
---

可以。這次我會完整保留今天的技術進度與架構，但全部改成**繁體中文**，方便你直接放進 GitHub `README.md`。

# Gmt_CMD_Parser

`Gmt_CMD_Parser` 是 GMT 運動控制系統中，部署於 Raspberry Pi CM5 的命令處理程式。

目前系統的整體架構如下：

```text
End User TCP Client
        │
        │ TCP Command
        ▼
┌──────────────────────┐
│     CM5 TCP Server   │
│                      │
│    Command Parser    │
└──────────┬───────────┘
           │
           │ VALID Command
           ▼
      USB CDC Transport
           │
           ▼
       STM32H755
           │
           ▼
     SPI1 / EtherCAT
           │
           ▼
       Motion Drivers
```

Command Parser 被設計成獨立模組，位於 TCP Server 與 USB Transport 之間。

---
# [2026-09-10] 更新狀態

# 1. 目前專案狀態

目前已完成：

* CM5 TCP Server 基礎架構。
* Command Parser 獨立模組。
* 基本命令的格式驗證。
* 部分帶參數命令的 Regex 格式驗證。
* 建立獨立的 Parser 測試程式。
* 建立獨立的帶參數命令測試程式。
* 基本命令測試：**16/16 PASS**。
* 帶參數命令測試：**16/16 PASS**。

目前 Parser 測試屬於**開發階段的暫時性驗證測試**。

由於目前使用的命令文件中，仍有部分命令格式、參數定義及 Regex 規則需要進一步釐清，因此目前的 Regex **尚不能視為最終正式規格**。

後續會依照命令文件確認後的結果，持續修改與驗證 Parser。

---

# 2. 系統命令處理架構

Command Parser 與 TCP Server 分離設計。

目前預期的命令處理流程：

```text
End User TCP Client
        │
        │ ASCII Command
        ▼
┌──────────────────┐
│    TCP Server    │
└────────┬─────────┘
         │
         │ Command String
         ▼
┌──────────────────┐
│ Command Parser   │
└────────┬─────────┘
         │
     ┌───┴────┐
     │        │
     ▼        ▼
   VALID    INVALID
     │        │
     ▼        ▼
   USB      TCP Error
Transport   Response
     │
     ▼
 STM32H755
```

### TCP Server

TCP Server 負責：

* 接受 TCP Client 連線。
* 接收 TCP Command。
* 將 Command 傳給 Command Parser。
* 根據 Parser 結果回覆 TCP Client。
* VALID 的 Command 才繼續送往 USB Transport。

TCP Server **不負責 Command-specific 的格式驗證**。

---

### Command Parser

Command Parser 負責：

* 辨識 Command。
* 從 Command Array 中找到對應的 Command。
* 取得該 Command 的 Regex Rule。
* 驗證 Command 及其參數格式。
* 回傳 `VALID` 或 `INVALID`。
* 發生錯誤時提供錯誤訊息。

Command Parser **不負責執行命令**。

它不直接：

* 控制 STM32。
* 控制 EtherCAT。
* 控制運動軸。
* 執行運動演算法。
* 傳送 USB 封包。

---

### USB Transport

USB Transport 只負責：

```text
VALID Command
      │
      ▼
USB CDC
      │
      ▼
STM32H755
```

USB Transport **不負責 Command Parser 的工作**。

---

# 3. Command Parser 設計方式

Command Parser 採用：

**Command Array / Command Table + Regular Expression（Regex）**

的設計方式。

基本概念：

```text
收到 Command
      │
      ▼
Command Array
      │
      ▼
尋找 Command
      │
      ▼
對應 Regex Rule
      │
      ▼
regex_match()
      │
   ┌──┴──┐
   ▼     ▼
 VALID INVALID
```

每個 Command 可以在 Command Array 中指定自己的 Regex Rule。

例如：

```cpp
{"SAH", REGEX_SAH},
```

對應：

```cpp
constexpr const char* REGEX_SAH =
    R"(^\s*(A|M[0-9]{2})(\s+(A|M[0-9]{2})){2}\s*$)";
```

如果某個 Command 的參數規則尚未建立，可以暫時使用：

```cpp
{"COMMAND", nullptr},
```

此時 Parser 會回傳：

```text
Parameter rule not implemented
```

這可以讓 Command 先加入 Command Array，而不必在 Regex 尚未確認時自行猜測命令格式。

---

# 4. Parser 模組

Command Parser 是獨立模組，不與 TCP Server 的解析邏輯混合。

目前主要檔案：

```text
include/
└── command_parser.h

src/
├── command_parser.cpp
├── main.cpp
├── tcp_server.cpp
├── parser_test.cpp
└── parser_parameter_test.cpp
```

正式程式：

```text
Gmt_CMD_Parser
```

使用：

```text
main.cpp
tcp_server.cpp
command_parser.cpp
```

Parser 測試則使用獨立 executable，不會把測試程式混入正式 TCP Server。

---

# 5. 基本 Command 測試：parser_test

測試程式：

```text
src/parser_test.cpp
```

CMake Target：

```text
parser_test
```

這個測試程式專門測試**不帶參數的基本 Command 與 Query Command**。

目前測試內容包括：

```text
STP
SVO
SVF
CAL
DSC

MOV?
POS?
PMS?
SPI?
FRS?
BKN?
```

同時也測試一些應該判定為 INVALID 的輸入：

```text
STP 123
SVO ABC
MOV? 123
UNKNOWN
空字串
```

目前測試結果：

```text
[RESULT] 16/16 tests passed.
```

`parser_test.cpp` 會保留，作為基本 Parser 的 Regression Test。

---

# 6. 帶參數 Command 測試：parser_parameter_test

為了避免把帶參數測試混入 `parser_test.cpp`，另外建立：

```text
src/parser_parameter_test.cpp
```

CMake Target：

```text
parser_parameter_test
```

這個測試程式專門驗證文件中**明確提供 Example 的帶參數命令**。

目前測試的 Example 全部直接採用命令文件中的內容，不自行創造額外的測試格式。

目前測試：

```text
INS 1

SAH M01 M02 M03

SHC M01 2 17 400000 40000 0 128

SHC? M01

VLS 0.15

MOV R 2000 2000 1000 0 0.5 1

MRV R 2000 2000 1000 0 0.5 1

MSV M02 2000

MSR M06 1

MPV M01 M03 1200.0 -35.5

MPR M01 M03 1200.0 -35.5

SPI R U 10 5 2

DFRS ScanRoutine01

FLM M10 0.2 V 0.01 TH 255

FLM M08 2 V 0.2

BKN 0.015
```

這些 Example 的預期結果全部為：

```text
VALID
```

目前測試結果：

```text
[RESULT] 16/16 tests passed.
```

---

# 7. 本次 Parser Regex 驗證過程

本次測試實際找出了目前 Parser 中的問題。

例如原本 `SAH` 的 Regex：

```cpp
constexpr const char* REGEX_SAH =
    R"(^\s*(A|M[0-9]{2})\s*$)";
```

只能接受單一參數。

但是命令文件提供的 Example 是：

```text
SAH M01 M02 M03
```

因此測試結果為：

```text
INVALID
```

經過確認文件 Example 後，將 Regex 修改為可以符合該 Example 的格式。

修改後：

```text
SAH M01 M02 M03
        ↓
      VALID
```

---

另外，部分命令原本在 Command Array 中為：

```cpp
{"MOV", nullptr},
{"MRV", nullptr},
{"MSV", nullptr},
{"MSR", nullptr},
{"MPV", nullptr},
{"MPR", nullptr},
{"DFRS", nullptr},
{"FLM", nullptr},
```

因此測試時會得到：

```text
Parameter rule not implemented
```

本次依照文件中提供的 Example，建立對應的 Regex Rule。

完成後：

```text
MOV   → VALID
MRV   → VALID
MSV   → VALID
MSR   → VALID
MPV   → VALID
MPR   → VALID
DFRS  → VALID
FLM   → VALID
```

最終帶參數測試：

```text
16/16 PASS
```

---

# 8. 目前測試架構

目前 CMake 中共有三個 executable：

```text
Gmt_CMD_Parser
    ├── main.cpp
    ├── tcp_server.cpp
    └── command_parser.cpp
```

```text
parser_test
    ├── parser_test.cpp
    └── command_parser.cpp
```

```text
parser_parameter_test
    ├── parser_parameter_test.cpp
    └── command_parser.cpp
```

這三個程式的用途不同。

### 正式程式

```text
Gmt_CMD_Parser
```

用於實際 TCP Server 系統。

### 基本 Parser 測試

```text
parser_test
```

專門驗證基本 Command / Query。

### 帶參數 Parser 測試

```text
parser_parameter_test
```

專門驗證命令文件中已提供 Example 的帶參數 Command。

---

# 9. 編譯與測試

進入專案：

```bash
cd ~/Gmt_CMD_Parser
```

重新建立 Build：

```bash
rm -rf build
mkdir build
cd build
cmake ..
make -j$(nproc)
```

執行基本 Parser 測試：

```bash
./parser_test
```

目前結果：

```text
[RESULT] 16/16 tests passed.
```

執行帶參數 Parser 測試：

```bash
./parser_parameter_test
```

目前結果：

```text
[RESULT] 16/16 tests passed.
```

---

# 10. 目前狀態：暫時性測試

**重要：目前 Regex 並不是最終正式版本。**

目前的測試目的，是先確認：

1. Command Array 的設計可以正常工作。
2. Parser 可以正確找到 Command。
3. Regex 可以驗證 Command 格式。
4. VALID / INVALID 的處理流程正常。
5. 文件中已提供的 Example 可以被目前 Parser 接受。
6. Parser 可以獨立於 TCP Server 進行測試。

目前命令文件中仍存在一些需要進一步釐清的項目，例如：

* 部分 Command 的完整參數規則。
* 部分參數的合法範圍。
* 部分 Command 是否存在其他合法格式。
* Regex 應該接受的精確輸入格式。

因此目前的 Regex 應視為：

```text
開發階段暫時規則
```

而不是：

```text
最終正式 Command Specification
```

當命令文件確認完成後，Parser 的 Regex 會再依照正式規格進行修改。

---

# 11. 測試原則

目前 Parser 開發遵循以下原則：

1. Command Parser 與 TCP Server 分離。
2. USB Transport 不負責 Command Parsing。
3. 使用 Command Array / Command Table 管理 Command。
4. 使用 Regex 驗證 Command 參數格式。
5. 不自行猜測尚未確認的 Command 格式。
6. 帶參數測試只使用命令文件中明確提供的 Example。
7. 基本 Command 與帶參數 Command 使用不同測試程式。
8. `parser_test.cpp` 保留作為基本 Parser Regression Test。
9. `parser_parameter_test.cpp` 用於文件 Example 的參數驗證。
10. 每次修改 Parser 後進行獨立編譯與測試。
11. 命令文件釐清後，再更新對應 Regex 與測試案例。

---

# 12. 目前 Checkpoint

```text
Gmt_CMD_Parser
│
├── CM5 TCP Server 基礎架構
│       ✅
│
├── Command Parser 獨立模組
│       ✅
│
├── Command Array / Regex 設計
│       ✅
│
├── parser_test.cpp
│       ✅ 16/16 PASS
│
├── parser_parameter_test.cpp
│       ✅ 16/16 PASS
│
├── 文件 Example 驗證
│       ✅ 目前提供的 Example 全部 PASS
│
├── 命令文件最終規格
│       ⏳ 尚需釐清
│
└── Parser 最終 Regex
        ⏳ 持續修改
```

此版本為目前 `Gmt_CMD_Parser` 的 **Command Parser 開發階段 Checkpoint**。

後續將依照命令文件的進一步確認結果，持續完善 Command Parser。

這版比較適合你現在的 GitHub checkpoint：**不只記錄「16/16 通過」，也把今天為什麼要建立兩個測試程式、Parser 為什麼採用 Command Array + Regex，以及目前為什麼不能把 Regex 當成最終規格，都留下來了。**

# 測試結果

- 命令不帶參數
```text
herman@RPiCM5:~/Gmt_CMD_Parser/build $ ./parser_test
[PASS] "STP" -> VALID
[PASS] "SVO" -> VALID
[PASS] "SVF" -> VALID
[PASS] "CAL" -> VALID
[PASS] "DSC" -> VALID
[PASS] "MOV?" -> VALID
[PASS] "POS?" -> VALID
[PASS] "PMS?" -> VALID
[PASS] "SPI?" -> VALID
[PASS] "FRS?" -> VALID
[PASS] "BKN?" -> VALID
[PASS] "STP 123" -> INVALID
[PASS] "SVO ABC" -> INVALID
[PASS] "MOV? 123" -> INVALID
[PASS] "UNKNOWN" -> INVALID
[PASS] "" -> INVALID

[RESULT] 16/16 tests passed.

```
- 命令帶參數
以下我貼給你在文件上的命令帶有 example者，如果 parser 正確，這些帶參數的命令都會返回有效的訊息，如果出現Invalid，那就表示Regular expression 有錯誤。
```text
Example: INS 1
Example: SAH M01 M02 M03
Example: SHC M01 2 17 400000 40000 0 128
Example: SHC? M01
Example: VLS 0.15
Example: MOV R 2000 2000 1000 0 0.5 1
Example: MRV R 2000 2000 1000 0 0.5 1
Example: MSV M02 2000
Example: MSR M06 1
Example: MPV M01 M03 1200.0 -35.5
Example: MPR M01 M03 1200.0 -35.5
Example: SPI R U 10 5 2
Example: DFRS ScanRoutine01
Example: FLM M10 0.2 V 0.01 TH 255
Example: FLM M08 2 V 0.2
Example: BKN 0.015
```

* 一開始發生 正則表示式 錯誤的情況

```text
herman@RPiCM5:~/Gmt_CMD_Parser/build $ ./parser_parameter_test
[PASS] "INS 1" -> VALID
[FAIL] "SAH M01 M02 M03" -> INVALID  ERROR: Invalid parameters
[PASS] "SHC M01 2 17 400000 40000 0 128" -> VALID
[PASS] "SHC? M01" -> VALID
[PASS] "VLS 0.15" -> VALID
[FAIL] "MOV R 2000 2000 1000 0 0.5 1" -> INVALID  ERROR: Parameter rule not implemented
[FAIL] "MRV R 2000 2000 1000 0 0.5 1" -> INVALID  ERROR: Parameter rule not implemented
[FAIL] "MSV M02 2000" -> INVALID  ERROR: Parameter rule not implemented
[FAIL] "MSR M06 1" -> INVALID  ERROR: Parameter rule not implemented
[FAIL] "MPV M01 M03 1200.0 -35.5" -> INVALID  ERROR: Parameter rule not implemented
[FAIL] "MPR M01 M03 1200.0 -35.5" -> INVALID  ERROR: Parameter rule not implemented
[PASS] "SPI R U 10 5 2" -> VALID
[FAIL] "DFRS ScanRoutine01" -> INVALID  ERROR: Parameter rule not implemented
[FAIL] "FLM M10 0.2 V 0.01 TH 255" -> INVALID  ERROR: Parameter rule not implemented
[FAIL] "FLM M08 2 V 0.2" -> INVALID  ERROR: Parameter rule not implemented
[PASS] "BKN 0.015" -> VALID

[RESULT] 6/16 tests passed.

```
* 修改正則表示式後的情況
```text
herman@RPiCM5:~/Gmt_CMD_Parser/build $ ./parser_parameter_test
[PASS] "INS 1" -> VALID
[PASS] "SAH M01 M02 M03" -> VALID
[PASS] "SHC M01 2 17 400000 40000 0 128" -> VALID
[PASS] "SHC? M01" -> VALID
[PASS] "VLS 0.15" -> VALID
[PASS] "MOV R 2000 2000 1000 0 0.5 1" -> VALID
[PASS] "MRV R 2000 2000 1000 0 0.5 1" -> VALID
[PASS] "MSV M02 2000" -> VALID
[PASS] "MSR M06 1" -> VALID
[PASS] "MPV M01 M03 1200.0 -35.5" -> VALID
[PASS] "MPR M01 M03 1200.0 -35.5" -> VALID
[PASS] "SPI R U 10 5 2" -> VALID
[PASS] "DFRS ScanRoutine01" -> VALID
[PASS] "FLM M10 0.2 V 0.01 TH 255" -> VALID
[PASS] "FLM M08 2 V 0.2" -> VALID
[PASS] "BKN 0.015" -> VALID

[RESULT] 16/16 tests passed.

```

# [2026-09-11] 更新

## 測試結果：
```text
herman@RPiCM5:~/Gmt_CMD_Parser/build $ ./GMT_Server_Command

========================================
GMT Server Command Test
========================================
[OK] socket() created. fd=3
[OK] bind() successful. port=9999
[OK] listen() successful.
[WAIT] Waiting for client connection...
[OK] Client connected. fd=4
[RX] SHC M01 2 17 400000 40000 0 128
[Parser] VALID
[TX] VALID
[RX] SHC? M01
[Parser] VALID
[TX] VALID
[RX] SVO
[Parser] VALID
[TX] VALID
[RX] SVF
[Parser] VALID
[TX] VALID
[RX] CAL
[Parser] VALID
[TX] VALID
[RX] DSC
[Parser] VALID
[TX] VALID
[RX] VLS 0.15
[Parser] VALID
[TX] VALID
[RX] MOV R 2000 2000 1000 0 0.5 1
[Parser] VALID
[TX] VALID
[RX] MRV R 2000 2000 1000 0 0.5 1
[Parser] VALID
[TX] VALID
[RX] MSV M02 2000
[Parser] VALID
[TX] VALID
[RX] MSR M06 1
[Parser] VALID
[TX] VALID
[RX] MPV M01 M03 1200.0 -35.5
[Parser] VALID
[TX] VALID
[RX] MPR M01 M03 1200.0 -35.5
[Parser] VALID
[TX] VALID
[RX] 17
[Parser] INVALID - Unknown command
[TX] INVALID
[INFO] Client disconnected.

```
---

# GMT_CMD_PARSER

## 1. 專案說明

`GMT_CMD_PARSER` 是 GMT Motion Control 系統中的 **CM5 Command Parser 專案**。

目前系統的目標架構如下：

```text
┌──────────────────────────────┐
│ End User                     │
│ Windows 11                   │
│ GMT_Client_Command           │
│ CLI TCP Client               │
└──────────────┬───────────────┘
               │
               │ TCP :9999
               ▼
┌──────────────────────────────┐
│ Raspberry Pi CM5             │
│ GMT_CMD_PARSER               │
│                              │
│ TCP Server                   │
│      │                       │
│      ▼                       │
│ Command Parser               │
└──────────────┬───────────────┘
               │
               │ VALID Command
               ▼
┌──────────────────────────────┐
│ USB CDC ACM Transport        │
└──────────────┬───────────────┘
               │
               ▼
┌──────────────────────────────┐
│ STM32H755                    │
│ Command Receiver             │
└──────────────┬───────────────┘
               │
               ▼
┌──────────────────────────────┐
│ SPI1 / EtherCAT              │
│ Motion Control               │
└──────────────────────────────┘
```

目前本階段主要完成：

```text
Windows Client
      ↓
TCP Server
      ↓
Command Parser
```

USB CDC、STM32H755 及 EtherCAT 尚未接入本階段測試。

---

# 2. 本次階段的開發目的

本次工作的主要目的，是先把：

> **End User → TCP Server → Command Parser**

這一段完整驗證。

在正式接上 USB Transport 之前，必須先確認以下功能：

1. Windows End User 可以建立 TCP 連線。
2. CM5 TCP Server 可以接受 TCP Client。
3. TCP Client 可以持續輸入多筆 Command。
4. TCP Server 可以收到完整 Command。
5. TCP Server 可以呼叫獨立的 `CommandParser`。
6. Parser 可以判斷 Command 是否 VALID。
7. VALID Command 回傳 `VALID`。
8. INVALID Command 回傳 `INVALID`。
9. TCP Client 可以顯示 Server 回應。
10. Client 與 Server 可以保持同一條 TCP Connection，連續測試多筆命令。
11. Client 斷線後，Server 可以正確結束目前的 Client Session。

這一階段的重點不是控制馬達，而是先建立穩定的 **Command TCP Interface + Parser Interface**。

---

# 3. 專案目前結構

目前 `GMT_CMD_PARSER` 的主要結構如下：

```text
GMT_CMD_PARSER/
├── CMakeLists.txt
├── include/
│   ├── command_parser.h
│   └── tcp_server.h
└── src/
    ├── main.cpp
    ├── tcp_server.cpp
    ├── command_parser.cpp
    ├── parser_test.cpp
    ├── parser_parameter_test.cpp
    └── GMT_Server_Command.cpp
```

各檔案用途如下：

| 檔案                          | 用途                                   |
| --------------------------- | ------------------------------------ |
| `main.cpp`                  | 正式 GMT Command Parser 程式入口           |
| `tcp_server.cpp`            | 正式 TCP Server 實作                     |
| `tcp_server.h`              | TCP Server Class Interface           |
| `command_parser.cpp`        | Command Parser 實作                    |
| `command_parser.h`          | Command Parser Interface             |
| `parser_test.cpp`           | Parser 基本 Command 測試                 |
| `parser_parameter_test.cpp` | Parser Parameter / Manual Example 測試 |
| `GMT_Server_Command.cpp`    | TCP Server + Parser 整合測試程式           |

---

# 4. Command Parser 的設計原則

Command Parser 必須與 TCP Server 分離。

目前設計：

```text
TCP Server
    │
    │ Command string
    ▼
CommandParser::parse()
    │
    ├── VALID
    │
    └── INVALID
```

Parser 的責任只有：

> 判斷 Command 的格式與目前已確認的參數規則是否正確。

Parser 不負責：

* TCP Socket 管理
* USB 傳輸
* STM32 控制
* EtherCAT
* Motion Algorithm
* Axis Control

因此未來 TCP Server、USB Transport 與 Command Parser 可以各自維護。

---

# 5. Parser Interface

目前 Parser 使用：

```cpp
ParseResult CommandParser::parse(const std::string& input) const;
```

回傳：

```cpp
struct ParseResult
{
    ParserResult result;
    std::string command;
    std::string payload;
    std::string error;
};
```

其中：

```text
ParserResult::VALID
ParserResult::INVALID
```

代表 Parser 的判斷結果。

這種設計可以讓 TCP Server 不需要知道 Parser 內部的 Regex 或參數規則。

---

# 6. 今天 TCP Server 的修改目的

原本 TCP Server 主要只是建立 TCP Socket、接受 Client，並進行非常基本的資料交換。

原始測試方式主要是：

```text
TCP Client
    ↓
TCP Server
    ↓
HELLO_OK
```

這樣只能確認 TCP Connection 本身正常。

但是正式系統需要的是：

```text
TCP Client
    ↓
Command
    ↓
TCP Server
    ↓
Command Parser
    ↓
VALID / INVALID
```

因此今天沒有直接大幅修改正式的：

```text
src/tcp_server.cpp
```

而是先新增獨立的測試程式：

```text
src/GMT_Server_Command.cpp
```

這樣可以避免在正式 TCP Server 尚未完全驗證之前，直接修改已存在的正式架構。

---

# 7. 為什麼新增 GMT_Server_Command.cpp

新增：

```text
src/GMT_Server_Command.cpp
```

主要目的：

> 建立一個獨立的 TCP Server + Command Parser Integration Test。

它不是最終正式 Server，而是用來驗證：

```text
TCP Socket
      ↓
Receive Command
      ↓
CommandParser
      ↓
VALID / INVALID
      ↓
TCP Response
```

這個做法可以將：

```text
TCP Server 問題
```

與：

```text
Command Parser 問題
```

先分開驗證。

---

# 8. GMT_Server_Command.cpp 的工作流程

目前測試 Server 的流程如下：

```text
Start
  │
  ▼
Create TCP Socket
  │
  ▼
Bind 0.0.0.0:9999
  │
  ▼
Listen
  │
  ▼
Accept Client
  │
  ▼
Create CommandParser
  │
  ▼
recv()
  │
  ▼
取得 Command
  │
  ▼
CommandParser::parse()
  │
  ├───────────────┐
  │               │
 VALID          INVALID
  │               │
  ▼               ▼
"VALID\r\n"     "INVALID\r\n"
  │               │
  └───────┬───────┘
          ▼
      send()
          │
          ▼
      Continue
```

只要 Client 沒有斷線，就可以持續接收下一筆 Command。

---

# 9. TCP Server 的測試 Port

Command TCP 使用：

```text
TCP Port = 9999
```

目前設定：

```text
Server Address:
0.0.0.0

Server Port:
9999
```

Windows Client 目前使用：

```text
192.168.137.200:9999
```

其中：

```text
192.168.137.200
```

為 CM5 在目前測試網路環境中的 IP。

---

# 10. GMT_Client_Command

為了測試 CM5 TCP Server，另外建立 Windows End User CLI Client：

```text
GMT_Client_Command
```

Windows 專案與 `GMT_CMD_PARSER` 是兩個不同專案。

架構：

```text
Windows 11
GMT_Client_Command
       │
       │ TCP
       ▼
Raspberry Pi CM5
GMT_CMD_PARSER
       │
       ▼
CommandParser
```

`GMT_Client_Command` 的角色就是模擬真正的 End User。

未來真正的上位機或 End User Application 也可以使用相同的 TCP Command Interface。

---

# 11. GMT_Client_Command 的工作方式

Windows Client 啟動後：

```text
[CONNECT] 192.168.137.200:9999
[OK] Connected to TCP Server.
```

之後進入：

```text
GMT>
```

使用者可以直接輸入：

```text
GMT> 1
```

也可以直接輸入完整 Command：

```text
GMT> MPV M01 M03 1200.0 -35.5
```

Client 將 Command 加上：

```text
\r\n
```

再透過 TCP 傳送給 CM5。

---

# 12. Menu Command 測試方式

目前 Windows Client 提供已確認 Manual Example 的選單。

例如：

```text
1.  INS 1
2.  STP
3.  SAH M01 M02 M03
4.  SHC M01 2 17 400000 40000 0 128
5.  SHC? M01
6.  SVO
7.  SVF
8.  CAL
9.  DSC
10. VLS 0.15
11. MOV R 2000 2000 1000 0 0.5 1
12. MRV R 2000 2000 1000 0 0.5 1
13. MSV M02 2000
14. MSR M06 1
15. MPV M01 M03 1200.0 -35.5
16. MPR M01 M03 1200.0 -35.5
17. MOV?
18. POS?
19. PMS?
20. SPI R U 10 5 2
21. SPI?
22. FRS?
23. DFRS ScanRoutine01
24. FLM M10 0.2 V 0.01 TH 255
25. BKN?
```

例如輸入：

```text
GMT> 15
```

Client 會將：

```text
MPV M01 M03 1200.0 -35.5
```

送給 TCP Server。

---

# 13. Menu Index Mapping

Client 內部使用：

```cpp
std::vector<std::string>
```

保存測試命令。

輸入：

```text
15
```

不應該直接把：

```text
15
```

送到 Server。

而是要轉換成：

```text
MPV M01 M03 1200.0 -35.5
```

這也是今天測試過程中發現並修正的重要問題。

修正後：

```text
GMT> 15
[TX] MPV M01 M03 1200.0 -35.5
[RX] VALID
```

代表：

```text
Menu
  ↓
Command Mapping
  ↓
TCP
  ↓
Parser
```

整條路徑正常。

---

# 14. Free-form Command

除了 Menu 之外，Windows Client 也可以直接輸入 Command。

例如：

```text
GMT> UNKNOWN
```

Client：

```text
[TX] UNKNOWN
```

Server：

```text
INVALID
```

Client：

```text
[RX] INVALID
```

這可以用來驗證 Parser 的 INVALID Path。

---

# 15. TCP Persistent Connection

今天另外確認了 TCP Connection 可以保持。

不是每一筆 Command 都重新建立 Socket。

流程為：

```text
Connect
   │
   ├── Command 1
   ├── Response 1
   │
   ├── Command 2
   ├── Response 2
   │
   ├── Command 3
   ├── Response 3
   │
   └── ...
```

例如：

```text
GMT> 1
[TX] INS 1
[RX] VALID

GMT> 15
[TX] MPV M01 M03 1200.0 -35.5
[RX] VALID

GMT> 25
[RX] VALID

GMT> UNKNOWN
[TX] UNKNOWN
[RX] INVALID
```

這表示 Client 與 Server 的基本 Command Session 已經可以正常工作。

---

# 16. CRLF Command Format

目前 TCP Command 使用：

```text
\r\n
```

作為 Command 結尾。

因此 Windows Client 實際傳送的是：

```text
COMMAND\r\n
```

例如：

```text
MOV R 2000 2000 1000 0 0.5 1\r\n
```

Parser 本身已經能夠處理：

```text
\r
\n
```

因此 TCP Server 不需要為了 Parser 再建立另一套特殊 Command 格式。

---

# 17. GMT_Server_Command 與正式 TCP Server 的關係

必須特別說明：

```text
src/GMT_Server_Command.cpp
```

目前是：

> 測試用 TCP Server。

而：

```text
src/tcp_server.cpp
```

才是：

> 正式 GMT_CMD_PARSER TCP Server。

目前沒有直接用測試程式取代正式程式。

這是刻意的架構保護方式。

目前開發順序：

```text
正式 TCP Server
       │
       │ 保持穩定
       ▼
新增 GMT_Server_Command
       │
       ▼
驗證 TCP + Parser
       │
       ▼
測試成功
       │
       ▼
再整合回正式 TcpServer
```

因此 `GMT_Server_Command.cpp` 的目的不是形成另一個永久 Server 架構。

測試成功後，正式功能應該回存到：

```text
src/tcp_server.cpp
include/tcp_server.h
```

而不是長期依賴測試程式。

---

# 18. CMake 測試 Target

目前 `CMakeLists.txt` 除了正式：

```text
Gmt_CMD_Parser
```

以及 Parser Test：

```text
parser_test
parser_parameter_test
```

之外，增加：

```text
GMT_Server_Command
```

對應：

```text
src/GMT_Server_Command.cpp
src/command_parser.cpp
```

這樣可以單獨編譯 TCP Server + Parser Integration Test。

---

# 19. 目前已完成的 Parser 測試

目前已確認的 Manual Example 包含：

```text
INS 1
SAH M01 M02 M03
SHC M01 2 17 400000 40000 0 128
SHC? M01
VLS 0.15
MOV R 2000 2000 1000 0 0.5 1
MRV R 2000 2000 1000 0 0.5 1
MSV M02 2000
MSR M06 1
MPV M01 M03 1200.0 -35.5
MPR M01 M03 1200.0 -35.5
SPI R U 10 5 2
DFRS ScanRoutine01
FLM M10 0.2 V 0.01 TH 255
FLM M08 2 V 0.2
BKN 0.015
```

另外也確認多個無參數 Command：

```text
STP
SVO
SVF
CAL
DSC
MOV?
POS?
PMS?
SPI?
FRS?
BKN?
```

INVALID 測試則包含：

```text
STP 123
SVO ABC
MOV? 123
UNKNOWN
empty input
```

目前 Parser 測試均已通過。

---

# 20. 今天的 End-to-End 測試

本階段最重要的測試路徑：

```text
Windows 11
GMT_Client_Command
        │
        │ TCP 192.168.137.200:9999
        ▼
CM5
GMT_Server_Command
        │
        ▼
CommandParser
```

測試結果：

```text
[CONNECT] 192.168.137.200:9999
[OK] Connected to TCP Server.
```

Menu Command 測試：

```text
GMT> 1
[TX] INS 1
[RX] VALID
```

```text
GMT> 15
[TX] MPV M01 M03 1200.0 -35.5
[RX] VALID
```

INVALID：

```text
GMT> UNKNOWN
[TX] UNKNOWN
[RX] INVALID
```

最後：

```text
GMT> 0
```

正常離開 Client。

本階段確認：

```text
TCP Connection        PASS
Persistent Connection PASS
Command Transmission  PASS
CRLF                  PASS
Menu Mapping          PASS
Free-form Command     PASS
Parser VALID          PASS
Parser INVALID        PASS
TCP Response          PASS
Client Disconnect     PASS
```

---

# 21. 為什麼目前還不直接接 USB

目前先不把：

```text
CommandParser
      ↓
USB Transport
      ↓
STM32H755
```

接進來，是刻意的開發順序。

原因是必須先把：

```text
End User
    ↓
TCP
    ↓
Parser
```

驗證完成。

如果 TCP、Parser、USB、STM32 同時接在一起，一旦出現：

```text
Command INVALID
```

或：

```text
Command 沒有送到 STM32
```

就很難立即判斷問題位於：

```text
Windows Client
TCP
TCP Server
Parser
USB Transport
STM32 USB CDC
```

哪一層。

因此目前採用分層驗證。

---

# 22. 後續正式架構

完成目前測試後，下一階段將把已驗證的：

```text
GMT_Server_Command.cpp
```

功能整合回正式：

```text
tcp_server.cpp
```

最後正式程式應該形成：

```text
End User TCP Client
        │
        ▼
TcpServer
        │
        ▼
CommandParser
        │
        ├── INVALID
        │      │
        │      ▼
        │   TCP Error Response
        │
        └── VALID
               │
               ▼
          USB Transport
               │
               ▼
           STM32H755
```

其中：

> INVALID Command 不得進入 USB Transport。

只有：

```text
ParserResult::VALID
```

的 Command 才能進入下一層。

---

# 23. Parser 的後續擴充

目前已確認的 Command 尚未達到最終預定的約 36 個 Command。

因此後續仍需依照正式 Manual / Command Document：

```text
確認 Command
      ↓
確認 Manual Example
      ↓
建立 Parser Rule
      ↓
建立 Parameter Test
      ↓
編譯
      ↓
Parser Test
      ↓
TCP End-to-End Test
```

重要原則：

> 沒有 Manual Example 或明確規格的 Command，不自行猜測參數格式與範圍。

這可以避免 Parser 先入為主地把尚未確認的規則寫死。

---

# 24. 測試檔案的定位

目前測試分成不同層級。

### Parser 基本測試

```text
src/parser_test.cpp
```

主要確認：

```text
Command 是否存在
Command 是否 VALID / INVALID
```

### Parser Parameter Test

```text
src/parser_parameter_test.cpp
```

主要確認：

```text
Manual Example
Parameter Format
Regex Rule
```

### TCP Server Integration Test

```text
src/GMT_Server_Command.cpp
```

主要確認：

```text
Windows TCP Client
        ↓
TCP Server
        ↓
CommandParser
        ↓
TCP Response
```

三者的責任不同，不應混在同一個測試程式中。

---

# 25. GitHub Checkpoint 原則

目前開發採用：

```text
修改
  ↓
編譯
  ↓
單元測試
  ↓
Integration Test
  ↓
End-to-End Test
  ↓
確認 PASS
  ↓
GitHub Checkpoint
```

測試用程式可以暫時存在，以降低直接修改正式架構造成的風險。

但是：

> 測試成功後，正式功能必須整合回正式程式。

不能讓：

```text
GMT_Server_Command.cpp
```

永久取代正式：

```text
tcp_server.cpp
```

也不應該建立一個長期維護的「測試版 Server 分支」。

---

# 26. 目前開發進度

目前完成：

```text
[✓] Command Parser 基本架構
[✓] Parser 基本測試
[✓] Parser Parameter Test
[✓] CM5 TCP Server 基礎
[✓] Windows GMT_Client_Command
[✓] TCP Port 9999
[✓] Persistent TCP Connection
[✓] CLI Command Menu
[✓] Menu Index Mapping
[✓] Free-form Command
[✓] CRLF Command
[✓] GMT_Server_Command Integration Test
[✓] TCP → Parser
[✓] VALID Response
[✓] INVALID Response
[✓] 25 個已確認 Command / Manual Example
[✓] End-to-End Test
```

尚未完成：

```text
[ ] Parser 剩餘 Command 完整化
[ ] GMT_Server_Command 功能整合回正式 TcpServer
[ ] 正式 TCP Server 完整 Session 管理
[ ] TCP Response 正式格式定義
[ ] USB Transport 整合
[ ] STM32H755 Command Receiver
[ ] STM32 → CM5 Response
[ ] CM5 → Windows TCP Response
[ ] EtherCAT Motion Execution
```

---

# 27. 最終目標

最終 Command Control Path：

```text
┌─────────────────────┐
│ End User            │
│ GMT_Client_Command  │
└──────────┬──────────┘
           │
           │ TCP :9999
           ▼
┌─────────────────────┐
│ CM5                 │
│ GMT_CMD_PARSER      │
│                     │
│ TcpServer           │
│      ↓              │
│ CommandParser       │
└──────────┬──────────┘
           │
           │ VALID
           ▼
┌─────────────────────┐
│ USB CDC Transport   │
└──────────┬──────────┘
           │
           ▼
┌─────────────────────┐
│ STM32H755           │
│ Command Receiver    │
└──────────┬──────────┘
           │
           ▼
┌─────────────────────┐
│ EtherCAT / Motion   │
│ Controller          │
└─────────────────────┘
```

Response Path 則為：

```text
STM32H755
    │
    │ Execution Result
    ▼
USB CDC
    │
    ▼
CM5
    │
    ▼
TCP Server
    │
    ▼
End User
```

因此 `GMT_CMD_PARSER` 最終會成為：

> **End User Command 與 STM32 Motion Control System 之間的 CM5 Command Gateway。**

---

# 28. 開發原則

本專案後續持續遵守以下原則：

1. **Parser 與 TCP Server 分離。**
2. **TCP Server 與 USB Transport 分離。**
3. **Parser 不負責 Motion Control。**
4. **INVALID Command 不進 USB。**
5. **只有 VALID Command 才能進入下一層。**
6. **優先依照正式 Manual 建立 Parser 規則。**
7. **沒有規格的參數不自行猜測。**
8. **先建立獨立 Test，再整合回正式程式。**
9. **每完成一個階段都必須 Compile + Test。**
10. **確認所有測試 PASS 後建立 GitHub Checkpoint。**
11. **避免不必要的大幅修改既有架構。**
12. **測試程式不應取代正式架構。**

---

# 29. 本階段 Checkpoint

目前已完成：

```text
GMT_Client_Command
        │
        │ TCP
        ▼
GMT_Server_Command
        │
        ▼
CommandParser
```

而且已完成 Windows → CM5 → Parser 的實際 End-to-End 測試。

因此目前可以將本階段視為：

```text
TCP Client
    +
TCP Server
    +
Command Parser
    +
Integration Test
```

的完整 Checkpoint。

下一階段應在這個穩定基礎上，逐步將測試成功的 TCP Server + Parser 邏輯整合回正式 `TcpServer`，再進一步處理 USB Transport。

> 今天的重點在 **「為什麼新增 `GMT_Server_Command.cpp`、它和正式 `tcp_server.cpp` 的關係、以及它如何對應 Windows `GMT_Client_Command`」**，並且保留後續要整合回正式架構的原則。



