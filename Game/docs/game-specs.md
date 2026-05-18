# ゲーム仕様記録 — InvaderGame

プロジェクト要件定義: [requirements.md](requirements.md)

---

## 仕様ドキュメント一覧

実装は以下のカテゴリ別に分割・管理されています。
各実装指示の完了後、エージェントが該当ドキュメントに自動追記します。

---

### [2026-05-18] ColliderComponent 導入・当たり判定の疎結合化

- **対象ファイル（新規）**: `Engine/include/ColliderComponent.h`, `Engine/src/ColliderComponent.cpp`
- **対象ファイル（変更）**: `Engine/include/ActorBase.h`, `Engine/include/SceneBase.h`, `Engine/src/SceneBase.cpp`, `Game/include/Actor/bullet.h`, `Game/src/Actor/bullet.cpp`, `Game/include/Actor/invader.h`, `Game/src/Actor/invader.cpp`, `Game/include/Actor/invader_swarm.h`, `Game/src/Actor/invader_swarm.cpp`, `Game/include/Scene/game_scene.h`, `Game/src/Scene/game_scene.cpp`
- **内容**:
  - `engine::ColliderComponent` を新設。衝突グループ（`CollisionGroup::PlayerBullet` / `EnemyBody`）、ヒットボックスセル（相対オフセット）、`HitResult` リストを保持
  - `ActorBase` に `virtual ColliderComponent* collider() { return nullptr; }` を追加。当たり判定が必要な Actor のみオーバーライドする
  - `SceneBase` に `mColliders_`（衝突走査のみ参加するアクターのリスト）と `calcCollisions_()` を追加。`calcActors_()` 内の preCalc〜postCalc 間に collision フェーズを挿入
  - `Bullet` が `engine::CollisionGroup::PlayerBullet` グループのコンポーネントを保持。`postCalc()` で `hasHit()` を確認して非アクティブ化
  - `Invader` が `engine::CollisionGroup::EnemyBody` グループの4セルコンポーネントを保持。`InvaderSwarm::postCalc()` がコンポーネントを読んで `kill_()` を呼ぶ
  - `InvaderSwarm` から `mBullets_` 参照を完全除去。Bullet の具体型を知らずに衝突結果だけで処理できるように変更
  - `mInvaders` を `std::vector<Invader>` → `std::list<Invader>` に変更（登録後のポインタ安定性確保）
  - `InvaderSwarm::sweepDead_()` を追加。`cleanupColliders_()` の後に呼ぶことで dangling pointer を防ぐ
  - `GameScene` でコンストラクタ・reset 時に `registerCollider_(&inv)` で Invader を衝突走査リストに登録
- **疎結合ポイント**: InvaderSwarm は Bullet の存在を知らない。SceneBase の衝突走査が group/mask マッチングで自動的に結果を書き込むため、Game層の各 Actor は自分のコンポーネントを読むだけでよい

### [2026-05-18] 背景描画をEngine側(SceneBase)に移動

- **対象ファイル（新規）**: なし
- **対象ファイル（変更）**: `Engine/include/SceneBase.h`, `Engine/src/SceneBase.cpp`, `Game/src/Scene/game_scene.cpp`
- **内容**:
  - `SceneBase` に保護メソッド `drawBackground_(int width, int height)` を追加
  - メソッド内で枠線（`=` と `|`）を Background レイヤーに描画
  - `GameScene::draw()` から背景描画ロジック（手書きの枠線ループ）を削除し、`drawBackground_(Config::FIELD_WIDTH, Config::FIELD_HEIGHT)` に置き換え
  - Engine層が画面描画の基盤機能（背景・枠線）を統一管理できるように設計
- **設計ポイント**: Game層は背景描画の詳細を知らず、Engine側で共通的に提供されるヘルパーメソッドを活用。将来他のシーン（例：MenuScene）を追加する際も、同じ `drawBackground_()` で一貫性を保ちやすい

### [2026-05-18] TestScriptLoader 導入・テストスクリプト設定ファイル化

- **対象ファイル（新規）**: `Engine/include/TestScriptLoader.h`, `Engine/src/TestScriptLoader.cpp`, `Game/docs/specs/test-script.cfg`
- **対象ファイル（変更）**: `Game/src/main.cpp`
- **内容**:
  - `engine::TestScriptLoader` を新設。`test-script.cfg` を読み込んで `TestInputScript` を構築し、期待ログエントリも返す
  - `test-script.cfg` の書式: `event <frame> <action>` でフレーム入力、`expect <log_entry>` で期待ログを定義。`#` コメント・空行対応
  - ゲーム終了後に `validateLogs()` が `debug.log` を読み込み、全期待ログを照合して `[TEST] PASS/FAIL:` を書き込む
  - ExitCode: 全 PASS → 0 / 一部 FAIL → 1（エージェントの TC-001 判定と統合）
  - `main.cpp` から `TestInputScript` の `addEvent` 直書きをすべて除去
- **設計ポイント**: 新機能追加時は `test-script.cfg` に `event` / `expect` 行を追記するだけでテストシナリオを拡張できる（C++ ファイルの再コンパイル不要）

### [2026-05-18] Invader 消滅アニメ修正・スコア計算バグ修正

- **対象ファイル（変更）**: `Game/src/Actor/invader.cpp`, `Game/src/Scene/game_scene.cpp`
- **内容**:
  - `Invader::isActive()` に `mDeathTimer > 0` を追加して、消滅アニメ中も `true` を返すように修正
  - 理由：`kill_()` で `mAlive=false` になると、`InvaderSwarm::preCalc()` の `if (inv.isActive())` でスキップされ、消滅アニメが進まない
  - スコア計算で `inv.isAlive_()` を使用するように修正（生存中のみカウント）

### [2026-05-18] Actor 更新を preCalc / postCalc 2フェーズに分割

- **対象ファイル（変更）**: `Engine/include/ActorBase.h`, `Engine/include/SceneBase.h`, `Engine/src/SceneBase.cpp`, `Game/include/Actor/player.h`, `Game/src/Actor/player.cpp`, `Game/include/Actor/bullet.h`, `Game/src/Actor/bullet.cpp`, `Game/include/Actor/invader.h`, `Game/src/Actor/invader.cpp`, `Game/include/Actor/invader_swarm.h`, `Game/src/Actor/invader_swarm.cpp`, `Game/include/Scene/game_scene.h`, `Game/src/Scene/game_scene.cpp`
- **内容**:
  - `ActorBase::calc()` を `preCalc()`（純粋仮想）と `postCalc()`（デフォルト空実装）に分割
  - `SceneBase` に Actor 管理機能を追加：`registerActor_()`, `calcActors_()`, `cleanupActors_()`, `drawActors_()`
  - `calcActors_()` が Phase1（全 preCalc） → Phase2（全 postCalc）の順で実行する
  - `InvaderSwarm` を `engine::ActorBase` 継承に変更し、移動を `preCalc()`・弾との当たり判定を `postCalc()` に分離
  - `GameScene::mBullets` を `std::vector<Bullet>` から `std::list<Bullet>` に変更（登録後のポインタ安定性確保）
  - 弾生成時に `registerActor_(&mBullets.back())` で `mActors_` に登録
  - `GameScene::calc()` は `calcActors_()` 一呼び出しに集約。状態遷移後に `cleanupActors_()` を呼んでから `mBullets` リストをクリーンアップ

### [2026-05-17] InputHandler を Engine 層へ移植・テスト機構分離

- **対象ファイル（新規）**: `Engine/include/InputHandler.h`, `Engine/src/InputHandler.cpp`, `Engine/include/TestInputScript.h`, `Engine/src/TestInputScript.cpp`
- **対象ファイル（変更）**: `Game/include/Scene/game_scene.h`, `Game/include/Scene/title_scene.h`, `Game/src/main.cpp`
- **対象ファイル（互換シム化）**: `Game/include/input_handler.h`, `Game/src/input_handler.cpp`
- **内容**:
  - `game::InputHandler` を `engine::InputHandler` へ移植（`namespace engine`、`Engine/include/` 配下）
  - テスト用フレームカウンタ・モードフラグを `InputHandler` から分離し、`engine::TestInputScript` クラスを新設
  - `InputHandler::setTestProvider(TestProvider)` を使ってラムダ（コールバック）を注入することで、テスト入力を差し替え可能に
  - `TestInputScript::addEvent(frame, setter)` でフレーム番号とボタン注入ラムダを登録し、`createProvider()` でプロバイダーを生成
  - テストスクリプトの**内容**（どのフレームで何を押すか）は `Game/src/main.cpp` に残し、**仕組み**のみ Engine 層に移動
  - Game 側の旧ファイルはコメント付き互換シムに置き換え（削除は行わず）
- **設計ポイント**: 将来 Google Test 等のテストフレームワークから `setTestProvider()` にモックを注入することで、シーン単体テストが可能になる

---

### [2025-02-17] ActorModelシステム導入

- **対象ファイル**: `Engine/include/ActorModel.h`（新規）, `Engine/include/RenderQueue.h`, `Engine/src/RenderQueue.cpp`, `Engine/include/ActorBase.h`, `Engine/src/ActorBase.cpp`, `Game/src/Actor/player.cpp`, `Game/src/Actor/bullet.cpp`, `Game/src/Actor/invader.cpp`, `Game/include/Actor/bullet.h`
- **内容**:
  - `ActorModel` 構造体を新設。`cells`（`std::vector<std::string>`）で2D描画形状を保持し、`pivotX/pivotY` で Actor 座標との対応を定義
  - `RenderQueue` に `submitModel()` と `ModelCommand` を追加。`flush()` でモデルをセルに展開し、スペース/`\0` は透過（他の描画を上書きしない）
  - `ActorBase` に `mModel` メンバと `draw()` デフォルト実装を追加。サブクラスは `calc()` 内で `mModel.cells` を更新するだけで描画形状を変更できる
  - `Player`, `Bullet`, `Invader` の各 `draw()` オーバーライドを廃止し、`calc()` 末尾でのモデル更新に一本化
  - アニメーション（射撃中、移動中、スポーン、消滅）も `mModel.cells` の差し替えで実現

---

### [2026-05-17] UIBaseシステム導入

- **対象ファイル**: `Engine/include/UIBase.h`（新規）, `Engine/src/UIBase.cpp`（新規）, `Engine/include/SceneBase.h`, `Engine/src/SceneBase.cpp`, `Game/include/Scene/game_ui.h`（新規）, `Game/src/Scene/game_ui.cpp`（新規）, `Game/include/Scene/game_scene.h`, `Game/src/Scene/game_scene.cpp`
- **内容**:
  - `engine::UIBase` を新設。文字列を指定座標に描画する UI の基底クラス。`calc()` でテキスト更新、`draw()` で RenderQueue UI 層に登録、`setVisible()` で表示制御
  - `engine::SceneBase` に `mUIs_`（`vector<unique_ptr<UIBase>>`）と `addUI_() / calcUIs_() / drawUIs_()` ヘルパーを追加。Game 層シーンは `addUI_()` で登録するだけで UI の lifecycle を管理できる
  - `game::ScoreUI` / `game::TimerUI` を `game_ui.h/.cpp` に実装。`calc()` で `setText()` を呼ぶパターンで動的テキストを実現
  - `GameScene` の draw() 内の直接 `submitString()` 呼び出しをすべて UIBase サブクラスに置き換え。CLEAR! メッセージは `mClearMessageUI->setVisible()` で状態連動して制御


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

