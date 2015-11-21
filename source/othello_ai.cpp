#include "othello_ai.hpp"

namespace Othello{
#ifdef OTHELLO_AI_USE_CPP11
    // define nothing
#else
    bool Color::Color() : val_(0) {}
    bool Color::Color(char val) : val_(val) {}
    Color::operator char() const { return static_cast<char>(val_); }
    bool Color::operator ==(Color other) const { return val_ == other.val_; }
    bool Color::operator <(Color other) const { return val_ < other.val_; }
    
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
    GameInfo::GameInfo(Color my, int r, int c) : my_color_(my), rows_(r), cols_(c){
        opponent_color_ = get_opponent_color(my);
    }
    
    Color GameInfo::my_color() const{ return my_color_; }
    Color GameInfo::opponent_color() const{ return opponent_color_; }
    int GameInfo::rows() const{ return rows_; }
    int GameInfo::cols() const{ return cols_; }
    
    Coord::Coord() : row_(COORD_NULL), col_(COORD_NULL) {}
    Coord::Coord(int r, int c) : row_(r), col_(c) {}
    Coord::Coord(const Coord & other) : row_(other.row_), col_(other.col_) {}
    int Coord::row() const{ return row_; }
    int Coord::col() const{ return col_; }
    bool Coord::is_valid() const{ return(row_ != COORD_NULL || col_ != COORD_NULL); }
    
    // 補助関数：座標(i, j)が盤面の内側か
    bool Board::is_within_board(int i, int j) const{
        return(
            (i >= 0 && i < rows_) &&
            (j >= 0 && j < cols_)
        );
    }
        
    // ---------- コンストラクタ ----------
    
    // 空の盤面を生成
    Board::Board() : rows_(0), cols_(0) {}
    
    Board::Board(int rows, int cols)
    : rows_(rows), cols_(cols),
      places_(rows * cols, Color::EMPTY) {}
    
    // 別の盤面をコピー
    Board::Board(const Board & other)
    : rows_(other.rows_), cols_(other.cols_), places_(other.places_) {}
    
    // 初期盤面を生成
    Board Board::init(int rows, int cols){
        Board board(rows, cols);
        board.put_only(rows/2 - 1, cols/2 - 1, Color::WHITE);
        board.put_only(rows/2 - 1, cols/2,     Color::BLACK);
        board.put_only(rows/2,     cols/2 - 1, Color::BLACK);
        board.put_only(rows/2,     cols/2,     Color::WHITE);
        return board;
    }
    
    // ---------- 各種処理 ----------
    
    // 盤面の大きさ
    int Board::rows() const{ return rows_; }
    int Board::cols() const{ return cols_; }
    
    // 石の数を取得。
    // Colorクラスのインスタンスをキー、石の数を値とする連想配列になっている
    std::map<Color, std::size_t> Board::pieces() const {
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
    Color Board::get(const Coord & coord) const{
        return get(coord.row(), coord.col());
    }
    
    Color Board::get(int i, int j) const{
        if(!is_within_board(i, j)) return Color::INVALID;
        return places_[i * cols_ + j];
    }
    
    // 位置を指定してmy_colorの石を設置
    // ※ここでいう「設置」とは、単にその場所にその石がある状態にするだけで
    //   ゲームのルールとして置く（裏返したりもする）わけではない
    bool Board::put_only(const Coord & coord, Color color){
        return put_only(coord.row(), coord.col(), color);
    }
    
    bool Board::put_only(int i, int j, Color color){
        if(!is_within_board(i, j)) return false;
        places_[i * cols_ + j] = color;
        return true;
    }
    
    // 位置を指定してmy_colorの石を置き、裏返す。
    // 裏返した石の数を返す。
    // その場所に石が置けない場合は0を返す（裏返した石の数が0の場合も）。
    std::size_t Board::put_and_flip(const Coord & coord, Color my_color){
        return put_and_flip(coord.row(), coord.col(), my_color);
    }
    
    std::size_t Board::put_and_flip(int i, int j, Color my_color){
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
    std::string Board::state(const Coord & coord) const{
        return state(coord.row(), coord.col());
    }
    
    std::string Board::state(int current_r, int current_c) const{
        std::stringstream ss;
        for(int i = 0; i < rows_; ++i){
            for(int j = 0; j <= cols_; ++j){
                if(i == current_r && j == current_c){
                    ss << '[';
                }else if(i == current_r && j == current_c+1){
                    ss << ']';
                }else{
                    ss << ' ';
                }
                if(j == cols_) break;
                ss << get_piece_name(get(i, j));
            }
            ss << "\n";
        }
        return ss.str();
    }
    
    std::string Board::state(void) const{
        return state(-1, -1);
    }
}
