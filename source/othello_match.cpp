#include "othello_ai.hpp"

// 対戦させるAIのヘッダファイルをここで読み込む
#define OTHELLO_AI_1_NAME "othello_sample_LV2A.hpp"
#define OTHELLO_AI_2_NAME "othello_sample_hhiro_manual.hpp"

#define OTHELLO_AI OthelloAI1
#include OTHELLO_AI_1_NAME
#undef OTHELLO_AI
#define OTHELLO_AI OthelloAI2
#include OTHELLO_AI_2_NAME
#undef OTHELLO_AI

// メインパート
#include <string>
#include <iostream>
#include <cstdlib>
#include <cstddef>

constexpr std::size_t BOARD_ROWS_DEFAULT = 8;
constexpr std::size_t BOARD_COLS_DEFAULT = 8;

// 指す手をAIプログラムから受け取り、問題ないか判定し、石を裏返す。
// パスした場合はfalse、それ以外の場合はtrueを返す。
template <class OthelloAIClass>
bool conduct_placement(OthelloAIClass & ai, const char * ai_name, Othello::Color turn, Othello::Board & board){
    bool piece_placed = true;
    Othello::Board board_tmp(board);
    
    const Othello::Coord place_from_ai = ai.place(board_tmp);
    if(place_from_ai.is_valid()){
        // 返ってきた値が有効な位置を示していた場合
        
        // その場所に石を置く
        std::size_t flipped = board.put_and_flip(place_from_ai, turn);
        
        // もし置いても石を何も裏返せなかった場合は、エラーとする
        if(flipped == 0){
            std::cerr << "[ERROR] Player " << Othello::get_piece_name(turn) << " [" << ai_name << "]: No piece flipped after putting at (" << place_from_ai.row() << ", " << place_from_ai.col() << ")" << std::endl;
            std::exit(-1);
        }
        piece_placed = true;
    }else{
        // 返ってきた値が「どこにも置けない」となっていた場合
        
        // 置ける場所があるのに「置けない」と答えた場合は、エラーとする
        for(int i = 0; i < board.rows(); ++i){
            for(int j = 0; j < board.cols(); ++j){
                board_tmp = Othello::Board(board);
                if(board_tmp.put_and_flip(i, j, turn) > 0){
                    std::cerr << "[ERROR] Player " << Othello::get_piece_name(turn) << " [" << ai_name << "]: Passed although he/she can put a piece" << std::endl;
                    std::exit(-1);
                }
            }
        }
        
        piece_placed = false;
    }
    std::cout << "Player " << Othello::get_piece_name(turn) << " [" << ai_name << "] put at (" << place_from_ai.row() << ", " << place_from_ai.col() << "):" << std::endl;
    std::cout << board.state(place_from_ai) << std::endl;
    
    return piece_placed;
}

// メインパート
int main(int argc, char ** argv){
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Othello Match" << std::endl;
    std::cout << "Player Black: " << OTHELLO_AI_1_NAME << std::endl;
    std::cout << "Player White: " << OTHELLO_AI_2_NAME << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    
    // コマンドライン引数解析（盤面のサイズ）。
    // デフォルトではBOARD_ROWS_DEFAULT/BOARD_COLS_DEFAULT
    std::size_t board_rows = 0, board_cols = 0;
    
    if(argc == 1){
        // 引数なし（デフォルトを利用）
        board_rows = BOARD_ROWS_DEFAULT;
        board_cols = BOARD_COLS_DEFAULT;
    }else if(argc == 3){
        // 引数2つあり
        board_rows = std::atoi(argv[1]);
        board_cols = std::atoi(argv[2]);
        // C++11で規格化されている関数だが、mingwで利用不可
        // board_rows = std::stoi(argv[1]);
        // board_cols = std::stoi(argv[2]);
    }
    
    if(board_rows < 4 || board_cols < 4){
        // 盤面のサイズが正しく指定されていない場合
        std::cerr << "Usage:" << std::endl;
        std::cerr << "<1> PROGRAMNAME" << std::endl;
        std::cerr << "<2> PROGRAMNAME [BoardRowSize] [BoardColumnSize]" << std::endl;
        std::cerr << std::endl;
        std::cerr << "With <1>, match is held with the default board size (" << BOARD_ROWS_DEFAULT << " x " << BOARD_COLS_DEFAULT << ")." << std::endl;
        return -1;
    }
    
    OthelloAI1 ai1(Othello::GameInfo(Othello::Color::BLACK, board_rows, board_cols));
    OthelloAI2 ai2(Othello::GameInfo(Othello::Color::WHITE, board_rows, board_cols));
    
    Othello::Board board = Othello::Board::init(board_rows, board_cols);
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
    
    // コンソールが消える前に入力待ちする。必要ないならばコメントアウトを
    std::cerr << std::endl << "<Press Enter key to exit>" << std::endl;
    std::cin.getline(tmp_input, 3);
    
    return 0;
}
