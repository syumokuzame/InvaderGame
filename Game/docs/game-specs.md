# ゲーム仕様記録 — InvaderGame

プロジェクト要件定義: [requirements.md](requirements.md)

---

## 仕様ドキュメント一覧

実装は以下のカテゴリ別に分割・管理されています。
各実装指示の完了後、エージェントが該当ドキュメントに自動追記します。

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
