// ------------------------------------------------------------
// オセロAI（サンプル）
// LV1: 単に「ルール上置ける場所を探し、見つかれば置く」
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
        
        for(int i = 0; i < board.rows(); ++i){
            for(int j = 0; j < board.cols(); ++j){
                // 盤面のインスタンスを複製しておく
                board_tmp = Othello::Board(board);
                // 石を置いてみる
                flipped = board_tmp.put_and_flip(i, j, gi_.my_color());
                
                if(flipped > 0){
                    // 1か所でも裏返せるとわかったらそこに置く
                    return Othello::Coord(i, j);
                }
            }
        }
        
        // どこにも置けない
        return Othello::Coord();
    }
};
