#pragma once

#include <vector>
#include <string>

namespace engine {

// Actor の描画形状を定義するモデル
// cells[row][col] の文字が1マスに対応する
// ' ' (スペース) は透過扱い（背後の描画を上書きしない）
struct ActorModel {
    // 描画セル（行方向に格納）
    std::vector<std::string> cells;

    // ピボット: Actor の (x, y) がモデル上のどのセルに対応するか
    // cells[pivotY][pivotX] が Actor の原点になる
    int pivotX = 0;
    int pivotY = 0;

    // モデルが空かどうか（描画不要）
    bool empty() const { return cells.empty(); }

    // 行数
    int height() const { return static_cast<int>(cells.size()); }

    // 最大列数
    int width() const {
        int w = 0;
        for (const auto& row : cells)
            if (static_cast<int>(row.size()) > w) w = static_cast<int>(row.size());
        return w;
    }

    // 指定セルの文字取得（範囲外は '\0'）
    char at(int row, int col) const {
        if (row < 0 || row >= height()) return '\0';
        if (col < 0 || col >= static_cast<int>(cells[row].size())) return '\0';
        return cells[row][col];
    }

    // 指定セルに文字をセット（範囲外は無視）
    void set(int row, int col, char ch) {
        if (row >= 0 && row < height() &&
            col >= 0 && col < static_cast<int>(cells[row].size()))
            cells[row][col] = ch;
    }

    // モデルをリセット（全セルをスペースで初期化）
    void resize(int rows, int cols) {
        cells.assign(rows, std::string(cols, ' '));
    }
};

}  // namespace engine
