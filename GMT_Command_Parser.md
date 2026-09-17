
# GMT Command Parser — Manual Clarification Questions

## 1. INS — Initialize System

目前已知：

```text
INS <device type>
```

`device type`：

* `0 = USB`
* `1 = PCIe`

### 需要釐清

1. **Command 大小寫是否嚴格區分？**

   * `INS 1` 是否合法？
   * `ins 1` 是否也合法？
   * `Ins 1` 是否合法？

2. **Command 前後是否允許空白？**

   * ` INS 1`
   * `INS 1 `
   * `INS  1`

   這些格式是否接受？

3. **`device type` 是否只能接受十進位 `0` / `1`？**

   * 例如 `INS 0x01` 是否合法？
   * Manual Overview 說 numeric values 可使用 decimal 或 hexadecimal，因此需要確認 Command Reference 是否也適用。

4. **INS 的正式 Response 是否確定為以下三種狀態？**

   * `Connecting...`
   * `Connected.`
   * `Connect failed.`

   Example 中的 `>Connecting...`、`>Connected.` 前面的 `>` 是否是正式 TCP Response 的一部分？

5. **INS 執行期間的 `Connecting...` 是獨立 Response，還是只在內部顯示/Log？**

   例如 Client 是否會實際收到：

   ```text
   >Connecting...
   ```

   然後再收到：

   ```text
   >Connected.
   ```

6. **INS 已經連線時「has no effect」的 Response 是什麼？**

   * 是否仍回 `Connected.`？
   * 是否沒有 Response？
   * 是否有其他 Response？

7. **INS 的 `Connect failed.` 是否會附帶 ErrorCode？**

   目前文件只有：

   ```text
   Connect failed.
   ```

   需要確認是否存在例如：

   ```text
   INS ERR <ErrorCode>
   ```

---

# 2. STP — Stop All Motor

目前已知：

```text
STP
```

無參數。

### 需要釐清

1. **STP 是否永遠只能是完全無參數？**

   例如：

   ```text
   STP 1
   STP M01
   ```

   是否一定視為 INVALID？

2. **STP 成功 Response 是否固定為：**

   ```text
   >STP
   >Done
   ```

3. **STP 執行失敗時 Response 格式為何？**

   目前只看到成功 Example，沒有看到 failure format。

   是否為：

   ```text
   >STP ERR <ErrorCode>
   ```

4. **如果目前沒有任何 Axis 在運動，執行 STP 的 Response 是什麼？**

   * 仍然 `>STP / >Done`？
   * 還是其他結果？

5. **STP 是否可以在 INS 尚未成功之前執行？**

   Manual 說 INS 必須在 motion/I/O commands 前執行，因此需要確認 STP 是否也受到這個前置條件限制。

---

# 3. SAH — Start Axis Homing

目前已知：

```text
SAH M01
SAH M01 M02 M03
SAH A
```

### 需要釐清

1. **`AxisName` 的正式完整格式是什麼？**

   文件目前只有：

   ```text
   e.g. M01
   ```

   需要確認正式規則，例如：

   * 是否一定是 `M` + 2 digits？
   * 是否可能有 `M1`？
   * 是否可能有其他 AxisName？

2. **AxisName 是否區分大小寫？**

   例如：

   ```text
   SAH M01
   SAH m01
   ```

   是否都合法？

3. **SAH 的 Axis list 是否允許任意數量？**

   例如：

   ```text
   SAH M01 M02 M03 M04 ...
   ```

   最大可以幾個 Axis？

4. **SAH Axis list 是否允許重複 Axis？**

   例如：

   ```text
   SAH M01 M01 M02
   ```

   是 VALID 還是 INVALID？

5. **SAH Axis list 的順序是否有意義？**

   例如：

   ```text
   SAH M01 M02
   ```

   與：

   ```text
   SAH M02 M01
   ```

   是否有不同的 Homing 順序？

6. **`A` 是否完全保留為特殊 keyword？**

   也就是：

   ```text
   SAH A
   ```

   合法，但：

   ```text
   SAH A M01
   ```

   是否一定 INVALID？

7. **`A` 是否區分大小寫？**

   ```text
   SAH A
   SAH a
   ```

   哪一個合法？

8. **如果指定的 Axis 沒有透過 SHC 設定 Homing Configuration，Response 是什麼？**

   是否會：

   ```text
   >SAH ERR <ErrorCode>
   ```

9. **如果指定的 Axis 不存在，Response / ErrorCode 是什麼？**

10. **SAH 的 `>SAH` 是「開始 Homing」的立即 Response，還是 Homing 完成後才回覆？**

    這一點目前有一點矛盾：

    Response 寫：

    ```text
    >SAH
    ```

    但 Notes 又寫：

    ```text
    No response will be returned until all axes have complete homing.
    ```

    需要明確確認實際行為。

11. **`>homing end` 是否是 SAH 的最終 Response？**

    例如：

    ```text
    >SAH
    >homing end
    ```

    還是實際只會收到：

    ```text
    >homing end
    ```

12. **SAH Homing 失敗時，是否一定使用：**

    ```text
    >SAH ERR <ErrorCode>
    ```

    還是 Homing 過程中可能出現其他非 `SAH ERR` 的 Response？

13. **SAH 是否必須先執行 INS？**

    目前依照總體說明應該是，但希望文件正式明確定義。

---

# 4. SHC — Set Homing Configuration

目前已知：

```text
SHC <AxisName> <SlaveIdx> <Group> <Method> <Speed> <Offset> <AddrOffset>
```

共 7 個參數。

### 需要釐清

1. **`AxisName` 的正式格式是否與 SAH 完全相同？**

   例如是否一定：

   ```text
   M01
   M02
   ...
   ```

2. **`SlaveIdx` 的合法範圍是多少？**

   例如：

   ```text
   0 ~ ?
   ```

3. **`Group` 的合法範圍是多少？**

4. **`Method` 的合法範圍 / 有效值有哪些？**

   這是目前最重要的問題之一。

   文件說：

   ```text
   Homing method code (integer)
   ```

   但 Example：

   ```text
   Method = 400000
   ```

   因此需要確認完整的 Method code 定義。

5. **`Speed` 的合法範圍是多少？**

   單位目前已知是：

   ```text
   pulses/sec
   ```

   但需要確認：

   * 最小值
   * 最大值
   * 是否允許 `0`
   * 是否允許負數

6. **`Offset` 的合法範圍是多少？**

   單位：

   ```text
   pulses
   ```

   是否允許負值？

7. **`AddrOffset` 的合法範圍是多少？**

   是否允許負值？

8. **上述數值參數是否只能使用十進位？**

   因為 Overview 說 numeric values 可以是 decimal 或 hexadecimal，需要確認：

   ```text
   SHC M01 0x02 0x11 ...
   ```

   是否合法。

9. **SHC 是否允許 `+100` 這種 signed integer 格式？**

10. **SHC 是否允許 leading zero？**

    例如：

    ```text
    SHC M01 02 017 ...
    ```

    是否與：

    ```text
    SHC M01 2 17 ...
    ```

    等價？

11. **SHC 是否必須在 INS 成功後執行？**

12. **SHC 是否可以在 Servo ON / Motion 狀態下執行？**

13. **如果 `AxisName` 不存在，Response / ErrorCode 是什麼？**

14. **如果 `SlaveIdx` 不存在，Response / ErrorCode 是什麼？**

15. **如果參數格式正確但數值超出範圍，是否統一回：**

    ```text
    >SHC ERR <ErrorCode>
    ```

16. **SHC 成功 Response 是否固定為：**

    ```text
    >SHC
    >Done
    ```

17. **SHC 寫入 INI configuration file 失敗時，ErrorCode 是什麼？**

---

# 5. 目前「共通 Parser 規則」也有幾個需要文件設計者確認

這些不是單一 Command，而是會直接影響我們整個 Parser。

### Command syntax

1. **Command name 是否 Case Sensitive？**

2. **參數之間是否只允許一個 Space，還是任意 whitespace 都可以？**

例如：

```text
INS 1
INS  1
INS\t1
```

哪些合法？

3. **Command 前後是否允許 whitespace？**

4. **`\r\n` 是否是所有 Command 的強制 terminator？**

5. **如果 TCP Client 傳送只有 `\n`，是否接受？**

6. **Parser 收到 command 時，是否應該移除 `\r\n` 後再解析？**

7. **空 Command：**

   ```text
   \r\n
   ```

   應該回什麼？

---

### Numeric parameters

8. Manual Overview 說 decimal / hexadecimal 都可以。

   **這個規則是否適用於所有 Command 的 numeric parameter？**

9. Hexadecimal 的正式格式是否限定：

```text
0x10
```

還是也接受：

```text
0X10
10h
```

10. Numeric 是否允許負號：

```text
-100
```

11. Numeric 是否允許 `+100`？

---

### Error Response

12. **所有 Parser-level INVALID command 是否有統一格式？**

例如：

```text
ERR <ErrorCode>
```

還是每個 Command 各自定義？

13. **ErrorCode 是否有完整的 Global ErrorCode Table？**

14. Command 不存在時的 Response 是什麼？

例如：

```text
ABC 123
```

15. Argument 數量錯誤時的 Response 是什麼？

例如：

```text
INS
INS 1 2
```

16. Argument type 錯誤時的 Response 是什麼？

例如：

```text
SHC M01 ABC 17 400000 40000 0 128
```

17. Argument value 超出範圍時的 Response 是什麼？

---

# 最重要的幾個問題

如果文件設計者時間有限，我會建議你**優先詢問這 10 個**：

1. **Command / AxisName / Keyword 是否 Case Sensitive？**
2. **Space / whitespace 的允許規則是什麼？**
3. **所有 numeric parameters 是否都允許 decimal + hexadecimal？**
4. **INS 的三種 Response 是否正式為 `Connecting...` / `Connected.` / `Connect failed.`？`>` 是否屬於正式 response？**
5. **INS 已經 connected 時，實際 Response 是什麼？**
6. **SAH `>SAH` 與 `>homing end` 的實際 Response 時序是什麼？**
7. **SAH AxisName 的完整合法格式與最大 Axis 數量？**
8. **SHC 每個 numeric parameter 的合法 range / enum / signedness？**
9. **所有 Command 的 INVALID / ERR Response 格式與 ErrorCode 定義？**
10. **新版增加 Command 時，Command Definition 是否可以完全由外部/設定資料擴充，而不修改 Parser Core？**

尤其是 **第 6、8、9 項**，會直接影響我們之後 Parser 的正確性。

另外，我會繼續看後面的 Command。**如果後面 Manual 已經回答了上述問題，我就不會重複列；如果後面又發現新的歧義，我會繼續加到這份清單。**


# 本頁我需要記錄的「待詢問作者」問題

依照你的要求，我現在**不急著問作者**，先累積。

### SHC?

1. `SHC?` 是否嚴格區分大小寫？
    - 目前我們依你的決定先視為嚴格大小寫。
2. `SHC?` Response 中各欄位的正式型別與單位為何？
    - 特別是 `Speed` 範例為 `400000.0`
    - 前面的 `SHC` 範例則為 `400000`
3. `SHC?` Example：

```
>SHC? M01 1 2 17 400000.0 40000 0 5
```

各欄位是否確實對應：

```
AxisName
SlaveIdx
Group
Method
Speed
Offset
AddrOffset
```

1. `SHC` 設定範例與 `SHC?` 查詢結果的數值：

```
SHC M01 2 17 400000 40000 0 128
```

vs.

```
SHC? M01 1 2 17 400000.0 40000 0 5
```

為什麼數量/排列看起來不同？這是否只是文件 Example 錯誤？

這一題我會列為**高優先級**，因為它直接影響 Parser 與未來 Response Decoder。

1. 查詢不存在的 Axis 時，Response 格式為何？
2. Axis 尚未配置時，`SHC?` 的 Response 為何？

---

### SVO

1. `SVO` 執行失敗時的 Response 是什麼？
2. `SVO` 尚未執行 `INS` 時，是 Parser INVALID，還是 Controller 執行後回傳 ERROR？

目前依整體架構，我傾向後者，但**不先自行定義文件規則**。

1. Servo 已經 ON 時再次執行：

```
SVO
```

文件說「no effect」，但 Response 是否仍然：

```
>SVO
>Done
```

1. `SVO` 是否永遠只控制「全部軸」，沒有：

```
SVO M01
```

這種形式？

目前依文件是**沒有參數**，所以 Parser 暫時嚴格限制為零參數。


### 待詢問作者

- `SVF` 執行失敗時 Response 是什麼？
- Servo 已經 OFF 時，是否仍固定回：
    
    ```
    >SVF
    >Done.
    ```
    
- `SVF` 是否一定需要先 `INS`？
- 「Motion commands will be rejected」的正式錯誤 Response 格式為何？

### 待詢問作者

- `CAL` 執行失敗時 Response 是什麼？
- 如果沒有 Alarm，執行 `CAL` 是否仍回：
    
    ```
    >CAL
    >Done.
    ```
    
- `CAL` 是否需要 `INS`？
- 如果某個 Driver Alarm 無法清除，Response 如何表示？是否有 ErrorCode？

# ERR? 特別需要問作者的問題

這幾題我會列為**高優先級**。

### AxisName

文件前面：

```
M01
M08
```

但 ERR? 範例：

```
M1
M5
```

所以需要確認：

> `M1` 與 `M01` 是否代表相同 Axis？
> 

以及正式格式到底是：

```
M1
```

還是：

```
M01
```

或兩者都接受？

---

### ErrorCode

目前文件只說：

> Error codes correspond to driver-specific status registers.
> 

所以 Parser **目前不能自行限制 ErrorCode 範圍**。

需要作者確認：

- ErrorCode 是 UINT16？
- UINT32？
- Decimal？
- 是否可能 hexadecimal？
- 是否允許負值？
- ErrorCode `0` 是否代表無錯誤？
- 是否有 GMT 自己統一的 ErrorCode Table？

---

### Error message 數量

需要確認：

- 最多會有幾筆？
- 是否每個 Axis 最多一筆？
- 是否可能同一 Axis 有多個 ErrorCode？
- Response 順序是否固定？
- 是否包含 I/O module？
- 文件 Description 說：
    
    > all motor axes and I/O modules
    > 
- 但 Example 只有：
    
    ```
    M1
    M5
    ```
    
    所以 I/O module 的正式識別格式還不清楚。

# 本頁新增待詢問問題

最後我先把**本頁新增的問題**記錄下來，不跟之前已經問過的重複：

### SVF

- 執行失敗 Response？
- 已經 OFF 時是否仍固定 `>SVF / >Done.`？
- 是否要求先 INS？
- Servo OFF 導致 Motion command 被拒絕時的正式 Error Response？

### CAL

- 執行失敗 Response？
- 沒有 Alarm 時是否仍 `>CAL / >Done.`？
- 是否要求先 INS？
- Alarm 無法清除時如何回報？

### ERR?

- `M1` vs `M01` 的正式 AxisName 格式？
- ErrorCode 的資料型別與範圍？
- ErrorCode 是否允許 HEX？
- 是否允許同一 Axis 多筆 Error？
- 最大 Error record 數量？
- Response record 順序是否固定？
- I/O module 的 identifier 格式？
- `No Error` 的大小寫、空格、標點是否嚴格固定？
- `ERR?` 是否一定只有 `No Error` 或 `ERR : ...` 兩種 Response？

### 待詢問作者

- DSC 執行失敗時的 Response？
- 是否永遠只有：
    
    ```
    >Disconnected.
    ```
    
- 已經 disconnected 時再次 `DSC` 的 Response？
- `DSC` 是否一定要求先 `SVF`？
- `DSC` 後如果直接執行 `SVO` / Motion command，正式 Error Response 是什麼？

# MOV 新增待詢問作者問題

這一頁我會加入：

### DSC

- Failure response？
- 已 disconnect 再 DSC 的 response？
- 是否必須 SVF 後才能 DSC？
- DSC 後 command rejected 的正式 error format？

### VLS

- `value` 的合法 range？
- 是否允許負值？
- 是否允許 `1` 這種沒有小數點的形式？
- 是否允許 scientific notation？
- decimal / hexadecimal 規則是否適用 FLOAT？
- `VLS` 執行失敗 response？

### VLS?

- Response `<value>` 的正式格式與 precision？
- 是否永遠以 decimal floating-point 回傳？
- 已未設定 VLS 時回什麼？

### MOV

**最高優先級：**

1. `R/L` 與 `PR/LP` 是否都是 6 個數值？
2. 如果 `PR/LP` 不需要 M3~M5，正式 Format 應如何寫？
3. M0~M5 是否全部允許 floating point？
4. M0~M2 的合法 range？
5. M3~M5 的合法 range？
6. M0~M5 是否允許負值？
7. `MOV R 2000 ...` 的單位 μm 是否允許小數？
8. `Excute` 是否為正式 Response 拼字？
9. `THRSHOLD` 是否為正式 Response 拼字？
10. `>MOV` 是「accepted / started」還是代表 execution success？
11. `>MOV ERR [error code]` 的 error code 格式？
12. `>Excute Result...` 與 `>Done` 的完整時序？
13. `>Done` 是否無論 NORMAL / ERR STOP / ERR / NOT ON TARGET 都一定出現？
14. `>Done` 是否一定代表整個 MOV transaction 結束？
15. MOV 執行中是否允許接收下一個 TCP command？
16. MOV 執行中的 intermediate response 是透過同一 TCP connection 傳送嗎？

---
# ⚠️ 本頁非常重要的文件矛盾 / 待確認

### ① MOV vs MRV：PR / RP

MOV：

```
PR = Right Side Piezo
```

MRV：

```
RP = Right Side Piezo
```

必須問作者：

> `MOV` 的 `PR` 與 `MRV` 的 `RP` 是否都是同一個 Right Side Piezo？其中一個是否為文件 typo？
> 

這個我會列為**高優先級**。

---

### ② MRV C 是什麼？

目前文件只有：

```
MRV C dM0 dM1 dM2
```

沒有解釋 `C`。

需要問：

> `C` 的正式意義是什麼？它與 R/L/ RP/LP 的關係為何？
> 

---

### ③ MRV C 的三個值是什麼單位？

文件只寫：

```
dM0 dM1 dM2
```

需要確認：

- μm？
- 其他 coordinate？
- 是否允許 rotation？

---

### ④ PR / RP / LP 是否需要 3 個還是 6 個參數？

MRV 的：

```
dM3 dM4 dM5
```

明確寫：

> R/L only
> 

所以目前我們只能確定：

```
R/L → 6
```

但：

```
RP/LP → ?
```

文件沒有說清楚。

---

### ⑤ MSV / MSR 的 AxisName

目前：

```
<axis>: MN
```

但 Examples：

```
M02
M06
```

需要確認 `MN` 的正式定義。

例如：

```
M01 ~ M12
```

是否就是完整範圍。

---

### ⑥ Axis 單位

MSV/MSR：

```
M02 → μm
M06 → °
```

需要確認：

> Axis 與 unit 的 mapping 是否固定？是否有完整 Axis → Unit 表？
> 

這個未來可能屬於 Semantic Validation。

---

# ⚠️ 又發現一個 Response 拼字問題

前面 MOV 已經出現：

```
>Excute Result
```

現在 MRV / MSV / MSR 全部再次出現：

```
>Excute Result
```

所以這可能不是單純一次 typo，而可能真的就是舊 controller 的 protocol 字串。

**目前我們絕對不改成 `Execute`。**

最後問作者時，我會統一問：

> `Excute Result` 是否為實際 Controller Response 的正式字串？還是 Manual 中的拼字錯誤？
> 

這樣比每個 command 各問一次更好。

---
# 這一批需要加入的問題

我先不要求你現在回答，全部累積到最後。

### MPV / MPR

**Q1. Axis list 的正式 grammar 是什麼？**

例如：

```
MPV M01 M03 1200.0 -35.5
```

Parser 是不是應該透過 `Mxx` 自動辨識 Axis list 的結束？

還是手冊其實有其他 delimiter？

---

**Q2. Axis 數量限制？**

例如：

```
MPV M01 100
```

是否至少 1 軸？

最多：

```
6
```

還是：

```
12
```

還是其他數量？

---

**Q3. Axis 是否允許重複？**

例如：

```
MPV M01 M01 100 200
```

應該：

```
INVALID
```

還是由執行層處理？

---

**Q4. Axis 順序是否重要？**

例如：

```
MPV M01 M03 100 200
```

是不是永遠：

```
M01 → 100
M03 → 200
```

這目前看起來是，但需要正式確認。

---

**Q5. MPV/MPR 的 position/delta 數值型別？**

目前範例出現：

```
1200.0
-35.5
```

但需要確認正式定義是：

```
FLOAT
DOUBLE
一般 numeric
```

以及是否允許：

```
1200
+1200
1.2e3
```

---

**Q6. 每一個 Axis 的單位怎麼判斷？**

例如：

```
M01 → μm
M06 → degree
```

這需要正式 Axis mapping。

Parser 是否需要知道：

```
M01~M03 = μm
M04~M06 = degree
```

還是這屬於 Execution Layer？

---

### MOV?

**Q7. MOV? 的 Response 是否永遠只有 `0` / `1`？**

目前看起來是：

```
>MOV? 0
>MOV? 1
```

需要確認是否存在其他狀態。

---

### POS?

**Q8. POS? 的六個值正式 mapping 是什麼？**

例如：

```
R1 = X
R2 = Y
R3 = Z
R4 = RX
R5 = RY
R6 = RZ
```

目前只能從文件推測，尚未正式確認。

---

**Q9. POS? 到底是 Right side、Left side，還是整體 Tool Position？**

這是目前文件內部矛盾，必須問作者。

---

**Q10. POS? 六個值的正式 numeric type / precision？**

例如是否：

```
double
```

以及輸出的 decimal precision 是否固定。

---

### PMS?

**Q11. PMS? 的 R1~R6 分別對應哪六個 mechanism/encoder？**

需要正式 mapping。

---

**Q12. PMS? 為什麼只有 6 個值？**

這與手冊其他地方的：

```
3 drive groups × 4 axes = 12 axes
```

存在明顯疑問。

需要作者確認 PMS? 的六個值究竟代表什麼。

---

**Q13. PMS? 的 R1~R6 是否與 POS? 使用完全相同的座標順序？**

目前不能假設。

### ⚠️ SPI 問題

**Q14. X/Y/Z 的 numeric type 是什麼？**

目前只有單位 mm，沒有正式說明：

- FLOAT / DOUBLE？
- 是否允許負值？
- 是否允許 `10`、`10.0`？
- 是否允許 scientific notation？

---

**Q15. X/Y/Z 是否有範圍限制？**

例如：

```
SPI R U -1000 0 0
```

Parser 是否接受？

---

**Q16. `SPI R U ...` 與 `SPI R V ...` 是否完全獨立儲存？**

手冊說每個 side 的 U/V/W 都可以有自己的 pivot，看起來是：

```
R-U
R-V
R-W
L-U
L-V
L-W
```

共 6 組，但正式資料結構可在最後確認。

### ⚠️ SPI? 問題

**Q17. SPI? 的正式輸出格式需要明確定義。**

目前手冊只寫：

```
SPI? <matrix values>
```

以及：

```
U: x y z
V: x y z
W: x y z
```

但沒有明確規定：

```
R:
U: ...
V: ...
W: ...

L:
U: ...
V: ...
W: ...
```

還是其他格式。

這對未來 Response Decoder 很重要。

---

**Q18. SPI? 的輸出順序是否固定？**

例如是否一定：

```
R-U
R-V
R-W
L-U
L-V
L-W
```

---

### ⚠️ FRS 問題

**Q19. RoutineName 的正式 grammar 是什麼？**

例如：

```
ScanRoutine01
```

是否允許：

```
Scan_Routine01
Scan-Routine01
123
```

是否區分大小寫？

---

**Q20. `FRS ERR` 的正式錯誤格式是否有 ErrorCode？**

目前只看到：

```
>FRS ERR
```

不像 MOV：

```
>MOV ERR [ErrorCode]
```

需要確認。

### ⚠️ FRS? 問題

**Q21. Routine list 的正式輸出格式？**

手冊只說：

```
[List of stored routines]
```

沒有實際 response example。

這需要日後確認。

### ⚠️ DFRS 問題

**Q22. `Wrong Parameter` 與 `Cannot Find Routine Name` 的判定界線？**

例如：

```
DFRS ABC
```

ABC 不存在，是：

```
Cannot Find Routine Name.
```

但如果 routine name 格式本身非法，是：

```
Wrong Parameter.
```

這需要定義。

---

**Q23. Controller disconnected 時，是否永遠回傳：**

```
>DFRS ERR: Check Controller Connecting Status.
```

還是某些 simulation mode 例外？

手冊有提到：

> simulation mode may bypass controller connection checking.
> 

這屬於 execution state，不建議 Parser 自己處理。

# ⚠️ FDR 需要確認的問題

**Q24. FDR 的正式 token spacing 是什麼？**

Format：

```
FDR <name><side><scanAxis>...
```

但其他 command 又是：

```
FDR name R X 1000 Y 500 10
```

到底哪一個才是正式 grammar？

---

**Q25. `<scanAxis>` / `<stepAxis>` 的合法 Axis 清單？**

目前只寫：

```
X, Y, Z...
```

需要正式定義完整 enum。

---

**Q26. scanRange / stepRange / spacing 的 numeric type、range、正負限制？**

例如 spacing 是否一定 > 0？

---

**Q27. Optional keyword 是否可以任意順序？**

例如：

```
FDR ... V 10 TH 5 TT 1 ST 0
```

和：

```
FDR ... ST 0 TT 1 V 10 TH 5
```

是否都合法？

---

**Q28. Optional keyword 可以省略哪些？**

目前只有 `V`、`TH`、`TT`、`ST`、`MP1`、`MP2` 被標成 optional，但沒有說：

- 哪些預設值？
- 某些參數是否互相依賴？
- MP1/MP2 是否必須一起出現？

---

**Q29. Optional keyword 是否可以重複？**

例如：

```
... V 10 V 20
```

應該 INVALID 還是後者覆蓋前者？

---

**Q30. `TT` / `ST` 是否只能接受列出的 enum？**

這一點目前可以高度確定應該是：

```
TT ∈ {0,1}
ST ∈ {0,1,2}
```

但仍應取得正式確認。

---

**Q31. FDR 錯誤中的 `FDG` 是否 typo？**

這個我會列為高優先級文件問題。

# ⚠️ FSM 需要確認的問題

**Q32. FSM 的正式 token spacing？**

同 FDR。

---

**Q33. `TH`、`S`、`V` 是否可以任意順序？**

例如：

```
FSM ... V 10 TH 5 S 20
```

是否合法？

---

**Q34. Optional parameter 是否最多出現一次？**

例如：

```
... TH 5 TH 10
```

怎麼處理？

---

**Q35. `TH` / `S` / `V` 的 numeric type 與 range？**

尤其：

```
S spacing
```

是否必須 > 0？

---

**Q36. FSM 的 `scanAxis` 與 `stepAxis` 是否必須不同？**

例如：

```
FSM ... X ... X ...
```

是否合法？

這屬於很典型的 semantic validation，需要文件明確規定。

### ⚠️ FLM 問題

**Q37. FLM 的 `<name>` 到底是不是 RoutineName？**

因為 Example：

```
FLM M03 2
```

看起來 `M03` 又非常像 Axis，而不是 routine name。

如果 Format 真的是：

```
FLM <name> <scanAxis> <scanRange>
```

那 Example 缺少一個參數。

這是**高優先級文件問題**。

**Q38. FLM Example 是否漏掉 `<name>`？**

例如實際想表達的是：

```
FLM Routine01 M03 2
```

還是 Format 本身寫錯？

---

**Q39. FLM 的 scanAxis 完整合法集合？**

目前只寫 X/Y。

---

**Q40. `TH`、`V` 是否可以任意順序？**

例如：

```
FLM ... V 0.2 TH 255
```

是否與：

```
FLM ... TH 255 V 0.2
```

都合法？

### FDG 問題

**Q41. FDG 的 `<name>` 是否 RoutineName？**

這次看起來是，但仍需統一 Routine 定義。

---

**Q42. scanAxis 與 stepAxis 是否必須不同？**

手冊稱 stepAxis 為 orthogonal step axis，所以很可能不能相同，但需要正式規則。

---

**Q43. TH 的 numeric type / range？**

### ADC? 問題

**Q44. Channel 的合法範圍？**

這一點目前依 AI mapping 可能可以從手冊其他部分找到，但不能自行推定。

---

**Q45. ADC? 不帶 Channel 時，查的是哪個 default channel？**

是由：

```
SAC
```

選擇的 active channel？

還是另一個 system default？

這一點非常重要，因為下一個 SAC 的描述是：

> active ADC channel to be used by ADC? query.
> 

因此目前推測：

```
SAC 3
↓
ADC?
↓
Channel 3
```

但應列為**高可信推論、尚未正式確認**。

### 問題

**Q46. 正式 ADC channel range 是多少？**

例如：

```
0~7
0~15
```

目前不能猜。

### 問題

**Q47. BKN 合法 range？**

例如：

```
BKN -1
```

是否合法？

---

**Q48. BKN 是否允許 `0`？**

---

**Q49. BKN 的實際物理單位？**

手冊只稱 threshold/noise level，沒有說單位。

### 問題

**Q50. BKN? 的 `<Noise>` precision / formatting？**

例如：

```
0.015
```

還是：

```
0.015000
```

Parser 不一定需要限制輸出格式，但如果 CM5 未來要做 response decode，就最好知道。

### WRD 問題

**Q51. 雖然文件寫 decimal，Index 是否允許 hexadecimal？**

這與前面總規則：

> numeric values may be decimal or hexadecimal
> 

存在需要釐清的地方。

---

**Q52. Data 的 signed/unsigned 型態？**

例如：

```
WRD ... 255
```

與：

```
WRD ... -1
```

是否允許？

---

**Q53. Data 是否必須符合 size 的範圍？**

例如：

```
WRD 1 1000 0 1 256
```

如果 size=1：

```
256
```

顯然超過 1 byte。

Parser 還是 Execution Layer 判斷？

這是很重要的 **semantic validation boundary** 問題。

---

**Q54. Index / Subindex 的合法範圍？**

目前沒有給。

### RRD 問題

**Q55. 成功讀取後，實際 data 到底回在哪裡？**

目前 Response 只寫：

```
>Done
```

但如果只回 `Done`：

```
RRD
↓
>Done
```

使用者拿不到讀出的 data。

所以文件必須補充成功 Response。

這是**高優先級問題**。

---

**Q56. RRD 的 data 是不是另外一行？**

例如是否實際為：

```
>RRD <data>
>Done
```

或：

```
>RRD
>DATA ...
>Done
```

目前完全不能猜。

---

**Q57. RRD data 的格式是否依 slave vendor 決定？**

如果是，那 CM5 parser/response decoder 必須允許：

```
opaque / variable response
```

而不能寫死某一種格式。


---

# 問題總整理
---

# INS

INS Q1～Q9：

```
Q1：device type 是否只能 0 / 1
Q2：USB / PCIe 是否目前都支援
Q3：Connect fail. 的正式格式
Q4：Connecting... 是否正式 intermediate response
Q5：失敗時是否也先 Connecting...
Q6：已連線後再次使用不同 device type 怎麼處理
Q7：already connected 是否仍需 Response
Q8：initialization failure 的具體範圍
Q9：connected 的正式 EtherCAT state 定義
```
---
# STP

## STP — 文件規格審查

### ① Format

Manual：

```text
STP
```

目前 Parser：

```cpp
{"STP", REGEX_NO_PARAMETER}
```

其中 `REGEX_NO_PARAMETER` 為：

```text
^\s*$
```

也就是 STP 必須沒有參數。

**文件已明確，沒有問題。**

---

### ② Arguments

Manual 明確寫：

> Arguments: None

**文件已明確，沒有問題。**

---

### ③ Numeric / Sign / Precision

STP 沒有數值參數。

**不適用，沒有問題。**

---

### ④ Parameter relationship

沒有參數，因此不適用。

**文件已明確，沒有問題。**

---

### ⑤ Response

Manual：

```text
>STP
>Done
```

目前 `GMT_Server_Command.cpp`：

```text
>STP\r\n
>Done\r\n
```

內容一致。

**文件已明確，沒有問題。**

---

### ⑥ Response sequence / condition

這裡有一個需要注意的地方。

Manual 寫：

> Response: `>STP` `>Done`

Example 也顯示：

```text
>STP
>Done
```

因此可以合理確認目前文件描述的是**兩段 Response**。

而目前測試 Server 的 `RESPONSE_RULES` 也確實列出了：

```cpp
{
    ">STP\r\n",
    ">Done\r\n"
}
```

但是目前 `GMT_Server_Command.cpp` 的實際測試程式使用：

```cpp
response = GetDefaultResponse(result.command.c_str());
```

而 STP 的 default response 是：

```cpp
">Done\r\n"
```

所以**目前測試 Server 實際只送 `>Done`，並沒有按照 Response Rule 依序送 `>STP` → `>Done`。**

這不是現在要修改程式，而是一個值得確認的**文件 / 測試程式行為差異**。

### STP Q1 — `>STP` 與 `>Done` 是否為兩個實際 Response？

請作者確認：

> STP 執行成功時，是否一定依序回傳兩個獨立 Response：`>STP` → `>Done`？

如果是，還需要確認：

> `>STP` 是 command acceptance / acknowledgement，而 `>Done` 是 command completion response 嗎？

這會直接影響未來 Response handling 的規格。

---

### ⑦ Error condition

Manual 沒有描述任何 STP error response。

這裡我認為**需要作者釐清**，因為 Description 說：

> Immediately stops all axis movements and halts ongoing commands.

但沒有說如果 STP 本身執行失敗、Controller 未初始化、某軸無法停止、或其他硬體/系統異常時怎麼回應。

### STP Q2 — STP 是否存在 Error Response？

請作者確認：

> STP 是否保證一定可以執行成功，因此不需要 Error Response？

或者：

> 如果 STP 執行失敗，是否有正式的 Error Response？請定義完整 Response 格式與觸發條件。

**這是需要作者釐清的文件問題。**

---

### ⑧ 「Immediately」的行為定義

Manual 使用：

> Immediately stops all axis movements

但沒有定義「Immediately」的系統行為或時間要求。

Notes 又說：

> May cause abrupt deceleration depending on current speed and system inertia.

這描述了可能的物理效果，但沒有定義 Controller 層面的執行條件。

因此有一個規格問題：

### STP Q3 — Immediately 的正式定義

請作者確認：

> `Immediately` 是否代表收到 STP 後，Controller 必須立即停止所有軸的 motion command？是否有明確的最大反應時間或 timing requirement？

如果沒有 timing requirement，也請作者明確說明 `Immediately` 僅為功能描述，而非可驗證的 timing specification。

---

### ⑨ 「halts ongoing commands」的範圍

Description：

> halts ongoing commands

這裡「ongoing commands」的範圍不明確。

例如它可能是：

* 目前正在執行的 MOV
* MRV / MSV / MSR
* MPV / MPR
* FRS
* routine
* FLM / FSM / FDR
* 甚至非-motion command

不能自行推定。

### STP Q4 — 「ongoing commands」的範圍

請作者明確定義：

> STP 所謂的 `ongoing commands` 包含哪些 Command？是否只包含 motion / execution commands？例如 MOV、MRV、MSV、MSR、MPV、MPR、FRS、FDR、FSM、FLM、FDG 等是否都會被 STP 中止？

這是文件需要釐清的問題。

---

### ⑩ Emergency Stop 定義

Manual：

> Typically used as an emergency stop

這句容易產生規格上的歧義。

「typically used as an emergency stop」並沒有明確定義 STP 是否就是正式的 **Emergency Stop / E-Stop function**。

### STP Q5 — STP 是否為正式 Emergency Stop？

請作者確認：

> STP 是否只是 software stop / motion stop command，還是正式定義為 Emergency Stop 功能？

如果是正式 Emergency Stop，還需要定義它與硬體 Emergency Stop 的關係及安全功能要求。

---

# STP 審查結果

### 文件已明確，沒有問題

* Format：`STP`
* Arguments：None
* 無數值參數
* 無 parameter relationship
* 基本 Response 文字：`>STP`、`>Done`
* Apply to all axes
* Deceleration 可能受到速度與系統 inertia 影響

### 需要文件作者釐清

**STP Q1**
`>STP` → `>Done` 是否一定是兩個獨立、依序產生的正式 Response？兩者各自代表什麼？

**STP Q2**
STP 是否有 Error Response？如果停止動作失敗，正式 Response 與 condition 是什麼？

**STP Q3**
`Immediately` 是純功能描述，還是有可驗證的 timing requirement？

**STP Q4**
`halts ongoing commands` 中的 ongoing commands 究竟包含哪些 Command？

**STP Q5**
STP 是否是正式 Emergency Stop，還是一般 software/motion stop？如果是 Emergency Stop，其安全功能定義為何？

---

目前 **INS Q1～Q9 + STP Q1～Q5** 繼續累積。

你可以直接貼下一個 Command。

---


# SAH

---

# SAH — Command Manual Specification Review

## ① Format

Manual 定義：

```text
SAH <AxisName>
SAH A
```

並給出：

```text
SAH M01
SAH M01 M02 M03
SAH A
```

目前 Parser：

```cpp
constexpr const char* REGEX_SAH =
    R"(^\s*(?:A|M(?:0?[1-9]|1[0-6])(?:\s+M(?:0?[1-9]|1[0-6]))*)\s*$)";
```

目前實際接受：

```text
A
M1
M01
M2
M02
...
M16
```

以及多個 Motor：

```text
M01 M02
M01 M02 M03
...
```

因此基本 Format 與目前 Parser **大致一致**。

但是有幾個文件細節需要確認。

---

# SAH Q1 — AxisName 的正式格式到底是什麼？

Manual 寫：

> Motor axis name (e.g., M01)

但沒有明確定義：

> AxisName 的完整格式是否固定為 `M01` ～ `M16`？

目前 Parser 允許：

```text
M1
M01
M2
M02
...
M16
```

也就是同時允許一位數與兩位數表示。

但 Manual Example 全部使用：

```text
M01
M02
M03
```

因此需要作者確認：

> `<AxisName>` 是否正式限定為 `M01`～`M16`，必須使用兩位數？還是 `M1`、`M2` 等形式也合法？

這會直接影響 Parser validation。

---

# SAH Q2 — AxisName 的最大範圍是否為 M01～M16？

目前 Parser 明確限制：

```text
M01 ~ M16
```

但 Manual 本身只說：

> Motor axis name (e.g., M01)

沒有明確寫：

> M01–M16

因此需要作者確認：

> 系統正式支援的 AxisName 範圍是否為 `M01`～`M16`？是否可能存在 M17、M18 或其他 AxisName？

目前不能因 Parser 已經限制 M16，就反推文件規格就是 M16。

---

# SAH Q3 — AxisName 是否區分大小寫？

目前 Parser 只接受：

```text
M01
```

不接受：

```text
m01
```

Manual 沒有明確說明 command / axis name 是否 case-sensitive。

需要作者確認：

> `SAH m01` 是否非法？AxisName 是否必須使用大寫 `M`？

同樣也需要確認：

```text
SAH a
```

是否非法，因為目前 Parser 只接受大寫 `A`。

---

# SAH Q4 — 多軸列表是否有數量上限？

Manual 說：

> a space-separated list

以及：

> Calling “SAH A” will sequentially home all axes configured.

但沒有明確寫：

> 一次 SAH 最多可以指定幾個 Axis。

目前 Parser 實際上允許：

```text
M01 M02 M03 ... M16
```

最多可以列出 16 個，但也沒有防止重複。

因此需要作者確認：

> `SAH` 的 AxisName list 是否最多 16 個？是否允許少於全部軸的任意數量？

---

# SAH Q5 — 是否允許重複 AxisName？

目前 Parser 允許例如：

```text
SAH M01 M01
```

因為 Regex 只驗證每一個 token 都是合法 Motor name。

Manual 沒有說明。

需要作者確認：

> 同一個 AxisName 在一次 SAH command 中是否可以重複？例如 `SAH M01 M01` 應該 VALID 還是 INVALID？

---

# SAH Q6 — Axis 順序是否有意義？

Manual 說：

> `SAH A` will sequentially home all axes configured.

但沒有明確定義指定多軸時：

```text
SAH M01 M02 M03
```

是否按照輸入順序：

```text
M01 → M02 → M03
```

執行。

還是 Controller 有自己的固定 homing order。

需要作者確認：

> 多軸 SAH 是否按照 command 中 AxisName 的順序執行？如果不是，正式執行順序由什麼決定？

---

# SAH Q7 — `SAH A` 的「all axes」到底是哪些？

Manual 說：

> home all axes defined in the configuration file

因此目前可以知道不是單純「固定 M01～M16 全部執行」。

但是沒有明確定義 configuration file 中哪些 Axis 算是「configured」。

需要作者確認：

> `SAH A` 的 Axis 集合是否完全由 configuration file 決定？如果 configuration file 只定義 M01、M03、M05，是否只 homing 這三軸？

---

# SAH Q8 — `SAH A` 的執行順序

Manual 明確說：

> sequentially home all axes configured.

但是沒有定義順序。

需要確認：

> `SAH A` 是按照 configuration file 的順序、Axis number 順序（例如 M01 → M02 → M03），還是其他固定順序依次 homing？

這會影響實際行為，因此是正式規格需要定義的項目。

---

# SAH Q9 — `SAH A` 是否要求至少存在一個 configured axis？

Manual 沒有說如果 configuration file 沒有任何 homing axis。

需要作者確認：

> 如果執行 `SAH A` 時 configuration file 沒有任何可 homing 的 Axis，應該回什麼 Response？

例如是否：

```text
>SAH ERR <ErrorCode>
```

以及 ErrorCode 是什麼。

---

# SAH Q10 — Homing configuration 不存在時的行為

Notes 說：

> Each axis must have its homing configuration ... predefined via the SHC command.

但沒有明確說：

如果：

```text
SAH M01
```

而 M01 沒有有效的 SHC configuration，應該怎麼處理。

需要作者確認：

> 如果指定 Axis 沒有 homing configuration，SAH 是否立即回 `>SAH ERR <ErrorCode>`？ErrorCode 是什麼？

---

# SAH Q11 — Group configuration 不完整時如何處理？

Notes 提到：

> group, method, speed, offset, addrOffset

都是 predefined via SHC。

但沒有說這些欄位是否全部 mandatory。

需要作者確認：

> SAH 所需的 homing configuration 中，`group`、`method`、`speed`、`offset`、`addrOffset` 是否全部必須存在且有效？

---

# SAH Q12 — Response `>SAH` 的時機

這是 SAH 最重要的問題之一。

Manual：

```text
Response:
>SAH
or
>SAH ERR <ErrorCode>

When completed, controller will return
>homing end
```

但是 Notes 又寫：

> No response will be returned until all axes have complete homing.

這裡存在**時序上的歧義**。

例如成功時究竟是：

```text
SAH M01
↓
等待 Homing 完成
↓
>SAH
↓
>homing end
```

還是：

```text
SAH M01
↓
>SAH
↓
等待 Homing 完成
↓
>homing end
```

需要作者明確定義：

> `>SAH` 是 homing 開始時的 immediate response，還是所有 homing 完成後才送出的 response？

---

# SAH Q13 — `>homing end` 是否為正式 Response？

Manual 明確寫：

> When completed, controller will return `>homing end`

因此看起來是正式 Response。

但 Response 欄位沒有把它與 `>SAH` / Error response 放在同一個完整 response sequence 中。

需要作者確認：

> 成功完成 Homing 時，正式 Response sequence 是否一定為：
>
> `>SAH`
> → `>homing end`
>
> 兩個獨立 Response？

---

# SAH Q14 — Error Response 的時機

如果 Homing 過程中發生錯誤：

```text
>SAH ERR <ErrorCode>
```

需要確認它是在：

* command validation 時？
* homing 開始前？
* homing 執行中？
* 某一軸失敗後？

需要作者定義：

> `>SAH ERR <ErrorCode>` 對應哪些錯誤階段與錯誤條件？

---

# SAH Q15 — 多軸 Homing 部分成功、部分失敗怎麼處理？

例如：

```text
SAH M01 M02 M03
```

假設：

* M01 成功
* M02 失敗
* M03 尚未執行

需要作者確認：

> Controller 是否立即停止後續 Axis？是否繼續 M03？最終 Response 是什麼？

這是目前文件沒有定義的 execution behavior。

---

# SAH Q16 — `SAH A` 部分 Axis 失敗怎麼處理？

同樣的問題對 `SAH A` 更重要。

例如：

```text
M01 → success
M02 → success
M03 → fail
M04 → 未執行
```

需要確認：

> 發生單一 Axis Homing failure 時，是否停止剩餘 Axis？是否繼續？最終 ErrorCode 如何表示？

---

# SAH Q17 — 「No response」與 Error Response 的關係

Notes：

> No response will be returned until all axes have complete homing.

但如果 homing 失敗，是否代表：

```text
>SAH ERR <ErrorCode>
```

可以在所有 Axis 都完成之前送出？

還是一定要等整個 sequence 結束後才 response？

需要作者確認：

> `No response until all axes have complete homing` 是否只適用成功情況？若中途發生 Error，Error Response 的送出時機是什麼？

---

# SAH Q18 — Homing 中是否允許另一個 Command？

這個 Command 是長時間 operation，而且文件說：

> No response will be returned until all axes have complete homing.

但沒有說 Controller 在 Homing 期間是否接受其他 command。

需要作者確認：

> SAH 執行期間，如果收到其他 Command，例如 `STP`、`MOV`、`SVO`，Controller 應接受、排隊、拒絕，還是只有 STP 可以介入？

這會影響 command interface 的同步行為。

---

# SAH Q19 — STP 是否可以中止 SAH？

這與前一個 STP 的規格也有 cross-command relationship。

如果 SAH 正在執行：

```text
SAH A
```

使用者是否可以：

```text
STP
```

中止 Homing？

如果可以：

> STP 後 SAH 最終 Response 是 `>homing end`、`>SAH ERR ...`，還是其他 Response？

這需要跨 Command 一起定義。

---

# SAH Q20 — Homing configuration 的來源與更新時機

Manual 說：

> predefined via the SHC command.

需要確認：

> `SHC` 設定完成後是否立即可供下一次 SAH 使用？還是需要重新初始化 / 儲存 / reload configuration？

這涉及 SAH 與 SHC 的 dependency。

---

# SAH 最終審查結果

### 文件已明確

* Command：`SAH`
* 可指定單一 Axis
* 可指定多個 Axis
* `A` 是特殊的 all-axis keyword
* Homing configuration 由 SHC 預先設定
* `SAH A` 為 sequential homing
* 成功完成後存在 `>homing end`
* 應用於指定 Axis / configured axes

### 需要文件作者釐清

**SAH Q1～Q20**

其中我認為目前最重要、最直接會影響 Parser / execution / response architecture 的，是：

1. **Q1/Q2：AxisName 的正式格式與範圍**
2. **Q5：是否允許重複 Axis**
3. **Q6/Q8：多軸與 `A` 的執行順序**
4. **Q12/Q13：`>SAH` 與 `>homing end` 的正式 Response sequence**
5. **Q14/Q15/Q16/Q17：Error 與 partial failure 行為**
6. **Q18/Q19：Homing 期間其他 Command / STP 的行為**
7. **Q20：SHC → SAH 的 configuration dependency**

另外，**SAH Q12 是目前文件最明顯的內部描述歧義**：

> `Response: >SAH`
> 與
> `No response will be returned until all axes have complete homing`

這兩句必須由作者明確說明時序，不能由我們自行推定。

目前累積：

**INS Q1～Q9**
**STP Q1～Q5**
**SAH Q1～Q20**

可以繼續貼下一個 Command。

---

# SHC

## 1. Format

文件：

`SHC <AxisName><SlaveIdx><Group><Method><Speed><Offset><AddrOffset>`

範例：

`SHC M01 2 17 400000 40000 0 128`

### 發現的問題

格式描述中各參數之間沒有明確寫出 `<space>`，但 Example 是以空白分隔。

需要確認：

**SHC Q1 — 正式語法是否應明確定義為：**

`SHC <AxisName> <SlaveIdx> <Group> <Method> <Speed> <Offset> <AddrOffset>`

也就是 **7 個參數、以一個或多個空白分隔**？

---

## 2. Arguments

目前文件只說明：

* AxisName：例如 M01
* SlaveIdx：EtherCAT slave index
* Group：Homing group number
* Method：Homing method code
* Speed：pulses/sec
* Offset：Post-switch trigger offset
* AddrOffset：Address alignment offset

### AxisName

目前文件只有 `M01` 範例。

現有 Parser 是：

* `M01 ~ M16`
* 同時也接受 `M1 ~ M16` 這種形式，例如 `M1`

因此需要確認：

**SHC Q2 — `<AxisName>` 是否正式限定為 `M01 ~ M16`，還是 `M1 ~ M16` 也合法？**

另外也需要確認：

**SHC Q3 — AxisName 是否區分大小寫？例如 `m01` 是否應視為非法？**

---

## 3. Numeric / Data Type / Range

這裡是目前 SHC 文件最需要補充的部分。

文件沒有說明：

* SlaveIdx 的範圍
* Group 的範圍
* Method 的合法值
* Speed 的最小/最大值
* Offset 的最小/最大值
* AddrOffset 的最小/最大值
* 是否允許負數
* 是否允許 Hex
* 是否允許小數

目前 Parser 對 SHC 的 6 個數值參數使用：

* 十進位整數
* 也允許 `0x...` / `0X...`
* 允許正負號

例如目前 Parser 理論上會接受：

`SHC M01 -1 17 400000 40000 0 128`

甚至：

`SHC M01 0x02 0x11 0x61A80 0x9C40 0 0x80`

但這不代表產品規格允許，只代表目前 Parser 的語法規則。

因此這一部分需要文件作者明確定義。

### SlaveIdx

**SHC Q4 — `<SlaveIdx>` 的合法範圍是多少？是否只能是非負整數？**

例如：

* `0`
* `1`
* `2`
* 最大值？

另外：

**SHC Q5 — `<SlaveIdx>` 是否允許 Hex？例如 `0x02`？**

---

### Group

**SHC Q6 — `<Group>` 的合法範圍是多少？**

例如 Example 使用：

`17`

但沒有說明 17 是任意整數，還是 Homing group 的特定範圍。

**SHC Q7 — `<Group>` 是否允許 0？是否允許負數？**

**SHC Q8 — `<Group>` 是否允許 Hex？**

---

### Method

文件只說：

`Homing method code (integer)`

這不足以讓 Parser 判斷合法值。

**SHC Q9 — `<Method>` 的所有合法 Homing method code 是哪些？**

例如是否存在：

`0, 1, 2, 3...`

以及每個 code 對應什麼 Homing method？

**SHC Q10 — `<Method>` 是否有固定範圍？是否允許負數？**

**SHC Q11 — `<Method>` 是否允許 Hex？**

---

### Speed

文件定義：

`Homing speed in pulses/sec`

但沒有範圍。

**SHC Q12 — `<Speed>` 是否只能是正整數？**

**SHC Q13 — `<Speed>` 的合法最小值與最大值是多少？**

**SHC Q14 — `<Speed>` 是否允許 0？**

**SHC Q15 — `<Speed>` 是否允許 Hex？**

---

### Offset

文件：

`Post-switch trigger offset (in pulses)`

但沒有定義正負。

**SHC Q16 — `<Offset>` 是否允許負數？**

**SHC Q17 — `<Offset>` 的合法範圍是多少？**

**SHC Q18 — `<Offset>` 是否允許 Hex？**

---

### AddrOffset

文件：

`Address alignment offset`

但沒有資料型態與範圍。

**SHC Q19 — `<AddrOffset>` 是整數、無號整數，還是其他型態？**

**SHC Q20 — `<AddrOffset>` 是否允許負數？**

**SHC Q21 — `<AddrOffset>` 的合法範圍是多少？**

**SHC Q22 — `<AddrOffset>` 是否允許 Hex？**

---

## 4. Parameter Relationships

這個 Command 有一個重要問題：目前文件只定義「儲存參數」，沒有定義參數彼此之間的限制。

例如：

* Group 是否必須存在？
* Method 是否必須與 Group 相容？
* Speed 是否受 Method 限制？
* Offset 是否受 Method 限制？
* AddrOffset 是否必須符合 SlaveIdx？
* AxisName 是否必須對應 SlaveIdx？

因此：

**SHC Q23 — SHC 的各參數之間是否存在相依或交叉驗證規則？如果有，請列出規則。**

例如：

`M01` 是否可以任意指定 `SlaveIdx = 2`？

還是 M01 必須對應某個特定 EtherCAT slave？

---

## 5. Response

文件定義：

成功：

```text
>SHC
>Done
```

失敗：

```text
>SHC ERR [ErrorCode]
```

這裡和 SAH 一樣，存在一個重要問題：**`>SHC` 與 `>Done` 是否是兩個不同時間點的 Response？**

需要確認：

**SHC Q24 — 成功時是否一定依序回傳：**

`>SHC` → `>Done`

還是 `>SHC`、`>Done` 只是兩種可能 Response？

---

## 6. Response Condition / Sequence

文件說：

> Stores homing parameters ... into the persistent configuration file

但沒有說明什麼時候算成功。

例如：

1. 參數檢查成功
2. INI 寫入成功
3. INI 寫入並重新讀取成功

哪一個才觸發 `>Done`？

因此：

**SHC Q25 — `>Done` 的成功條件是什麼？僅代表參數驗證成功，還是必須確認 INI 檔案實際寫入成功？**

另外：

**SHC Q26 — 如果 INI 檔案不存在，SHC 是建立檔案還是回傳 Error？**

**SHC Q27 — 如果 INI 檔案存在但無法寫入，Response 是否為 `>SHC ERR [ErrorCode]`？對應 ErrorCode 是什麼？**

---

## 7. Default Response

目前 `GMT_Server_Command.cpp` baseline：

```text
>Done
```

作為 SHC 的 default response。

這與文件的成功 Response 相符。

因此目前：

### 文件已明確

* SHC 成功至少存在 `>Done`
* baseline default response 是 `>Done`

但仍需要釐清：

**SHC Q28 — `>SHC` 是否為正式的必然 Response？如果是，為什麼它與 `>Done` 都存在？兩者各自代表什麼階段？**

---

## 8. Error Conditions

文件只有：

`>SHC ERR [ErrorCode]`

但沒有定義 ErrorCode。

因此需要：

**SHC Q29 — SHC 可能有哪些 ErrorCode？請提供 ErrorCode 與對應錯誤條件。**

至少需要涵蓋：

* Invalid AxisName
* SlaveIdx 不存在
* Group 不存在/非法
* Method 非法
* Speed 超出範圍
* Offset 非法
* AddrOffset 非法
* INI 寫入失敗

另外：

**SHC Q30 — Parser 層面的格式錯誤，例如參數數量錯誤或非數字，是否應該與 SHC 執行錯誤區分？**

目前 Parser 對這類錯誤會產生：

`Invalid parameters`

而不是：

`>SHC ERR [ErrorCode]`

這兩者的責任邊界需要文件明確定義。

---

## 9. Example

目前唯一 Example：

```text
SHC M01 2 17 400000 40000 0 128
```

這個 Example 可以確認：

* AxisName = M01
* SlaveIdx = 2
* Group = 17
* Method = 400000
* Speed = 40000
* Offset = 0
* AddrOffset = 128

但有一個值得特別注意的地方：

文件定義：

> `<Method>`: Homing method code (integer)

而 Example 中：

`Method = 400000`

這個數值非常大，因此需要確認它確實是 Method code，而不是欄位順序或文件定義有誤。

**SHC Q31 — Example 中的 `400000` 確定是 `<Method>`，而 `40000` 確定是 `<Speed>` 嗎？**

這一點我建議**務必向文件作者確認**，不要由 Parser 反推。

---

# 10. Cross-command / Global Consistency

SHC 與 SAH 有直接依賴關係：

> SHC 儲存 Homing Configuration → SAH 使用 Homing Configuration 執行 Homing

因此有幾個跨 Command 問題。

### SHC → SAH

**SHC Q32 — SHC 成功後，設定是否立即可以被 SAH 使用？**

還是需要：

* reload configuration
* restart
* INS
* 其他初始化動作

---

### Persistent configuration

文件明確說：

> persistent configuration file

因此：

**SHC Q33 — SHC 寫入的設定在 controller 重開機後是否仍然保留並直接使用？**

---

### 一個 Axis 一份設定？

**SHC Q34 — 同一 Axis 重複執行 SHC 時，是覆蓋原設定、建立多筆設定，還是拒絕？**

例如：

```text
SHC M01 2 17 400000 40000 0 128
SHC M01 3 18 500000 50000 0 256
```

第二次是否完全覆蓋第一次？

---

### 與 SAH 的配置要求

SAH 文件說每個 Axis 必須有 Homing configuration。

因此：

**SHC Q35 — SHC 是否是建立/更新 SAH 所需完整 Homing Configuration 的唯一方式？**

---

# 文件已明確，沒有問題

目前可以直接確認：

* Command 名稱：`SHC`
* 用途：設定特定 Axis 的 Homing configuration
* 有 7 個參數
* 設定會保存到 persistent configuration / INI
* SHC 設定會供後續 SAH 使用
* 有成功與錯誤兩類 Response
* Example 提供了一組完整參數

---

# 需要文件作者釐清

這次 SHC 的核心問題集中在 **三個地方**：

### A. 數值規格

目前最大缺口。

需要明確定義：

* SlaveIdx range
* Group range
* Method valid codes
* Speed range
* Offset range/sign
* AddrOffset range/sign
* Decimal / Hex
* 是否允許 0
* 是否允許負數

### B. Response 時序

尤其是：

`>SHC` → `>Done`

究竟是**兩個正式的連續 Response**，還是兩種可能 Response。

### C. Example / 欄位意義

最需要確認的是：

```text
SHC M01 2 17 400000 40000 0 128
     │   │  │    │      │    │  │
     │   │  │    │      │    │  └ AddrOffset
     │   │  │    │      │    └ Offset
     │   │  │    │      └ Speed
     │   │  │    └ Method = 400000 ?
     │   │  └ Group
     │   └ SlaveIdx
     └ Axis
```

**`400000` 作為 Homing Method code 是否正確，是這份文件中特別值得先確認的一點。**

目前我**不會因為現有 Parser 已經接受這些數字，就把它們當成正式規格**。等文件作者回答後，再依回答決定 Parser 的真正限制。

你可以繼續貼下一個 Command，我會用同樣方式往下審查。


---

# SHC? (Query Homing Configuration)

好的，這一個是 **SHC? (Query Homing Configuration)**。我會繼續以目前兩個 baseline 檔案為基準，只做規格比對，**不修改任何程式，也不自行推定文件作者的意思**。

## 1. Format

文件：

`SHC? <AxisName>`

Example：

`SHC? M01`

這部分很明確。

### 文件已明確

* Command 為 `SHC?`
* 需要一個 AxisName
* Query 單一 Axis

### 需要釐清

**SHC? Q1 — `<AxisName>` 是否正式限定為 `M01 ~ M16`？**

目前文件只舉：

* M01
* M08

而現有 Parser 是 M01～M16。

---

**SHC? Q2 — AxisName 是否區分大小寫？**

例如：

```text
SHC? m01
```

是否必須判定為 Invalid？

---

## 2. Arguments

只有：

`<AxisName>`

目前 Parser 與文件方向一致：需要一個 `M01~M16` Axis。

### 需要釐清

**SHC? Q3 — 是否允許 `M1` 這種非兩位數寫法？**

目前 Parser 對 `SHC?` 實際接受的是：

`M01 ~ M16`

**不接受 `M1`。**

這和之前 SAH 的情況不同，因此這裡應由文件正式定義。

---

## 3. Response Format

文件：

```text
>SHC? <AxisName><SlaveIdx><Group><Method><Speed><Offset><AddrOffset>
```

但 Example 是：

```text
>SHC? M01 1 2 17 400000.0 40000 0 5
```

因此實際格式明顯是：

```text
>SHC? M01 1 2 17 400000.0 40000 0 5
       │   │ │  │     │       │     │ │
       │   │ │  │     │       │     │ └ AddrOffset
       │   │ │  │     │       │     └ Offset
       │   │ │  │     │       └ Speed
       │   │ │  │     └ Method
       │   │ │  └ Group
       │   │ └ SlaveIdx
       │   └ AxisName
       └ SHC?
```

### 重要問題

這裡與上一份 SHC 文件存在一個需要確認的地方。

SHC Example：

```text
SHC M01 2 17 400000 40000 0 128
```

SHC? Example：

```text
SHC? M01 1 2 17 400000.0 40000 0 5
```

除了 AxisName 以外，數值完全不同。

因此：

**SHC? Q4 — `SHC? M01` 的 Example 是否只是獨立範例資料，而不是代表先前 SHC Example 寫入後的查詢結果？**

如果它們應該能互相對應，那目前兩份文件的 Example 明顯不一致，需要修正。

---

## 4. Numeric / Data Type

這裡和 SHC 一樣，文件沒有定義回傳數值的資料型態與格式。

尤其 Example 中：

```text
400000.0
```

但 SHC 文件將 `<Method>` 定義為：

`integer`

所以這裡有一個明確需要確認的問題：

**SHC? Q5 — Response 中 `<Method>` 是否一定以整數格式輸出？為什麼 Example 使用 `400000.0`？**

這一點非常重要，因為如果 Method 真的是 integer，正常輸出應該比較像：

```text
400000
```

而不是：

```text
400000.0
```

但我不會自行判定哪一個才是正確格式。

---

### Speed

SHC? Example：

`40000`

SHC 文件定義 Speed 為 pulses/sec。

需要確認輸出格式：

**SHC? Q6 — `<Speed>` Response 是否固定為整數？**

---

### Offset / AddrOffset

同樣需要確認：

**SHC? Q7 — `<Offset>` 與 `<AddrOffset>` Response 是否固定為整數？是否可能輸出負值？**

---

## 5. Parameter Relationships

這個 Command 本身只有 AxisName，因此沒有輸入參數之間的關係問題。

但輸出的六個 configuration fields 應該要與 SHC 的欄位一一對應。

因此：

**SHC? Q8 — SHC? Response 的欄位順序是否與 SHC 的設定順序完全一致？**

也就是固定：

```text
AxisName
SlaveIdx
Group
Method
Speed
Offset
AddrOffset
```

---

## 6. Response Condition

文件說：

> Retrieves the homing configuration for a specified motor axis from the controller’s stored settings.

以及：

> reads from the controller’s persistent settings file

因此需要定義「找不到設定」的行為。

例如：

```text
SHC? M05
```

但 M05 尚未設定。

### 需要釐清

**SHC? Q9 — 如果 Axis 沒有 Homing Configuration，Response 是什麼？**

例如是否：

```text
>SHC? ERR [ErrorCode]
```

或：

```text
>SHC? M05 ...
```

或：

```text
>SHC ERR ...
```

目前文件完全沒有定義。

---

## 7. Error Conditions

文件沒有任何 Error Response。

這會造成一個規格缺口。

至少需要區分：

1. AxisName 格式錯誤
2. Axis 存在但沒有設定
3. Axis 存在但 INI 沒有對應 section/key
4. INI 檔案不存在
5. INI 無法讀取
6. 設定資料格式錯誤

因此：

**SHC? Q10 — SHC? 是否有專用 Error Response？如果有，完整格式及 ErrorCode 為何？**

---

**SHC? Q11 — 如果 persistent configuration file 無法讀取，應回傳什麼？**

---

## 8. Default Response

目前 `GMT_Server_Command.cpp` baseline 中，SHC? 的 Response 是：

```text
>SHC? M01 1 2 17 400000.0 40000 0 5
```

這與文件 Example 完全一致。

所以 baseline 可以確認目前測試伺服器預期的範例格式。

但是要注意：

目前 test server 實際上是透過 `GetDefaultResponse()` **只回傳一個固定 default response**，並不是從 INI 真正查詢資料。

因此這個 baseline **不能證明實際產品的資料讀取行為**。

---

## 9. Example

文件：

```text
SHC? M01
>SHC? M01 1 2 17 400000.0 40000 0 5
```

這裡有一個非常值得確認的地方。

上一份 SHC Example：

```text
SHC M01 2 17 400000 40000 0 128
```

SHC? 卻回：

```text
M01 1 2 17 400000.0 40000 0 5
```

所以不只是格式不同，而是：

* SlaveIdx：2 → 1
* Method：400000 → 400000.0
* AddrOffset：128 → 5

因此這兩份 Example **不能視為同一組設定的 Set → Query 流程**。

### 需要釐清

**SHC? Q12 — SHC 與 SHC? 的 Example 是否應該互相對應？如果是，請統一兩份 Example 的資料。**

---

# 10. Cross-command / Global Consistency

### 與 SHC

SHC：

```text
SHC <AxisName> <SlaveIdx> <Group> <Method> <Speed> <Offset> <AddrOffset>
```

SHC?：

```text
SHC? <AxisName>
```

這個關係非常清楚。

---

### 與 SAH

SHC? 文件最後再次提到：

> Each axis must be pre-configured before issuing SAH.

這與 SAH 文件一致。

因此：

**SHC? Q13 — SHC? 查詢到的設定是否就是 SAH 實際使用的那一份 persistent configuration？**

這是確認資料來源一致性的重要問題。

---

### 與 SHC 更新後的資料

還需要確認：

**SHC? Q14 — 執行 SHC 修改設定後，SHC? 是否立即回傳最新值？**

還是需要重新載入 configuration？

這也與前一題 SHC → SAH 的 reload 行為相關。

---

# 文件已明確，沒有問題

目前可以確定：

* `SHC?` 是 Query command
* 只需要一個 AxisName
* 從 persistent settings 讀取
* 回傳完整 Homing Configuration
* Response 包含 AxisName、SlaveIdx、Group、Method、Speed、Offset、AddrOffset
* Query 結果與 SAH 使用的 Homing Configuration 有關
* Example 提供完整 Response 格式

---

# 需要文件作者釐清

這次我認為最重要的是以下幾項：

### 必須確認

**SHC? Q1** — AxisName 正式範圍是否 M01～M16？

**SHC? Q2** — AxisName 是否大小寫敏感？

**SHC? Q3** — 是否允許 M1，還是一定要 M01？

**SHC? Q4** — SHC 與 SHC? 的 Example 是否應該互相對應？

**SHC? Q5** — Method 為 integer，為什麼 SHC? Response 是 `400000.0`？

**SHC? Q6** — Speed Response 是否固定整數？

**SHC? Q7** — Offset / AddrOffset 是否固定整數、是否允許負值？

**SHC? Q8** — Response 欄位順序是否固定為 SHC 的欄位順序？

**SHC? Q9** — Axis 沒有 Homing Configuration 時的 Response？

**SHC? Q10** — SHC? 是否有 Error Response / ErrorCode？

**SHC? Q11** — INI 不存在或無法讀取時的 Response？

**SHC? Q12** — SHC 與 SHC? 的 Example 是否需要統一？

**SHC? Q13** — SHC? 查詢的設定是否就是 SAH 實際使用的設定？

**SHC? Q14** — SHC 修改後，SHC? 是否立即取得新值，還是需要 reload / reinit？

其中 **Q4/Q12 本質上是同一個問題**，正式整理總問題清單時可以合併成一題，避免重複詢問文件作者。


---

# SVO (Servo ON)

好的，接續審查 **SVO (Servo ON)**。這個 Command 本身很簡單，但「all servo」「already on」「執行失敗」以及 Response 時序仍有幾個需要文件作者明確定義的地方。

---

# SVO 規格審查

## 1. Format

文件：

```text
SVO
```

### 文件已明確

* Command 不帶參數。
* 現有 Parser 的 SVO 規則也是 **無參數**。
* 因此目前一致。

### 需要釐清

**SVO Q1 — `SVO` 是否必須完全大寫？**

例如：

```text
svo
Svo
```

是否都應視為 Invalid？

目前 Parser 的 Command 比對是大小寫敏感，因此這是需要文件定義的，而不是由 Parser 決定。

---

## 2. Arguments

文件：

> Arguments: None

### 文件已明確

SVO 不接受任何參數。

例如：

```text
SVO 1
SVO M01
```

都應屬於參數錯誤。

---

## 3. Servo 範圍

文件描述：

> Enable all servo motors in the system.

這裡的 **all** 需要進一步定義。

### 需要釐清

**SVO Q2 — 「all servo motors」是指所有已配置的 Servo Axis，還是所有 EtherCAT Servo Slave？**

例如系統有：

* M01
* M02
* M03

但 M03 尚未配置，SVO 是否仍然嘗試開啟 M03？

---

**SVO Q3 — 如果某些 Axis 沒有 Servo、不是 Servo type，SVO 是否忽略它們？**

---

**SVO Q4 — SVO 是否會包含未啟用、未配置或不存在的 Axis？**

---

## 4. INS dependency

文件明確說：

> This command must be called after initializing the system via INS.

這代表存在明確的 Command state dependency。

但沒有定義違反順序時的行為。

### 需要釐清

**SVO Q5 — 如果沒有先執行 INS 就執行 SVO，應該回什麼 Response？**

例如：

```text
>SVO ERR [ErrorCode]
```

還是：

```text
>Done.
```

還是其他錯誤訊息？

這一點很重要，因為目前 Parser 只負責**語法**，無法判斷 INS 是否已完成。

---

## 5. Response

文件：

```text
>SVO
>Done.
```

這裡有一個與前面 SHC / SAH 類似的重要問題。

### 需要釐清

**SVO Q6 — 成功時是否一定依序回傳兩個 Response：**

```text
>SVO
>Done.
```

還是 `>SVO` 與 `>Done.` 是兩種可能 Response？

---

### 特別注意句點

目前 baseline `GMT_Server_Command.cpp` 的 SVO response 是：

```text
>SVO
>Done
```

而這份新文件是：

```text
>SVO
>Done.
```

也就是：

**文件：`>Done.`**

**目前 baseline：`>Done`**

這是明確的規格差異。

### 必須詢問

**SVO Q7 — 正式 Response 是 `>Done.` 還是 `>Done`？**

這個問題建議直接記錄，不要讓 Parser / Server 自行決定。

---

## 6. Response condition / timing

文件說：

> This command enables power to all motion axes.

但沒有定義 `>SVO` 和 `>Done.` 各自的時間點。

### 需要釐清

**SVO Q8 — `>SVO` 是代表「收到命令」，還是代表「Servo ON 已開始執行」？**

**SVO Q9 — `>Done.` 是否代表所有 Servo Axis 都已確認進入 Servo ON 狀態？**

例如某一 Axis 尚未完成 Servo ON：

* 是否等待全部完成後才 `>Done.`？
* 還是只要命令送出就 `>Done.`？

---

## 7. Already ON behavior

文件：

> If the servo is already on, calling SVO again has no effect.

這裡「no effect」需要定義 Response。

### 需要釐清

**SVO Q10 — 如果 Servo 已經 ON，再執行 SVO，Response 是否仍然：**

```text
>SVO
>Done.
```

還是有特殊 Response？

---

**SVO Q11 — 如果只有部分 Axis 已經 ON，其他 Axis OFF，SVO 是只開啟 OFF 的 Axis，還是重新對全部 Axis 執行 Servo ON？**

---

## 8. Error conditions

目前文件完全沒有 Error Response。

但實際 Servo ON 可能發生問題，例如：

* EtherCAT 未初始化
* Slave 不存在
* Servo driver fault
* Axis fault
* Servo power 無法開啟
* 某 Axis 無法進入 Servo ON state

因此：

**SVO Q12 — SVO 執行失敗時是否有 Error Response？**

如果有：

**SVO Q13 — Error Response 的正式格式是什麼？是否為 `>SVO ERR [ErrorCode]`？**

---

**SVO Q14 — 如果只有部分 Axis Servo ON 成功、部分失敗，整體 Response 如何表示？**

例如：

```text
M01 ON
M02 ON
M03 FAIL
```

是：

* 整體 ERROR？
* 回傳每個 Axis 的結果？
* 還是只要有一個失敗就 `SVO ERR`？

---

## 9. Movement command dependency

文件：

> It must be executed before any movement-related commands such as MOV, MRV, or MAV.

這裡有兩個問題。

### 第一個：MAV

目前我們已審查／看到的 Command baseline 中，已知有：

* MOV
* MRV
* MSV
* MSR
* MPV
* MPR

但目前 Parser command rule 中沒有 `MAV`。

因此：

**SVO Q15 — 文件中的 `MAV` 是正式 Command 嗎？如果是，請提供 MAV 的完整規格。**

如果只是文件筆誤，也需要確認。

---

### 第二個：movement-related commands 範圍

文件只列：

> MOV, MRV, or MAV

但沒有明確說「哪些 Command 必須在 SVO 後才能執行」。

因此：

**SVO Q16 — 除了 MOV / MRV / MAV 外，MSV、MSR、MPV、MPR 等 Movement commands 是否也必須先 SVO？**

---

## 10. INS → SVO → Movement 的狀態關係

目前可以建立出文件描述的流程：

```text
INS
 ↓
SVO
 ↓
MOV / MRV / ...
```

但每一個狀態的正式條件尚未完全定義。

因此：

**SVO Q17 — SVO 是否必須在 INS 成功完成後才能執行，而不是僅僅「INS 曾經被呼叫過」？**

例如：

```text
INS → Connect fail
SVO
```

這時 SVO 應該拒絕。

---

# 文件已明確，沒有問題

目前可以直接確認：

* `SVO` 無參數。
* 用途是開啟 Servo。
* 目標是系統中的 Servo motors / motion axes。
* SVO 應在 INS 後執行。
* SVO 應在 movement commands 前執行。
* Servo 已 ON 時再次 SVO 不應產生額外效果。

---

# 需要文件作者釐清

整理後，SVO 主要問題如下：

1. **SVO Q1** — Command 是否大小寫敏感？
2. **SVO Q2** — 「all servo motors」具體指哪些 Axis？
3. **SVO Q3** — 非 Servo Axis 如何處理？
4. **SVO Q4** — 未配置/不存在 Axis 如何處理？
5. **SVO Q5** — 未先 INS 就 SVO 的 Response？
6. **SVO Q6** — `>SVO` → `>Done.` 是否為固定兩段 Response？
7. **SVO Q7** — 正式是 `>Done.` 還是 baseline 的 `>Done`？
8. **SVO Q8** — `>SVO` 的語意與發送時機？
9. **SVO Q9** — `>Done.` 是否代表所有 Axis 已實際 Servo ON？
10. **SVO Q10** — 已經 Servo ON 時再次 SVO 的 Response？
11. **SVO Q11** — 部分 Axis ON、部分 OFF 時如何處理？
12. **SVO Q12** — SVO 執行失敗是否有 Error Response？
13. **SVO Q13** — Error Response 格式與 ErrorCode？
14. **SVO Q14** — 部分 Axis 成功、部分失敗時的 Response？
15. **SVO Q15** — `MAV` 是否為正式 Command？
16. **SVO Q16** — MSV/MSR/MPV/MPR 等是否也屬於 SVO dependency？
17. **SVO Q17** — 是否要求 INS **成功完成**後才能 SVO？

其中 **Q6/Q7/Q10** 都涉及 Response 規格，後續整理總問題清單時可以再合併，避免重複詢問文件作者。

---

# SVF (Servo OFF)

好的，接續審查 **SVF (Servo OFF)**。這個 Command 與上一個 SVO 高度對稱，因此我會特別檢查 **SVO/SVF 的狀態關係、Response、DSC dependency，以及「motion commands rejected」的具體規則**。

---

# SVF 規格審查

## 1. Format

文件：

```text
SVF
```

### 文件已明確

* SVF 不帶參數。
* 現有 Parser 也是無參數。
* 與 SVO 的 Command 形式一致。

### 需要釐清

**SVF Q1 — `SVF` 是否正式區分大小寫？**

例如：

```text
svf
Svf
```

是否都應視為 Invalid？

目前 Parser Command 比對是大小寫敏感。

---

# 2. Arguments

文件：

> Arguments: None

### 文件已明確

SVF 不接受參數。

例如：

```text
SVF M01
SVF 1
```

應屬於參數錯誤。

目前與 Parser 一致。

---

# 3. Servo 範圍

文件：

> Disable all servo motors in the system.

與 SVO 一樣，`all servo motors` 的實際範圍需要明確定義。

### 需要釐清

**SVF Q2 — 「all servo motors」是否代表所有已配置的 Servo Axis？**

**SVF Q3 — 如果系統存在非 Servo Axis，SVF 是否忽略？**

**SVF Q4 — 未配置、未連線或不存在的 Axis 是否會被 SVF 處理？**

這些問題最好與 SVO 的定義完全一致。

---

# 4. Response

文件：

```text
>SVF
>Done.
```

目前 `GMT_Server_Command.cpp` baseline 是：

```text
>SVF
>Done
```

因此存在與 SVO 相同的明確差異：

> 文件：`>Done.`
> Baseline：`>Done`

### 必須釐清

**SVF Q5 — 正式 Response 是 `>Done.` 還是 `>Done`？**

---

另外：

**SVF Q6 — `>SVF` 與 `>Done.` 是否一定是兩個依序發送的 Response？**

還是它們只是兩種可能的 Response？

---

# 5. Response timing / condition

文件說：

> This command turns off power to all controlled axes.

但沒有說 `Done` 的判定條件。

### 需要釐清

**SVF Q7 — `>Done.` 是否代表所有 Servo Axis 都已實際進入 Servo OFF 狀態？**

還是只代表 OFF 命令已經送出？

---

**SVF Q8 — 如果某個 Axis 無法 Servo OFF，是否仍然回 `>Done.`？**

如果不是：

**SVF Q9 — 部分 Axis OFF 成功、部分失敗時，Response 如何表示？**

---

# 6. Already OFF behavior

文件：

> If motors are already disabled, this command has no additional effect.

這裡和 SVO 的「already on」完全對稱，但仍需要正式定義 Response。

### 需要釐清

**SVF Q10 — 如果所有 Servo 已經 OFF，再執行 SVF，是否仍然回：**

```text
>SVF
>Done.
```

還是會有特殊 Response？

---

**SVF Q11 — 如果部分 Axis 已 OFF、部分 Axis ON，SVF 是否只關閉 ON 的 Axis，最後仍視為成功？**

---

# 7. Error conditions

文件沒有定義任何 Error Response。

但 Servo OFF 仍可能發生：

* EtherCAT communication failure
* Axis fault
* Servo driver 不允許切換狀態
* Slave disconnected
* Controller 尚未 INS
* 其他 hardware/state error

因此：

**SVF Q12 — SVF 執行失敗時是否存在 Error Response？**

---

**SVF Q13 — 如果有 Error Response，正式格式是否為：**

```text
>SVF ERR [ErrorCode]
```

以及 ErrorCode 的定義為何？

---

# 8. INS dependency

SVO 明確說：

> This command must be called after initializing the system via INS.

SVF 文件沒有寫這個限制。

這造成一個很重要的狀態問題。

例如：

```text
SVF
```

在完全沒有執行 INS 時，到底是否合法？

### 需要釐清

**SVF Q14 — SVF 是否也要求先執行 INS？**

如果需要：

**SVF Q15 — 未 INS 就執行 SVF 時，Response 是什麼？**

如果不需要，也需要確認這是刻意的設計。

---

# 9. DSC dependency

文件：

> It is recommended to execute this command before disconnecting the device (DSC) or shutting down the system.

這裡用了 **recommended**，不是 **must**。

因此需要區分：

* 建議流程
* 強制流程

### 需要釐清

**SVF Q16 — SVF 是否只是建議在 DSC 前執行，還是 DSC 前實際上必須先 SVF？**

---

**SVF Q17 — 如果 Servo 仍然 ON 就執行 DSC，DSC 是否允許執行？**

如果允許，DSC 會：

* 自動 Servo OFF？
* 直接 disconnect？
* 回 Error？

這個問題可能需要在 DSC 文件中一起確認。

---

# 10. Motion command rejection

文件非常明確說：

> When the servo is off, all motion commands will be rejected until SVO is issued again.

這是很重要的狀態規則。

### 需要釐清

**SVF Q18 — 「all motion commands」具體包含哪些 Command？**

目前我們已知：

* MOV
* MRV
* MSV
* MSR
* MPV
* MPR

而 SVO 文件還提到 MAV。

需要確認完整列表。

---

**SVF Q19 — Servo OFF 狀態下，Motion command 的 Response 是什麼？**

例如：

```text
MOV ...
```

會回：

```text
>ERR ...
```

還是：

```text
Invalid ...
```

或某個專用 Servo OFF ErrorCode？

---

**SVF Q20 — Servo OFF 時，Parser 是否仍然將 Motion command 判定為 VALID，只由執行層回報 Servo OFF error？**

這一點對目前架構尤其重要：

> Parser 負責語法，Servo state 屬於執行層。

所以需要文件明確定義兩者的責任邊界。

---

# 11. SVO ↔ SVF 狀態關係

文件現在可以形成：

```text
INS
 ↓
SVO
 ↓
Motion commands
 ↓
SVF
 ↓
Motion commands rejected
 ↓
SVO
 ↓
Motion commands allowed
```

但是「SVO issued again」仍然有一個重要問題。

### 需要釐清

**SVF Q21 — Servo OFF 後，必須重新執行 SVO 才能恢復 Motion command，這個狀態是否只由 Servo ON/OFF 決定？**

例如：

```text
SVO
SVF
SVO
MOV
```

是否一定恢復正常？

還是重新 SVO 後需要等待 EtherCAT/Servo ready 狀態？

---

# 12. 文件已明確，沒有問題

目前可以確認：

* SVF 無參數。
* 用途是關閉系統 Servo。
* 會關閉受控制的 Motion Axis。
* Servo 已 OFF 時，再次 SVF 不產生額外效果。
* Servo OFF 狀態下，Motion commands 應被拒絕。
* 重新 SVO 後才恢復 Motion command。
* 建議在 DSC 或 shutdown 前執行 SVF。

---

# 需要文件作者釐清

整理後，SVF 主要問題：

1. **SVF Q1** — Command 是否大小寫敏感？
2. **SVF Q2** — `all servo motors` 的正式範圍？
3. **SVF Q3** — 非 Servo Axis 如何處理？
4. **SVF Q4** — 未配置/未連線 Axis 如何處理？
5. **SVF Q5** — 正式是 `>Done.` 還是 `>Done`？
6. **SVF Q6** — `>SVF` → `>Done.` 是否為固定 Response sequence？
7. **SVF Q7** — `>Done.` 是否代表所有 Axis 實際 OFF？
8. **SVF Q8** — 單一 Axis OFF 失敗時如何處理？
9. **SVF Q9** — 部分成功/部分失敗時的 Response？
10. **SVF Q10** — 已經全部 OFF 時再次 SVF 的 Response？
11. **SVF Q11** — 部分 ON / 部分 OFF 時如何處理？
12. **SVF Q12** — SVF 是否有 Error Response？
13. **SVF Q13** — Error Response 格式與 ErrorCode？
14. **SVF Q14** — SVF 是否要求先 INS？
15. **SVF Q15** — 未 INS 時執行 SVF 的 Response？
16. **SVF Q16** — DSC 前 SVF 是「建議」還是「必要」？
17. **SVF Q17** — Servo ON 狀態直接 DSC 時如何處理？
18. **SVF Q18** — `all motion commands` 的完整 Command 清單？
19. **SVF Q19** — Servo OFF 時 Motion command 的正式 Error Response？
20. **SVF Q20** — Servo OFF 是 Parser validation 還是 execution-state error？
21. **SVF Q21** — SVF → SVO → Motion 的恢復條件是否還有 Servo/EtherCAT ready 等等待條件？

### 與 SVO 可以合併的問題

後續總表整理時，**SVO/SVF 的 Q1、Servo 範圍、Response 格式、部分成功/失敗、Already ON/OFF、ErrorCode、INS dependency、Motion command state** 很多可以合併成一組「Servo State / SVO-SVF 共通規格」，避免最後問文件作者一堆重複問題。


---

# CAL (Clear Alarm)

好的，接續審查 **CAL (Clear Alarm)**。這個 Command 與前面的 SVO / SVF 有狀態關係，尤其文件提到「emergency stop / fault」「re-enable motion」「CAL 不會重新 Servo ON」，所以需要把這幾個狀態邊界釐清。

# CAL 規格審查

## 1. Format

文件：

```text
CAL
```

### 文件已明確

* CAL 不帶參數。
* 目前 Parser 也是無參數。
* 格式一致。

### 需要釐清

**CAL Q1 — `CAL` 是否區分大小寫？**

例如 `cal`、`Cal` 是否應判定為 Invalid？

目前 Parser Command 比對是大小寫敏感。

---

## 2. Arguments

文件：

> Arguments: None

### 文件已明確

CAL 不接受參數。

例如：

```text
CAL M01
CAL 1
```

應屬於參數錯誤。

---

# 3. 「all alarm or error statuses」的範圍

文件：

> Clear all alarm or error statuses from the motor drivers.

這裡的 **all** 需要明確定義。

### 需要釐清

**CAL Q2 — 「all alarm or error statuses」是指所有 Servo Axis / motor driver 嗎？**

還是只清除目前有 fault 的 Axis？

---

**CAL Q3 — CAL 是否會清除 Controller 本身的 Error 狀態，還是只清除 motor driver 的 Alarm？**

這一點尤其重要，因為文件後面又提到：

> use ERR? to check remaining fault codes.

因此需要知道 CAL 與 `ERR?` 的資料來源是否完全相同。

---

# 4. Alarm 與 Error 的定義

文件把兩者寫在一起：

> alarm or error statuses

但沒有定義差異。

### 需要釐清

**CAL Q4 — Alarm 與 Error 在系統中是否是兩種不同狀態？如果是，CAL 是否兩者都清除？**

---

# 5. Emergency Stop / Fault

文件：

> This is typically required after an emergency stop or fault event.

這裡的「emergency stop」需要特別注意，因為前面的 STP 也有 Stop 行為。

### 需要釐清

**CAL Q5 — 文件中的 emergency stop 是指 STP Command，還是實體 Emergency Stop / E-stop？**

如果兩者都包含：

**CAL Q6 — CAL 對 STP 與實體 Emergency Stop 所造成的狀態是否有不同處理？**

---

# 6. Response

文件：

```text
>CAL
>Done.
```

目前 baseline `GMT_Server_Command.cpp` 是：

```text
>CAL
>Done
```

因此又出現和 SVO/SVF 相同的句點差異。

### 必須釐清

**CAL Q7 — 正式 Response 是 `>Done.` 還是 `>Done`？**

---

另外：

**CAL Q8 — `>CAL` → `>Done.` 是否為固定的兩段 Response sequence？**

還是 `>CAL` / `>Done.` 只是可能的 Response？

---

# 7. Response timing

CAL 真正完成的條件沒有定義。

例如：

```text
CAL
```

可能需要對多個 motor driver 發送 Clear Fault。

### 需要釐清

**CAL Q9 — `>Done.` 是否代表所有 motor driver 都已確認 Alarm/Error 清除？**

還是只代表 Clear Alarm command 已經送出？

---

**CAL Q10 — 如果部分 Axis 清除成功、部分 Axis 清除失敗，Response 如何表示？**

---

# 8. Error conditions

文件沒有定義 CAL 失敗時的 Response。

但實際上可能發生：

* EtherCAT 尚未初始化
* Driver 不存在
* Driver communication failure
* Fault condition 無法 clear
* Hardware issue 尚未解除
* 某 Axis 無法接受 Clear Alarm

### 需要釐清

**CAL Q11 — CAL 執行失敗是否有 Error Response？**

---

**CAL Q12 — 如果有，正式格式是否為：**

```text
>CAL ERR [ErrorCode]
```

以及有哪些 ErrorCode？

---

# 9. 「If errors persist after CAL」

文件：

> If errors persist after CAL, use ERR? to check remaining fault codes.

這句話表示 CAL **不保證所有錯誤都一定可以被清除**。

因此需要定義：

**CAL Q13 — CAL 成功的定義是「Clear command 執行成功」，還是「所有 Alarm/Error 已經消失」？**

這兩個定義會直接影響 `>Done.` 的意義。

---

# 10. CAL 與 ERR?

這是非常重要的跨 Command 關係。

文件說：

> use ERR? to check remaining fault codes.

因此可以理解成：

```text
CAL
 ↓
ERR?
```

但 `ERR?` 的文件需要進一步確認才能知道完整規則。

目前 baseline 的 ERR? response 包含：

```text
>ERR : M1 2048
>ERR : M5 8192
>No Error
```

因此需要確認：

**CAL Q14 — CAL 清除的 Alarm/Error，是否就是 ERR? 所查詢的那些 fault codes？**

---

# 11. CAL does not re-enable servo

文件非常明確：

> CAL does not re-enable the servo; use SVO afterward if needed.

這與前面的 SVF/SVO 狀態關係一致。

目前可以建立：

```text
SVF
 ↓
CAL
 ↓
SVO
 ↓
Motion
```

但還需要確認：

**CAL Q15 — 如果 Servo 已經 OFF，執行 CAL 後 Servo 是否保持 OFF？**

依照文件文字應該是保持 OFF，但正式規格仍建議確認。

---

## 12. CAL 是否需要 SVO？

文件沒有說 CAL 必須先 SVO。

這點很重要。

### 需要釐清

**CAL Q16 — Servo OFF 狀態下是否允許執行 CAL？**

例如：

```text
SVF
CAL
```

是否合法？

---

**CAL Q17 — CAL 是否要求先 INS？**

文件沒有像 SVO 那樣明確寫「must be called after INS」。

因此需要確認。

---

# 13. CAL 與 Motion command

文件：

> Use this command after resolving hardware issues or emergency stops to re-enable motion.

但後面又說：

> CAL does not re-enable the servo; use SVO afterward if needed.

所以「re-enable motion」實際上可能是：

```text
CAL
+
SVO
```

而不是 CAL 單獨恢復 Motion。

### 需要釐清

**CAL Q18 — CAL 單獨執行後，Servo OFF 狀態下 Motion command 是否仍然被拒絕？**

依目前文件應該是，但最好正式確認。

---

# 14. Error persistence

還有一個重要的狀態問題。

例如：

```text
CAL
ERR?
```

如果仍有 Error：

```text
>ERR : M1 2048
```

那麼：

**CAL Q19 — CAL 後仍存在 Error 時，是否允許直接 SVO？**

還是必須先處理所有 remaining faults？

---

# 文件已明確，沒有問題

目前可以確認：

* CAL 無參數。
* 用途是清除 motor driver 的 Alarm/Error status。
* 常用於 fault / emergency stop 後。
* CAL 不會自動 Servo ON。
* 必要時必須另外執行 SVO。
* CAL 後仍存在錯誤時，可以使用 ERR? 查詢。
* CAL 的 Response 規格文件寫為 `>CAL`、`>Done.`。

---

# 需要文件作者釐清

整理後：

1. **CAL Q1** — Command 是否大小寫敏感？
2. **CAL Q2** — `all alarm/error statuses` 的正式範圍？
3. **CAL Q3** — 是否只清除 motor driver，不清除 Controller Error？
4. **CAL Q4** — Alarm 與 Error 是否為不同狀態？CAL 是否兩者都清？
5. **CAL Q5** — emergency stop 是 STP 還是實體 E-stop？
6. **CAL Q6** — STP 與實體 E-stop 後的 CAL 行為是否相同？
7. **CAL Q7** — 正式是 `>Done.` 還是 `>Done`？
8. **CAL Q8** — `>CAL` → `>Done.` 是否固定兩段 Response？
9. **CAL Q9** — `>Done.` 的完成條件？
10. **CAL Q10** — 部分 Axis 清除成功、部分失敗時如何回應？
11. **CAL Q11** — CAL 是否有 Error Response？
12. **CAL Q12** — Error Response 格式與 ErrorCode？
13. **CAL Q13** — CAL 的成功定義是 command 成功執行，還是所有 Alarm/Error 都消失？
14. **CAL Q14** — CAL 清除的 fault 是否就是 ERR? 查詢的 fault？
15. **CAL Q15** — Servo OFF 時 CAL 是否維持 Servo OFF？
16. **CAL Q16** — Servo OFF 狀態是否允許 CAL？
17. **CAL Q17** — CAL 是否必須先 INS？
18. **CAL Q18** — CAL 後未 SVO 時 Motion 是否仍然拒絕？
19. **CAL Q19** — CAL 後仍有 fault 時是否允許 SVO？

其中 **Q15/Q16/Q18** 可以在最後整理成「CAL 與 Servo State」共通問題；**Q7/Q8** 則可與 SVO/SVF/SHC 的 `>Done` 格式與多段 Response 問題合併。

---

# ERR? (Query Error Status)

好的，接續審查 **ERR? (Query Error Status)**。這個 Command 與剛剛的 **CAL、SVO、SVF** 有直接關聯，因此我會特別檢查「Error 的來源、回傳格式、M1/M5 的定義、No Error 條件，以及多筆 Error 的順序」。

# ERR? 規格審查

## 1. Format

文件：

```text id="x9f7jt"
ERR?
```

### 文件已明確

* Command 為 `ERR?`
* 不帶參數。
* 與目前 Parser 一致。

### 需要釐清

**ERR? Q1 — `ERR?` 是否區分大小寫？**

例如：

```text id="6l8h0f"
err?
Err?
```

是否都應判定為 Invalid？

目前 Parser Command 比對是大小寫敏感。

---

# 2. Arguments

文件：

> Arguments: None

### 文件已明確

ERR? 不接受任何參數。

例如：

```text id="50pnor"
ERR? M01
ERR? 1
```

應屬於參數錯誤。

---

# 3. Error Status 的範圍

文件說：

> current error or alarm codes from all motor axes and I/O modules

這裡比先前 CAL 的描述更廣：

* Motor axes
* I/O modules

### 需要釐清

**ERR? Q2 — `all motor axes and I/O modules` 的正式範圍是什麼？**

例如是否包含：

* M01～M16
* EtherCAT I/O modules
* 其他非 EtherCAT I/O
* Controller 自身 Error

---

**ERR? Q3 — ERR? 是否會回傳 Controller/System-level error，還是只回傳 Axis / I/O module error？**

---

# 4. Response format

文件範例：

```text id="c1q6t4"
>ERR : M1 2048
>ERR : M5 8192
```

或者：

```text id="p0t4oc"
>No Error
```

目前 baseline 的 `GMT_Server_Command.cpp` 也是這三種形式：

```text id="3dl8b0"
>ERR : M1 2048
>ERR : M5 8192
>No Error
```

因此 baseline 與文件一致。

但是這裡有幾個重要規格問題。

---

# 5. `M1` 的意義

文件說：

> Each message indicates which motor group and axis triggered the error.

Example：

```text id="2px99e"
>ERR : M1 2048
```

這裡的 `M1` 究竟代表：

* Axis M01？
* Motor group 1？
* Group 1 + 某個 Axis？
* Driver-specific identifier？

### 必須釐清

**ERR? Q4 — Response 中的 `M1` 是 Axis 名稱 `M01` 的另一種表示法，還是 Motor Group ID？**

---

**ERR? Q5 — 為什麼 SHC / SAH 使用 `M01`，而 ERR? Response 使用 `M1`？**

是否正式規定：

```text
M01 → Command input
M1  → ERR? output
```

還是只是 Example 的寫法？

---

# 6. Error code

文件：

> Error codes correspond to driver-specific status registers.

這表示 `2048`、`8192` 是 Driver-specific code。

但目前沒有說明資料格式。

### 需要釐清

**ERR? Q6 — ErrorCode 是十進位整數輸出，還是可能輸出 Hex？**

例如：

```text
2048
```

是否也可能：

```text
0x0800
```

---

**ERR? Q7 — ErrorCode 是否允許負數？**

通常看起來不像，但規格目前沒有正式定義。

---

# 7. Error code interpretation

文件說：

> refer to driver manual if needed.

這表示 ERR? 本身可能不需要理解 ErrorCode。

### 文件已明確

ERR? 的工作是**讀取並回傳目前 Error/Alarm code**，而不是解析 Driver-specific code 的意義。

### 需要釐清

**ERR? Q8 — ERR? 是否只原樣回傳 Driver status register value，不負責轉換或解碼 ErrorCode？**

如果是，這應該成為正式規格。

---

# 8. Multiple errors

Example 有兩筆：

```text id="9ecx0r"
>ERR : M1 2048
>ERR : M5 8192
```

這代表 ERR? 可以有多筆 Response。

但沒有定義數量與順序。

### 需要釐清

**ERR? Q9 — 一次 ERR? 最多可能回傳幾筆 Error message？**

---

**ERR? Q10 — 多筆 Error 的輸出順序是什麼？**

例如：

* Axis 編號順序
* 發生時間順序
* Driver 回傳順序
* 固定掃描順序

---

# 9. No Error condition

文件：

> `>No Error if all axes are normal.`

但前面又說 ERR? 查詢：

> all motor axes and I/O modules

所以這裡的：

> all axes are normal

範圍似乎比前面窄。

### 需要釐清

**ERR? Q11 — 只有所有 Motor Axis 正常，但 I/O module 有 Error 時，應該回 `>No Error` 還是 I/O Error？**

我認為這是文件中很值得直接詢問作者的矛盾點。

---

# 10. No Error exact format

目前 baseline：

```text id="8ybvxm"
>No Error
```

文件也是：

```text id="sgrgqh"
>No Error
```

這部分一致。

### 需要釐清

**ERR? Q12 — `>No Error` 是否為唯一的「無任何 Error」正式 Response？**

例如是否可能是：

```text
>No Error.
```

或其他格式？

---

# 11. Response timing

文件沒有說 ERR? 是立即查詢，還是需要等待所有 driver 回覆。

### 需要釐清

**ERR? Q13 — `ERR?` 是否為同步查詢，必須等所有 Axis / I/O module 查詢完成後才回 Response？**

---

**ERR? Q14 — 如果某一 Driver 暫時無法通訊，ERR? 是回該 Driver 的 communication error，還是直接忽略該 Axis？**

---

# 12. CAL relationship

文件：

> To clear alarms, use CAL

這表示：

```text id="fjhjkp"
ERR?
 ↓
發現 Error
 ↓
CAL
 ↓
ERR?
```

這個流程與前面 CAL 文件一致。

### 需要釐清

**ERR? Q15 — CAL 成功後，ERR? 是否一定應該回 `>No Error`？**

如果硬體 Fault 尚未排除，文件前面已說 CAL 後 Error 可能 persist，所以這裡應該是：

* CAL 成功 ≠ 一定 No Error

但需要文件作者正式確認這個狀態定義。

---

# 13. SVO relationship

文件：

> To resume operation, re-enable with SVO.

因此目前流程可以理解為：

```text id="50bxr0"
ERR?
 ↓
CAL
 ↓
ERR?
 ↓
SVO
 ↓
Motion
```

### 需要釐清

**ERR? Q16 — 如果 ERR? 仍有 Error，是否禁止 SVO？**

還是 SVO 可以執行，但 Servo ON 可能失敗？

這個問題其實是 **ERR? / CAL / SVO 三者的狀態責任邊界**。

---

# 14. I/O modules 與 motor axes 的 Response identifier

這是目前文件比較大的缺口。

文件說 ERR? 同時查：

> motor axes and I/O modules

但範例全部是：

```text
M1
M5
```

沒有 I/O module 的 Response example。

### 必須釐清

**ERR? Q17 — I/O module 的 Error Response 格式是什麼？**

例如是否：

```text
>ERR : IO1 2048
```

還是：

```text
>ERR : I01 2048
```

或其他格式？

---

**ERR? Q18 — Motor Axis 與 I/O module 是否使用不同的 identifier 格式？**

---

# 15. Error message vs Error code

文件一開始說：

> List of error messages per axis

但 Example 實際上是：

```text
>ERR : M1 2048
```

這比較像：

**Error identifier + Error code**

而不是完整 Error message。

### 需要釐清

**ERR? Q19 — Response 中的 `2048` 是純 ErrorCode，還是已經是完整的 Error message identifier？**

如果只是 code，是否需要由 End User / 上層程式再查 Driver manual 解碼？

---

# 16. 文件已明確，沒有問題

目前可以確認：

* `ERR?` 無參數。
* 用於查詢目前 Error / Alarm status。
* 查詢範圍包含 Motor axes 與 I/O modules。
* 可以有多筆 Error。
* 沒有 Error 時回 `>No Error`。
* Error response 至少包含 identifier + error code。
* ErrorCode 與 Driver-specific status register 對應。
* CAL 用於清除 Error。
* SVO 用於恢復 Servo / operation。

---

# 17. 需要文件作者釐清

整理後，我建議正式問題先保留這些：

1. **ERR? Q1** — Command 是否大小寫敏感？
2. **ERR? Q2** — `all motor axes and I/O modules` 的正式範圍？
3. **ERR? Q3** — 是否包含 Controller/System-level Error？
4. **ERR? Q4** — Response 中的 `M1` 是 Axis 還是 Motor Group？
5. **ERR? Q5** — 為何其他 Command 用 M01，而 ERR? 用 M1？
6. **ERR? Q6** — ErrorCode 是十進位還是可 Hex？
7. **ERR? Q7** — ErrorCode 是否允許負數？
8. **ERR? Q8** — ERR? 是否原樣回傳 Driver register value，不解碼？
9. **ERR? Q9** — 一次最多幾筆 Error？
10. **ERR? Q10** — 多筆 Error 的排列順序？
11. **ERR? Q11** — Motor Axis 正常但 I/O 有 Error 時是否 `No Error`？
12. **ERR? Q12** — `>No Error` 是否唯一正式無錯誤 Response？
13. **ERR? Q13** — ERR? 是否等待所有 Axis/I/O 查詢完成？
14. **ERR? Q14** — Driver 無法通訊時如何表示？
15. **ERR? Q15** — CAL 後 ERR? 是否只有在真正沒有 fault 時才回 No Error？
16. **ERR? Q16** — ERR? 有 Error 時是否禁止 SVO，還是由 SVO 自己判斷？
17. **ERR? Q17** — I/O module 的 Error Response 格式？
18. **ERR? Q18** — Axis / I/O 是否使用不同 identifier？
19. **ERR? Q19** — `2048` 是 ErrorCode 還是 Error message identifier？

其中 **Q4/Q5**、**Q6/Q7**、**Q17/Q18** 後續都可以合併，避免最後給文件作者過多重複問題。

另外，這一份 ERR? 文件目前和 **CAL / SVO / SVF** 的關聯已經很明確；等這四個 Command 全部確認後，可以把它們整理成一個「**Servo / Alarm / Error 狀態機規格問題**」，會比逐條問更有效率。

---

# DSC (Disconnect Device)

好的，接續審查 **DSC (Disconnect Device)**。這一個與前面的 **INS、SVO、SVF、CAL** 有非常直接的狀態關係，尤其是「DSC 後哪些 Command 失效」需要定義清楚。

# DSC 規格審查

## 1. Format

文件：

```text
DSC
```

### 文件已明確

* DSC 不帶參數。
* 目前 Parser 也是無參數。
* 格式一致。

### 需要釐清

**DSC Q1 — `DSC` 是否區分大小寫？**

例如：

```text
dsc
Dsc
```

是否都應判定為 Invalid？

目前 Parser Command 比對是大小寫敏感。

---

# 2. Arguments

文件：

> Arguments: None

### 文件已明確

DSC 不接受任何參數。

例如：

```text
DSC M01
DSC 1
```

應屬於參數錯誤。

---

# 3. Disconnect 的實際範圍

文件：

> Terminate connection with the EtherCAT master and release all related resources.

這裡的「all related resources」範圍不夠明確。

### 需要釐清

**DSC Q2 — DSC 實際會釋放哪些資源？**

例如是否包含：

* EtherCAT master
* EtherCAT slave state
* PDO / process data
* 通訊 buffer
* driver resources
* configuration resources
* Servo state

---

# 4. Response

文件：

```text
>Disconnected.
```

目前 `GMT_Server_Command.cpp` baseline：

```text
>Disconnected
```

因此存在明確差異：

> 文件：`>Disconnected.`
> Baseline：`>Disconnected`

### 必須釐清

**DSC Q3 — 正式 Response 是 `>Disconnected.` 還是 `>Disconnected`？**

---

## 5. Response timing

文件只說：

> Terminate connection ... and release all related resources.

沒有定義 `>Disconnected.` 代表什麼。

### 需要釐清

**DSC Q4 — `>Disconnected.` 是否代表 EtherCAT master 已經實際停止、所有資源已釋放完成？**

還是只代表 DSC request 已接受？

---

# 6. Disconnect failure

文件沒有定義失敗 Response。

例如：

* EtherCAT master 尚未 INS
* Master 已經 disconnect
* Resource release failure
* EtherCAT communication failure

### 需要釐清

**DSC Q5 — DSC 執行失敗時是否有 Error Response？**

如果有：

**DSC Q6 — Error Response 的正式格式與 ErrorCode 是什麼？**

例如是否：

```text
>DSC ERR [ErrorCode]
```

---

# 7. Already disconnected

文件沒有描述重複執行 DSC。

例如：

```text
INS
DSC
DSC
```

### 需要釐清

**DSC Q7 — 如果系統已經 disconnected，再執行 DSC，是否視為成功並回 `>Disconnected.`？**

還是應該回 Error？

---

# 8. INS relationship

文件：

> After disconnecting, all servo and motion commands become invalid until a new INS is issued.

這是一個很重要的 state rule。

目前流程可理解為：

```text
INS
 ↓
DSC
 ↓
Servo / Motion commands invalid
 ↓
INS
 ↓
恢復
```

但「invalid」需要明確定義。

### 需要釐清

**DSC Q8 — DSC 後 SVO、SVF、CAL 等 Servo-related commands 是否也全部禁止？**

文件只寫：

> all servo and motion commands

所以至少 SVO/SVF 應該包含，但 CAL 是否包含需要確認。

---

**DSC Q9 — DSC 後 ERR? 是否仍然可以執行？**

ERR? 是 Query command，不是 Servo / Motion command。

因此需要確認它是否：

* 仍可查詢
* 直接拒絕
* 回 `No Error`
* 回 disconnected error

---

# 9. 「all servo and motion commands」

這裡與前面 SVF 文件存在跨 Command 規格問題。

目前已知：

* SVO
* SVF
* CAL
* MOV
* MRV
* MSV
* MSR
* MPV
* MPR

另外 SVO 文件提過 `MAV`。

### 需要釐清

**DSC Q10 — 「all servo and motion commands」的完整 Command 清單是什麼？**

尤其需要確認：

* SVO
* SVF
* CAL
* MOV
* MRV
* MSV
* MSR
* MPV
* MPR
* MAV

哪些屬於 DSC 後禁止的 Command？

---

# 10. Command Parser vs execution state

這一點對目前架構非常重要。

目前 Parser 的工作是判斷：

> Command syntax 是否合法。

但 DSC 文件說 DSC 後：

> all servo and motion commands become invalid

這比較像 **Controller execution state**，而不是 syntax invalid。

### 需要釐清

**DSC Q11 — DSC 後的「invalid」是指 Parser 層的 Invalid，還是 Command syntax 仍 VALID、但執行層拒絕？**

例如：

```text
DSC
SVO
```

`SVO` 的格式本身仍然正確，但 controller 尚未 INS。

這會影響最終 TCP Response 的責任分層。

---

# 11. Re-INS condition

文件：

> until a new INS is issued.

但「issued」和「INS successful」不是同一件事。

例如：

```text
DSC
INS 0
```

但 INS 失敗。

### 需要釐清

**DSC Q12 — DSC 後是否只要重新執行 INS 就解除 disconnected state，還是必須 INS 成功完成後才恢復 Servo/Motion commands？**

這一點也應與之前 INS 的 Q1～Q9 一起確認。

---

# 12. SVF before DSC

文件：

> Recommended to call SVF before DSC to safely power down servos.

注意這裡使用：

> Recommended

而不是：

> Must

### 需要釐清

**DSC Q13 — DSC 前 SVF 是「建議」還是實際上的必要條件？**

---

**DSC Q14 — 如果 Servo ON 狀態直接執行 DSC，DSC 是否仍允許？**

如果允許：

* DSC 是否自動 Servo OFF？
* 是否直接斷開？
* 是否產生 Warning/Error？

---

# 13. Motion ongoing during DSC

這是非常重要但文件沒有說明的情況。

例如：

```text
SVO
MOV ...
DSC
```

如果 Axis 正在運動，DSC 是否允許直接 disconnect？

### 需要釐清

**DSC Q15 — 如果有 Motion 正在執行時呼叫 DSC，系統會如何處理？**

例如：

1. 拒絕 DSC
2. 先 Stop 再 Disconnect
3. 直接 Disconnect
4. 回 Error

這和前面的 STP / SVF 有直接關係。

---

# 14. CAL / Alarm 狀態

如果 Servo 有 Alarm：

```text
CAL
```

是否需要在 DSC 前先 CAL？

文件只推薦：

`SVF → DSC`

沒有提 CAL。

### 需要釐清

**DSC Q16 — DSC 是否要求 Servo 必須 OFF，但不要求 Alarm 必須清除？**

---

# 15. 文件已明確，沒有問題

目前可以確認：

* DSC 無參數。
* 用途是終止 EtherCAT master connection。
* 會釋放相關資源。
* 建議在 shutdown / restart communication 前執行。
* DSC 後 Servo / Motion commands 不可使用。
* 需要新的 INS 才能恢復。
* 文件建議 DSC 前先 SVF。

---

# 16. 需要文件作者釐清

整理後：

1. **DSC Q1** — Command 是否大小寫敏感？
2. **DSC Q2** — 「all related resources」具體包含哪些資源？
3. **DSC Q3** — 正式 Response 是 `>Disconnected.` 還是 `>Disconnected`？
4. **DSC Q4** — `>Disconnected.` 的完成條件？
5. **DSC Q5** — DSC 是否有 Error Response？
6. **DSC Q6** — Error Response 格式與 ErrorCode？
7. **DSC Q7** — 已經 disconnected 時再次 DSC 的 Response？
8. **DSC Q8** — DSC 後 SVO/SVF/CAL 是否全部禁止？
9. **DSC Q9** — DSC 後 ERR? 是否仍可執行？
10. **DSC Q10** — 「all servo and motion commands」完整清單？
11. **DSC Q11** — DSC 後的 invalid 是 Parser invalid 還是 execution-state rejection？
12. **DSC Q12** — 重新 INS 是「issued」即可恢復，還是必須 INS 成功？
13. **DSC Q13** — DSC 前 SVF 是建議還是必要？
14. **DSC Q14** — Servo ON 時直接 DSC 如何處理？
15. **DSC Q15** — Motion 正在執行時 DSC 如何處理？
16. **DSC Q16** — DSC 是否要求沒有 Alarm，還是只要求 Servo OFF？

---

### 目前累積的跨 Command 重點

到目前 **INS → STP → SAH → SHC → SHC? → SVO → SVF → CAL → ERR? → DSC**，已經開始出現一組共同的規格問題：

```text
INS
 │
 ├─ 初始化成功/失敗
 │
 ↓
SVO ←── CAL / ERR?
 │
 ↓
Motion
 │
 ↓
SVF
 │
 ↓
DSC
 │
 ↓
必須重新 INS
```

其中 **「Controller State 到底有哪些正式狀態，以及每個 Command 在各狀態下允不允許執行」**，後面會是很重要的總規格問題。

我先不把它提前定義成 State Machine，因為目前文件還沒有提供足夠資訊；等後續 Command 文件全部審查完，再把重複問題合併。


---

# VLS (Set System Speed)

以下依照目前已確認的 **Command Parser + `GMT_Server_Command.cpp` response baseline**，只做規格審查，**不修改任何程式碼**。

# VLS（Set System Speed）規格審查

## 1. Format

文件：

```text
VLS <value>
```

### 已明確

* Command 名稱為 `VLS`
* 需要 1 個參數 `<value>`
* 格式與目前 parser 的 VLS 規則一致：一個浮點數。

### 需要文件作者釐清

**Q1：`<value>` 的正式語法是否允許以下形式？**

目前文件只說 Floating point number，但沒有定義：

* `0.15`
* `.15`
* `1.0`
* `1`
* `-0.15`
* `+0.15`
* `1.`
* 科學記號，例如 `1e-3`

目前 parser 的 VLS 是一般十進位浮點格式，**不接受科學記號**。需要確認 Manual 的正式 numeric syntax。

---

# 2. Arguments

文件：

> `<value>`: Floating point number representing the system speed

並給例：

> `0.15 = normal speed, 0.3 = twice the speed`

### 已明確

* `<value>` 是浮點數。
* `0.15` 為一個範例值。
* 文件表示 `0.3` 相對於 `0.15` 是兩倍。

### 需要文件作者釐清

**Q2：`<value>` 的合法數值範圍是多少？**

目前完全沒有寫：

* 最小值
* 最大值
* 是否允許 `0`
* 是否允許負值

這是重要問題，因為「system speed」通常需要明確定義合法範圍。

---

**Q3：`0.15` 是否代表固定的實際速度比例（15%），還是僅為某種 system speed 參數值？**

文件說：

> `0.15 = normal speed, 0.3 = twice the speed`

需要確認正式定義是否為：

```text
1.0 = 100% system speed
0.15 = 15%
0.30 = 30%
```

還是 `0.15` 只是系統內部定義的速度係數。

---

**Q4：`<value>` 是否允許 `0`？**

如果允許，`VLS 0` 的意義需要定義，例如：

* 所有 motion 的速度變成 0？
* 等同禁止 motion？
* 合法但 motion 無法執行？
* 應視為 Invalid parameters？

目前文件沒有說明。

---

**Q5：`<value>` 是否允許負值？**

如果不允許，需要明確寫出，例如：

```text
0 < value <= ...
```

如果允許，則需要說明負速度的意義。

---

**Q6：數值精度是否有限制？**

例如：

```text
VLS 0.15
VLS 0.150000
VLS 0.1500001
```

是否都合法？

是否有固定 decimal places 或最小解析度？

---

# 3. Parameter Relationships

文件說：

> affects all motion commands including MOV, MRV.

### 已明確

* VLS 是 global system speed。
* 至少明確提到會影響 `MOV`、`MRV`。

### 需要文件作者釐清

**Q7：VLS 實際影響哪些 motion commands？**

文件寫「all motion commands including MOV, MRV」，但目前專案還有：

```text
MOV
MRV
MSV
MSR
MPV
MPR
```

以及 Manual 其他地方曾出現 `MAV`。

需要正式定義 **所有受 VLS 影響的 commands 清單**。

---

**Q8：VLS 是否會影響個別 motion command 自己指定的速度參數？**

例如 `MOV` 本身具有 motion parameters。

需要明確定義：

```text
effective speed = individual speed × VLS
```

是否為這種關係？

還是 VLS 直接覆蓋 individual speed？

目前文件只能知道「affects」，無法知道兩者的數學關係。

---

**Q9：VLS 是否影響已經執行中的 motion？**

例如：

```text
VLS 0.15
MOV ...
```

motion 執行途中再輸入：

```text
VLS 0.30
```

需要確認：

* 只影響下一個 motion？
* 立即影響目前 motion？
* 不允許 motion 執行中修改？

這一點對實際控制行為很重要。

---

**Q10：VLS 是否需要 Servo ON 才能設定？**

目前其他文件建立了 `INS → SVO → motion` 的使用關係，但 VLS 本身是否：

* 必須 INS 後才能執行？
* 必須 SVO 後才能執行？
* Servo OFF 時也可以設定？

Manual 沒有定義。

---

**Q11：VLS 設定值是否持續有效，直到下一次 VLS？**

例如：

```text
VLS 0.30
DSC
INS
```

重新 INS 後 VLS 是否仍為 `0.30`？

需要確認 VLS 是：

* runtime-only
* persistent configuration

---

# 4. Response

文件：

```text
>VLS <value>
>Done.
```

### 與目前 baseline 有明顯 discrepancy

目前 `GMT_Server_Command.cpp` baseline 是：

```text
>VLS <value>
>Done
```

也就是 **baseline 沒有 `Done` 後面的句點 `.`**。

而 Manual 明確寫：

```text
>Done.
```

因此這是一個必須記錄的規格差異。

### 需要文件作者釐清

**Q12：正式 response 是 `>Done.` 還是 `>Done`？**

這不是單純格式問題，因為 SVO、SVF、CAL 等 Manual 也使用：

```text
>Done.
```

而目前 response baseline 多數使用：

```text
>Done
```

需要由 Manual 作者確認正式標準。

---

# 5. Response Sequence

文件列出：

```text
>VLS <value>
>Done.
```

### 需要文件作者釐清

**Q13：這兩個 response 是否為固定且有順序的兩個 response？**

也就是：

```text
VLS 0.15
    ↓
>VLS 0.15
    ↓
>Done.
```

還是其中一個只是文件中的 response example？

目前文件沒有明確寫「sequence」。

---

**Q14：`>VLS <value>` 是設定開始的通知，還是設定成功後的結果？**

尤其需要確認：

```text
>VLS 0.15
>Done.
```

其中：

* `>VLS 0.15` = command accepted？
* `>Done.` = system speed 已實際設定完成？

還是兩者都只是一般 acknowledgement？

---

**Q15：`<value>` response 是否必須使用與輸入完全相同的文字格式？**

例如輸入：

```text
VLS 0.150
```

response 是：

```text
>VLS 0.150
```

還是：

```text
>VLS 0.15
```

如果輸入：

```text
VLS 1
```

是否回：

```text
>VLS 1
```

或：

```text
>VLS 1.0
```

需要定義 response numeric formatting。

---

# 6. Default Response

目前 baseline：

```text
>VLS <value>
```

### 需要文件作者釐清

**Q16：VLS 成功執行時的 default response 是哪一個？**

因為文件同時列：

```text
>VLS <value>
>Done.
```

需要確認 `>Done.` 是否：

* 第二個固定 response
* completion response
* default response

這會直接影響後續 parser/TCP response 的實作。

---

# 7. Error Conditions

文件沒有描述任何 error response。

### 需要文件作者釐清

**Q17：VLS 執行失敗時的 response 格式為何？**

例如是否：

```text
>VLS ERR <ErrorCode>
```

或：

```text
>Invalid parameters
```

或其他格式。

---

**Q18：哪些情況屬於 VLS execution error？**

至少需要定義：

* value 超出範圍
* value = 0（如果不允許）
* negative value（如果不允許）
* controller 尚未 INS
* EtherCAT 尚未連線
* Servo OFF
* system currently moving
* 其他 system state 不允許

目前無法從文件判斷。

---

# 8. Case Sensitivity

### 需要文件作者釐清

**Q19：Command 是否大小寫敏感？**

例如：

```text
VLS 0.15
vls 0.15
Vls 0.15
```

目前 parser command comparison 是 case-sensitive，因此只有正式規格確認後才能決定是否需要一致。

---

# 9. Example

文件：

```text
VLS 0.15
```

### 已明確

* Example 與 Format 一致。
* `0.15` 是合法範例。

### 需要文件作者釐清

**Q20：是否需要提供 boundary examples？**

目前只有正常案例，沒有：

* 最小合法值
* 最大合法值
* 0
* 負值
* 超出範圍

因此目前不足以定義 parser 的完整 validation boundary。

---

# 10. Cross-command / Global Consistency

這個 command 有幾個值得列入 **Global Specification** 的問題。

### A. `Done` punctuation

VLS：

```text
>Done.
```

目前 baseline：

```text
>Done
```

而 SVO / SVF / CAL Manual 也使用：

```text
>Done.
```

➡️ **需要統一確認所有 command 的正式 response punctuation。**

---

### B. VLS 與個別 Motion Speed 的關係

VLS 定義為：

> globally adjust speed

但目前沒有正式數學/優先級規則。

需要作者明確定義：

```text
VLS
  ↓
global speed factor
  ↓
MOV / MRV / ...
```

以及 individual speed 與 VLS 的計算關係。

---

### C. Motion command 清單

VLS 明確提到：

```text
MOV, MRV
```

但使用了：

> all motion commands including ...

因此需要正式列出所有受影響 command，避免 parser/manual/firmware 三邊定義不同。

---

### D. Runtime state

VLS 沒有說明與：

```text
INS
SVO
SVF
DSC
STP
```

的關係。

這與前面已審查的 commands 會形成 global state 規格問題。

---

# 最終結論

## 文件已明確

* Command：`VLS`
* 參數數量：1
* 參數名稱：`value`
* 參數型態：Floating point
* `0.15`、`0.3` 的比例關係範例
* 用途：設定 global system speed
* 至少影響 `MOV`、`MRV`
* 範例：`VLS 0.15`

## 需要文件作者釐清

本次 VLS 建議記錄 **20 個問題**，其中最重要的是：

1. **value 合法範圍**
2. **0 / 負值是否允許**
3. **numeric syntax / precision**
4. **0.15 的正式物理/比例定義**
5. **VLS 與 individual motion speed 的計算關係**
6. **VLS 是否影響正在執行的 motion**
7. **完整受 VLS 影響的 motion command 清單**
8. **INS/SVO/SVF/DSC 等 state dependency**
9. **`>Done.` 與目前 baseline `>Done` 的明確衝突**
10. **兩個 response 是否為固定順序**
11. **response `<value>` 的 numeric formatting**
12. **execution error response 與 ErrorCode 規則**
13. **case sensitivity**

其中 **Q12（`>Done.` vs `>Done`）是目前最直接的 baseline discrepancy**，應該明確交給文件作者決定，暫時不要改 parser 或 response baseline。

---

# VLS? (Query System Speed)

以下依照目前相同的審查基準，檢查 **VLS?（Query System Speed）**。這一項不涉及修改程式碼。

# VLS?（Query System Speed）規格審查

## 1. Format

文件：

```text
VLS?
```

### 已明確

* Command 為 `VLS?`
* 不需要參數。
* 與目前 parser 的 `VLS?` command 規則一致。

### 需要文件作者釐清

**Q1：`VLS?` 是否大小寫敏感？**

例如：

```text
VLS?
vls?
Vls?
```

目前 parser command 比對是 case-sensitive，因此需要正式規格確認。

---

# 2. Arguments

文件：

> Arguments: None

### 已明確

* 無參數。
* `VLS? 0.15` 應不屬於目前文件定義的合法格式。

### 需要文件作者釐清

**Q2：`VLS?` 後面若帶參數，正式錯誤應為什麼？**

例如：

```text
VLS? 0.15
```

是：

* `Invalid parameters`
* `Invalid command format`
* 其他專用錯誤

目前 Manual 沒有定義。

---

# 3. `<value>` Data Type / Range / Precision

Response：

```text
>VLS? <value>
```

### 已明確

* 回傳目前 system speed。
* `<value>` 是 VLS 所設定的值。

### 需要文件作者釐清

**Q3：VLS? 回傳的 `<value>` numeric format 是否與 VLS 輸入值相同？**

例如：

```text
VLS 0.15
```

執行：

```text
VLS?
```

是否一定回：

```text
>VLS? 0.15
```

還是可能回：

```text
>VLS? 0.150000
```

需要定義格式與 precision。

---

**Q4：VLS? 回傳值的合法範圍是否與 VLS `<value>` 完全相同？**

目前 VLS 本身尚未定義：

* min
* max
* zero
* negative
* precision

因此 VLS? 的回傳範圍也尚未完整定義。

這一題可以與 VLS Q2～Q6 合併成 **VLS/VLS? 共用 numeric specification**。

---

# 4. Response

文件：

```text
>VLS? <value>
```

### 與目前 baseline 一致

目前 `GMT_Server_Command.cpp` baseline 為：

```text
>VLS? <value>
```

所以目前 **沒有 response string discrepancy**。

### 需要文件作者釐清

**Q5：`>VLS? <value>` 是否是唯一正式 response？**

也就是正常情況下是否永遠只有：

```text
>VLS? 0.15
```

而不會有：

```text
>VLS?
>Done
```

或其他第二個 response。

目前文件看起來是單一 response，但沒有明確寫「only response」。

---

# 5. Response Condition

文件：

> This value indicates the active system-wide speed setting.

這裡有一個重要問題。

### 需要文件作者釐清

**Q6：`VLS?` 查詢的是「目前設定值」還是「目前實際生效值」？**

文件同時使用：

> current system speed set for motion operations

以及：

> active system-wide speed setting

需要確認兩者是否相同。

例如：

```text
VLS 0.30
```

之後如果系統因某些狀態暫時限制實際 motion speed：

```text
VLS?
```

應該回：

```text
>VLS? 0.30
```

還是回實際目前限制後的 speed？

這個差異對後續 motion control 很重要。

---

# 6. Default Response

### 已明確

目前 baseline：

```text
>VLS? <value>
```

與 Manual 一致。

### 需要文件作者釐清

**Q7：系統尚未執行過 VLS 時，VLS? 的 default value 是多少？**

這是目前比較重要的一個缺口。

例如剛：

```text
INS
```

之後直接：

```text
VLS?
```

需要知道回：

```text
>VLS? 0.15
```

還是其他預設值。

---

**Q8：DSC → INS 後，VLS 的值是否保留？**

例如：

```text
VLS 0.30
DSC
INS
VLS?
```

需要確認是否仍然：

```text
>VLS? 0.30
```

還是恢復 default value。

這與上一個 VLS command 的 **runtime / persistent** 定義直接相關。

---

# 7. Error Conditions

文件沒有描述錯誤 response。

### 需要文件作者釐清

**Q9：VLS? 在 system 尚未 INS / EtherCAT 未連線時是否可以執行？**

例如：

```text
VLS?
```

在完全未初始化狀態下：

* 是否仍可查詢？
* 是否回目前 stored value？
* 是否回 error？

目前沒有定義。

---

**Q10：VLS? 是否可能發生 execution error？如果會，error response 格式為何？**

例如 system speed 設定不存在、設定資料無效等情況，目前沒有規格。

---

# 8. 與 VLS 的 Cross-command Consistency

這裡有幾個很重要的對應關係。

### A. VLS ↔ VLS?

文件已經清楚表達：

```text
VLS 0.15
    ↓
設定 system speed

VLS?
    ↓
查詢 system speed
```

但需要正式確認：

**Q11：`VLS?` 是否必須回傳「最後一次成功執行的 VLS 值」？**

如果是，這可以成為非常清楚的 command semantics。

---

### B. VLS ↔ Motion

VLS 說：

> affects all motion commands including MOV, MRV.

VLS? 說：

> active system-wide speed setting

因此需要確認：

**Q12：VLS? 查詢的值是否就是所有受 VLS 影響的 motion commands 所使用的 global speed factor？**

這一點最好與 VLS 本身的計算規則一起定義。

---

### C. VLS ↔ individual motion speed

前一個 VLS 已經有一個尚未釐清的重要問題：

如果：

```text
VLS 0.15
MOV ... <individual speed>
```

到底如何計算實際速度？

那麼 VLS? 回傳的 `0.15` 是否就是該 calculation 中使用的 global factor，需要一起確認。

---

# 9. Example

本 command **沒有提供實際 Example**。

### 需要文件作者釐清

**Q13：是否應增加一個完整 query example？**

例如：

```text
VLS 0.15
VLS?
>VLS? 0.15
```

這可以明確說明：

* VLS 設定值
* VLS? 查詢值
* response format
* numeric formatting

不過這屬於文件完整性建議，不是 parser 必須修改的問題。

---

# 10. Global Consistency

這個 command 暫時沒有發現新的 response punctuation discrepancy：

| Command | Manual                    | 目前 baseline              |
| ------- | ------------------------- | ------------------------ |
| VLS     | `>VLS <value>` / `>Done.` | `>VLS <value>` / `>Done` |
| VLS?    | `>VLS? <value>`           | `>VLS? <value>`          |

所以：

* **VLS? response 本身一致**
* **VLS 的 `>Done.` vs `>Done` 仍然是前一題已記錄的 discrepancy**

另外，VLS/VLS? 應該一起確認：

* default value
* range
* precision
* runtime/persistent
* active value 定義
* motion speed calculation

---

# 最終結論

## 文件已明確

* Command：`VLS?`
* 無參數
* 用途：查詢 system-wide speed
* Response：`>VLS? <value>`
* 回傳的是 system speed setting
* 調整速度使用 `VLS`

## 需要文件作者釐清

本次建議新增 **13 個問題**，其中最重要的是：

1. **VLS? 是否大小寫敏感**
2. **VLS? 回傳 value 的 numeric format / precision**
3. **VLS? 是否只回單一 response**
4. **「current setting」與「active setting」是否完全相同**
5. **VLS 未設定時的 default value**
6. **DSC → INS 後 VLS 值是否保留**
7. **未 INS 時 VLS? 是否允許**
8. **VLS? execution error 的處理**
9. **VLS? 是否一定回最後一次成功的 VLS 值**
10. **VLS? 回傳值是否就是 motion 使用的 global speed factor**
11. **VLS/VLS? 的 range、precision、numeric format 是否共用同一規格**

**這一個 command 本身與目前 parser / response baseline 沒有直接的格式衝突。** 最大的規格缺口是 **VLS 與 VLS? 的數值定義及 runtime/persistent 行為尚未明確化**。

---

# MOV (Absolute Move)

以下依照目前的 **Command Parser + `GMT_Server_Command.cpp` baseline** 審查 `MOV`。這個 command 比前面的 VLS/VLS? 明顯更需要釐清，尤其是 **PR/LP 與 6 個參數的關係、數值單位/範圍，以及 response sequence**。

# MOV（Absolute Move）規格審查

## 1. Format

文件：

```text
MOV <side>M0 M1 M2 M3 M4 M5
```

但 Arguments 又說：

```text
<side>: R / L / PR / LP
```

### 與目前 Parser 有明顯格式 discrepancy

目前 parser 的 MOV 規則是：

```text
MOV (R|L|PR|LP) + 6 個 signed decimal floating-point values
```

也就是實際接受形式為：

```text
MOV R 2000 2000 1000 0 0.5 1
```

文件 Format：

```text
MOV <side>M0 M1 M2 M3 M4 M5
```

少了 `<side>` 與 `M0` 之間的空白表示。

但 Example 又使用：

```text
MOV R 2000 2000 1000 0 0.5 1
```

### 需要文件作者釐清

**Q1：正式 Format 是否應為：**

```text
MOV <side> <M0> <M1> <M2> <M3> <M4> <M5>
```

而不是：

```text
MOV <side>M0 M1 M2 M3 M4 M5
```

目前 parser 與 Example 都顯示應該有空白，但不能自行把 Manual 改成正確版本，需作者確認。

---

# 2. `<side>` 定義

文件：

```text
R = Right
L = Left
PR = Right Side Piezo
LP = Left Side Piezo
```

### 與目前 parser 一致

目前 parser 接受：

```text
R
L
PR
LP
```

### 需要文件作者釐清

**Q2：`PR` 與 `LP` 是否也必須提供 6 個參數？**

因為文件後面明確寫：

> For side R or L, provide 6 values

這句話只明確要求 R/L。

但是目前 parser 對 `PR` / `LP` **同樣要求 6 個 numeric values**。

因此需要確認：

```text
MOV PR ...
MOV LP ...
```

究竟是：

* 也需要 6 個值？
* 只需要 3 個 position values？
* 其他專用格式？

這是目前 **Parser 與 Manual 的重要 discrepancy**。

---

# 3. Position `<M0 M1 M2>`

文件：

> Position in μm (converted internally)

### 已明確

* M0/M1/M2 是位置。
* 單位為 μm。
* 系統內部會轉換。

### 需要文件作者釐清

**Q3：M0/M1/M2 的合法數值範圍是多少？**

例如：

```text
MOV R -100 200 300 0 0 0
```

是否合法？

目前 parser 接受 signed floating-point，因此負值可以通過 syntax validation，但 Manual 沒有定義機構位置範圍。

---

**Q4：M0/M1/M2 是否允許小數？**

例如：

```text
MOV R 2000.5 2000.25 1000.1 0 0 0
```

目前 parser 接受，但 Manual 沒有明確說 position 的 precision。

---

**Q5：位置值是否有固定解析度？**

例如 μm 是否允許：

```text
0.001 μm
```

還是系統實際最小解析度為：

```text
0.1 μm
1 μm
```

目前沒有定義。

---

# 4. Rotation `<M3 M4 M5>`

文件：

> Rotation in degrees (applicable to R/L only)

### 需要文件作者釐清

**Q6：M3/M4/M5 的正式 rotation 範圍是多少？**

例如是否允許：

```text
-180 ~ +180
```

或：

```text
-360 ~ +360
```

或其他機構限制。

目前 parser 沒有 range validation。

---

**Q7：Rotation 是否允許小數？**

Example：

```text
0.5
1
```

顯示允許小數，但正式 precision 未定義。

---

**Q8：Rotation 的三個參數分別對應哪三個軸？**

文件只說：

> 3 angles (U, V, W)

但最好明確定義：

```text
M3 = U
M4 = V
M5 = W
```

目前從 Notes 可以合理理解，但正式 Format 應明確。

---

# 5. Side 與參數關係

這是 MOV 很重要的一項。

目前文件表示：

```text
R/L → 3 position + 3 rotation
PR/LP → 未明確
```

### 需要文件作者釐清

**Q9：PR/LP 的 position/rotation 定義到底是什麼？**

如果 PR/LP 是 Piezo，是否：

* 只支援某些 axis？
* M0/M1/M2 是否仍代表 XYZ？
* M3/M4/M5 是否被忽略？
* rotation 是否完全不適用？

目前不能從文件推定。

---

**Q10：不同 `<side>` 是否有不同的合法 position range？**

例如 R/L 與 PR/LP 可能具有不同的 mechanical range。

Manual 沒有說明。

---

# 6. Response — Command Accepted

文件：

```text
> MOV
```

或：

```text
> MOV ERR [error code]
```

### 與目前 baseline 有部分一致

目前 baseline 有：

```text
> MOV
> MOV ERR <ErrorCode>
```

但文件中的：

```text
> MOV ERR [error code]
```

`[error code]` 的括號是否為實際輸出格式需要確認。

### 需要文件作者釐清

**Q11：正式 response 是：**

```text
>MOV ERR 123
```

還是：

```text
>MOV ERR [123]
```

目前文件中的 `[error code]` 通常看起來像 placeholder，但不能自行假定。

---

**Q12：`>MOV` 是「command accepted」還是「motion started」？**

這對 response timing 很重要。

---

# 7. Trajectory Execution Response

文件列出：

```text
>Excute Result : NORMAL FINISHED
>Excute Result : ERR STOP
>Excute Result : ERR NOT ON TARGET
>Excute Result : ERR
>Excute Result : ERR OVER GAP SENSOR THRSHOLD
```

### 與目前 baseline 一致的方向

目前 baseline 也有：

* NORMAL FINISHED
* ERR STOP
* ERR NOT ON TARGET
* ERR
* ERR OVER GAP SENSOR THRSHOLD

### 需要文件作者釐清

**Q13：`Excute` 是否為正式 response 字串的一部分，還是文件拼字錯誤？**

目前文件使用：

```text
>Excute Result
```

如果這是正式 protocol string，就必須保持 `Excute`。

如果正確拼字應該是：

```text
>Execute Result
```

則需要作者確認。

這一點不能自行修正，因為 response protocol 是 exact string。

---

**Q14：`THRSHOLD` 是否為正式 response 字串？**

文件：

```text
ERR OVER GAP SENSOR THRSHOLD
```

`THRSHOLD` 看起來可能是 `THRESHOLD` 的拼字問題。

但目前 baseline 也使用 `THRSHOLD` 的版本，因此這是非常重要的確認項：

> `THRSHOLD` 是故意定義的 protocol string，還是 Manual typo？

---

# 8. Response Sequence

文件明確寫：

> During trajectory execution...

並說：

> After any of the above result messages, the controller will always send `>Done`

### 這裡規格相對明確

成功流程看起來是：

```text
>MOV
    ↓
>Excute Result : NORMAL FINISHED
    ↓
>Done
```

錯誤流程則可能是：

```text
>MOV
    ↓
>Excute Result : ERR STOP
    ↓
>Done
```

### 仍需要釐清

**Q15：`>MOV` 是否一定先於 Execute Result？**

文件把兩者都列在 Response，但沒有明確說固定 sequence。

需要確認是否一定：

```text
>MOV
→ Execute Result
→ >Done
```

---

**Q16：`>Done` 是否一定最後一個 response？**

文件寫：

> always send

所以看起來是，但建議正式確認為 protocol rule。

---

**Q17：`>Done` 的意思是 motion 完成，還是 trajectory processing 完成？**

尤其：

```text
ERR STOP
ERR NOT ON TARGET
ERR
```

後面仍然送：

```text
>Done
```

因此 `Done` 顯然不能單純理解為「motion successfully finished」。

需要明確定義。

---

# 9. Error Conditions

### 需要文件作者釐清

**Q18：`>MOV ERR [error code]` 的觸發條件有哪些？**

例如：

* Servo OFF
* EtherCAT 未連線
* Axis 不存在
* position 超過 mechanical limit
* rotation 超過 limit
* gap sensor condition
* trajectory generation failure
* system not initialized

目前沒有定義。

---

**Q19：`>MOV ERR <ErrorCode>` 與 Execute Result `ERR` 的差異是什麼？**

這是 MOV 規格非常重要的一點。

目前看起來可能存在兩種不同層級：

```text
>MOV ERR <ErrorCode>
```

以及：

```text
>Excute Result : ERR ...
```

需要作者明確定義兩者的：

* 發生時機
* 意義
* ErrorCode 是否相同
* 是否都會再送 `>Done`

---

**Q20：如果 `>MOV ERR <ErrorCode>` 發生，還會不會送 `>Done`？**

文件寫：

> After any of the above result messages...

但 `>MOV ERR` 是前面另一組 response，不在「above result messages」清單中。

因此目前不明確。

---

# 10. `STP` / Motion Interaction

### 需要文件作者釐清

**Q21：MOV 執行期間收到 STP 時，正式 response sequence 是什麼？**

例如：

```text
MOV
→ Execute Result : ERR STOP
→ Done
```

是否就是正式規則？

這應該與 STP command 的規格互相確認。

---

**Q22：`ERR STOP` 是否只代表 STP 造成的停止？**

還是其他原因造成的 stop 也可能回：

```text
ERR STOP
```

---

# 11. VLS Interaction

前一個 command 已定義：

> VLS affects all motion commands including MOV, MRV.

因此 MOV 必須與 VLS 規格一致。

### 需要文件作者釐清

**Q23：MOV 的實際速度如何受到 VLS 影響？**

需要明確定義 VLS 與 MOV 自身 motion profile 的計算關係。

---

**Q24：MOV 執行中修改 VLS 是否影響目前 MOV？**

這與前一個 VLS Q9 相同，可以最後合併成 global question。

---

# 12. Servo / INS State

根據其他 Manual：

* MOV 必須在 SVO 後使用。
* SVO 又需要 INS。

### 需要文件作者釐清

**Q25：MOV 在 Servo OFF 狀態下的正式 response 是什麼？**

例如：

```text
>MOV ERR <ErrorCode>
```

還是其他 response？

---

**Q26：MOV 在尚未 INS / EtherCAT 未連線時的正式 response 是什麼？**

目前 parser 可以判斷 syntax，但 runtime state error 完全沒有在 MOV Manual 中定義。

---

# 13. Axis / Position Safety

### 需要文件作者釐清

**Q27：MOV 是否有 software position limit / mechanical limit？**

如果有，需要定義：

* limit 值從哪裡取得
* 超過 limit 時是立即拒絕還是執行後報錯
* ErrorCode
* 是否送 Execute Result
* 是否送 Done

---

# 14. Case Sensitivity

**Q28：`MOV`、`R/L/PR/LP` 是否大小寫敏感？**

例如：

```text
MOV r ...
MOV pr ...
```

目前 parser 是 case-sensitive。

Manual 沒有說明。

---

# 15. Example

文件：

```text
MOV R 2000 2000 1000 0 0.5 1
```

### 已明確

這個 example 與目前 parser 的：

```text
R + 6 signed decimal floating-point values
```

一致。

### 需要文件作者釐清

**Q29：Example 的數值單位是否全部依照前述定義？**

即：

```text
2000  → μm
2000  → μm
1000  → μm
0     → degree
0.5   → degree
1     → degree
```

最好明確標示 M0～M5 對應關係。

---

# 16. 與目前 Parser 的直接比對

| 項目             | Manual    | 目前 Parser       | 結果                   |
| -------------- | --------- | --------------- | -------------------- |
| MOV            | 有         | 有               | 一致                   |
| R              | 有         | 有               | 一致                   |
| L              | 有         | 有               | 一致                   |
| PR             | 有         | 有               | 需確認參數定義              |
| LP             | 有         | 有               | 需確認參數定義              |
| R/L 6 values   | 明確        | 6 values        | 一致                   |
| PR/LP 6 values | **未明確**   | **要求 6 values** | ⚠️ discrepancy       |
| Position float | 未完整定義     | 支援              | 規格缺口                 |
| Rotation float | 未完整定義     | 支援              | 規格缺口                 |
| 負數             | 未定義       | 支援              | 規格缺口                 |
| Range          | 未定義       | 未限制             | 規格缺口                 |
| `>MOV`         | 有         | 有               | 一致                   |
| `>MOV ERR`     | 有         | 有               | 基本一致                 |
| Execute Result | 有         | baseline 有      | 一致                   |
| `>Done`        | 有         | 有               | 一致                   |
| `Excute` 拼字    | Manual 使用 | baseline 使用     | **需確認是否正式 protocol** |
| `THRSHOLD` 拼字  | Manual 使用 | baseline 使用     | **需確認是否正式 protocol** |

---

# 最終結論

## 文件已明確

* `MOV` 是 absolute move。
* `<side>` 至少包含 `R / L / PR / LP`。
* R/L 使用 6 個參數。
* M0～M2 是 position，單位 μm。
* M3～M5 是 rotation，單位 degree。
* MOV 有 command response：

  * `>MOV`
  * `>MOV ERR [error code]`
* trajectory execution 有 5 種 result。
* trajectory result 後會有 `>Done`。
* MOV 是 absolute move，relative movement 使用 MRV。
* Example 與目前 parser 的 **R + 6 個浮點數**格式一致。

## 需要文件作者釐清

這次最重要的是以下幾項：

### **第一優先**

1. **正式 Format 是否為 `MOV <side> <M0> ... <M5>`？**
2. **PR / LP 到底是否也需要 6 個參數？**
3. **PR / LP 的 rotation 參數是否有效？**
4. **M0～M5 的合法 range / precision / 負值規則**
5. **`>MOV ERR <ErrorCode>` 與 Execute Result `ERR` 的差異**
6. **`>MOV ERR` 發生後是否仍然送 `>Done`**
7. **`>MOV` → Execute Result → `>Done` 是否為固定 sequence**
8. **`>Done` 的正式意義**

### **第二優先**

9. `Excute` 是否是故意的正式 protocol 拼字？
10. `THRSHOLD` 是否是故意的正式 protocol 拼字？
11. MOV 執行期間 STP 的正式 response 行為
12. Servo OFF / 尚未 INS 時 MOV 的 execution error
13. position / rotation safety limits
14. VLS 與 MOV 的實際速度計算關係
15. command / side 是否大小寫敏感

**特別注意：目前不要因為 `Excute` 或 `THRSHOLD` 看起來像 typo 就修改。** 因為目前 baseline 也使用這兩個字串；這反而更需要文件作者確認它們是不是已經成為正式 protocol response。

---

# MRV (Relative Move)

以下依照目前相同的 **Manual ↔ Command Parser ↔ `GMT_Server_Command.cpp` baseline** 審查 `MRV`。這次特別注意 `MRV C`、`RP/LP`、以及與 `MOV` 的 response 一致性。

# MRV（Relative Move）規格審查

## 1. Format

文件定義兩種格式：

```text id="5c2q8m"
MRV <side>dM0 dM1 dM2 dM3 dM4 dM5
MRV C dM0 dM1 dM2
```

### 與目前 Parser 基本一致

目前 parser 的 MRV 支援：

```text id="7h8v2p"
MRV R/L/RP/LP + 6 個浮點數
MRV C + 3 個浮點數
```

但文件第一行同樣缺少 `<side>` 與 `dM0` 之間的空白。

Example 則是：

```text id="7j4r1c"
MRV R 2000 2000 1000 0 0.5 1
```

### 需要文件作者釐清

**Q1：正式 Format 是否應為：**

```text id="4k0wzj"
MRV <side> <dM0> <dM1> <dM2> <dM3> <dM4> <dM5>
```

而不是目前文件寫的：

```text id="2gk8nc"
MRV <side>dM0 dM1 ...
```

---

# 2. `<side>` 定義

文件：

```text id="v1y0hh"
R = Right
L = Left
RP = Right Side Piezo
LP = Left Side Piezo
```

### 與目前 Parser 一致

目前 parser 接受：

```text
R
L
RP
LP
```

### 需要文件作者釐清

**Q2：`RP` / `LP` 是否正式需要 6 個參數？**

文件說：

> dM3 dM4 dM5: Relative rotation in degrees (R/L only)

因此目前明確知道 rotation 只適用 R/L。

但沒有說明：

```text
MRV RP ...
MRV LP ...
```

到底是：

* 仍然提供 6 個參數，但 dM3～dM5 必須為 0？
* 只提供 dM0～dM2？
* 還是有其他格式？

目前 parser 對 `RP`、`LP` 都要求 **6 個參數**。

這是與 MOV 相同的重要規格缺口。

---

# 3. `MRV C` 格式

文件額外定義：

```text
MRV C dM0 dM1 dM2
```

### 與目前 Parser 一致

目前 parser 確實支援：

```text
MRV C + 3 個 signed decimal floating-point values
```

### 需要文件作者釐清

**Q3：`C` 的正式意義是什麼？**

目前文件只列出 syntax：

```text
MRV C dM0 dM1 dM2
```

但完全沒有解釋 `C` 代表什麼。

需要定義：

* C 是哪一個 side？
* C 是 Center？
* C 是 Cartesian？
* C 是其他特殊 motion mode？

不能單從 parser 推定。

---

**Q4：`MRV C` 的 dM0～dM2 分別代表哪三個方向？**

是：

```text
X / Y / Z
```

還是其他座標系？

---

**Q5：`MRV C` 是否完全不支援 rotation？**

目前 syntax 只有 3 個參數，但需要文件正式確認。

---

**Q6：`MRV C` 是否受 VLS global speed 影響？**

因為 VLS 文件說「all motion commands」，而 MRV 是 motion command。

需要確認 C mode 是否也包含在 VLS 影響範圍內。

---

# 4. dM0～dM2 Relative Displacement

文件：

> Relative displacement in μm

### 已明確

* dM0～dM2 是相對位移。
* 單位 μm。

### 需要文件作者釐清

**Q7：dM0～dM2 是否允許負值？**

Relative move 通常需要正負方向，但 Manual 沒有明確規定。

目前 parser 接受 signed floating-point，因此例如：

```text id="8m3nqk"
MRV R -100 0 0 0 0 0
```

syntax 上可以通過。

但不能把 parser acceptance 當成正式產品規格。

---

**Q8：dM0～dM2 是否允許小數？**

目前 parser 接受，例如：

```text
MRV R 0.5 0 0 0 0 0
```

Manual 只寫 displacement in μm，沒有 precision 定義。

---

**Q9：Relative displacement 是否有最大/minimum range？**

例如單次：

```text
MRV R 1000000 0 0 0 0 0
```

是否合法？

需要定義。

---

# 5. dM3～dM5 Relative Rotation

文件：

> Relative rotation in degrees (R/L only)

### 已明確

* R/L 才有 rotation。
* 單位 degree。

### 需要文件作者釐清

**Q10：dM3～dM5 是否允許負值？**

例如：

```text id="qq4yl6"
MRV R 0 0 0 -1 0 0
```

是否合法？

---

**Q11：dM3～dM5 的合法 range / precision 是什麼？**

目前 parser 沒有限制 range。

---

**Q12：dM3～dM5 分別對應 U/V/W 嗎？**

與 MOV 一樣，建議正式明確：

```text id="q3xv7z"
dM3 = U
dM4 = V
dM5 = W
```

目前文件沒有明確列出。

---

# 6. Relative Position 的 Reference Point

這是 MRV 與 MOV 最大的語意差異之一。

文件說：

> offset from its current position

### 需要文件作者釐清

**Q13：`current position` 是哪一個 position？**

例如：

* command 執行當下的 actual position？
* controller command position？
* 上一次 target position？
* encoder feedback position？

需要正式定義。

---

**Q14：如果前一個 MOV/MRV 尚未完成，又收到新的 MRV，新的 offset 是相對於什麼位置？**

例如 motion queue 是否存在？

這涉及 command concurrency，需要明確規範。

---

# 7. Response

文件：

```text id="w4ihm7"
>MRV
```

或：

```text id="6t9k1d"
>MRV ERR [error code]
```

### 與目前 baseline 一致

目前 `GMT_Server_Command.cpp` 有相對應的：

```text
>MRV
>MRV ERR <ErrorCode>
```

### 需要文件作者釐清

**Q15：`[error code]` 中的方括號是否為實際輸出字元？**

需要確認正式格式究竟是：

```text
>MRV ERR 123
```

還是：

```text
>MRV ERR [123]
```

---

# 8. Trajectory Result

文件列出：

```text
>Excute Result : NORMAL FINISHED
>Excute Result : ERR STOP
>Excute Result : ERR NOT ON TARGET
>Excute Result : ERR
>Excute Result : ERR OVER GAP SENSOR THRESHOLD
```

### 與 MOV 比較

MOV 文件最後一項寫：

```text
ERR OVER GAP SENSOR THRSHOLD
```

MRV 則寫：

```text
ERR OVER GAP SENSOR THRESHOLD
```

這裡有**直接的 response string discrepancy**。

### 需要文件作者釐清

**Q16：正式字串是 `THRESHOLD` 還是 `THRSHOLD`？**

目前 baseline 使用的是 MOV 那一版：

```text
THRSHOLD
```

但 MRV Manual 使用：

```text
THRESHOLD
```

不能自行修正。

---

# 9. `Excute` 拼字

MRV 使用：

```text id="bb30jw"
>Excute Result : ...
```

與 MOV 完全相同。

### 需要文件作者釐清

**Q17：`Excute` 是否為正式 protocol response string？**

如果是，所有 command 必須維持這個拼字。

如果是文件 typo，需要統一更正。

---

# 10. Response Sequence

文件明確表示：

> During trajectory execution...

以及：

> followed by a final “>Done” message upon completion.

### 目前可以理解為

```text id="n4c2yz"
MRV
 ↓
>MRV
 ↓
>Excute Result : ...
 ↓
>Done
```

但仍有一點需要確認。

### 需要文件作者釐清

**Q18：`>MRV` → Execute Result → `>Done` 是否為固定順序？**

也就是 `>MRV` 是否一定在 trajectory result 之前。

---

**Q19：Execute Result 發生錯誤時是否仍一定送 `>Done`？**

文件寫：

> After any of the above result messages, the controller will always send `>Done`.

這一點相對明確，**目前可理解為 YES**。

但建議正式確認 `>Done` 在 error result 下代表「trajectory 結束」，而不是「motion success」。

---

# 11. `STP` Interaction

### 需要文件作者釐清

**Q20：MRV 執行期間收到 STP 時，是否一定產生：**

```text
>Excute Result : ERR STOP
>Done
```

這與 STP Manual 需要交叉確認。

---

# 12. Motion Error vs Command Error

### 需要文件作者釐清

**Q21：`>MRV ERR <ErrorCode>` 與：**

```text
>Excute Result : ERR ...
```

兩者的錯誤層級是否不同？

例如是否：

```text
>MRV ERR <ErrorCode>
```

= command 根本無法開始

而：

```text
>Excute Result : ERR ...
```

= motion 已開始但 execution 過程發生問題？

這是目前文件最需要明確化的地方之一。

---

**Q22：如果 MRV 在開始前發生 `>MRV ERR <ErrorCode>`，是否還會送 `>Done`？**

目前文件只說 trajectory result 後一定 Done，沒有說 command-level error 的情況。

---

# 13. INS / SVO / SVF / DSC State

根據前面 Manual：

* MOV/MRV 屬於 motion。
* SVO 後才能進行 motion。
* DSC 後 motion command 不應正常執行。

### 需要文件作者釐清

**Q23：MRV 在 Servo OFF 時的正式 response 是什麼？**

是否：

```text
>MRV ERR <ErrorCode>
```

---

**Q24：MRV 在尚未 INS 時的正式 response 是什麼？**

同樣需要定義 execution-state error。

---

**Q25：MRV 在 DSC 後是否屬於「Invalid command」還是「valid command but execution error」？**

這涉及 parser validation 與 runtime validation 的分層。

---

# 14. VLS Interaction

VLS Manual 明確說會影響：

> all motion commands including MOV, MRV

因此 MRV 確定屬於 VLS 影響範圍。

但仍需釐清：

**Q26：MRV 的 relative displacement 與 VLS 如何共同決定實際速度？**

例如 VLS：

```text
0.15
```

是否表示 MRV 執行速度乘以 0.15？

目前 Manual 沒有定義數學關係。

---

# 15. Case Sensitivity

**Q27：`MRV`、`R/L/RP/LP/C` 是否大小寫敏感？**

目前 parser 是 case-sensitive。

Manual 未定義。

---

# 16. Example

文件：

```text
MRV R 2000 2000 1000 0 0.5 1
```

### 已明確

這與目前 parser 的：

```text
R/L/RP/LP + 6 floating-point values
```

一致。

### 需要文件作者釐清

**Q28：Example 的 dM0～dM5 對應是否為：**

```text
dM0 = X = 2000 μm
dM1 = Y = 2000 μm
dM2 = Z = 1000 μm
dM3 = U = 0°
dM4 = V = 0.5°
dM5 = W = 1°
```

目前可以從 MOV 的說明推導，但這次 MRV 本身沒有完整列出對應關係。

---

# 17. 與目前 Parser 直接比對

| 項目                | Manual      | 目前 Parser             | 結果             |
| ----------------- | ----------- | --------------------- | -------------- |
| `MRV`             | 有           | 有                     | 一致             |
| `R`               | 有           | 有                     | 一致             |
| `L`               | 有           | 有                     | 一致             |
| `RP`              | 有           | 有                     | ⚠️ 參數規格未明確     |
| `LP`              | 有           | 有                     | ⚠️ 參數規格未明確     |
| `C`               | 有           | 有                     | ⚠️ 語意未定義       |
| R/L 6 values      | 有           | 6 values              | 一致             |
| C 3 values        | 有           | 3 values              | 一致             |
| dM0～2             | μm          | signed float          | 型態方向一致         |
| dM3～5             | degree, R/L | signed float          | 型態方向一致         |
| Numeric range     | 未定義         | 未限制                   | 規格缺口           |
| `>MRV`            | 有           | 有                     | 一致             |
| `>MRV ERR`        | 有           | 有                     | 基本一致           |
| Execute Result    | 有           | baseline 有            | 一致             |
| Gap sensor string | `THRESHOLD` | baseline 為 `THRSHOLD` | ⚠️ discrepancy |
| `Excute`          | 有           | baseline 有            | ⚠️ 需確認正式拼字     |
| `>Done`           | 有           | 有                     | 一致             |

---

# 最終結論

## 文件已明確

* `MRV` 是 relative movement。
* R/L/RP/LP 為 side。
* R/L 使用 6 個 displacement/rotation values。
* `MRV C` 使用 3 個 values。
* dM0～dM2 為 μm。
* dM3～dM5 為 degree，R/L only。
* 有 `>MRV` / `>MRV ERR`。
* trajectory 有 5 種 result。
* trajectory result 後送 `>Done`。
* Example 與目前 parser 的 R + 6 float 格式一致。
* VLS 已明確說會影響 MRV。

## 需要文件作者釐清

這次主要問題為：

1. **Format 是否應在 `<side>` 與 dM0 間加空白**
2. **RP/LP 是否也要求 6 個參數**
3. **RP/LP 的 rotation 參數如何處理**
4. **`C` 的正式意義**
5. **C 的三個參數代表什麼**
6. **C 是否支援 rotation**
7. **dM0～dM2 的正負、range、precision**
8. **dM3～dM5 的正負、range、precision**
9. **dM3～dM5 是否為 U/V/W**
10. **current position 的正式定義**
11. **`>MRV ERR` 與 Execute Result ERR 的層級差異**
12. **`>MRV ERR` 後是否送 `>Done`**
13. **`>MRV` → Execute Result → `>Done` 是否固定 sequence**
14. **STP 對 MRV 的正式中斷行為**
15. **Servo OFF / 未 INS / DSC 後的 execution error**
16. **MRV 與 VLS 的實際速度計算關係**
17. **大小寫規則**
18. **`Excute` 是否為正式 protocol 字串**
19. **`THRESHOLD` vs `THRSHOLD` 的正式字串**

### ⚠️ 本次最重要的直接 discrepancy

**MRV Manual：**

```text
>Excute Result : ERR OVER GAP SENSOR THRESHOLD
```

**目前 baseline（MOV/MRV response 定義）：**

```text
>Excute Result : ERR OVER GAP SENSOR THRSHOLD
```

這個必須交給文件作者確認，**目前不要自行修改任何 response string**。

另外，`MRV C` 是這份 Manual 新增的一個特殊模式；雖然目前 parser 已經有 `C + 3 values`，但 **「parser 能接受」不代表 `C` 的產品語意已經被文件完整定義**，因此這部分應列為規格釐清，而不是直接當成已完成。

---

# MSV (Move Single Axis)

以下依照目前相同的 **Manual ↔ Command Parser ↔ `GMT_Server_Command.cpp` baseline** 審查 `MSV`。這個 command 的主要問題集中在 **Axis 格式 `MN`、Left tool group 的定義、position 單位如何決定，以及 response error/result 的規格**。

# MSV（Move Single Axis）規格審查

## 1. Format

文件：

```text id="f6s8p3"
MSV <axis><position>
```

Example：

```text id="z7v2km"
MSV M02 2000
MSV M06 1
```

### 與目前 Parser 有明顯格式 discrepancy

目前 parser 的 MSV 規則是：

```text id="h2q9wc"
MSV <AxisName> <value>
```

也就是 `M02` 與 `2000` 之間需要 whitespace。

文件 Format：

```text id="v3m8qa"
MSV <axis><position>
```

則容易理解成：

```text id="v4p1yx"
MSV M022000
```

但 Example 明確使用：

```text id="m9q7kd"
MSV M02 2000
```

### 需要文件作者釐清

**Q1：正式 Format 是否應為：**

```text id="z5k4tr"
MSV <axis> <position>
```

而不是目前文件寫的 `<axis><position>`？

---

# 2. Axis Definition

文件：

> `<axis>: MN`

### 與目前 Parser 的實際規則不一致／規格不完整

目前 parser 要求：

```text id="e4h7xn"
M01 ~ M16
```

也就是：

```text id="x2q6pk"
M01
M02
...
M16
```

但 Manual 寫的是：

```text id="a9c3vz"
MN
```

這裡的 `N` 到底是 placeholder 還是 literal 字元並不明確。

### 需要文件作者釐清

**Q2：`MN` 是表示 `M01~M16` 的格式符號，還是實際 command 中允許輸入 `MN`？**

例如：

```text id="n8w3fs"
MSV M02 2000
```

從 Example 看起來應該是 axis number，但正式 syntax 沒有定義。

---

**Q3：Axis 的正式範圍是否為 M01～M16？**

目前 parser 是 M01～M16。

但需要 Manual 正式確認。

---

**Q4：`M1` 是否等同於 `M01`？**

目前 parser 對 MSV 是：

```text id="g5p2wd"
M01 ~ M16
```

不接受 `M1`。

而其他 command 的 Manual 有時候使用 `M1`，有時候 `M01`。

因此需要統一確認 Axis naming convention。

---

# 3. Left Tool Group

文件描述：

> Move one axis of the **Left tool group**

### 這是一個重要規格缺口。

目前 parser 只知道：

```text id="p0s6hj"
M01 ~ M16
```

並不知道哪些 M 軸屬於 Left tool group。

### 需要文件作者釐清

**Q5：Left tool group 包含哪些 Axis？**

例如是否：

```text id="3s8qvx"
M01 ~ M08
```

或：

```text id="z1c6nm"
由 configuration 定義
```

或其他規則。

---

**Q6：如果輸入的 Axis 不屬於 Left tool group，MSV 應該如何處理？**

例如：

```text id="f9r2kp"
MSV M10 2000
```

如果 M10 屬於 Right tool group，應該：

* `MSV ERR`
* `Invalid parameters`
* 其他錯誤

需要正式定義。

---

# 4. Position

文件：

> `<position>: Target position in μm or degrees`

### 這裡有一個重要問題

同一個 `<position>` 可以是：

```text id="v2k7mc"
μm
```

也可以是：

```text id="r6p4ys"
degrees
```

但文件沒有定義 **如何知道某個 Axis 使用哪個單位**。

### 需要文件作者釐清

**Q7：position 的單位是由 Axis 決定嗎？**

例如：

```text id="7n3xkq"
M02 → μm
M06 → degree
```

如果是，必須提供完整 mapping。

---

**Q8：每一個 M01～M16 的 position unit 是什麼？**

至少需要定義：

| Axis | Unit        |
| ---- | ----------- |
| M01  | ?           |
| M02  | μm / degree |
| ...  | ...         |
| M16  | ?           |

目前只有 Example 告訴我們：

```text id="4k5rnx"
M02 → μm
M06 → degree
```

不足以完整定義。

---

# 5. Position Numeric Type

目前 parser 的 MSV：

```text id="7d2vmp"
M01~M16 + signed decimal floating-point
```

### 需要文件作者釐清

**Q9：position 是否允許負值？**

例如：

```text id="u8m1qc"
MSV M02 -100
```

是否合法？

---

**Q10：position 是否允許小數？**

例如：

```text id="j6v9rs"
MSV M02 2000.5
MSV M06 1.25
```

目前 parser 可以接受，但 Manual 沒有定義 precision。

---

**Q11：每個 Axis 的 position range 是多少？**

例如 M02：

```text id="x7q2dn"
-...? ~ +...?
```

M06：

```text id="s4h8kc"
-...?° ~ +...?°
```

目前沒有任何 range specification。

---

# 6. Axis / Position Relationship

這是 MSV 最需要補充的部分。

### 需要文件作者釐清

**Q12：不同 Axis 是否有不同 position range / resolution？**

例如：

```text id="v9c2mh"
M02：μm，resolution = ?
M06：degree，resolution = ?
```

如果有，這些應屬於 axis configuration 還是 command specification？

---

# 7. Response

文件：

```text id="q3m7tx"
>MSV
```

或：

```text id="r8k2vf"
>MSV ERR [error code]
```

### 與目前 baseline 一致

目前 baseline 有：

```text id="e5y9wc"
>MSV
>MSV ERR <ErrorCode>
```

### 需要文件作者釐清

**Q13：`[error code]` 是否表示 placeholder，而非實際輸出的 `[]`？**

正式 protocol 是：

```text id="p6d4za"
>MSV ERR 123
```

還是：

```text id="q2f7mn"
>MSV ERR [123]
```

---

# 8. Trajectory Result

文件：

```text id="g7k3pv"
>Excute Result : NORMAL FINISHED
>Excute Result : ERR STOP
>Excute Result : ERR NOT ON TARGET
>Excute Result : ERR
>Excute Result : ERR OVER GAP SENSOR THRESHOLD
```

與前面 MOV/MRV 相同。

### 需要文件作者釐清

**Q14：`Excute` 是否為正式 protocol string？**

目前 MOV/MRV baseline 也使用：

```text id="q9r4bc"
>Excute Result
```

如果這是正式 protocol，三個 command 必須一致。

---

**Q15：`THRESHOLD` 是否為正式字串？**

這裡 MSV 使用：

```text id="j8w5nz"
THRESHOLD
```

而 MOV baseline 使用：

```text id="e1c6yd"
THRSHOLD
```

MRV Manual 又使用 `THRESHOLD`。

因此這個 global response string 必須由文件作者統一確認。

---

# 9. Response Sequence

文件明確說：

> followed by a final “>Done” message upon completion.

並且：

> After any of the above result messages, the controller will always send: `>Done`

### 已相對明確

看起來是：

```text id="n6t2kp"
>MSV
    ↓
>Excute Result : ...
    ↓
>Done
```

### 需要文件作者釐清

**Q16：`>MSV` → Execute Result → `>Done` 是否為固定順序？**

---

**Q17：如果 trajectory result 是 ERROR，`>Done` 是否仍然一定送出？**

文件目前的文字表示 YES。

但需要確認 `Done` 在此代表：

> trajectory execution finished

而不是：

> movement successfully reached target

---

# 10. `>MSV ERR` vs Execute Result

### 需要文件作者釐清

**Q18：`>MSV ERR <ErrorCode>` 與 Execute Result `ERR ...` 的差異是什麼？**

建議文件明確區分：

```text id="2b7cxs"
>MSV ERR <ErrorCode>
```

是否表示：

> motion command 在 trajectory 開始前就被拒絕

而：

```text id="0v5qnm"
>Excute Result : ERR ...
```

是否表示：

> trajectory 已開始，執行期間發生問題

目前文件沒有明確定義。

---

**Q19：如果出現 `>MSV ERR <ErrorCode>`，是否還會送 `>Done`？**

與 MOV/MRV 相同，目前只說 trajectory result 後一定 Done。

---

# 11. Servo / System State

### 需要文件作者釐清

**Q20：MSV 是否需要 INS 成功後才能使用？**

---

**Q21：MSV 是否需要 SVO / Servo ON？**

文件只說是 move command，沒有寫 state requirement。

---

**Q22：Servo OFF 時執行 MSV 的 response 是什麼？**

例如：

```text id="c4y8fj"
>MSV ERR <ErrorCode>
```

還是其他 response？

---

**Q23：EtherCAT disconnected 時執行 MSV 的 response 是什麼？**

---

# 12. STP Interaction

因為 MSV 是 trajectory execution command：

### 需要文件作者釐清

**Q24：MSV 執行中收到 STP 是否一定回：**

```text id="8s6qwc"
>Excute Result : ERR STOP
>Done
```

---

# 13. VLS Interaction

VLS Manual 明確說：

> all motion commands including MOV, MRV

MSV 顯然也是 motion command，但文件沒有明確提到。

### 需要文件作者釐清

**Q25：MSV 是否受 VLS global system speed 影響？**

如果 YES，應正式列入 VLS 的 affected command list。

---

**Q26：VLS 與 MSV 的實際速度計算關係為何？**

尤其 MSV 是 single-axis move，需要確認 global speed factor 如何作用。

---

# 14. Absolute Position Semantics

文件說：

> absolute position

### 已明確

MSV 的 target 是 absolute position，而不是 relative offset。

### 需要文件作者釐清

**Q27：absolute position 的 reference coordinate system 是什麼？**

例如：

* machine coordinate
* axis home coordinate
* encoder coordinate
* calibration coordinate

目前只知道是 absolute，還不知道 reference frame。

---

# 15. Position Conversion

MOV 明確寫：

> Position in μm (converted internally)

MSV 沒有提到 conversion。

### 需要文件作者釐清

**Q28：MSV 的 μm / degree position 是否也會經過 internal unit conversion？**

如果會，需要確認：

* μm → pulse
* degree → pulse
* conversion factor 來源

如果不會，則需要說明 MSV 與 MOV 的差異。

---

# 16. Case Sensitivity

**Q29：`MSV` 與 Axis name 是否大小寫敏感？**

例如：

```text id="d7k2pm"
MSV M02 2000
MSV m02 2000
```

目前 parser 是 case-sensitive。

---

# 17. Examples

文件：

```text id="q5v8nx"
MSV M02 2000
MSV M06 1
```

### 已明確

兩個 Example 都與目前 parser：

```text id="x2n9mc"
M01~M16 + floating-point value
```

相容。

而且提供了：

* linear axis example
* angular axis example

這對說明 unit 很有幫助。

### 仍需要釐清

**Q30：M02 / M06 的 unit 是否代表正式固定 Axis mapping？**

也就是：

```text id="n1x6qb"
M02 = μm
M06 = degree
```

如果是，應該正式寫入 Axis definition。

---

# 18. 與目前 Parser 直接比對

| 項目                 | Manual      | 目前 Parser                | 結果             |
| ------------------ | ----------- | ------------------------ | -------------- |
| `MSV`              | 有           | 有                        | 一致             |
| Axis               | `MN`        | M01～M16                  | ⚠️ 規格未定義       |
| M02 example        | 有           | 接受                       | 一致             |
| M06 example        | 有           | 接受                       | 一致             |
| Position           | μm / degree | signed float             | 型態方向一致         |
| Position range     | 未定義         | 未限制                      | 規格缺口           |
| Position precision | 未定義         | float syntax             | 規格缺口           |
| Left tool group    | 有           | parser 無 group semantics | ⚠️ 規格缺口        |
| `>MSV`             | 有           | 有                        | 一致             |
| `>MSV ERR`         | 有           | 有                        | 基本一致           |
| Execute Result     | 有           | baseline 有               | 一致             |
| `THRESHOLD`        | Manual      | baseline 部分使用 `THRSHOLD` | ⚠️ discrepancy |
| `Excute`           | Manual      | baseline 有               | ⚠️ 需確認正式字串     |
| `>Done`            | 有           | 有                        | 一致             |

---

# 最終結論

## 文件已明確

* `MSV` 是 single-axis absolute move。
* 用於 Left tool group。
* 一次移動一個 Axis。
* 有 `<axis>` + `<position>`。
* Position 可以是 μm 或 degree。
* `MSV` / `MSV ERR` response 已定義。
* trajectory result 與 `>Done` 已定義。
* `MSV` 是 absolute move，`MSR` 是 relative move。
* `M02 2000` 與 `M06 1` 提供了兩種 unit 的實例。

## 需要文件作者釐清

本次主要問題：

1. **Format 是否應為 `MSV <axis> <position>`**
2. **`MN` 的正式意義**
3. **Axis 是否為 M01～M16**
4. **M1 與 M01 是否等價**
5. **Left tool group 包含哪些 Axis**
6. **非 Left group Axis 的錯誤行為**
7. **每個 Axis 的 position unit mapping**
8. **position 的正負、range、precision**
9. **absolute position 的 coordinate reference**
10. **MSV 是否需要 INS**
11. **MSV 是否需要 SVO**
12. **Servo OFF / EtherCAT disconnected 的 response**
13. **`>MSV ERR` 與 Execute Result ERR 的差異**
14. **`>MSV ERR` 後是否送 `>Done`**
15. **`>MSV` → Execute Result → `>Done` 是否固定**
16. **STP 中斷 MSV 的正式 response**
17. **MSV 是否受 VLS 影響**
18. **VLS 與 MSV 的 speed calculation**
19. **MSV 是否做與 MOV 相同的 internal unit conversion**
20. **`Excute` 是否正式 protocol 拼字**
21. **`THRESHOLD` / `THRSHOLD` 的正式字串**

### ⚠️ 本次最重要的規格問題

**MSV 的 `position` 單位目前無法由 command 本身判斷。**

文件只說：

> `<position>`：μm or degrees

但實際使用：

```text
MSV M02 2000
MSV M06 1
```

表示**單位很可能由 Axis 決定**。

如果確實如此，文件需要提供完整的：

```text
M01 → ?
M02 → μm
M03 → ?
...
M06 → degree
...
M16 → ?
```

否則 Parser 雖然可以驗證「這是一個合法數字」，卻無法知道產品規格上這個數字對該 Axis 是否合理。這應列為 MSV 的核心規格問題，而不是由我們自行推定。


---

# MSR (Move Relative Single Axis)

以下依照目前 **Command Manual Specification Review** 的既定方式審查 `MSR (Move Relative Single Axis)`。
**這一階段不修改任何程式碼，只找出文件需要釐清的規格問題。**

## MSR — Move Relative Single Axis

### 1. Format

文件：

> `MSR <axis><delta>`

但兩個 Example 都是：

> `MSR M02 2000`
> `MSR M06 1`

因此存在**格式表示不一致**。

目前 parser 的 MSR 規則也是以：

> `<axis> <value>`

的形式解析。

**需要文件作者釐清：**

1. 正式格式是否應為：
   `MSR <axis> <delta>`
2. `<axis><delta>` 是否只是文件排版上的簡寫，而非代表兩者沒有空白？

---

### 2. Arguments

文件：

> `<axis>: MN`

這裡的 `MN` 不明確。

與 MSV 相同，需要確認：

1. `MN` 是 placeholder，還是實際允許輸入的文字？
2. Axis 是否正式限定為 `M01`～`M16`？
3. 是否允許 `M1`～`M16`？
4. `M01` 與 `M1` 是否視為相同 Axis？
5. 是否區分大小寫？

目前 parser 對 MSR 的 Axis 規則是：

* `M01`～`M16`
* 不接受 `M1` 形式
* 大小寫依目前規則為大寫

因此這部分需要 manual author 明確定義。

---

### 3. `<delta>` 數值規格

文件只說：

> Relative offset in μm or degrees

目前缺少：

1. `<delta>` 是否允許負值？

   * 例如 `MSR M02 -200`
2. 是否允許 `+200`？
3. 是否允許小數？

   * 例如 `MSR M02 200.5`
4. 最大／最小值？
5. 精度／小數位數？
6. 最小移動解析度？
7. 是否允許 `0`？
8. 是否允許 scientific notation？
9. 是否允許 hexadecimal？

目前 parser 對 MSR 的數值採 signed decimal floating-point 格式，但這只能代表**目前 parser 的格式驗證行為**，不能當成 Manual 的正式規格。

因此上述內容仍需要文件作者定義。

---

### 4. μm / degree 的 Axis 對應

這是 MSR 最重要的規格缺口之一。

文件只說：

> `<delta>: Relative offset in μm or degrees`

Example：

> `MSR M02 2000` → 2000 μm
> `MSR M06 1` → 1°

因此可以看出不同 Axis 的單位可能不同，但文件沒有完整定義。

需要作者明確提供：

| Axis | Unit |
| ---- | ---- |
| M01  | ?    |
| M02  | ?    |
| ...  | ...  |
| M16  | ?    |

至少需要確認：

1. 哪些 Axis 使用 μm？
2. 哪些 Axis 使用 degree？
3. Unit 是否由 Axis 固定決定？
4. 是否存在其他 Unit？
5. 每個 Axis 的 range 是否不同？
6. 每個 Axis 的 resolution 是否不同？

**不能只從 M02 / M06 兩個 Example 推導其他 Axis 的規則。**

---

### 5. Relative movement 定義

文件說：

> initial position is retained internally.

以及：

> relative to the current position

這裡有一個需要釐清的語意問題。

需要確認：

1. Relative offset 的 reference point 是否就是**收到 MSR 時的實際當前位置**？
2. 如果目前有另一個 motion 正在執行，MSR 的 current position 是：

   * command received 時的位置？
   * trajectory 開始時的位置？
   * trajectory 完成後的位置？
3. `initial position is retained internally` 的「initial position」具體是：

   * MSR 接收瞬間的位置？
   * 上一次 motion 完成位置？
   * Controller 內部 command target？

---

### 6. Response

文件定義：

> `>MSR`

或：

> `>MSR ERR [error code]`

需要釐清：

1. `>MSR` 代表：

   * command accepted？
   * motion started？
   * validation passed？
2. `[error code]` 是 placeholder 還是實際會包含 `[` `]`？
3. Error code 的格式是：

   * decimal？
   * hexadecimal？
   * predefined symbolic code？
4. Error code 有沒有正式列表？

---

### 7. Trajectory Response Sequence

文件定義：

> `>MSR`

或：

> `>MSR ERR [error code]`

接著 trajectory result：

```text
>Excute Result : NORMAL FINISHED
>Excute Result : ERR STOP
>Excute Result : ERR NOT ON TARGET
>Excute Result : ERR
>Excute Result : ERR OVER GAP SENSOR THRESHOLD
```

最後：

```text
>Done
```

文件又特別明確寫：

> After any of the above result messages, the controller will always send: `>Done`

因此這裡相對清楚。

但仍需確認：

1. 正常流程是否固定為：

   `>MSR` → `>Excute Result : NORMAL FINISHED` → `>Done`

2. Error trajectory 是否固定為：

   `>MSR` → `>Excute Result : ERR ...` → `>Done`

3. `>MSR ERR` 是否也會再產生 trajectory result？

4. `>MSR ERR` 後是否也一定送 `>Done`？

5. `>Done` 是否代表「motion execution finished」，而不是「motion successful」？

尤其第 3、4 點很重要，因為 **command-level error** 與 **trajectory execution error** 目前看起來是兩個不同層級。

---

### 8. Response 字串一致性

這裡有兩個需要注意的文字問題。

#### 8.1 `Excute`

文件使用：

> `>Excute Result : ...`

`Excute` 很可能是 `Execute` 的拼字，但目前不能自行修正，因為它可能已經是正式 protocol response string。

需要作者確認：

> 正式 response 是否真的使用 `Excute`，還是應為 `Execute`？

---

#### 8.2 `THRESHOLD`

MSR 使用：

> `ERR OVER GAP SENSOR THRESHOLD`

這與目前 baseline 的其他 command 有文字差異。

目前已經看到：

* MOV：`THRSHOLD`
* MRV：`THRESHOLD`
* MSV：`THRESHOLD`
* MSR：`THRESHOLD`

因此這是目前累積的**跨 command response string discrepancy**。

需要文件作者統一確認正式字串到底是：

> `THRESHOLD`

還是：

> `THRSHOLD`

不能由 parser 端自行修正。

---

### 9. Error Conditions

目前文件列出 trajectory errors，但沒有說明每一種錯誤的觸發條件：

* `ERR STOP`
* `ERR NOT ON TARGET`
* `ERR`
* `ERR OVER GAP SENSOR THRESHOLD`

需要作者定義至少：

1. `ERR STOP` 何時發生？
2. `ERR NOT ON TARGET` 的判定條件？
3. `ERR` 是 general execution error 嗎？
4. `ERR OVER GAP SENSOR THRESHOLD` 的 threshold 數值與單位？
5. threshold 是否每 Axis 不同？
6. 是否有其他可能的 execution result？
7. `MSR ERR [error code]` 與上述 execution result 的區別？

---

### 10. System State / Dependency

文件沒有明確說明 MSR 對 system state 的要求。

需要確認：

1. 是否必須先 `INS`？
2. 是否必須先 `SVO`？
3. Servo OFF 時 MSR 的 response 是什麼？
4. EtherCAT 尚未 connected 時 response 是什麼？
5. `DSC` 後執行 MSR 的 response 是什麼？
6. 如果 Axis 未 configured，response 是什麼？
7. 如果 Axis alarm/error，response 是什麼？

這些屬於 runtime error，不應與 parser 的 `"Invalid parameters"` 混在一起。

---

### 11. 與 STP 的關係

需要確認：

如果：

```text
MSR M02 2000
```

正在執行時收到：

```text
STP
```

則：

1. MSR 是否產生 `ERR STOP`？
2. `STP` 本身 response sequence 是什麼？
3. MSR 最後是否仍會送 `>Done`？
4. `initial position` / current position 如何定義？

---

### 12. 與 VLS 的關係

目前 VLS 定義為 global speed setting，而 MSR 是 motion command。

需要確認：

1. MSR 是否受 VLS 影響？
2. 如果受影響，relative offset 不變，只改變速度？
3. VLS 與 MSR 的實際速度計算公式？
4. MSR 執行途中修改 VLS 是否影響目前 MSR？
5. VLS 是否只影響 MOV/MRV，還是所有 motion command？

---

### 13. 與 MSV 的關係

MSV：

> Move Single Axis — absolute

MSR：

> Move Relative Single Axis — relative

這個基本概念已經清楚。

但仍需確認：

1. MSR 的 current position 是否與 MSV 的 absolute coordinate 使用同一 coordinate system？
2. MSR 的 μm/degree unit mapping 是否與 MSV 完全相同？
3. 同一 Axis 的 range/resolution 是否相同？
4. MSR 是否允許 relative movement 超過 absolute position range？
5. 如果 relative target 超過 Axis limit，產生哪一種 error？

---

## 本次結論

### 文件已明確

以下部分目前沒有主要語意問題：

* MSR 是 **Single Axis Relative Move**
* 使用 relative offset
* M02 example 為 μm
* M06 example 為 degree
* 有 command-level response `>MSR`
* 有 `>MSR ERR [error code]`
* trajectory 有列出的 execution result
* trajectory result 後固定送 `>Done`
* MSR 用於小幅手動修正／incremental adjustment
* 與 MSV 的 absolute / MSR relative 定位關係已表達

### 需要文件作者釐清

本次 MSR 建議提出以下問題：

1. 正式 Format 是否為 `MSR <axis> <delta>`？
2. `<axis>: MN` 中的 `MN` 是 placeholder 還是 literal？
3. Axis 正式範圍是否 M01～M16？
4. 是否接受 M1 與 M01？
5. Axis 是否區分大小寫？
6. `<delta>` 是否允許負值、零值、小數、`+` 號？
7. `<delta>` 的 range、precision、resolution？
8. 是否允許 scientific notation / hexadecimal？
9. 完整 M01～M16 的 μm / degree mapping？
10. 各 Axis 是否有不同 range / resolution？
11. Relative reference point 的正式定義？
12. `initial position is retained internally` 的正式定義？
13. `>MSR` 的確切意義是 accepted、started 還是其他狀態？
14. `[error code]` 是 placeholder 還是 literal format？
15. Error code 的格式與完整定義？
16. `>MSR ERR` 是否會再產生 trajectory result？
17. `>MSR ERR` 是否也會送 `>Done`？
18. `>Done` 的正式語意？
19. `Excute` 是否為正式 protocol spelling，還是應為 `Execute`？
20. `THRESHOLD` 是否為正式字串？需與 MOV/MRV/MSV 統一確認。
21. 四種 trajectory error 的正式觸發條件？
22. `ERR OVER GAP SENSOR THRESHOLD` 的 threshold 定義、單位與判定方式？
23. MSR 是否要求 INS？
24. MSR 是否要求 SVO？
25. Servo OFF / EtherCAT disconnected / Axis alarm 時的 response？
26. STP 中斷 MSR 時的 response sequence？
27. MSR 是否受 VLS 影響？
28. VLS 在 MSR 執行途中修改時是否影響目前 motion？
29. MSR 與 MSV 是否使用完全相同的 Axis unit mapping、range、resolution？
30. relative target 超過 Axis limit 時的正式 error 行為？

**另外記錄一項跨 command discrepancy：**
目前已累積 `MOV` 使用 `THRSHOLD`、而 `MRV / MSV / MSR` 使用 `THRESHOLD`。這應該最後統一交給文件作者確認，**目前不自行修正任何一個字串**。

---

# MPV (Move Particular Axes by Absolutely)

以下依照目前相同的 **Command Manual Specification Review** 方式審查 `MPV (Move Particular Axes by Absolutely)`。
本次仍然**不修改任何程式碼，也不自行推定文件作者意圖**。

# MPV — Move Particular Axes by Absolutely

## 1. Format

文件：

> `MPV [Mn] [Pn]`

Example：

> `MPV M01 M03 1200.0 -35.5`

這裡格式表示仍然不夠明確。

主要問題是 `[Mn]` 與 `[Pn]` 看起來代表兩個 list，但實際 command 沒有明確分隔符號。

需要作者釐清：

1. `[Mn]` 是否代表一個或多個 Axis token？
2. `[Pn]` 是否代表一個或多個 Position token？
3. 正式語法是否其實是：
   `MPV <M1> <M2> ... <Mn> <P1> <P2> ... <Pn>`？
4. Axis 與 Position 在 command 中如何知道哪裡是分界？
5. 是否允許任意數量的 Axis？
6. 是否允許只指定一個 Axis？

這一點非常重要，因為目前 Example：

```text
MPV M01 M03 1200.0 -35.5
```

實際解析方式應該是：

```text
Axis:     M01 M03
Position: 1200.0 -35.5
```

但 Manual 沒有正式定義這個分界規則。

---

# 2. Arguments — `<Mn>`

文件：

> `<Mn>: Axis name string (e.g., “M01 M08”)`

這裡需要進一步定義。

### Axis 範圍

需要確認：

* 是否正式限定 `M01`～`M16`？
* 是否接受 `M1`～`M16`？
* `M01` 與 `M1` 是否等價？
* 是否允許重複 Axis，例如：

```text
MPV M01 M01 100 200
```

* Axis 是否必須按照 M01 → M16 順序排列？
* 還是可以任意順序，例如：

```text
MPV M08 M01 100 200
```

目前 parser 有 M01～M16 的格式限制，但這些 semantic rules 並不能由目前文件直接確定。

---

# 3. `<Pn>` Position 規格

文件只說：

> Position values corresponding to each axis in Mn

但沒有定義：

* position 是否 signed？
* 是否允許負值？
* 是否允許小數？
* 最大／最小值？
* precision？
* resolution？
* 是否允許 `+`？
* 是否允許 scientific notation？
* 是否允許 hexadecimal？

Example：

```text
1200.0
-35.5
```

只能證明 Example 使用 decimal floating-point，不能由此推導完整 numeric specification。

---

# 4. Axis ↔ Position 對應關係

文件明確說：

> The number of axes in Mn must exactly match the number of values in Pn.

這部分是**明確的**。

例如：

```text
MPV M01 M03 1200.0 -35.5
```

應理解為：

```text
M01 → 1200.0
M03 → -35.5
```

但仍需要確認：

1. Position 是依 Axis 出現順序一一對應嗎？
2. Axis 順序是否影響 Position mapping？
3. 是否允許 Axis 順序任意排列？
4. 重複 Axis 是否禁止？

---

# 5. Axis Unit

這是 MPV 一個很重要的規格缺口。

文件只說：

> Position values corresponding to each axis

沒有說 Position 的 unit。

而前面的：

* MOV
* MRV
* MSV
* MSR

已經涉及 μm / degree。

因此需要確認 MPV：

1. M01～M16 各自使用 μm 還是 degree？
2. 是否與 MSV 完全相同？
3. 是否與 MOV 完全相同？
4. 是否存在同一 command 中不同 Axis 使用不同 unit？
5. Position range 是否依 Axis 不同？

例如 Example：

```text
MPV M01 M03 1200.0 -35.5
```

目前无法从 Manual 判断：

* M01 的 `1200.0` 是 μm 還是 degree？
* M03 的 `-35.5` 是 μm 還是 degree？

這是必須由文件作者明確定義的。

---

# 6. Axis 數量

需要確認：

1. 最少可以指定幾個 Axis？
2. 最多可以指定幾個 Axis？
3. 是否最多 16 個？
4. 是否允許一次指定全部 Axis？
5. 如果全部 Axis 都指定，與 MOV 是否等價？
6. Axis 數量是否一定 ≥1？

---

# 7. Response

文件：

```text
>MPV
```

或：

```text
>MPV ERR [ErrorCode]
```

需要釐清：

1. `>MPV` 代表 command accepted 還是 motion started？
2. `[ErrorCode]` 是否只是 placeholder？
3. ErrorCode 的格式？
4. ErrorCode 的完整列表？
5. `>MPV ERR` 是 parser/format error，還是 execution/runtime error？

目前 parser 如果 syntax 不符合規則，會產生：

```text
Invalid parameters
```

這與：

```text
>MPV ERR [ErrorCode]
```

應該是不同層級，需要 Manual 明確區分。

---

# 8. Trajectory Response

文件定義：

```text
>Excute Result : NORMAL FINISHED
>Excute Result : ERR STOP
>Excute Result : ERR NOT ON TARGET
>Excute Result : ERR
>Excute Result : ERR OVER GAP SENSOR THRESHOLD
```

然後：

```text
>Done
```

並明確寫：

> After any of the above result messages, the controller will always send: `>Done`

因此這部分規格相對清楚：

**Trajectory result → `>Done`**

但仍需確認：

1. `>MPV ERR [ErrorCode]` 是否也會送 `>Done`？
2. `>MPV` → execution result → `>Done` 是否為固定 sequence？
3. `>Done` 是否一定代表 trajectory 結束，而不是成功？
4. Error result 是否仍代表 trajectory 已經結束？

---

# 9. `Excute` 字串

同前面的 MOV / MRV / MSV / MSR：

```text
>Excute Result : ...
```

`Excute` 是否為正式 protocol string，還是 `Execute` 的 typo，需要文件作者確認。

**不能由我們自行修改。**

---

# 10. `THRESHOLD` 字串

MPV 使用：

```text
>Excute Result : ERR OVER GAP SENSOR THRESHOLD
```

目前與：

* MRV：`THRESHOLD`
* MSV：`THRESHOLD`
* MSR：`THRESHOLD`

一致。

但與 MOV baseline 的：

```text
THRSHOLD
```

不同。

因此目前應繼續記錄為：

> **Global response string discrepancy：`THRESHOLD` vs `THRSHOLD`**

最後統一詢問文件作者。

---

# 11. Error Conditions

文件列出幾種 result，但沒有定義條件。

需要確認：

### `ERR STOP`

什麼情況產生？

例如：

* STP？
* safety stop？
* user stop？
* hardware stop？

### `ERR NOT ON TARGET`

需要定義：

* Position tolerance？
* tolerance 是多少？
* 每個 Axis 是否不同？
* unit 是 μm 還是 degree？

### `ERR`

這個 generic error 的正式定義？

### `ERR OVER GAP SENSOR THRESHOLD`

需要定義：

* GAP sensor 是什麼？
* threshold 是多少？
* 單位？
* 每 Axis 是否不同？
* 是否所有 MPV Axis 都可能觸發？

---

# 12. 「Axes not listed will remain unchanged」

這一句很重要，而且目前語意基本清楚：

> Axes not listed will remain unchanged.

但仍需要確認：

1. 「unchanged」是 target position 不變，還是實際位置也保證完全不變？
2. 未列出的 Axis 是否會完全不參與 trajectory？
3. 未列出的 Axis 是否仍可能因機構耦合而產生物理位移？
4. 如果指定 Axis 中有一個 execution error，其他指定 Axis 是否停止？

尤其最後一點是 partial-axis command 的重要行為。

---

# 13. Multi-Axis Execution

MPV 最大的規格缺口之一，是它一次可以控制多個 Axis，但沒有定義這些 Axis 如何執行。

例如：

```text
MPV M01 M03 M05 1000 2000 3000
```

需要確認：

1. 三個 Axis 是同步開始嗎？
2. 是否同步結束？
3. 是否做 coordinated trajectory？
4. 還是依序執行？
5. 如果其中一個 Axis 到達 target，其他 Axis 尚未完成，系統如何處理？
6. `NORMAL FINISHED` 的判定條件是所有指定 Axis 都完成嗎？
7. `NOT ON TARGET` 是單一 Axis 還是任一 Axis 失敗？
8. Error response 是否會指出是哪個 Axis？

這些對 MPV 的實際行為非常重要。

---

# 14. Axis 重複

需要明確定義是否允許：

```text
MPV M01 M01 1000 2000
```

如果不允許：

* 是 parser error？
* 還是 `>MPV ERR [ErrorCode]`？

如果允許：

* 最終 target 是 1000 還是 2000？
* 或者是非法但尚未定義？

目前不能自行推定。

---

# 15. Axis 順序

例如：

```text
MPV M01 M03 1000 2000
```

與：

```text
MPV M03 M01 2000 1000
```

是否完全等價？

如果是，表示 Position mapping 是按照 Axis token 的出現順序。

如果不是，Manual 必須定義 Axis ordering rule。

---

# 16. VLS 關係

需要確認 MPV 是否受到 `VLS` global speed setting 影響。

如果受影響，需要定義：

1. MPV 是否使用 VLS？
2. VLS 如何影響 MPV trajectory？
3. 多 Axis 是否各自套用 VLS？
4. MPV 執行途中修改 VLS 是否影響正在執行的 MPV？

---

# 17. STP 關係

需要確認：

```text
MPV ...
```

執行中收到：

```text
STP
```

時：

1. MPV 是否產生 `ERR STOP`？
2. 所有正在移動的指定 Axis 是否停止？
3. `>Done` 是否仍然送出？
4. 未列出的 Axis 是否維持不動？

---

# 18. INS / SVO / DSC dependency

需要確認：

1. MPV 是否必須先 `INS`？
2. 是否必須 `SVO`？
3. Servo OFF 時 response？
4. EtherCAT disconnected 時 response？
5. DSC 後執行 MPV 時 response？
6. 某個指定 Axis 沒有成功 Servo ON 時，整個 MPV 如何處理？
7. 某個 Axis alarm 時，是整個 command fail 還是其他 Axis 繼續？

---

# 19. 與 MOV 的關係

MPV 與 MOV 都是 absolute movement。

需要文件作者確認：

1. MPV 是否只是 MOV 的 partial-axis 版本？
2. MPV 是否使用與 MOV 完全相同的 coordinate system？
3. MPV 的 Position unit/range/resolution 是否與 MOV 相同？
4. MOV 的 R/L/PR/LP 不同 side/coordinate mode 是否與 MPV 無關？
5. MPV 是否可以達到與 MOV 相同的六軸 absolute target？
6. MPV 的 trajectory algorithm 是否與 MOV 相同？

---

# 20. Example

Example：

```text
MPV M01 M03 1200.0 -35.5
```

可以清楚表達：

```text
M01 → 1200.0
M03 → -35.5
```

但 Example 本身仍不足以確認：

* M01 unit
* M03 unit
* M01/M03 是否允許負值
* precision
* range
* 是否同步執行

因此不能從 Example 自行推導這些規格。

---

# 本次結論

## 文件已明確

以下部分目前可以視為清楚：

* MPV 是 partial-axis **absolute movement**
* 可以指定 subset of axes
* Position 數量必須與 Axis 數量完全一致
* 未指定 Axis 不應被 command 主動控制
* 有 `>MPV` / `>MPV ERR [ErrorCode]`
* trajectory 有明確列出的五種 result
* 每一種 trajectory result 後都會送 `>Done`
* Example 已示範多 Axis + 對應 Position

## 需要文件作者釐清

本次建議正式記錄：

1. `MPV [Mn] [Pn]` 的正式 grammar 與 Axis/Position 分界方式。
2. `[Mn]` 是否代表一個或多個 Axis。
3. `[Pn]` 是否代表一個或多個 Position。
4. Axis 正式範圍是否 M01～M16。
5. 是否接受 M1～M16。
6. 是否允許重複 Axis。
7. Axis 是否必須依固定順序排列。
8. Position 是否依 Axis 出現順序一一對應。
9. Position 的正負、零值、小數、range、precision、resolution。
10. M01～M16 完整的 μm / degree mapping。
11. MPV 最少／最多可指定幾個 Axis。
12. `>MPV` 的正式意義。
13. `[ErrorCode]` 的正式格式與 error code 定義。
14. `>MPV ERR` 與 trajectory execution error 的層級差異。
15. `>MPV ERR` 是否會再送 `>Done`。
16. `>Done` 的正式語意。
17. `Excute` 是否為正式 protocol spelling。
18. `ERR STOP` 的觸發條件。
19. `ERR NOT ON TARGET` 的 tolerance / 判定方式。
20. `ERR OVER GAP SENSOR THRESHOLD` 的 threshold 定義。
21. 多 Axis 是同步、協調還是依序執行。
22. 多 Axis 中單一 Axis failure 時其他 Axis 的行為。
23. `NORMAL FINISHED` 是否代表所有指定 Axis 都完成。
24. 未列 Axis 的「unchanged」在實際 trajectory 中的正式定義。
25. MPV 是否受 VLS 影響。
26. STP 與 MPV 的 interaction。
27. INS / SVO / DSC / Servo state 下 MPV 的正式行為。
28. MPV 與 MOV 是否共用相同 coordinate、unit、range、resolution 規則。

**另外保留前面已發現的 global discrepancy：**
`THRESHOLD` vs `THRSHOLD`，以及 `Excute` vs `Execute`，目前都不自行修改，最後統一交由文件作者確認。

---

# MPR (Move Particular Axes by Relatively)

以下依照前面相同的 **Command Manual Specification Review** 方式審查 `MPR (Move Particular Axes by Relatively)`。
本次仍然**不修改程式碼、不自行推定文件作者意圖**。

# MPR — Move Particular Axes by Relatively

## 1. Format

文件：

> `MPR [Mn] [Pn]`

Example：

> `MPR M01 M03 1200.0 -35.5`

與 MPV 完全相同，因此同樣存在一個重要的 grammar 問題：

目前文件沒有明確定義：

* `[Mn]` 在哪裡結束？
* `[Pn]` 從哪裡開始？
* 是否靠 `M01`～`M16` 的 token 型態來區分？
* Axis 與 Position 是否一定按照先全部 Axis、再全部 Position 的方式排列？

需要作者確認正式 grammar 是否為：

```text
MPR <M1> <M2> ... <Mn> <P1> <P2> ... <Pn>
```

以及是否允許 `n = 1`。

---

# 2. `<Mn>` Axis 規格

文件：

> `<Mn>: Axis name string (e.g., “M01 M08”)`

需要確認：

1. Axis 是否正式限定 M01～M16？
2. 是否接受 M1～M16？
3. M01 與 M1 是否等價？
4. 是否區分大小寫？
5. 是否允許重複 Axis？
6. Axis 是否必須按照 M01～M16 排序？
7. 是否允許任意順序，例如：

```text
MPR M08 M01 100 -200
```

---

# 3. `<Pn>` Relative Offset 規格

文件稱：

> Relative position offsets

但沒有定義 numeric format。

需要確認：

1. 是否允許正值？
2. 是否允許負值？
3. 是否允許 `0`？
4. 是否允許小數？
5. 最大／最小 relative offset？
6. precision？
7. resolution？
8. 是否允許 `+` 號？
9. 是否允許 scientific notation？
10. 是否允許 hexadecimal？

Example：

```text
MPR M01 M03 1200.0 -35.5
```

只能證明目前 Example 使用 signed decimal floating-point，不能由此推導完整 numeric specification。

---

# 4. Axis 與 Offset 的對應

文件明確規定：

> The number of axes in Mn must exactly match the number of values in Pn.

這一點是清楚的。

Example：

```text
MPR M01 M03 1200.0 -35.5
```

應理解為：

```text
M01 → +1200.0
M03 → -35.5
```

但仍需要確認：

1. 是否按照 Axis 出現順序配對？
2. Axis 可以任意排序嗎？
3. Position 是否也按照相同順序配對？
4. 重複 Axis 是否禁止？

---

# 5. Unit Mapping

這是 MPR 很重要的規格缺口。

文件只說：

> relative distance / relative position offsets

沒有說 offset 的單位。

前面的 MSR 已經出現：

* μm
* degree

因此需要作者確認 MPR 是否使用完全相同的 Axis unit mapping：

| Axis | MPR Offset Unit |
| ---- | --------------- |
| M01  | ?               |
| M02  | ?               |
| ...  | ...             |
| M16  | ?               |

尤其 Example：

```text
MPR M01 M03 1200.0 -35.5
```

目前無法從文件確定：

* `1200.0` 是 μm 還是 degree？
* `-35.5` 是 μm 還是 degree？

不能自行從 MSR 或 MSV 推導。

---

# 6. 「Relative」的 reference point

MPR 是 relative movement，因此需要明確定義 offset 的 reference。

需要確認：

1. offset 是否相對於 command 收到時的 current position？
2. 還是 trajectory 開始時的位置？
3. 如果前一個 motion 尚未完成，current position 如何定義？
4. MPR 執行完後的新 target 是否：

`current position + relative offset`

5. 如果連續送兩個 MPR，第二個 MPR 的 reference 是否包含第一個 MPR 的實際完成位置？

---

# 7. Multi-Axis Execution

MPR 一次可以指定多個 Axis，因此需要確認 execution model。

例如：

```text
MPR M01 M03 M05 100 -20 300
```

需要確認：

1. 三個 Axis 是否同步執行？
2. 是否 coordinated motion？
3. 還是各 Axis 個別執行？
4. 是否同時開始？
5. 是否同時完成？
6. `NORMAL FINISHED` 是否代表所有指定 Axis 都完成？
7. 如果其中一個 Axis 發生 error，其他 Axis 是否停止？
8. 是否有 partial completion？

這些目前文件沒有定義。

---

# 8. Axes not listed

文件：

> Axes not listed will remain stationary.

語意比 MPV 的：

> Axes not listed will remain unchanged.

更直接。

但仍需要確認：

1. 是否表示 controller 不會對未列 Axis 發送 motion command？
2. 如果機械結構有耦合，物理位置被動改變是否算違反這個定義？
3. 未列 Axis 是否完全排除在 trajectory planning 外？
4. 如果指定 Axis 發生 error，未指定 Axis 是否始終不動？

---

# 9. Response

文件：

```text
>MPR
```

或：

```text
>MPR ERR [ErrorCode]
```

需要確認：

1. `>MPR` 是 accepted 還是 started？
2. `[ErrorCode]` 是否為 placeholder？
3. ErrorCode 格式？
4. ErrorCode 完整列表？
5. `MPR ERR` 是 command-level/runtime error，還是 parameter error？

這點應與 MPV 保持一致，但仍應由文件明確定義，而不是我們自行假設。

---

# 10. Trajectory Response Sequence

文件明確列出：

```text
>Excute Result : NORMAL FINISHED
>Excute Result : ERR STOP
>Excute Result : ERR NOT ON TARGET
>Excute Result : ERR
>Excute Result : ERR OVER GAP SENSOR THRESHOLD
```

以及：

```text
>Done
```

並明確說：

> After any of the above result messages, the controller will always send `>Done`.

因此這部分基本明確。

仍需要確認：

1. 是否固定：

`>MPR` → `>Excute Result : ...` → `>Done`

2. `>MPR ERR` 是否也會送 `>Done`？
3. `>Done` 是否代表 trajectory 結束，而非成功？
4. Error trajectory 是否一定以 `>Done` 結束？

---

# 11. `Excute` 拼字

與前面 MOV / MRV / MSV / MSR / MPV 完全相同：

> `>Excute Result : ...`

需要文件作者確認：

* `Excute` 是否為正式 protocol string？
* 還是應為 `Execute`？

這應列入 **global response string clarification**。

---

# 12. `THRESHOLD`

MPR 使用：

> `ERR OVER GAP SENSOR THRESHOLD`

目前：

* MRV：`THRESHOLD`
* MSV：`THRESHOLD`
* MSR：`THRESHOLD`
* MPV：`THRESHOLD`
* MOV：baseline 曾出現 `THRSHOLD`

因此仍然存在 global discrepancy：

> `THRESHOLD` vs `THRSHOLD`

MPR 本身與 MPV/MRV/MSV/MSR 一致，但整體 command family 尚未完全一致。

---

# 13. Error Conditions

目前文件只列出結果名稱，沒有定義觸發條件。

需要作者定義：

### `ERR STOP`

* STP？
* safety stop？
* hardware stop？
* 其他？

### `ERR NOT ON TARGET`

* target tolerance 是多少？
* tolerance 是否依 Axis 不同？
* μm / degree 的 tolerance 是否不同？

### `ERR`

* generic execution error 的正式定義？

### `ERR OVER GAP SENSOR THRESHOLD`

* threshold 數值？
* 單位？
* 每 Axis 是否不同？
* 如何判定？

---

# 14. Position Limit / Relative Limit

這是 MPR 特別需要釐清的地方。

假設目前位置是：

```text
M01 = 9000
```

然後：

```text
MPR M01 2000
```

如果 Axis 最大位置是 `10000`，新 target 就可能變成 `11000`。

需要定義：

1. Relative offset 本身有 range 嗎？
2. 最終 absolute target 有 range 嗎？
3. 如果超過 limit：

   * command-level error？
   * trajectory error？
   * 哪個 ErrorCode？
4. 是否 clamp 到 maximum？
5. 是否完全拒絕？

---

# 15. STP Interaction

需要確認 MPR 執行期間收到 `STP`：

1. 是否產生：

```text
>Excute Result : ERR STOP
```

2. 是否一定接著：

```text
>Done
```

3. 多 Axis 是否全部停止？
4. 已完成的 Axis 是否保持目前位置？
5. 未完成 Axis 是否回到原 position？

---

# 16. VLS Interaction

需要確認 MPR 是否受 `VLS` 影響。

如果受影響：

1. relative offset 不變，只改速度？
2. VLS 是否套用到所有指定 Axis？
3. 不同 unit Axis 是否使用相同 VLS factor？
4. MPR 執行途中修改 VLS 是否影響當前 motion？

---

# 17. INS / SVO / DSC Dependency

需要確認：

1. MPR 是否必須先 `INS`？
2. 是否必須先 `SVO`？
3. Servo OFF 時 response？
4. EtherCAT disconnected 時 response？
5. DSC 後執行 MPR 的 response？
6. 某個指定 Axis 沒有 Servo ON 時整個 command 如何處理？
7. 某個 Axis alarm 時其他指定 Axis 是否仍執行？

---

# 18. 與 MSR 的關係

MPR 與 MSR 都是 relative movement，但：

* MSR：single axis
* MPR：multiple particular axes

因此需要確認：

1. MPR 是否就是 MSR 的 multi-axis version？
2. 是否完全共用：

   * unit mapping
   * position range
   * resolution
   * VLS
   * error conditions
   * trajectory algorithm
3. 單 Axis 的 MPR 是否與 MSR 完全等價？

例如：

```text
MSR M01 100
```

與：

```text
MPR M01 100
```

是否必須得到完全相同的 motion behavior / response？

這是一個很值得文件作者明確定義的 cross-command relationship。

---

# 19. 與 MPV 的關係

MPV：

> Particular Axes by Absolutely

MPR：

> Particular Axes by Relatively

兩者 command structure 完全相似。

需要確認：

1. 是否只有 absolute / relative reference 不同？
2. Axis parsing 規則是否完全相同？
3. Axis ordering 是否相同？
4. duplicate Axis policy 是否相同？
5. unit mapping 是否相同？
6. range/resolution 是否相同？
7. multi-axis synchronization 是否相同？
8. error conditions 是否相同？
9. trajectory result 是否完全相同？

目前兩份 Manual 看起來非常相似，但沒有正式說明這些 cross-command rules。

---

# 20. Example

文件：

```text
MPR M01 M03 1200.0 -35.5
```

可以合理確認：

```text
M01 → +1200.0
M03 → -35.5
```

但 Example 無法確認：

* M01 unit
* M03 unit
* allowed range
* precision
* 是否同步執行

因此不能把 Example 當成完整規格。

---

# 本次結論

## 文件已明確

* MPR 是 partial-axis **relative movement**。
* 可以指定 subset of axes。
* Axis 數量必須等於 offset 數量。
* 未列 Axis 應保持 stationary。
* 有 `>MPR` / `>MPR ERR [ErrorCode]`。
* 有五種 trajectory result。
* 每種 trajectory result 後都會送 `>Done`。
* Example 已示範多 Axis、正負 relative offset。

## 需要文件作者釐清

本次建議正式記錄：

1. `MPR [Mn] [Pn]` 的正式 grammar 與 Axis/Offset 分界。
2. Axis 正式範圍是否 M01～M16。
3. 是否接受 M1～M16。
4. Axis 是否區分大小寫。
5. 是否允許 duplicate Axis。
6. Axis 是否有固定排序要求。
7. Offset 是否依 Axis 出現順序一一對應。
8. Offset 的正負、零值、小數、range、precision、resolution。
9. M01～M16 完整的 μm / degree mapping。
10. 最少／最多可以指定幾個 Axis。
11. Relative reference point 的正式定義。
12. 多 Axis 是同步、協調還是個別執行。
13. 單一 Axis execution failure 時其他指定 Axis 的行為。
14. `>MPR` 的正式意義。
15. `[ErrorCode]` 的格式與完整定義。
16. `>MPR ERR` 是否會再送 `>Done`。
17. `>Done` 的正式語意。
18. 五種 trajectory error 的正式觸發條件。
19. Relative offset 超出 absolute Axis limit 時的行為。
20. STP 中斷 MPR 時的正式 response sequence。
21. MPR 是否受 VLS 影響。
22. INS / SVO / DSC / Servo state 下 MPR 的正式行為。
23. MPR 與 MSR 是否共用完全相同的 unit/range/resolution 規則。
24. 單 Axis 的 MPR 是否與 MSR 完全等價。
25. MPR 與 MPV 是否除 absolute/relative 外，其餘 execution rules 完全相同。
26. `Excute` 是否為正式 protocol spelling。
27. `THRESHOLD` 是否為正式 protocol string，並與 MOV 的 `THRSHOLD` 統一確認。

**目前 MPR 與 MPV 的規格結構高度相似，因此最後做正式問題清單時，可以把兩者的共同問題合併成一個「MPV/MPR 共通規格」問題，只有 absolute/relative 特有問題另外列出。**

---

# MOV? (Is Moving)

以下依照目前相同的 **Command Manual Specification Review** 方式審查 `MOV? (Is Moving)`。
本次仍然**不修改程式碼，也不自行推定文件作者意圖**。

# MOV? — Is Moving

## 1. Format

文件：

> `MOV?`

這部分明確。

目前 parser 也將 `MOV?` 定義為無參數 command，與 Manual 一致。

---

## 2. Arguments

文件：

> Arguments: None

這部分明確。

需要補充確認的是：如果使用者錯誤輸入：

```text
MOV? 1
```

應該得到什麼 response？

例如：

* `Invalid parameters`
* `Invalid command format`
* 其他正式 error response

目前 Manual 沒有定義。

---

## 3. Status 定義

文件：

> Returns “1” if any axis is still in motion, “0” if system is idle.

這裡基本清楚：

```text
> MOV? 1
```

= 至少一個 Axis 正在 motion。

```text
> MOV? 0
```

= system idle。

但需要釐清「motion」的範圍。

---

## 4. 「all types of absolute or relative movement commands」

文件說：

> This includes all types of absolute or relative movement commands.

這個描述需要具體化。

目前至少包含：

* MOV
* MRV
* MSV
* MSR
* MPV
* MPR

但需要作者正式確認：

1. 是否包含全部六種 command？
2. 是否包含未來新增的 motion command？
3. 是否包含 MAV？
4. 是否包含其他 trajectory / homing movement？
5. `SAH` Homing 是否算 motion？
6. 其他自動運動 routine 是否算 motion？

這裡尤其要注意，之前 SVO Manual 曾經提到 `MAV`，但目前 parser / command review baseline 並沒有完成對 MAV 的正式定義，因此不能自行判斷 MOV? 是否包含它。

---

## 5. 「any axis」的範圍

需要確認：

> any axis

是否代表：

* M01～M16 所有 Axis？
* 只有已 configured Axis？
* 只有 Servo ON Axis？
* 所有 physical axes？
* Left / Right tool group 都包含？

例如：

```text
M01 moving
M02 idle
M03 idle
```

→ `MOV? 1` 很清楚。

但如果：

```text
M01 idle
M02 alarm
M03 not configured
```

是否仍為：

```text
MOV? 0
```

需要正式定義。

---

# 6. Response Format

文件：

> `>MOV? <status>`

Example：

> `>MOV? 1`

目前 baseline 也是：

```text
>MOV? 1
>MOV? 0
```

因此 response format 基本一致。

需要確認：

1. `<status>` 是否**只允許 `0` / `1`**？
2. 是否可能有其他 status？
3. 是否一定為 decimal integer？
4. 是否允許 `true/false`？
5. 是否永遠只有一行 response？

目前從 Manual 看起來應該是 0/1，但正式規格最好明確寫成：

> `<status>: 0 or 1`

---

# 7. Response Timing

這個 command 的核心問題之一是 timing。

文件說：

> Query whether the system is currently executing any motion.

但沒有定義 query 的取樣時間點。

需要確認：

1. status 是 command 被收到**瞬間**的狀態？
2. 還是 response 產生瞬間的狀態？
3. 是否存在 motion state 更新 latency？
4. 如果 motion 剛好在 query 執行期間完成，回傳 0 還是 1？

例如：

```text
MOV? → motion 完成 → response
```

到底回：

```text
>MOV? 1
```

還是：

```text
>MOV? 0
```

需要文件作者定義。

---

# 8. Multiple Axis Motion

如果：

```text
M01 moving
M03 moving
M05 idle
```

則文件目前可以理解為：

```text
>MOV? 1
```

但需要確認：

1. 只要一個 Axis moving 就是 1？
2. 必須所有 Axis 都 idle 才是 0？
3. 是否存在「部分 motion 完成」狀態？

這應明確寫成 boolean rule。

---

# 9. Motion Completion / `>Done` 關係

這點非常重要。

目前 motion commands 有：

```text
>Excute Result : ...
>Done
```

MOV? 則是即時查詢。

需要確認：

1. `>Done` 發送前，MOV? 是否一定回 `1`？
2. `>Done` 發送後，MOV? 是否一定立即回 `0`？
3. 如果 `>Done` 已經送出但 Axis 實際仍在 settling，MOV? 回什麼？
4. `>Done` 與 `MOV? 0` 是否代表完全相同的 motion state？

不能自行假設兩者完全等價。

---

# 10. STP 關係

需要確認：

如果 Motion 正在執行：

```text
MOV? → 1
```

接著執行：

```text
STP
```

在不同時間點查詢：

```text
MOV?
```

需要定義：

* STP accepted 後是否立即變成 0？
* 還是等 trajectory 完全停止後才變 0？
* 如果 STP 導致 `ERR STOP` → `Done`，哪一個事件才算 idle？

---

# 11. Homing / SAH 關係

這是一個重要 cross-command 問題。

SAH 是 homing operation，而且涉及 Axis movement。

需要確認：

```text
SAH M01
```

執行期間：

```text
MOV?
```

應該回：

```text
>MOV? 1
```

嗎？

文件目前說：

> all types of absolute or relative movement commands

但沒有明確提到 homing。

因此需要作者確認：

> `MOV?` 是否將 SAH homing 視為 motion？

---

# 12. Servo State

需要確認：

### Servo OFF

如果所有 Servo 都 OFF：

```text
MOV?
```

是否：

```text
>MOV? 0
```

這應該很可能是，但不能自行替文件定義。

### Servo ON but idle

應：

```text
>MOV? 0
```

這部分語意比較清楚，但仍可列為正式狀態定義。

---

# 13. INS / DSC State

需要確認：

### 尚未 INS

執行：

```text
MOV?
```

是：

```text
>MOV? 0
```

還是：

```text
Error
```

### DSC 後

執行：

```text
MOV?
```

是：

```text
>MOV? 0
```

還是 disconnected error？

這是 query command 的重要 state behavior。

---

# 14. Error Conditions

Manual 沒有定義任何 error response。

需要確認：

1. `MOV?` 是否永遠都能回答 0/1？
2. EtherCAT disconnected 時是否仍可 query？
3. Controller internal error 時如何回答？
4. Axis communication failure 是否會影響 status？
5. 如果 motion state 不可取得，是否有第三種 error response？

尤其目前格式明確限定：

```text
>MOV? <status>
```

所以如果 status 無法取得，error behavior 必須另外定義。

---

# 15. Automated Script / Sequencing

文件說：

> Always check this before issuing subsequent commands in an automated script.

這是一個**使用建議**，但還不算完整 protocol rule。

需要確認：

1. 是否規定後續 motion command 必須等待 `MOV? 0`？
2. Controller 本身是否會拒絕 motion-overlap command？
3. 如果不先查 MOV? 就送下一個 motion command，會：

   * queue？
   * reject？
   * interrupt current motion？
   * merge？
4. MOV? 是建議使用，還是 protocol 上的必要 sequencing mechanism？

這會影響 End User / CM5 Client 的 command sequencing behavior。

---

# 16. 與其他 Motion Commands 的關係

需要確認 MOV? 是否涵蓋：

| Command          | 是否包含於 MOV? |
| ---------------- | ---------- |
| MOV              | ?          |
| MRV              | ?          |
| MSV              | ?          |
| MSR              | ?          |
| MPV              | ?          |
| MPR              | ?          |
| SAH              | ?          |
| MAV              | ?          |
| 其他 future motion | ?          |

目前 Manual 只明確說：

> all types of absolute or relative movement commands

因此六個已審查 movement commands 應該需要作者正式確認。

---

# 17. Example

Example：

```text
MOV?
>MOV? 1
```

這只證明：

> Example 的查詢當下 system 有 motion。

但它沒有說明：

* 哪個 Axis？
* 哪種 motion？
* 何時開始？
* 何時完成？

因此 Example 本身沒有問題，但不足以定義完整 behavior。

---

# 本次結論

## 文件已明確

* `MOV?` 無參數。
* Response 格式為 `>MOV? <status>`。
* `1` = 至少有 motion。
* `0` = system idle。
* Query 是即時 motion-state query。
* 用於 automated sequencing / wait-until-complete。
* 目前與 baseline 的 `>MOV? 1 / >MOV? 0` response format 一致。

## 需要文件作者釐清

1. `<status>` 是否正式限定只有 `0` / `1`？
2. `MOV?` 是否永遠只回一個 status response？
3. status 的取樣時間點是 command received 還是 response generated？
4. 「all types of absolute or relative movement」完整包含哪些 commands？
5. `MOV`、`MRV`、`MSV`、`MSR`、`MPV`、`MPR` 是否全部包含？
6. `SAH` homing 是否算 motion？
7. `MAV` 是否包含？
8. `any axis` 的正式 Axis scope 是什麼？
9. 是否只要一個 Axis moving 就回 `1`？
10. `>Done` 與 `MOV? 0` 是否具有相同的 motion-complete 定義？
11. `>Done` 發送時是否保證 `MOV?` 已經是 `0`？
12. STP 執行後何時由 `1` 變成 `0`？
13. INS 前執行 MOV? 的 response？
14. DSC 後執行 MOV? 的 response？
15. Servo OFF / Axis alarm / EtherCAT disconnected 時的 response？
16. 如果 motion state 無法取得，是否存在 error response？
17. Automated script 中 `MOV? 0` 是建議還是必要 sequencing condition？
18. 如果 motion 尚未完成就送下一個 motion command，正式行為是 reject、queue、interrupt 還是其他？
19. 如果多個 Axis 同時 motion，是否只要任一 Axis moving 即回 `1`？
20. 未 configured Axis、alarm Axis 是否影響 `MOV?` status？

### 特別記錄

這個 command 本身**沒有發現與目前 parser response baseline 的直接文字矛盾**。
但它與前面 `MOV / MRV / MSV / MSR / MPV / MPR / SAH / STP` 有較多 **cross-command state relationship**，建議最後文件問題清單統一整理，而不要每個 command 重複問一次相同問題。

---


































































































