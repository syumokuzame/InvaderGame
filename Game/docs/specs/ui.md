# UI・スコア機能 — 実装記録

## 上部UI実装 [2026-05-15]

### 対象ファイル
- `include/renderer.h`, `src/renderer.cpp`
- `include/game.h`, `src/game.cpp`
- `src/score_manager.cpp`
- `include/config.h`
- `src/invader_swarm.cpp`

### 仕様

#### UI レイアウト
- 1行目（Y=0）：
  - 左端（X=1）: タイトル "InvaderGame"
  - 中央右（X=50）: スコア表示（例："Score: 12345"）
  - 右端（X=72）: 経過時間表示（例："00:45"）
- 2行目（Y=1）：
  - 左側（X=2）: 操作方法表示（"LEFT/RIGHT: Move  SPACE: Shoot  P: Pause  Q: Quit"）
- ゲームエリア: Y=2（上枠）から Y=24（下枠）に変更
- `Config::UI_HEIGHT = 2` を定数として追加
- 敵の配置マージンを `UI_HEIGHT` に対応（敵の開始Y座標 = UI_HEIGHT + 2）

#### 実装内容
- `Game` にゲーム開始時刻 `gameStartTime_` メンバを追加
- `ScoreManager` クラスを初期化（スコア管理・ハイスコア保存/読み込み）
- `Renderer::drawHeader()` でタイトル・スコア・時間を描画
- `Renderer::drawInstructions()` で操作方法を描画
- `Game::render()` で上部2行のUI描画と、ゲームエリア（Y=2以下）のメイン表示を統合
- `src/score_manager.cpp` を実装（ハイスコア管理、ファイル I/O）
- `invader_swarm.cpp` で敵の配置マージンを `UI_HEIGHT` に対応

### 動作
- UI表示確認済み
- スコア・時間は毎フレーム更新
- 敵配置がUI領域を避けて正しく配置される
- ビルド成功、git push 完了
