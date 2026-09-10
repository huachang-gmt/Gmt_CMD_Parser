
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








































































































































































