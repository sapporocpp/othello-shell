// ------------------------------------------------------------
// オセロAI（サンプル）
// LV2の微修正版:
// 「ルール上置ける場所を探し、最も多く石を裏返せる場所に置く」
// 「ただし、角のマスに置けるときはそれを優先し、角の隣のマスに
//   おくことは極力行わない」
// ------------------------------------------------------------

#include "othello_ai.hpp"

// クラス名は「OTHELLO_AI」で固定すること。
// また、ユーザ定義関数はこのクラスの中に定義すること。
class OTHELLO_AI{
private:
    Othello::GameInfo gi_;
    
public:
    // コンストラクタ（必要ならば）
    // giには対局情報（あなたがどちらの手番か、盤面のサイズ、等）が入る
    OTHELLO_AI(const Othello::GameInfo & gi) : gi_(gi) {}
    
    // 指す手を決める。
    Othello::Coord place(const Othello::Board & board){
        Othello::Board board_tmp;
        std::size_t flipped;
        
        // 置ける場所のうち、（各分類において）石を最も多く裏返せるときの
        // 数ならびに場所。
        // [0]: 角のマス（優先度最高）
        // [1]: それ以外
        // [2]: 角に隣接するマス（優先度最低）
        std::size_t max_flipped[3];
        Othello::Coord max_flipped_place[3];
        for(std::size_t i = 0; i < 3; ++i){
            max_flipped[i] = 0;
        }
        
        for(int i = 0; i < board.rows(); ++i){
            for(int j = 0; j < board.cols(); ++j){
                // 盤面のインスタンスを複製しておく
                board_tmp = Othello::Board(board);
                // 石を置いてみる
                flipped = board_tmp.put_and_flip(i, j, gi_.my_color());
                
                // 今まで試した中で一番多く石を裏返せていたら、
                // その座標を max_flipped_place に保存しておく
                if((i == 0 || i == board.rows() - 1) && (j == 0 || j == board.cols() - 1)){
                    // 角。優先して置く
                    if(flipped > max_flipped[0]){
                        max_flipped[0] = flipped;
                        max_flipped_place[0] = Othello::Coord(i, j);
                    }
                }else if((i <= 1 || i >= board.rows() - 2) && (j <= 1 || j >= board.cols() - 2)){
                    // 角の隣。優先度最低
                    if(flipped > max_flipped[2]){
                        max_flipped[2] = flipped;
                        max_flipped_place[2] = Othello::Coord(i, j);
                    }
                }else{
                    // その他
                    if(flipped > max_flipped[1]){
                        max_flipped[1] = flipped;
                        max_flipped_place[1] = Othello::Coord(i, j);
                    }
                }
            }
        }
        
        // 置く場所を返す
        if(max_flipped_place[0].is_valid()) return max_flipped_place[0];
        if(max_flipped_place[1].is_valid()) return max_flipped_place[1];
        return max_flipped_place[2];
    }
};
