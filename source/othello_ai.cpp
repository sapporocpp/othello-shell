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

    // �ΐ푊��̐F�𓾂�
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
    
    // �΂̔ԍ��iBLACK, WHITE�Ȃǁj��΂̕����i'B', 'W'�Ȃǁj�ɕϊ�
    // �������s���ł����i�̃`�F�b�N�͂��Ă��Ȃ�
    char get_piece_name(Color color){
        return ".BW"[static_cast<size_t>(color)];
    }
    
    // �΋ǂɂ��Ă̏��i�����̐F�͂ǂ��炩�A�Ֆʂ̍s��/�񐔁j
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
    
    // �⏕�֐��F���W(i, j)���Ֆʂ̓�����
    bool Board::is_within_board(int i, int j) const{
        return(
            (i >= 0 && i < rows_) &&
            (j >= 0 && j < cols_)
        );
    }
        
    // ---------- �R���X�g���N�^ ----------
    
    // ��̔Ֆʂ𐶐�
    Board::Board() : rows_(0), cols_(0) {}
    
    Board::Board(int rows, int cols)
    : rows_(rows), cols_(cols),
      places_(rows * cols, Color::EMPTY) {}
    
    // �ʂ̔Ֆʂ��R�s�[
    Board::Board(const Board & other)
    : rows_(other.rows_), cols_(other.cols_), places_(other.places_) {}
    
    // �����Ֆʂ𐶐�
    Board Board::init(int rows, int cols){
        Board board(rows, cols);
        board.put_only(rows/2 - 1, cols/2 - 1, Color::WHITE);
        board.put_only(rows/2 - 1, cols/2,     Color::BLACK);
        board.put_only(rows/2,     cols/2 - 1, Color::BLACK);
        board.put_only(rows/2,     cols/2,     Color::WHITE);
        return board;
    }
    
    // ---------- �e�폈�� ----------
    
    // �Ֆʂ̑傫��
    int Board::rows() const{ return rows_; }
    int Board::cols() const{ return cols_; }
    
    // �΂̐����擾�B
    // Color�N���X�̃C���X�^���X���L�[�A�΂̐���l�Ƃ���A�z�z��ɂȂ��Ă���
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
    
    // �ʒu���w�肵�Đ΂��擾
    Color Board::get(const Coord & coord) const{
        return get(coord.row(), coord.col());
    }
    
    Color Board::get(int i, int j) const{
        if(!is_within_board(i, j)) return Color::INVALID;
        return places_[i * cols_ + j];
    }
    
    // �ʒu���w�肵��my_color�̐΂�ݒu
    // �������ł����u�ݒu�v�Ƃ́A�P�ɂ��̏ꏊ�ɂ��̐΂������Ԃɂ��邾����
    //   �Q�[���̃��[���Ƃ��Ēu���i���Ԃ����������j�킯�ł͂Ȃ�
    bool Board::put_only(const Coord & coord, Color color){
        return put_only(coord.row(), coord.col(), color);
    }
    
    bool Board::put_only(int i, int j, Color color){
        if(!is_within_board(i, j)) return false;
        places_[i * cols_ + j] = color;
        return true;
    }
    
    // �ʒu���w�肵��my_color�̐΂�u���A���Ԃ��B
    // ���Ԃ����΂̐���Ԃ��B
    // ���̏ꏊ�ɐ΂��u���Ȃ��ꍇ��0��Ԃ��i���Ԃ����΂̐���0�̏ꍇ���j�B
    std::size_t Board::put_and_flip(const Coord & coord, Color my_color){
        return put_and_flip(coord.row(), coord.col(), my_color);
    }
    
    std::size_t Board::put_and_flip(int i, int j, Color my_color){
        if(!is_within_board(i, j)) return 0;
        
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
