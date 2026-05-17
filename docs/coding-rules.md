# InvaderGame コーディングルール

プロジェクト全体で統一されたコーディング規約を定義します。

## 命名規則

### メンバ変数

メンバ変数は **`m` + キャメルケース（大文字始まり）** の形式で命名します。

- ✅ 正: `mPos`, `mVelocity`, `mPlayerHealth`
- ❌ 誤: `pos`, `Velocity`, `m_player_health`, `mplayer_health`

### 関数名・メソッド名

基本的にキャメルケース（小文字始まり）で命名します。

```cpp
// 正：キャメルケース（小文字始まり）
void doCalc();
int getHealth();
void updatePosition();
```

### メンバ関数名の層別ルール

メンバ関数の命名は、定義される層によって異なります：

#### Engine 層（`Engine/include/`, `Engine/src/`）

Engine 層のメンバ関数はキャメルケース（小文字始まり）のみ：

```cpp
class engine::ActorBase {
    void update();          // ✅ キャメルケース
    void render();          // ✅ キャメルケース
};
```

#### Game 層（`Game/include/`, `Game/src/`）

- **Engine 層にも存在する仮想メンバ関数**：キャメルケース（小文字始まり）
- **Game 層にのみ存在するメンバ関数**：キャメルケース（小文字始まり）**+ 末尾に `_`**

```cpp
class game::Player : public engine::ActorBase {
    // Engine で定義される仮想メソッド - 末尾に _ はつけない
    void update() override;   // ✅
    void render() override;   // ✅
    
    // Game 層のみの独自メソッド - 末尾に _ をつける
    void handleInput_();      // ✅
    void checkCollision_();   // ✅
    int calculateDamage_();   // ✅
};
```

## コンパイラ・ビルド設定

- **言語**: C++17
- **コンパイラ**: MSVC
- **ビルドシステム**: CMake + Ninja
- **ターゲット**: Windows Console API (`windows.h`)
- **ヘッダ保護**: `#pragma once` を使用

## コード構成

### ヘッダファイル
- `#pragma once` で二重インクルード防止
- マクロは UPPER_SNAKE_CASE
- 型定義は PascalCase

### 実装ファイル
- 名前空間を活用（`engine::`, `game::`）
- 関数・変数は本ルールに従う

## その他の規約

- インデント：4 スペース（またはタブ）
- 改行：LF を統一
- 末尾の空白：削除

---

**最終更新**: 2026年5月17日
