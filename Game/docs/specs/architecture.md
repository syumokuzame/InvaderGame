# アーキテクチャ — 実装記録

## プロジェクト構造の再構成 — Engine 層と Game 層の分割 [2026-05-16]

### 対象ファイル
- プロジェクトルート構造全体
- `CMakeLists.txt`

### 内容
- ディレクトリ構造を2層に再構成：
  ```
  Root
  ├── Engine/          (ゲームエンジン層 — 今後のコア機能)
  └── Game/            (ゲームアプリ層 — InvaderGame 実装)
      ├── include/
      ├── src/
      ├── docs/
      └── save/
  ```
- 既存の `include/`, `src/`, `docs/`, `save/` をすべて `Game/` 配下に移動
- `CMakeLists.txt` を新構造に対応：
  - `file(GLOB_RECURSE SOURCES "Game/src/*.cpp")` に変更
  - `target_include_directories(InvaderGame PRIVATE Game/include)` に変更
- ビルド成功、出力ファイル `build/InvaderGame.exe` は従来通り

### 仕様
- Engine 層は将来の拡張用（グラフィックス API、物理エンジン等）
- Game 層は現在のすべての実装を含む（プレイヤー、敵、弾、UI等）
- 層間の依存関係は今後整理予定

### ビルド・デプロイ
- 成功、git push 完了

---

## ActorBase メソッド名変更 — update → calc、render → draw [2026-05-16]

### 対象ファイル
- `Engine/include/ActorBase.h`
- `Engine/src/ActorBase.cpp`
- `Game/include/actor.h`
- `Game/include/player.h`, `src/player.cpp`
- `Game/include/bullet.h`, `src/bullet.cpp`
- `Game/include/invader.h`, `src/invader.cpp`
- `Game/include/invader_swarm.h`, `src/invader_swarm.cpp`
- `Game/src/game.cpp`

### 内容
- `ActorBase` の仮想メソッド：`update()` → `calc()`、`render()` → `draw()`
- `Player`、`Bullet`、`Invader` の実装メソッドも同じく変更
- `InvaderSwarm::render()` → `InvaderSwarm::draw()`
- `Game::update()` / `Game::render()` 内の呼び出しをすべて更新

### 仕様変更
- ゲーム層の命名規則を統一
- 計算処理は `calc()` で呼ばれる
- 描画処理は `draw()` で呼ばれる

### ビルド
- 成功、git push 完了

---

## FrameWork / SceneBase / GameScene 導入 — Engine/Game 層分離完成 [2026-05-16]

### 対象ファイル（新規）
- `Engine/include/Renderer.h`, `Engine/src/Renderer.cpp`（`game::Renderer` → `engine::Renderer` に移動）
- `Engine/include/SceneBase.h`（抽象シーン基底クラス）
- `Engine/include/FrameWork.h`, `Engine/src/FrameWork.cpp`（メインループ管理）
- `Game/include/game_scene.h`, `Game/src/game_scene.cpp`（ゲームシーン実装）

### 対象ファイル（削除）
- `Game/include/renderer.h`, `Game/src/renderer.cpp`（Engine 層に移動）
- `Game/include/game.h`, `Game/src/game.cpp`（GameScene に置き換え）

### 対象ファイル（更新）
- `Engine/include/ActorBase.h` — `draw(game::Renderer&)` → `draw(engine::Renderer&)`
- `Game/include/actor.h`, `player.h`, `bullet.h`, `invader.h`, `invader_swarm.h` — 同上
- 各 `.cpp` ファイル — include パス・関数シグネチャ更新
- `Game/src/main.cpp` — `FrameWork` + `GameScene` を使う形に書き換え

### アーキテクチャ

```
main() [Game]
  └─ engine::FrameWork (Renderer を所有、メインループ)
       └─ game::GameScene : engine::SceneBase
            ├─ calc()  — 入力処理・物理更新
            └─ draw(engine::Renderer&) — 全描画
```

### 仕様

#### engine::SceneBase
- `calc()` / `draw(Renderer&)` 純粋仮想
- `running_` / `quit()` 管理

#### engine::FrameWork
- `Renderer` を値メンバとして所有
- フレームレート制御

#### game::GameScene
- 元の `Game` クラスのロジックをすべて内包

#### 実装上の注意
- `Renderer` は Engine 層で定義（`Config` 定数参照のみ Game 依存が残る）
- シーン遷移は将来対応予定

### ビルド・デプロイ
- 成功、git push 完了

---

## Game層のファイル構造整理 [2026-05-16]

### 対象ファイル
- `Game/include/` ディレクトリ配下
- `Game/src/` ディレクトリ配下
- 全体の #include パス修正

### 内容

#### ディレクトリ構造の改善
Game層内のinclude/srcを機能別に分類：

**Before:**
```
Game/include/
├── actor.h, player.h, bullet.h, invader.h, invader_swarm.h
├── game_scene.h, title_scene.h, score_manager.h
├── config.h, input_handler.h
├── ... (その他)
```

**After:**
```
Game/include/
├── Actor/
│   ├── actor.h
│   ├── player.h
│   ├── bullet.h
│   ├── invader.h
│   └── invader_swarm.h
├── Scene/
│   ├── game_scene.h
│   ├── title_scene.h
│   └── score_manager.h
├── config.h
└── input_handler.h
```

同様に `Game/src/` も Actor/Scene ディレクトリを作成して分類

#### #include パスの統一
- 全ファイルの #include を CMake の include directories（`Game/include` と `Engine/include`）を基準とした絶対パスに統一
- ヘッダー間の参照パス例：
  - `Game/include/Actor/player.h`: `#include "config.h"` / `#include "Actor/actor.h"`
  - `Game/include/Scene/game_scene.h`: `#include "Actor/player.h"` / `#include "Scene/score_manager.h"`
  - `Game/src/Actor/player.cpp`: `#include "Actor/player.h"` / `#include "config.h"`

### 仕様

#### メリット
- **可読性向上**: ファイルカテゴリが可視化される
- **保守性向上**: Actor関連ファイルはActor/ディレクトリに集約、Scene関連もScene/ディレクトリに集約
- **スケーラビリティ**: 新規ファイル追加時の配置が明確

#### ファイル分類ルール
- **Actor/** : キャラクター（Actor, Player, Bullet, Invader）
- **Scene/** : ゲーム進行管理（GameScene, TitleScene, ScoreManager）
- **Root** : 共通設定・入力（config.h, input_handler.h）

### ビルド・デプロイ
- 成功、git push 完了
