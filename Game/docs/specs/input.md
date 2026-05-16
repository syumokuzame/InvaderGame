# 入力機能 — 実装記録

## InputHandler の X マクロ化 [2026-05-15]

### 対象ファイル
- `include/input_handler.h`, `src/input_handler.cpp`

### 内容
- 6 つのキー入力（左、右、発射、ポーズ、終了、決定）を `KEY_BUTTONS` X マクロで統一
- メンバ変数、ゲッター関数、リセット処理が自動生成されるように

### 仕様
- 新しいキー入力追加時は `KEY_BUTTONS` マクロ定義のみ変更で OK
- 関数・変数の重複定義排除で保守性向上

---

## 入力ハンドラー修正（GetAsyncKeyState 改善） [2026-05-15]

### 対象ファイル
- `src/input_handler.cpp`

### 問題
- `_kbhit()` / `_getch()` を使用した入力検出が不安定
- プレイヤーキー（左右）が反応しない
- 弾発射キー（スペース）が反応しない

### 修正内容
- `#include <conio.h>` から `#include <windows.h>` に変更
- `_kbhit()` / `_getch()` から Windows API の `GetAsyncKeyState()` に変更
- キーコード判定を仮想キーコード（VK_LEFT, VK_RIGHT, VK_SPACE 等）に統一
- 各キーの状態を戻り値の最上位ビット（0x8000）で判定

### 実装の詳細

| キー | 仮想キーコード | 対象メンバ |
|------|---------------|-----------|
| 左矢印キー | VK_LEFT | left_ |
| 右矢印キー | VK_RIGHT | right_ |
| スペースキー | VK_SPACE | shoot_ |
| Escキー | VK_ESCAPE | quit_ |

### 動作
- 左右キーでプレイヤー移動が正常に動作
- スペースキーで弾発射が正常に動作
- ビルド成功、git push 完了
