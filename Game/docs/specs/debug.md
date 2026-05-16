# デバッグ機能 — 実装記録

## デバッグ機能：A キーで全敵削除 [2026-05-16]

### 対象ファイル
- `include/input_handler.h`, `src/input_handler.cpp`
- `include/game.h`, `src/game.cpp`

### 内容
- `KEY_BUTTONS` X マクロに `X(DebugKillAll, debugKillAll)` を追加
- `input_handler.cpp` で `'A'` キー（ASCII 0x41）の状態を `debugKillAll_` に設定
- `Game` クラスに `debugKillAllInvaders()` プライベートメソッドを追加
- `Game::processInput()` で A キー検出時に `debugKillAllInvaders()` を呼び出す

### 動作
- A キー押下で全敵に対して `kill()` を呼び出し
- スポーン中の敵も、生存敵も、一度に削除アニメーション開始
- ゲーム開発時のテスト・デバッグ用機能

### ビルド・デプロイ
- 成功、git push 完了
