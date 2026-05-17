#pragma once

#include "RenderQueue.h"
#include <string>

namespace engine {

/**
 * @class UIBase
 * @brief UI要素の基底クラス（Engine層）
 *
 * 文字列を画面の指定座標に描画するUIの基底クラス。
 * calc()内でsetText()を呼ぶことで動的テキストを表現できる。
 * isVisible()/setVisible()で表示/非表示を制御する。
 *
 * 使い方:
 *   - 固定テキスト  : コンストラクタで文字列を渡してそのまま使う
 *   - 動的テキスト  : サブクラスで calc() をオーバーライドし setText() を呼ぶ
 *   - 条件付き表示  : 外部から setVisible(bool) を呼ぶ、
 *                     またはサブクラスで isVisible() をオーバーライドする
 */
class UIBase {
public:
    UIBase(int x, int y, std::string text = "");
    virtual ~UIBase() = default;

    // 毎フレーム呼ばれる更新処理（デフォルトは何もしない）
    virtual void calc() {}

    // RenderQueue に描画コマンドを登録する（isVisible() が false なら何もしない）
    virtual void draw() const;

    // 表示状態の取得・設定
    virtual bool isVisible() const { return mVisible; }
    void setVisible(bool v) { mVisible = v; }

    // 位置の変更
    void setPos(int x, int y) { mX = x; mY = y; }

protected:
    int         mX;
    int         mY;
    std::string mText;  // draw()でsubmitStringされる文字列

    // サブクラスの calc() 内から呼ぶ
    void setText(const std::string& text) { mText = text; }

private:
    bool mVisible = true;
};

}  // namespace engine
