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
