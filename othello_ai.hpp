#ifndef OTHELLO_AI_HPP
#define OTHELLO_AI_HPP

#define OTHELLO_AI_USE_CPP11

#include <vector>
#include <map>
#include <iostream>
#include <utility>
#include <cstddef>

namespace Othello{
    enum class Color : char {
        BLACK = 1, // �����u����Ă���
        WHITE = 2, // �����u����Ă���
        EMPTY = 0, // �΂͒u����Ă��Ȃ�
        INVALID = 4 // �����ȍ��W���w�肵���ꍇ�Ȃ�
    };
    
    // �ΐ푊��̐F�𓾂�
    Color get_opponent_color(Color c){
        switch(c){
        case Color::BLACK:
            return Color::WHITE;
        case Color::WHITE:
            return Color::BLACK;
        default:
            return c;
        }
    }
    
    // �΂̔ԍ��iBLACK, WHITE�Ȃǁj��΂̕����i'B', 'W'�Ȃǁj�ɕϊ�
    // �������s���ł����i�̃`�F�b�N�͂��Ă��Ȃ�
    char get_piece_name(Color color){
        return ".BW"[static_cast<size_t>(color)];
    }
    
    // �΋ǂɂ��Ă̏��i�����̐F�͂ǂ��炩�A�Ֆʂ̍s��/�񐔁j
    class GameInfo{
    private:
        Color my_color_, opponent_color_;
        std::size_t rows_, cols_;
        
    public:
        GameInfo(Color my, std::size_t r, std::size_t c) : my_color_(my), rows_(r), cols_(c){
            opponent_color_ = get_opponent_color(my);
        }
        
        inline Color my_color() const{ return my_color_; }
        inline Color opponent_color() const{ return opponent_color_; }
        inline std::size_t rows() const{ return rows_; }
        inline std::size_t cols() const{ return cols_; }
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
        std::size_t rows_, cols_;
        std::vector<Color> places_;
        
    public:
        // ---------- �R���X�g���N�^ ----------
        
        // ��̔Ֆʂ𐶐�
        Board() : rows_(0), cols_(0) {}
        
        Board(std::size_t rows, std::size_t cols)
        : rows_(rows), cols_(cols), places_(rows * cols, Color::EMPTY) {}
        
        // �ʂ̔Ֆʂ��R�s�[
        Board(const Board & other)
        : rows_(other.rows_), cols_(other.cols_), places_(other.places_) {}
        
        // �����Ֆʂ𐶐�
        static Board init(std::size_t rows, std::size_t cols){
            Board board(rows, cols);
            board.put_only(rows/2 - 1, cols/2 - 1, Color::WHITE);
            board.put_only(rows/2 - 1, cols/2,     Color::BLACK);
            board.put_only(rows/2,     cols/2 - 1, Color::BLACK);
            board.put_only(rows/2,     cols/2,     Color::WHITE);
            return board;
        }
        
        // ---------- �e�폈�� ----------
        
        // �Ֆʂ̑傫��
        inline std::size_t rows() const{ return rows_; }
        inline std::size_t cols() const{ return cols_; }
        
        // �΂̐����擾�B
        // Color�N���X�̃C���X�^���X���L�[�A�΂̐���l�Ƃ���A�z�z��ɂȂ��Ă���
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
        
        // �ʒu���w�肵�Đ΂��擾
        inline Color get(const Coord & coord) const{
            return get(coord.row(), coord.col());
        }
        
        Color get(int i, int j) const{
            if(i < 0 || i >= rows_ || j < 0 || j >= cols_) return Color::INVALID;
            return places_[i * cols_ + j];
        }
        
        // �ʒu���w�肵��my_color�̐΂�ݒu
        // �������ł����u�ݒu�v�Ƃ́A�P�ɂ��̏ꏊ�ɂ��̐΂������Ԃɂ��邾����
        //   �Q�[���̃��[���Ƃ��Ēu���i���Ԃ����������j�킯�ł͂Ȃ�
        inline bool put_only(const Coord & coord, Color color){
            return put_only(coord.row(), coord.col(), color);
        }
        
        bool put_only(int i, int j, Color color){
            if(i < 0 || i >= rows_ || j < 0 || j >= cols_) return false;
            places_[i * cols_ + j] = color;
            return true;
        }
        
        // �ʒu���w�肵��my_color�̐΂�u���A���Ԃ��B
        // ���Ԃ����΂̐���Ԃ��B
        // ���̏ꏊ�ɐ΂��u���Ȃ��ꍇ��0��Ԃ��i���Ԃ����΂̐���0�̏ꍇ���j�B
        inline std::size_t put_and_flip(const Coord & coord, Color my_color){
            return put_and_flip(coord.row(), coord.col(), my_color);
        }
        
        std::size_t put_and_flip(int i, int j, Color my_color){
            if(i < 0 || i >= rows_ || j < 0 || j >= cols_) return 0;
            
            // �������̏ꏊ�ɂ��łɐ΂�����ꍇ�́A�u���Ȃ�
            if(get(i, j) != Color::EMPTY) return 0;
            
            Color opponent_color = get_opponent_color(my_color); // ����̐F
            int flipnum = 0; // ���Ԃ�����
            
            // 8�����ɐ΂�L�΂��Ă���
            int distance;
            for(int row_direction = -1; row_direction <= 1; ++row_direction){
                for(int col_direction = -1; col_direction <= 1; ++col_direction){
                    // 8�����ɐL�΂��̂ŁA�L�΂��Ȃ��ꍇ�͏��O
                    if(row_direction == 0 && col_direction == 0) continue;
                    
                    // �܂��A�ׂ�����̐΂łȂ���΁A�΂͗��Ԃ��Ȃ�
                    if(get(i + row_direction, j + col_direction) != opponent_color){
                        continue;
                    }
                    
                    // �����āA��������΂�L�΂��Ă����āA
                    // �u�ΐ푊��̐΂������āA���̂��Ǝ����̐΂������v�ꏊ��
                    // ������
                    distance = 2;
                    while(get(i + distance * row_direction, j + distance * col_direction) == opponent_color){
                        ++distance;
                    }
                    
                    if(get(i + distance * row_direction, j + distance * col_direction) == my_color){
                        // �u����̐΂������v���Ƃ��؂ꂽ�ꏊ�ɂ���΂�
                        // �����̐΂ł���΁i�󔒂��s�j�A�����Ő΂𗠕Ԃ���B
                        
                        // ���Ԃ����΂̐������Z
                        flipnum += distance - 1;
                        
                        // ���ۂɗ��Ԃ�
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
        
        // �\������
        // current_r, current_c�́u�΂��ǂ��ɒu���ꂽ���v�i�ȗ��\�j
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
