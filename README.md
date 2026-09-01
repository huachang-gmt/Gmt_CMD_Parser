
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
