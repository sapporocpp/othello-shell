#include "othello_ai.hpp"

// �ΐ킳����AI�̃w�b�_�t�@�C���������œǂݍ���
#define OTHELLO_AI OthelloAI1
#include "othello_sample_LV3.cpp"
#define OTHELLO_AI_1_NAME "othello_sample_LV3.cpp"
#undef OTHELLO_AI
#define OTHELLO_AI OthelloAI2
#include "othello_sample_LV2A.cpp"
#define OTHELLO_AI_2_NAME "othello_sample_LV2A.cpp"
#undef OTHELLO_AI

// ���C���p�[�g

#include <iostream>
#include <cstdlib>
#include <cstddef>

constexpr std::size_t BOARD_ROWS = 8;
constexpr std::size_t BOARD_COLS = 8;

// �w�����AI�v���O��������󂯎��A���Ȃ������肵�A�΂𗠕Ԃ��B
// �p�X�����ꍇ��false�A����ȊO�̏ꍇ��true��Ԃ��B
template <class OthelloAIClass>
bool conduct_placement(OthelloAIClass & ai, const char * ai_name, Othello::Color turn, Othello::Board & board){
    Othello::Coord pl;
    bool placed = true;
    
    pl = ai.place(board);
    if(pl.is_valid()){
        // �Ԃ��Ă����l���L���Ȉʒu�������Ă����ꍇ�A
        // �u���ۂɒu���āA���Ȃ��Ƃ�1�΂��Ԃ�΁v�悢�B
        std::size_t flipped = board.put_and_flip(pl, turn);
        if(flipped == 0){
            std::cerr << "[ERROR] Player " << Othello::get_piece_name(turn) << " [" << ai_name << "]: No piece flipped" << std::endl;
            std::exit(-1);
        }
        placed = true;
    }else{
        // �Ԃ��Ă����l���u�ǂ��ɂ��u���Ȃ��v�ƂȂ��Ă����ꍇ
        placed = false;
    }
    std::cout << "Player " << Othello::get_piece_name(turn) << " [" << ai_name << "] put at (" << pl.row() << ", " << pl.col() << "):" << std::endl;
    board.display(pl);
    
    return placed;
}

// ���C���p�[�g
int main(void){
    OthelloAI1 ai1(Othello::GameInfo(Othello::Color::BLACK, BOARD_ROWS, BOARD_COLS));
    OthelloAI2 ai2(Othello::GameInfo(Othello::Color::WHITE, BOARD_ROWS, BOARD_COLS));
    
    Othello::Board board = Othello::Board::init(BOARD_ROWS, BOARD_COLS);
    std::size_t flipped;
    std::size_t passed = 0; // �A�����ăp�X���ꂽ�񐔁B2�ɂȂ�Ƒ΋ǏI��
    
    // �΋ǂ����{
    for(;;){
        if(conduct_placement(ai1, OTHELLO_AI_1_NAME, Othello::Color::BLACK, board)){
            passed = 0;
        }else{
            ++passed;
            if(passed >= 2) break;
        }
        
        if(conduct_placement(ai2, OTHELLO_AI_2_NAME, Othello::Color::WHITE, board)){
            passed = 0;
        }else{
            ++passed;
            if(passed >= 2) break;
        }
    }
    
    // ���ʂ𐔂���
    std::map<Othello::Color, std::size_t> result = board.pieces();
    
    std::cout << "[Final Result]" << std::endl;
    std::cout << "Player B [" << OTHELLO_AI_1_NAME << "]: " << result[Othello::Color::BLACK] << std::endl;
    std::cout << "Player W [" << OTHELLO_AI_2_NAME << "]: " << result[Othello::Color::WHITE] << std::endl;
}
