# ゲーム仕様記録 — InvaderGame

プロジェクト要件定義: [docs/requirements.md](requirements.md)

各実装指示の完了後、エージェントが自動追記します。

---

## 実装ログ

### [2026-05-15] 弾発射システムの実装
- **対象ファイル**: `include/bullet.h`, `src/bullet.cpp`, `include/player.h`, `src/player.cpp`, `include/game.h`, `src/game.cpp`
- **内容**: 
  - `Bullet` クラスを実装（上方向に移動、フィールド外で自動無効化）
  - `Player::shoot()` を実装（同時1発制限）
  - `Game` で弾の管理・更新・当たり判定・描画を統合
  - スペースキーで弾発射
- **仕様**: 敵機なし状態で弾の動作確認済み

### [2026-05-15] InputHandler の X マクロ化
- **対象ファイル**: `include/input_handler.h`, `src/input_handler.cpp`
- **内容**: 
  - 6 つのキー入力（左、右、発射、ポーズ、終了、決定）を `KEY_BUTTONS` X マクロで統一
  - メンバ変数、ゲッター関数、リセット処理が自動生成されるように
  - 新しいキー入力追加時は `KEY_BUTTONS` マクロ定義のみ変更で OK
- **保守性向上**: 関数・変数の重複定義排除

### [2026-05-15] 敵機システムの実装
- **対象ファイル**: `include/invader.h`, `src/invader.cpp`, `include/invader_swarm.h`, `src/invader_swarm.cpp`, `include/game.h`, `src/game.cpp`
- **内容**: 
  - `Invader` クラス：各敵機の状態・描画・消滅アニメ管理
  - `InvaderSwarm` クラス：敵機群の配置・更新・当たり判定
  - 敵機キャラ：上段M(30pt)、中段W(20pt)、下段V(10pt)
  - 消滅アニメ：12フレーム（* → + → . → 消滅）
  - `FIELD_WIDTH` と `FIELD_HEIGHT` に応じた動的配置
- **仕様**: 敵機5列×3行（計15体）がフィールド内に均等配置、プレイヤー弾での消滅確認済み

### [2026-05-15] 敵機の4マス表現と動的個数決定
- **対象ファイル**: `src/invader.cpp`, `src/invader_swarm.cpp`
- **内容**: 
  - 敵機を1マスから4マス（テトリスのTミノ形）に変更
  - 敵の形：上行 `***` （左-中-右）、下行 `*` （中央）
  - フィールドサイズから敵の列数・行数を自動計算
  - 敵1体サイズ：幅3、高さ2；間隔1で配置
  - 列数：3～10列の範囲（フィールド幅依存）
  - 行数：2～3行の範囲（フィールド高さ依存）
  - 当たり判定をTミノの4マス全てに対応
- **仕様変更**: `Config::INVADER_COLS` と `Config::INVADER_ROWS` は使用しなくなった（敵数は動的計算）

### [2026-05-15] 敵機の移動機能実装
- **対象ファイル**: `src/invader.cpp`, `src/invader_swarm.cpp`
- **内容**: 
  - 敵機の向きを逆さまTミノから上向きTミノに修正（上行***、下行*）
  - 敵機集団の左右移動を実装
  - 敵機の最左・最右の座標から フィールド枠（左壁・右壁）との当たり判定
  - 枠に到達したら移動方向を反転
  - 全敵機が統一して移動（毎 speed フレーム毎に1マス移動）
  - 移動速度: 20フレーム間隔（60FPS時、約0.33秒ごと）
- **動作**: 敵機集団が左右に繰り返し移動、端で折り返す

### [2026-05-15] 上部UI実装
- **対象ファイル**: `include/renderer.h`, `src/renderer.cpp`, `include/game.h`, `src/game.cpp`, `src/score_manager.cpp`, `include/config.h`, `src/invader_swarm.cpp`
- **仕様**:
  - 1行目（Y=0）：
    - 左端（X=1）: タイトル "InvaderGame"
    - 中央右（X=50）: スコア表示（例："Score: 12345"）
    - 右端（X=72）: 経過時間表示（例："00:45"）
  - 2行目（Y=1）：
    - 左側（X=2）: 操作方法表示（"LEFT/RIGHT: Move  SPACE: Shoot  P: Pause  Q: Quit"）
  - ゲームエリア: Y=2（上枠）から Y=24（下枠）に変更
  - `Config::UI_HEIGHT = 2` を定数として追加
  - 敵の配置マージンを `UI_HEIGHT` に対応（敵の開始Y座標 = UI_HEIGHT + 2）
- **実装内容**:
  - `Game` にゲーム開始時刻 `gameStartTime_` メンバを追加
  - `ScoreManager` クラスを初期化（スコア管理・ハイスコア保存/読み込み）
  - `Renderer::drawHeader()` でタイトル・スコア・時間を描画
  - `Renderer::drawInstructions()` で操作方法を描画
  - `Game::render()` で上部2行のUI描画と、ゲームエリア（Y=2以下）のメイン表示を統合
  - `src/score_manager.cpp` を実装（ハイスコア管理、ファイル I/O）
  - `invader_swarm.cpp` で敵の配置マージンを `UI_HEIGHT` に対応
- **動作**: 
  - UI表示確認済み、スコア・時間は毎フレーム更新
  - 敵配置がUI領域を避けて正しく配置される
  - ビルド成功、git push 完了

### [2026-05-15] 敵撃破スコア加算・Clear表示・敵リスポーン
- **対象ファイル**: `include/invader_swarm.h`, `src/invader_swarm.cpp`, `include/game.h`, `src/game.cpp`, `include/renderer.h`, `src/renderer.cpp`
- **仕様**:
  - 敵を1体倒すたびに10点加算（ScoreManager::addScore()）
  - 敵がすべて消滅した場合、GameState を GameClear に変更
  - 画面中央に「CLEAR!」と表示
  - 1秒（60フレーム）待機
  - その後、敵をすべてリスポーン（InvaderSwarm::reset()）し GameState を Playing に戻す
- **実装内容**:
  - `Game` に `clearCounter_`（Clear状態カウンター）と `lastAliveCount_`（前フレームの生存敵数）メンバを追加
  - `Game::update()` で：
    - Playing 状態時は通常ゲーム処理
    - 生存敵数を毎フレーム確認し、撃破された敵数 × 10点をスコア加算
    - 敵が全滅すれば GameClear 状態へ
    - Clear 状態で 60フレーム待機後、敵をリスポーン＆Playing 状態へ復帰
  - `Game::render()` で GameClear 状態時に「CLEAR!」メッセージを画面中央に表示
  - `Game` コンストラクタで初期敵数をカウント
- **動作**: 
  - 敵撃破時にスコア加算確認済み
  - 全敵撃破で「CLEAR!」表示を確認済み
  - 1秒後に敵がリスポーン確認済み
  - ビルド成功、git push 完了

### [2026-05-15] 入力ハンドラー修正（GetAsyncKeyState 改善）
- **対象ファイル**: `src/input_handler.cpp`
- **問題**:
  - `_kbhit()` / `_getch()` を使用した入力検出が不安定
  - プレイヤーキー（左右）が反応しない
  - 弾発射キー（スペース）が反応しない
- **修正内容**:
  - `#include <conio.h>` から `#include <windows.h>` に変更
  - `_kbhit()` / `_getch()` から Windows API の `GetAsyncKeyState()` に変更
  - キーコード判定を仮想キーコード（VK_LEFT, VK_RIGHT, VK_SPACE 等）に統一
  - 各キーの状態を戻り値の最上位ビット（0x8000）で判定
- **実装の詳細**:
  - VK_LEFT（左矢印キー） → left_
  - VK_RIGHT（右矢印キー） → right_
  - VK_SPACE（スペースキー） → shoot_
  - VK_ESCAPE（Escキー） → quit_
  - その他は従来どおり
- **動作**: 
  - 左右キーでプレイヤー移動が正常に動作
  - スペースキーで弾発射が正常に動作
  - ビルド成功、git push 完了

### [2026-05-15] 敵スポーンアニメーション実装（計画中）
- **対象ファイル**: `include/invader.h`, `src/invader.cpp`, `include/game.h`, `src/game.cpp`
- **仕様**:
  - ゲーム開始時に敵は画面に出ていない状態で開始
  - スポーン・リスポーン時にアニメーション表現（4フレーム）：
    - フレーム 0-3: `.` (小ドット) → `o` (中ドット) → `O` (大ドット) → `*` (アスタリスク)
    - フレーム 4 以降: T-ミノ形の敵として正常表示・当たり判定有効
  - 各敵は個別にスポーンフレームをカウント
  - リスポーン時も同じアニメーション適用
- **実装内容**:
  - `Invader` クラスに `spawnFrame_` メンバ追加（-1=スポーン未開始、0-3=スポーン中、4以上=スポーン完了）
  - `Invader::update()` で spawnFrame_ をインクリメント
  - `Invader::render()` でスポーン状態に応じた文字を描画
  - `Invader::isAlive()` がスポーン完了（spawnFrame_ >= 4）時のみ true を返すように修正
  - `InvaderSwarm::reset()` で敵を spawnFrame_ = 0 の状態で生成（スポーン予約）
  - `Game::update()` で敵の当たり判定をスポーン完了敵のみに限定

---

*計画作成済み*
