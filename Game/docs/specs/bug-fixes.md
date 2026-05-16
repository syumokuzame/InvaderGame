# バグ修正 — 実装記録

## Clear 状態での弾の動作修正 [2026-05-16]

### 対象ファイル
- `src/game.cpp`

### 問題
- Clear 状態で弾を発射すると、弾がその場で静止する

### 原因
- `Game::update()` で弾の更新処理（`for (auto& b : bullets_) { b.update(); }`）が `if (state_ == GameState::Playing)` ブロック内に閉じ込められていた

### 修正内容
- プレイヤーの `update()` をゲーム状態チェックの外に出す
- 弾の更新処理 (`bullets_[i].update()`) をゲーム状態チェックの外に出す
- インベーダー群の更新処理（`swarm_.update(bullets_)`）は Playing 状態のみで実行
- 結果：Clear 状態でもプレイヤーは移動でき、弾も通常通り飛行する

### 動作
- Clear 表示中に弾を発射すると、弾が画面上部に向かって正常に飛行する

### ビルド・デプロイ
- 成功、git push 完了
