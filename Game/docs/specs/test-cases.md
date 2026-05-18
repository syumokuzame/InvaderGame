# InvaderGame テストケース定義

ビルド成功後、`InvaderGame.exe --test` を実行してログファイル `save/debug.log` を読み込み、
以下の各テストケースのログエントリが存在するかを確認します。

---

## テスト実行方法（エージェント用）

```powershell
# 1. 旧ログを削除してからテスト実行
Remove-Item "save\debug.log" -ErrorAction SilentlyContinue
$proc = Start-Process -FilePath ".\build\InvaderGame.exe" -ArgumentList "--test" -PassThru -WindowStyle Normal
$proc.WaitForExit(10000)
Write-Host "ExitCode: $($proc.ExitCode)"

# 2. ログを読み込んで判定
$log = Get-Content "save\debug.log" -Raw -ErrorAction SilentlyContinue
```

**テスト所要時間**: 約 4〜5 秒で自動終了します（260フレーム × 16ms ≈ 4.2秒）。

---

## Phase 1: コア動作テスト（必須）

すべての実装後に必ず確認するテスト。TC-001〜TC-006。

### TC-001: アプリケーション起動・正常終了
- **説明**: exe が --test モードでクラッシュなく起動・終了できるか
- **判定**: `$proc.ExitCode -eq 0`
- **期待ログ**: `[GAME] Start` が存在する
- **関連ファイル**: `Game/src/main.cpp`, `Engine/src/FrameWork.cpp`

### TC-002: タイトルシーン初期化
- **説明**: タイトルシーンが正しく初期化されるか
- **期待ログ**: `[SCENE] TitleScene started` が存在する
- **関連ファイル**: `Game/src/Scene/title_scene.cpp`

### TC-003: タイトル → ゲームシーン遷移
- **説明**: Enter キーでゲームシーンに遷移できるか（フレーム65でスクリプト入力）
- **期待ログ（2件とも必須）**:
  - `[INPUT] Enter pressed -> GameScene`
  - `[SCENE] GameScene started, invaders=30`
- **関連ファイル**: `Game/src/Scene/title_scene.cpp`, `Game/src/Scene/game_scene.cpp`

### TC-004: プレイヤー射撃
- **説明**: スペースキーで弾が発射されるか（フレーム130でスクリプト入力）
- **期待ログ**: `[INPUT] Player shoot` が存在する
- **関連ファイル**: `Game/src/Scene/game_scene.cpp`, `Game/src/Actor/player.cpp`

### TC-005: ゲーム → タイトルシーン遷移
- **説明**: Q キーでタイトルシーンに戻れるか（フレーム160でスクリプト入力）
- **期待ログ（2件とも必須）**:
  - `[INPUT] Quit pressed -> TitleScene`
  - `[SCENE] TitleScene started` が2回以上存在する
- **関連ファイル**: `Game/src/Scene/game_scene.cpp`

### TC-006: スコア加算
- **説明**: 敵撃破時にスコアが加算されるか（フレーム130に射撃、フレーム145にデバッグ全滅）
- **期待ログ**: `[SCORE] Added` が存在する
- **関連ファイル**: `Game/src/Scene/game_scene.cpp`, `Game/src/Scene/score_manager.cpp`

---

## Phase 2: 拡張テスト（実装追加時に増加）

新機能実装時に以下のテストケースを追加してください。

### テストケース追加ガイドライン

| 実装内容 | 追加するログエントリ | TCコード | ログ追加場所 |
|---------|---------------------|---------|-------------|
| プレイヤー機能（アニメーション等） | `[ACTOR] Player <action>` | TC-101〜 | `Game/src/Actor/player.cpp` |
| 敵機能（移動パターン、複数種） | `[ACTOR] Invader <action>` | TC-110〜 | `Game/src/Actor/invader.cpp` |
| UI / スコア / タイマー | `[UI] <name> displayed` | TC-120〜 | 該当UIクラス |
| シーン追加（GameOver等） | `[SCENE] <Name> started` | TC-130〜 | 新シーンのコンストラクタ |
| 入力システム拡張 | `[INPUT] <action>` | TC-140〜 | `Game/src/Scene/*.cpp` |

**テストスクリプト拡張方法:**
新しい操作が必要な場合は `Game/src/input_handler.cpp` の `poll_()` テストモードブロックに
フレームと入力を追加してください:

```cpp
// 例: フレーム170にPキー（ポーズ）を追加
else if (sTestFrame == 170) mPause = true;
// 終了キーは最後のフレームに配置（現在は260）
```

---

## テスト実行ログ記録フォーマット

実装ごとに以下の形式でこのファイル末尾に追記してください。

```markdown
### [YYYY-MM-DD] <実装内容>
- TC-001: ✓ PASS (ExitCode=0)
- TC-002: ✓ PASS ([SCENE] TitleScene started 確認)
- TC-003: ✓ PASS ([INPUT] Enter / [SCENE] GameScene started 確認)
- TC-004: ✓ PASS ([INPUT] Player shoot 確認)
- TC-005: ✓ PASS ([INPUT] Quit / [SCENE] TitleScene x2 確認)
- TC-006: ✓ PASS ([SCORE] Added 確認)
- 問題: なし
```

---

## テスト実行履歴

### [2026-05-18] Invader 消滅アニメ・スコア計算バグ修正
- TC-001: ✓ PASS (ExitCode=0)
- TC-002: ✓ PASS
- TC-003: ✓ PASS
- TC-004: ✓ PASS
- TC-005: ✓ PASS
- TC-006: ✓ PASS
- 変更内容: `Invader::isActive()` に消滅アニメ中の判定追加、スコア計算で `isAlive_()` を使用

### [2026-05-18] TestScriptLoader 導入・テストスクリプトの設定ファイル化
- TC-001: ✓ PASS (ExitCode=0、全期待ログ検証済み)
- TC-002: ✓ PASS ([TEST] PASS: [SCENE] TitleScene started)
- TC-003: ✓ PASS ([TEST] PASS: [INPUT] Enter / [SCENE] GameScene started)
- TC-004: ✓ PASS ([TEST] PASS: [INPUT] Player shoot)
- TC-005: ✓ PASS ([TEST] PASS: [INPUT] Quit / TitleScene)
- TC-006: ✓ PASS ([TEST] PASS: [SCORE] Added)
- 変更内容: `TestScriptLoader::load()` で `test-script.cfg` からイベントと期待ログを読み込む。`validateLogs()` でゲーム終了後に自動検証し、`[TEST] PASS/FAIL:` をログへ書き込む。ExitCode が全PASS=0/一部FAIL=1

### [2026-05-18] preCalc/postCalc 2フェーズ分割・SceneBase Actor 管理機能追加
- TC-001: ✓ PASS (ExitCode=0)
- TC-002: ✓ PASS
- TC-003: ✓ PASS
- TC-004: ✓ PASS
- TC-005: ✓ PASS
- TC-006: ✓ PASS
- 変更内容: `ActorBase::calc()` → `preCalc()/postCalc()`、`InvaderSwarm` を `ActorBase` 継承に変更、`GameScene` で `calcActors_()/cleanupActors_()` を使用

### [2026-05-17] InputHandler を Engine 層へ移植・TestInputScript 分離
- TC-001: ✓ PASS (ExitCode=0)
- TC-002: ✓ PASS
- TC-003: ✓ PASS
- TC-004: ✓ PASS
- TC-005: ✓ PASS
- TC-006: ✓ PASS
- 変更内容: `engine::InputHandler` + `engine::TestInputScript` 導入、Game 側 `input_handler` を互換シム化

### [2026-05-17] ログベーステスト基盤構築
- TC-001: ✓ PASS (ExitCode=0)
- TC-002: ✓ PASS
- TC-003: ✓ PASS
- TC-004: ✓ PASS
- TC-005: ✓ PASS
- TC-006: ✓ PASS ([SCORE] Added 10, Added 290 確認)
- 問題: なし
- ログ出力全文:
  ```
  [GAME] Start
  [SCENE] TitleScene started
  [INPUT] Enter pressed -> GameScene
  [SCENE] GameScene started, invaders=30
  [INPUT] Player shoot
  [SCORE] Added 10 (total=0)
  [SCORE] Added 290 (total=10)
  [INPUT] Quit pressed -> TitleScene
  [SCENE] TitleScene started
  [INPUT] Quit pressed -> game exit
  [GAME] End
  ```

## コア動作テスト（Phase 1: Basic Smoke Test）

必ず実行されるテスト。実装開始時に定義済み。

### TC-001: アプリケーション起動
- **説明**: exe を実行してクラッシュなく起動できるか
- **手順**: `.\build\InvaderGame.exe` を実行
- **期待結果**: ウィンドウが表示され、タイトル画面が描画される
- **判定**: クラッシュしなければ成功
- **関連ファイル**: `Game/src/main.cpp`, `Game/src/Scene/title_scene.cpp`

### TC-002: タイトル画面の表示
- **説明**: タイトル画面が正しく表示されるか
- **手順**: 起動後の初期状態を確認
- **期待結果**: タイトル画面が表示される
- **判定**: 画面にテキスト「TITLE」が含まれる
- **関連ファイル**: `Game/src/Scene/title_scene.cpp`

### TC-003: タイトル→ゲームシーン遷移
- **説明**: キー入力でゲームシーンに遷移できるか
- **手順**: 遷移キー（通常は Enter など）を入力
- **期待結果**: ゲームシーンが起動し、敵・プレイヤーが表示される
- **判定**: クラッシュせず、ゲーム画面が表示される
- **関連ファイル**: `Game/src/Scene/title_scene.cpp`, `Game/src/Scene/game_scene.cpp`

### TC-004: ゲーム基本操作
- **説明**: ゲームシーン内での基本操作が機能するか
- **手順**: 
  - プレイヤー移動キーを入力（通常は ← → キー）
  - 射撃キーを入力（通常はスペースキー）
- **期待結果**: プレイヤーが移動し、弾が発射される
- **判定**: クラッシュなく動作し、敵・プレイヤー・弾が表示される
- **関連ファイル**: `Game/src/Actor/player.cpp`, `Game/src/Actor/bullet.cpp`, `Game/src/input_handler.cpp`

### TC-005: ゲーム→タイトル遷移
- **説明**: ゲーム終了時にタイトルシーンに戻れるか
- **手順**: ゲーム終了キー（通常は ESC など）を入力
- **期待結果**: タイトル画面に戻る
- **判定**: クラッシュなく遷移し、タイトル画面が表示される
- **関連ファイル**: `Game/src/Scene/game_scene.cpp`, `Game/src/input_handler.cpp`

### TC-006: スコア計算（基本）
- **説明**: 敵撃破時にスコアが加算されるか
- **手順**: ゲーム中に敵を撃破する
- **期待結果**: スコアが増加する
- **判定**: スコア表示が変更される
- **関連ファイル**: `Game/src/Scene/score_manager.cpp`, `Game/src/Actor/invader.cpp`

---

## 拡張テスト（Phase 2以降: 実装追加時に増加）

実装内容に応じて以下に追記してください。

### TC-101: [敵パターン管理]（実装予定）
- **関連**: `docs/specs/invader.md` 参照

### TC-102: [UI レイアウト確認]（実装予定）
- **関連**: `docs/specs/ui.md` 参照

---

## テスト実行ログ

各実装ごとのテスト実行結果を記録します。

### [2026-05-18] 背景描画をEngine側(SceneBase)に移動
- **テスト実行者**: エージェント自動実行
- **実行コマンド**: `.\build\InvaderGame.exe --test`
- **テスト結果**: 
  - TC-001: ✓ PASS (ExitCode=0)
  - TC-002: ✓ PASS ([SCENE] TitleScene started)
  - TC-003: ✓ PASS ([INPUT] Enter / [SCENE] GameScene started)
  - TC-004: ✓ PASS ([INPUT] Player shoot)
  - TC-005: ✓ PASS ([INPUT] Quit / [SCENE] TitleScene x2)
  - TC-006: ✓ PASS ([SCORE] Added)
- **問題**: なし

### [実装日] <実装内容>
- **テスト実行者**: エージェント自動実行
- **実行コマンド**: `.\build\InvaderGame.exe`
- **テスト結果**: 
  - TC-001: ✓ PASS / ✗ FAIL
  - TC-002: ✓ PASS / ✗ FAIL
  - TC-003: ✓ PASS / ✗ FAIL
  - TC-004: ✓ PASS / ✗ FAIL
  - TC-005: ✓ PASS / ✗ FAIL
  - TC-006: ✓ PASS / ✗ FAIL
- **問題**: なし / （問題があれば記載）

---

## テストケース追加ガイドライン

新機能実装時に以下の判定で該当するテストケースを追加してください：

| 実装内容 | テストケース追加 | 追加先 |
|---------|-----------------|--------|
| プレイヤー機能（移動・射撃・アニメーション） | ゲーム操作系テスト | TC-104以降 |
| 敵機能（移動・射撃・敵パターン） | 敵表示・動作系テスト | TC-105以降 |
| UI / スコア | UI表示系テスト | TC-110以降 |
| シーン遷移 | シーン遷移系テスト | TC-120以降 |
| 入力システム | 入力応答系テスト | TC-130以降 |
| Engine層（FrameWork・Renderer） | アーキテクチャ系テスト | TC-200以降 |
| バグ修正 | 対応する既存テストを更新 | - |

**追加テストケース作成例:**
```markdown
### TC-10X: [新機能の説明]
- **説明**: 実装した機能の説明
- **手順**: テスト手順
- **期待結果**: 期待される動作
- **判定**: 成功判定の条件
- **関連ファイル**: 関連するソースファイルのパス
```
