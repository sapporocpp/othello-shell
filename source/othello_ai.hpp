#ifndef OTHELLO_AI_HPP
#define OTHELLO_AI_HPP

#define OTHELLO_AI_USE_CPP11

#include <vector>
#include <map>
#include <iostream>
#include <utility>
#include <cstddef>
#include <sstream>

namespace Othello{
#ifdef OTHELLO_AI_USE_CPP11
    enum class Color : char {
        BLACK = 1, // 黒が置かれている
        WHITE = 2, // 白が置かれている
        EMPTY = 0, // 石は置かれていない
        INVALID = 4 // 無効な座標を指定した場合など
    };
#else
    class Color {
    private:
        char val_;
    public:
        Color();
        Color(char val);
        operator char() const;
        bool operator ==(Color other) const;
        bool operator <(Color other) const;

        static const char BLACK;
        static const char WHITE;
        static const char EMPTY;
        static const char INVALID;
    };
#endif

    // 対戦相手の色を得る
    Color get_opponent_color(Color color);
    
    // 石の番号（BLACK, WHITEなど）を石の文字（'B', 'W'など）に変換
    // 引数が不正でも特段のチェックはしていない
    char get_piece_name(Color color);
    
    // 対局についての情報（自分の色はどちらか、盤面の行数/列数）
    class GameInfo{
    private:
        Color my_color_, opponent_color_;
        int rows_, cols_;
        
    public:
        GameInfo(Color my, int r, int c);
        Color my_color() const;
        Color opponent_color() const;
        int rows() const;
        int cols() const;
    };
    
    constexpr int COORD_NULL = -1;
    
    class Coord{
    private:
        int row_, col_;
    public:
        Coord();
        Coord(int r, int c);
        Coord(const Coord & other);
        
        int row() const;
        int col() const;
        bool is_valid() const;
    };
    
    class Board{
    private:
        int rows_, cols_;
        std::vector<Color> places_;
        
        // 補助関数：座標(i, j)が盤面の内側か
        bool is_within_board(int i, int j) const;
        
    public:
        // ---------- コンストラクタ ----------
        
        Board();
        Board(int rows, int cols); // サイズを指定して盤面を生成
        Board(const Board & other); // 他の盤面インスタンスをコピー
        static Board init(int rows, int cols); // 初期盤面（黒石2個、白石2個がある）を生成
        
        // ---------- 各種処理 ----------
        
        // 盤面の大きさ
        int rows() const;
        int cols() const;
        
        // 石の数を取得。
        // Colorクラスのインスタンスをキー、石の数を値とする連想配列になっている
        std::map<Color, std::size_t> pieces() const;
        
        // 位置を指定して石を取得
        Color get(const Coord & coord) const;
        Color get(int i, int j) const;
        
        // 位置を指定してmy_colorの石を設置
        // ※ここでいう「設置」とは、単にその場所にその石がある状態にするだけで
        //   ゲームのルールとして置く（裏返したりもする）わけではない
        bool put_only(const Coord & coord, Color color);
        bool put_only(int i, int j, Color color);
        
        // 位置を指定してmy_colorの石を置き、裏返す。
        // 裏返した石の数を返す。
        // その場所に石が置けない場合は0を返す（裏返した石の数が0の場合も）。
        std::size_t put_and_flip(const Coord & coord, Color my_color);
        std::size_t put_and_flip(int i, int j, Color my_color);
        
        // 表示する
        // coord および current_r, current_cは「石がどこに置かれたか」（省略可能）
        std::string state(const Coord & coord) const;
        std::string state(int current_r, int current_c) const;
        std::string state(void) const;
    };
}

#endif // OTHELLO_AI_HPP
