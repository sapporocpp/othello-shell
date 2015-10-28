#ifndef OTHELLO_AI_HPP
#define OTHELLO_AI_HPP

#define OTHELLO_AI_USE_CPP11

#include <vector>
#include <map>
#include <iostream>
#include <utility>
#include <cstddef>

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
		Color() : val_(0) {}
		Color(char val) : val_(val) {}
		operator char() const { return static_cast<char>(val_); }
		bool operator ==(Color other) const { return val_ == other.val_; }
		bool operator <(Color other) const { return val_ < other.val_; }

		static const char BLACK;
		static const char WHITE;
		static const char EMPTY;
		static const char INVALID;
	};
	const char Color::BLACK = 1;
	const char Color::WHITE = 2;
	const char Color::EMPTY = 0;
	const char Color::INVALID = 4;
#endif

    // 対戦相手の色を得る
    Color get_opponent_color(Color color){
        switch(color){
        case Color::BLACK:
            return Color::WHITE;
        case Color::WHITE:
            return Color::BLACK;
        default:
            return color;
        }
    }
    
    // 石の番号（BLACK, WHITEなど）を石の文字（'B', 'W'など）に変換
    // 引数が不正でも特段のチェックはしていない
    char get_piece_name(Color color){
        return ".BW"[static_cast<size_t>(color)];
    }
    
    // 対局についての情報（自分の色はどちらか、盤面の行数/列数）
    class GameInfo{
    private:
        Color my_color_, opponent_color_;
        int rows_, cols_;
        
    public:
        GameInfo(Color my, int r, int c) : my_color_(my), rows_(r), cols_(c){
            opponent_color_ = get_opponent_color(my);
        }
        
        inline Color my_color() const{ return my_color_; }
        inline Color opponent_color() const{ return opponent_color_; }
        inline int rows() const{ return rows_; }
        inline int cols() const{ return cols_; }
    };
    
    constexpr int COORD_NULL = -1;
    
    class Coord{
    private:
        int row_, col_;
    public:
        Coord() : row_(COORD_NULL), col_(COORD_NULL) {}
        Coord(int r, int c) : row_(r), col_(c) {}
        Coord(const Coord & other) : row_(other.row_), col_(other.col_) {}
        
        inline int row() const{ return row_; }
        inline int col() const{ return col_; }
        inline bool is_valid() const{ return(row_ != COORD_NULL || col_ != COORD_NULL); }
    };
    
    class Board{
    private:
        int rows_, cols_;
        std::vector<Color> places_;
        
        // 補助関数：座標(i, j)が盤面の内側か
        bool is_within_board(int i, int j) const{
            return(
                (i >= 0 && i < rows_) &&
                (j >= 0 && j < cols_)
            );
        }
        
    public:
        // ---------- コンストラクタ ----------
        
        // 空の盤面を生成
        Board() : rows_(0), cols_(0) {}
        
        Board(int rows, int cols)
        : rows_(rows), cols_(cols),
          places_(rows * cols, Color::EMPTY) {}
        
        // 別の盤面をコピー
        Board(const Board & other)
        : rows_(other.rows_), cols_(other.cols_), places_(other.places_) {}
        
        // 初期盤面を生成
        static Board init(int rows, int cols){
            Board board(rows, cols);
            board.put_only(rows/2 - 1, cols/2 - 1, Color::WHITE);
            board.put_only(rows/2 - 1, cols/2,     Color::BLACK);
            board.put_only(rows/2,     cols/2 - 1, Color::BLACK);
            board.put_only(rows/2,     cols/2,     Color::WHITE);
            return board;
        }
        
        // ---------- 各種処理 ----------
        
        // 盤面の大きさ
        inline int rows() const{ return rows_; }
        inline int cols() const{ return cols_; }
        
        // 石の数を取得。
        // Colorクラスのインスタンスをキー、石の数を値とする連想配列になっている
        std::map<Color, std::size_t> pieces(){
            std::map<Color, std::size_t> result;
            Color c;
            
            for(int i = 0; i < rows_; ++i){
                for(int j = 0; j < cols_; ++j){
                    c = get(i, j);
                    if(result.count(c) == 0) result[c] = 0;
                    ++(result[c]);
                }
            }
            
            return result;
        }
        
        // 位置を指定して石を取得
        inline Color get(const Coord & coord) const{
            return get(coord.row(), coord.col());
        }
        
        Color get(int i, int j) const{
            if(!is_within_board(i, j)) return Color::INVALID;
            return places_[i * cols_ + j];
        }
        
        // 位置を指定してmy_colorの石を設置
        // ※ここでいう「設置」とは、単にその場所にその石がある状態にするだけで
        //   ゲームのルールとして置く（裏返したりもする）わけではない
        inline bool put_only(const Coord & coord, Color color){
            return put_only(coord.row(), coord.col(), color);
        }
        
        bool put_only(int i, int j, Color color){
            if(!is_within_board(i, j)) return false;
            places_[i * cols_ + j] = color;
            return true;
        }
        
        // 位置を指定してmy_colorの石を置き、裏返す。
        // 裏返した石の数を返す。
        // その場所に石が置けない場合は0を返す（裏返した石の数が0の場合も）。
        inline std::size_t put_and_flip(const Coord & coord, Color my_color){
            return put_and_flip(coord.row(), coord.col(), my_color);
        }
        
        std::size_t put_and_flip(int i, int j, Color my_color){
            if(!is_within_board(i, j)) return 0;
            
            // もしその場所にすでに石がある場合は、置けない
            if(get(i, j) != Color::EMPTY) return 0;
            
            Color opponent_color = get_opponent_color(my_color); // 相手の色
            int flipnum = 0; // 裏返した数
            
            // 8方向に石を伸ばしていく
            int distance;
            for(int row_direction = -1; row_direction <= 1; ++row_direction){
                for(int col_direction = -1; col_direction <= 1; ++col_direction){
                    // 8方向に伸ばすので、伸ばせない場合は除外
                    if(row_direction == 0 && col_direction == 0) continue;
                    
                    // まず、隣が相手の石でなければ、石は裏返せない
                    if(get(i + row_direction, j + col_direction) != opponent_color){
                        continue;
                    }
                    
                    // そして、そこから石を伸ばしていって、
                    // 「対戦相手の石が続いて、そのあと自分の石が現れる」場所を
                    // 見つける
                    distance = 2;
                    while(get(i + distance * row_direction, j + distance * col_direction) == opponent_color){
                        ++distance;
                    }
                    
                    if(get(i + distance * row_direction, j + distance * col_direction) == my_color){
                        // 「相手の石が続く」ことが切れた場所にある石が
                        // 自分の石であれば（空白も不可）、そこで石を裏返せる。
                        
                        // 裏返した石の数を加算
                        flipnum += distance - 1;
                        
                        // 実際に裏返す
                        --distance;
                        for(; distance >= 1; --distance){
                            put_only(i + distance * row_direction, j + distance * col_direction, my_color);
                        }
                        put_only(i, j, my_color);
                    }
                }
            }
            
            return flipnum;
        }
        
        // 表示する
        // current_r, current_cは「石がどこに置かれたか」（省略可能）
        inline void display(const Coord & coord) const{
            display(coord.row(), coord.col());
        }
        
        void display(int current_r, int current_c) const{
            for(int i = 0; i < rows_; ++i){
                for(int j = 0; j <= cols_; ++j){
                    if(i == current_r && j == current_c){
                        std::cout << '[';
                    }else if(i == current_r && j == current_c+1){
                        std::cout << ']';
                    }else{
                        std::cout << ' ';
                    }
                    if(j == cols_) break;
                    std::cout << get_piece_name(get(i, j));
                }
                std::cout << std::endl;
            }
        }
        
        void display(void) const{
            display(-1, -1);
        }
    };
}

#endif // OTHELLO_AI_HPP
