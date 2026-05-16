# 敵機機能 — 実装記録

## 敵機システムの実装 [2026-05-15]

### 対象ファイル
- `include/invader.h`, `src/invader.cpp`
- `include/invader_swarm.h`, `src/invader_swarm.cpp`
- `include/game.h`, `src/game.cpp`

### 内容
- `Invader` クラス：各敵機の状態・描画・消滅アニメ管理
- `InvaderSwarm` クラス：敵機群の配置・更新・当たり判定
- 敵機キャラ：上段M(30pt)、中段W(20pt)、下段V(10pt)
- 消滅アニメ：12フレーム（* → + → . → 消滅）
- `FIELD_WIDTH` と `FIELD_HEIGHT` に応じた動的配置

### 仕様
- 敵機5列×3行（計15体）がフィールド内に均等配置
- プレイヤー弾での消滅確認済み

---

## 敵機の4マス表現と動的個数決定 [2026-05-15]

### 対象ファイル
- `src/invader.cpp`, `src/invader_swarm.cpp`

### 内容
- 敵機を1マスから4マス（テトリスのTミノ形）に変更
- 敵の形：上行 `***` （左-中-右）、下行 `*` （中央）
- フィールドサイズから敵の列数・行数を自動計算

### 仕様
- 敵1体サイズ：幅3、高さ2；間隔1で配置
- 列数：3～10列の範囲（フィールド幅依存）
- 行数：2～3行の範囲（フィールド高さ依存）
- 当たり判定をTミノの4マス全てに対応
- `Config::INVADER_COLS` と `Config::INVADER_ROWS` は使用しなくなった（敵数は動的計算）

---

## 敵機の移動機能実装 [2026-05-15]

### 対象ファイル
- `src/invader.cpp`, `src/invader_swarm.cpp`

### 内容
- 敵機の向きを逆さまTミノから上向きTミノに修正（上行***、下行*）
- 敵機集団の左右移動を実装
- 敵機の最左・最右の座標から フィールド枠（左壁・右壁）との当たり判定
- 枠に到達したら移動方向を反転

### 仕様
- 全敵機が統一して移動（毎 speed フレーム毎に1マス移動）
- 移動速度: 20フレーム間隔（60FPS時、約0.33秒ごと）

### 動作
- 敵機集団が左右に繰り返し移動、端で折り返す

---

## 敵スポーンアニメーション実装 [2026-05-16]

### 対象ファイル
- `include/invader.h`, `src/invader.cpp`
- `include/game.h`, `src/game.cpp`

### 仕様

#### スポーンアニメーション
- ゲーム開始時に敵は画面に出ていない状態で開始
- スポーン・リスポーン時にアニメーション表現（4フレーム）：
  - フレーム 0: `.` (小ドット)
  - フレーム 1: `o` (中ドット)
  - フレーム 2: `O` (大ドット)
  - フレーム 3: `*` (アスタリスク)
  - フレーム 4 以降: T-ミノ形の敵として正常表示・当たり判定有効
- 各敵は個別にスポーンフレームをカウント
- リスポーン時も同じアニメーション適用

### 実装内容

#### Invader クラスの新規メンバ
- `spawnFrame_` : スポーンフレームカウンター（0-3=スポーン中、4以上=スポーン完了）

#### 新規メソッド
- `Invader::spawn()` : spawnFrame_ をリセット
- `Invader::isSpawning()` : スポーン中か判定
- `Invader::isActive()` : スポーン中 || 生存中

#### 修正内容
- `Invader::update()` で spawnFrame_ をインクリメント、4に到達時に alive_ を true に設定
- `Invader::render()` でスポーン状態に応じた文字を描画
- `Invader::isAlive()` がスポーン完了後の敵のみを示すように修正
- `InvaderSwarm::allDefeated()` 修正：スポーン中の敵がいれば false、生存敵がいれば false、スポーン敵も生存敵もいない場合のみ true
- `InvaderSwarm::reset()` で敵を spawn() 状態で生成
- `Game::update()` で敵数カウント時に isActive() を使用（スポーン中も含める）
- 当たり判定時は isAlive() を使用（スポーン完了敵のみ）

### 動作
- ゲーム開始時に敵が画面に見えない状態から段階的に出現
- リスポーン時も同じスポーンアニメーション表示

---

## 敵全滅判定バグ修正 [2026-05-15]

### 対象ファイル
- `include/invader_swarm.h`, `src/invader_swarm.cpp`

### 問題
- スポーン中の敵が alive_=false のため、allDefeated() が誤判定し、敵が画面に出ていないうちに Clear になってしまった

### 修正内容
- `Invader::isSpawning()` メソッド追加
- `InvaderSwarm::allDefeated()` ロジック改善：
  - スポーン中の敵をチェック（isSpawning() = true なら敵が存在）
  - 生存敵をチェック（isAlive() = true なら敵が存在）
  - 両方ともなくなった場合のみ全滅と判定

### 検証
- ゲーム開始時に敵が出現するまで待機
- 敵を倒すまで Clear にならない
- ビルド成功、git push 完了

---

## 敵撃破スコア加算・Clear表示・敵リスポーン [2026-05-15]

### 対象ファイル
- `include/invader_swarm.h`, `src/invader_swarm.cpp`
- `include/game.h`, `src/game.cpp`
- `include/renderer.h`, `src/renderer.cpp`

### 仕様

#### スコア・Clear・リスポーン
- 敵を1体倒すたびに10点加算（ScoreManager::addScore()）
- 敵がすべて消滅した場合、GameState を GameClear に変更
- 画面中央に「CLEAR!」と表示
- 1秒（60フレーム）待機
- その後、敵をすべてリスポーン（InvaderSwarm::reset()）し GameState を Playing に戻す

### 実装内容

#### Game クラスの新規メンバ
- `clearCounter_` : Clear 状態カウンター
- `lastAliveCount_` : 前フレームの生存敵数

#### 処理フロー
- `Game::update()` で：
  - Playing 状態時は通常ゲーム処理
  - 生存敵数を毎フレーム確認し、撃破された敵数 × 10点をスコア加算
  - 敵が全滅すれば GameClear 状態へ
  - Clear 状態で 60フレーム待機後、敵をリスポーン＆Playing 状態へ復帰
- `Game::render()` で GameClear 状態時に「CLEAR!」メッセージを画面中央に表示
- Game コンストラクタで初期敵数をカウント

### 動作
- 敵撃破時にスコア加算確認済み
- 全敵撃破で「CLEAR!」表示を確認済み
- 1秒後に敵がリスポーン確認済み
- ビルド成功、git push 完了
