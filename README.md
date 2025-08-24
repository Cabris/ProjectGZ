# 物品拾取 / 互動 / 背包系統（UE5 + GAS）

> 以 Gameplay Ability System（GAS）與資料驅動設計，實作在編輯器即可擴充的「可互動物件 → 對應能力」機制；並提供物品定義/實例分離的背包系統，方便網路同步與儲存。摘自[筆記](https://hackmd.io/@AazYIRv0S5C9XgggT80SOA/r1CLK7LYge)。 

## 專案
- Repo：<https://github.com/Cabris/ProjectGZ> 

## Demo
[![Watch the demo](https://img.youtube.com/vi/EooB9iTYJ4I/hqdefault.jpg)](https://www.youtube.com/watch?v=EooB9iTYJ4I "Watch the demo")

---

## 系統概要

### 設計目標
- 使用 **GAS** 與 **GameplayTag ↔ Ability** 的 **資料驅動** 對應，讓設計師能在編輯器新增互動行為，不需改 C++。 

### 架構一覽
- **互動（Interaction）**
  - 以一個「入口能力」`StartInteractAbility` 常駐監測玩家周邊可互動物件。
  - 物件是否可互動由 **介面** `IInteractable` 判斷，並支援 `OnFocus` / `OnUnfocus` 視覺回饋。
  - 互動鍵觸發時，依物件的 `InteractTag` 查表啟動對應 **Ability**（資料資產維護 Tag→Ability 對應）。 
- **背包（Inventory）**
  - 拾取能力 `CollectItemAbility` 透過 `InventoryManagerComponent::AddItemDefToInventory` 將 **物品定義** 轉為 **物品實例**，加入可複製陣列 `InventoryList`。
  - 變更以 **Delegate** 廣播給 UI 控制器；目前 UI 使用 `TileView` 顯示。
- **資料驅動**
  - GameplayTag 與 Ability 的對應存放於 **DataAsset**；新增/更換互動行為時只需在編輯器增修 Row。

---

## 物品系統（Definition / Instance 分離）
- **InventoryItemDefinition**
  - 定義靜態屬性：Icon、Mesh、名稱、預設數量、對應的 `InventoryItemInstance` 類型等。
  - 一般僅使用 **CDO**；同種物品共用一份定義，改藍圖即可全體生效。
- **InventoryItemInstance**
  - 保存**執行期**狀態：例如數量、耐久度等。
  - 以 **GameplayTag ↔ int32** 類 Map 表示任意屬性，便於**網路同步/存讀檔**。

---

## 延伸與擴充

### 新增一種可互動物件
1. 讓目標 Actor **實作** `IInteractable`，回傳對應的 `InteractTag`，並視需要實作 `OnFocus` / `OnUnfocus`。
2. 建立/指派對應的 **Ability**（可用藍圖，繼承互動用 Ability 基類）。
3. 在 **資料資產** 中新增一列 **GameplayTag → Ability** 對應即可。 

### 新增一個物品
1. 建立 `InventoryItemDefinition`（設定 Icon、Mesh、名稱、預設數量、Instance 類型…）。
2. 需要使用行為時，指定該物品對應的 **使用 Ability Tag**；UI 觸發時即由 GAS 啟動對應能力。

---

## 相關核心類別（C++ 範例）
> 以下為專案中與本系統相關、可供參考的類別（命名以實作為準）：
- **Ability 基底**
  - `UGZGameplayAbility`：共用啟停流程與啟動策略（`EAbilityActivationPolicy`）。
  - `UGZCombatAbility`：戰鬥系能力基底，能取回當前武器實例與物品實例。 
  - `UGZMeleeAttackAbility` / `UGZRangeAttackAbility` / `UGZBowAttackTestAbility`：近戰/遠程/弓測試等能力示例。
- **裝備與物品**
  - `UGZEquipmentInstance`：武器/裝備的**靜態屬性**與產生掛載 Actor、對應授權 Ability、持有者等。
  - `UGZInventoryItemDefinition`：物品**定義**（Icon、Mesh、名稱、預設堆疊、Tag 堆疊等）。 
  - `UGZInventoryItemInstance`：物品**實例**（`Tag ↔ Stack` 數值、Definition 類別、可複製）。

---

## 開發重點
- 互動由**單一入口能力**統一處理偵測與派發；實際行為交給資料驅動的 Ability 對應，**弱化程式依賴**、便於設計師擴充。
- 物品 Definition / Instance 分離：**CDO 共用靜態資料**、Instance 僅存執行期數值，有利**網路同步/效能/儲存**。 
---

## 授權
- TBD
