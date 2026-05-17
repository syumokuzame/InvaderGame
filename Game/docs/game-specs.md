# ゲーム仕様記録 — InvaderGame

プロジェクト要件定義: [requirements.md](requirements.md)

---

## 仕様ドキュメント一覧

実装は以下のカテゴリ別に分割・管理されています。
各実装指示の完了後、エージェントが該当ドキュメントに自動追記します。

---

### [2025-02-17] ActorModelシステム導入

- **対象ファイル**: `Engine/include/ActorModel.h`（新規）, `Engine/include/RenderQueue.h`, `Engine/src/RenderQueue.cpp`, `Engine/include/ActorBase.h`, `Engine/src/ActorBase.cpp`, `Game/src/Actor/player.cpp`, `Game/src/Actor/bullet.cpp`, `Game/src/Actor/invader.cpp`, `Game/include/Actor/bullet.h`
- **内容**:
  - `ActorModel` 構造体を新設。`cells`（`std::vector<std::string>`）で2D描画形状を保持し、`pivotX/pivotY` で Actor 座標との対応を定義
  - `RenderQueue` に `submitModel()` と `ModelCommand` を追加。`flush()` でモデルをセルに展開し、スペース/`\0` は透過（他の描画を上書きしない）
  - `ActorBase` に `mModel` メンバと `draw()` デフォルト実装を追加。サブクラスは `calc()` 内で `mModel.cells` を更新するだけで描画形状を変更できる
  - `Player`, `Bullet`, `Invader` の各 `draw()` オーバーライドを廃止し、`calc()` 末尾でのモデル更新に一本化
  - アニメーション（射撃中、移動中、スポーン、消滅）も `mModel.cells` の差し替えで実現


### [プレイヤー機能](specs/player.md)
- 弾発射システムの実装
- プレイヤーアニメーション実装（射撃＆移動表現）

### [敵機機能](specs/invader.md)
- 敵機システムの実装
- 敵機の4マス表現と動的個数決定
- 敵機の移動機能実装
- 敵スポーンアニメーション実装
- 敵全滅判定バグ修正
- 敵撃破スコア加算・Clear表示・敵リスポーン

### [入力機能](specs/input.md)
- InputHandler の X マクロ化
- 入力ハンドラー修正（GetAsyncKeyState 改善）

### [UI・スコア機能](specs/ui.md)
- 上部UI実装

### [アーキテクチャ](specs/architecture.md)
- プロジェクト構造の再構成 — Engine 層と Game 層の分割
- ActorBase メソッド名変更 — update → calc、render → draw
- FrameWork / SceneBase / GameScene 導入 — Engine/Game 層分離完成

### [デバッグ機能](specs/debug.md)
- デバッグ機能：A キーで全敵削除

### [バグ修正](specs/bug-fixes.md)
- Clear 状態での弾の動作修正

---

*ドキュメント整理完了 [2026-05-16]*

---

### [2026-05-16] シーン切り替え機能（TitleScene 追加）
- **対象ファイル**: `Engine/include/SceneBase.h`, `Engine/include/FrameWork.h`, `Engine/src/FrameWork.cpp`, `Game/include/title_scene.h`, `Game/src/title_scene.cpp`, `Game/src/game_scene.cpp`, `Game/src/main.cpp`
- **内容**:
  - `engine::SceneBase` に `changeScene(SceneBase*)` / `takeNextScene()` を追加し、派生シーンがシーン切り替えをリクエストできる仕組みを実装
  - `engine::FrameWork` がシーンの所有権を持つよう変更（デストラクタで `delete`）、`run()` ループ内で `takeNextScene()` を毎フレームチェックし、非 nullptr なら旧シーンを `delete` して新シーンに切り替える
  - `game::TitleScene` を新規作成。ENTER でゲームシーンへ遷移、Q/ESC でアプリ終了。テキストの点滅エフェクト付き
  - `game::GameScene` の Q/ESC キーを `quit()` から `changeScene(new TitleScene())` に変更（ゲーム中にタイトルへ戻れる）
  - `main.cpp` を `new game::TitleScene()` からの起動に変更

### [2026-05-16] シーン遷移：Factory パターンでの再実装
- **対象ファイル**: `Engine/include/SceneBase.h`, `Engine/include/FrameWork.h`, `Engine/src/FrameWork.cpp`, `Game/src/Scene/title_scene.cpp`, `Game/src/Scene/game_scene.cpp`, `Game/src/main.cpp`
- **内容**:
  - `engine::SceneType` enum class を追加（Title, Game）
  - `engine::SceneBase::changeScene()` の引数を `SceneBase*` から `SceneType` に変更 — シーン側では次の行先のみ指定
  - `engine::FrameWork` に `setSceneFactory()` メソッドを追加。Game層が factory 関数を登録できる
  - `engine::FrameWork::run()` で factory を使ってシーンを生成（new をFramework側で実施）
  - `game::main.cpp` で lambda factory を登録し、SceneType::Title/Game に対応するシーンを生成
  - **メリット**: Engine層がGame層のシーン型情報を持たず、Framework側がシーン生成を統一的に管理

### [2026-05-17] 汎用Allocator導入・ヒープ管理のFramework集約
- **対象ファイル**: 
  - `Engine/include/Allocator.h`（新規作成）
  - `Engine/include/SceneBase.h`（Allocator& 参照に変更、updateActors等削除）
  - `Engine/src/SceneBase.cpp`（簡略化）
  - `Engine/include/FrameWork.h`（Allocator 内包、SceneFactory引数変更、run(SceneType)）
  - `Engine/src/FrameWork.cpp`（run() に初期SceneType受け取り、scene切替時に allocator_.clear()）
  - `Game/include/Scene/game_scene.h`（コンストラクタ引数を Allocator& に変更）
  - `Game/src/Scene/game_scene.cpp`（allocator_.create<Player>() でヒープ確保）
  - `Game/include/Scene/title_scene.h`（同上）
  - `Game/src/Scene/title_scene.cpp`（同上）
  - `Game/src/main.cpp`（actors vector 除去、factory に Allocator& を渡す形に変更）
- **内容**:
  - `engine::Allocator` を新規作成。`create<T>(args...)` で任意の型をヒープ確保し所有権を持つ。`clear()` で逆順に一括 delete
  - `Allocator` の型消去デリータにより `ActorBase` に限らず任意の型を管理可能
  - `engine::FrameWork` が `Allocator allocator_` を内包し、全シーンで共有
  - `SceneFactory` の型を `std::function<SceneBase*(SceneType, Allocator&)>` に変更
  - `FrameWork::run(SceneType initialScene)` でループ開始時に最初のシーンを生成
  - シーン遷移時: 旧シーン delete → `allocator_.clear()` → 新シーン生成 の順で安全に解放
  - `main.cpp` から `std::vector<ActorBase*>` 手動管理を撤去、factory lambda で `alloc` を受け取るだけのシンプルな構造に
- **メリット**: Engine 層の Allocator が汎用型管理、将来的に Actor 以外（UIオブジェクト等）もヒープ確保できる拡張性
- **対象ファイル**: 
  - `Engine/include/SceneBase.h`（新規：updateActors(), drawActors(), clearActors()）
  - `Engine/src/SceneBase.cpp`（新規作成）
  - `Engine/include/FrameWork.h`（std::vector<ActorBase*> 追加）
  - `Engine/src/FrameWork.cpp`（Actor管理の追加）
  - `Game/include/Scene/game_scene.h`（Player* に変更）
  - `Game/src/Scene/game_scene.cpp`（コンストラクタで Player* を new）
  - `Game/include/Scene/title_scene.h`（コンストラクタ引数追加）
  - `Game/src/Scene/title_scene.cpp`（コンストラクタ実装）
  - `Game/src/main.cpp`（actors vector を定義・管理）
- **内容**:
  - `engine::FrameWork` が `std::vector<ActorBase*> actors_` を保持し、全シーンで共有するメモリプール
  - `engine::SceneBase` のコンストラクタで `actors_` への参照を受け取る

### [2026-05-17] コーディングルール適用
- **対象ファイル**: 
  - Engine層: `Engine/include/ActorBase.h`, `Engine/include/SceneBase.h`, `Engine/include/FrameWork.h`, `Engine/include/Renderer.h`, `Engine/include/Allocator.h`、およびそれらの実装ファイル（.cpp）
  - Game層: 全Actor・Scene関連ヘッダ・実装ファイル、InputHandler、ScoreManager、メインファイル
- **内容**:
  - **メンバ変数**: `m + キャメルケース（大文字始まり）` に統一
    - 例: `x_` → `mX`、`lives_` → `mLives`、`state_` → `mState`
  - **関数名**: キャメルケース（小文字始まり）に統一
  - **メンバ関数名の層別ルール**:
    - Engine層のメンバ関数: キャメルケース（小文字始まり）のみ
    - Game層のみのメンバ関数: キャメルケース + 末尾に `_` を追記
      - 例: `moveLeft()` → `moveLeft_()`、`addScore()` → `addScore_()`、`poll()` → `poll_()`
- **修正ファイル数**: 25+ ファイル（全ヘッダ・実装ファイル）
- **コンパイル結果**: ✅ 成功（終了コード 0）

  - `SceneBase` に `updateActors()`（全actor→calc()）、`drawActors()`（全actor→draw()）、`clearActors()`（全actor削除）を追加
  - `game::GameScene` のコンストラクタで `Player* player_ = new Player()` を実施し、`actors_.push_back(player_)`
  - `game::main.cpp` で actor vector を main スコープで定義、lambda factory でそれを参照
  - **メリット**: Actor のライフサイクル管理がFramework統一、シーン遷移時の自動クリーンアップ、メモリリーク防止

### [2026-05-17] RenderQueue パターン導入 — 描画の一括フラッシュ方式に変更
- **対象ファイル**:
  - `Engine/include/RenderQueue.h`（新規作成）
  - `Engine/src/RenderQueue.cpp`（新規作成）
  - `Engine/include/ActorBase.h`（draw() シグネチャ変更）
  - `Engine/include/SceneBase.h`（draw() シグネチャ変更）
  - `Engine/src/FrameWork.cpp`（描画ループ修正）
  - Game層: `Game/src/Actor/player.cpp`, `bullet.cpp`, `invader.cpp`, `invader_swarm.cpp`
  - Game層: `Game/src/Scene/game_scene.cpp`, `title_scene.cpp`
  - Game層: 上記に対応する全ヘッダ
- **内容**:
  - `engine::RenderQueue` をシングルトンで新設。`submit(x, y, ch, layer)` / `submitString(x, y, str, layer)` で描画コマンドをキューに蓄積し、`flush(Renderer&)` でレイヤー昇順ソート後に一括書き込み
  - `engine::RenderLayer` enum class を追加（Background=0, Actor=10, Bullet=20, UI=30）
  - `ActorBase::draw(Renderer&)` → `draw()` に変更。内部で `RenderQueue::instance().submit()` を呼ぶ
  - `SceneBase::draw(Renderer&)` → `draw()` に変更
  - `FrameWork::run()` の描画処理を `clear() → draw() → flush() → present()` の順に変更
  - `drawHeader()` / `drawInstructions()` 等の Renderer 直接呼び出しを `submitString()` 経由に統一。スコア・時刻などの動的文字列は `std::string` で内部コピーするためダングリング問題なし
- **メリット**: 描画の z-order 制御が可能になり、将来的にエフェクトや重ね描画の拡張が容易

