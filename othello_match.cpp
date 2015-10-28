#include "othello_ai.hpp"

// 対戦させるAIのヘッダファイルをここで読み込む
#define OTHELLO_AI_1_NAME "othello_sample_LV1.hpp"
#define OTHELLO_AI_2_NAME "othello_sample_LV2.hpp"

#define OTHELLO_AI OthelloAI1
#include OTHELLO_AI_1_NAME
#undef OTHELLO_AI
#define OTHELLO_AI OthelloAI2
#include OTHELLO_AI_2_NAME
#undef OTHELLO_AI

// メインパート
#include <iostream>
#include <cstdlib>
#include <cstddef>

constexpr std::size_t BOARD_ROWS = 8;
constexpr std::size_t BOARD_COLS = 8;

// 指す手をAIプログラムから受け取り、問題ないか判定し、石を裏返す。
// パスした場合はfalse、それ以外の場合はtrueを返す。
template <class OthelloAIClass>
bool conduct_placement(OthelloAIClass & ai, const char * ai_name, Othello::Color turn, Othello::Board & board){
    Othello::Coord place_from_ai;
    bool piece_placed = true;
    
    Othello::Board board_tmp(board);
    
    place_from_ai = ai.place(board_tmp);
    if(place_from_ai.is_valid()){
        // 返ってきた値が有効な位置を示していた場合、
        // 「実際に置いて、少なくとも1つ石が返れば」よい。
        std::size_t flipped = board.put_and_flip(place_from_ai, turn);
        if(flipped == 0){
            std::cerr << "[ERROR] Player " << Othello::get_piece_name(turn) << " [" << ai_name << "]: No piece flipped" << std::endl;
            std::exit(-1);
        }
        piece_placed = true;
    }else{
        // 返ってきた値が「どこにも置けない」となっていた場合
        piece_placed = false;
    }
    std::cout << "Player " << Othello::get_piece_name(turn) << " [" << ai_name << "] put at (" << place_from_ai.row() << ", " << place_from_ai.col() << "):" << std::endl;
    board.display(place_from_ai);
    
    return piece_placed;
}

// メインパート
int main(void){
    OthelloAI1 ai1(Othello::GameInfo(Othello::Color::BLACK, BOARD_ROWS, BOARD_COLS));
    OthelloAI2 ai2(Othello::GameInfo(Othello::Color::WHITE, BOARD_ROWS, BOARD_COLS));
    
    Othello::Board board = Othello::Board::init(BOARD_ROWS, BOARD_COLS);
    std::size_t passed = 0; // 連続してパスされた回数。2になると対局終了
	char tmp_input[3];

    // 対局を実施
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
    
    // 結果を数える
    std::map<Othello::Color, std::size_t> result = board.pieces();
    
    std::cout << "[Final Result]" << std::endl;
    std::cout << "Player B [" << OTHELLO_AI_1_NAME << "]: " << result[Othello::Color::BLACK] << std::endl;
    std::cout << "Player W [" << OTHELLO_AI_2_NAME << "]: " << result[Othello::Color::WHITE] << std::endl;

	std::cin.getline(tmp_input, 3); // コンソールが消える前に入力待ちする。必要ならば
	
	return 0;
}
