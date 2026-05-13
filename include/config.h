#pragma once

// ゲーム全体で共有する定数・型定義

namespace Config {
    constexpr int FIELD_WIDTH  = 80;   // フィールド幅（文字数）
    constexpr int FIELD_HEIGHT = 25;   // フィールド高さ（行数）
    constexpr int FPS          = 30;   // 目標フレームレート
    constexpr int FRAME_MS     = 1000 / FPS;

    constexpr int PLAYER_LIVES      = 3;
    constexpr int MAX_LEVELS        = 5;

    constexpr int INVADER_COLS      = 5;
    constexpr int INVADER_ROWS      = 3;

    inline const char* const HIGHSCORE_FILE = "save/highscore.dat";
}
