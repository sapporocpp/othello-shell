// ------------------------------------------------------------
// オセロAI（H.Hiro作成）
// 打つ手を手作業で決めてます
// ------------------------------------------------------------

#include "othello_ai.hpp"

// クラス名は「OTHELLO_AI」で固定すること。
// また、ユーザ定義関数はこのクラスの中に定義すること。
class OTHELLO_AI{
private:
    Othello::GameInfo gi_;
    
    // (i, j)に石を置いた結果の盤面がboardとなったとき、その手はどれだけ優先すべきか評価
    //
    // [1] ＜優先度0＞角は最優先。
    // [2] ＜優先度1＞角以外の辺については、そこから角に繋がる全部の石が自分のになるなら優先。
    //     例えば下の図で、「＋」については黒石は優先的に置くが、「×」はそうではない。
    //     ●○○○＋×・・
    //     ●・・・・○・・
    //     ●・・・・●・・
    //     ＋○●・・・・・
    //     ○・・・・・・・
    //     ×○●・・・・・
    //     ・・・・・・・・
    //     ・・・・・・・・
    // [3] ＜優先度3＞角の隣は、対応する角が自分の石でないならば、優先度最低
    // [4] ＜優先度2＞以上に該当しないものは優先度同一
    int calc_priority(const Othello::Board & board, int i, int j){
        // 角。最優先
        if((i == 0 || i == board.rows() - 1) && (j == 0 || j == board.cols() - 1)){
            return 0;
        }
        
        // 辺。
        // どちらかの角まで自分の石が続いていれば優先(1)
        // そうでなければ先に進む
        if(i == 0 || i == board.rows() - 1){
            int c;
            
            // 右端か左端の列に置いた場合
            for(c = j-1; c >= 0; --c){
                // 置いた場所から上に見て行って、角まで自分の石が続いていれば
                if(board.get(i, c) != gi_.my_color()) break;
            }
            if(c == -1) return 1;
            
            for(c = j+1; c < board.cols(); ++c){
                // 置いた場所から下に見て行って、角まで自分の石が続いていれば
                if(board.get(i, c) != gi_.my_color()) break;
            }
            if(c == board.cols()) return 1;
        }
        
        if(j == 0 || j == board.cols() - 1){
            int c;
            
            // 上端か下端の列に置いた場合
            for(c = i-1; c >= 0; --c){
                // 置いた場所から左に見て行って、角まで自分の石が続いていれば
                if(board.get(c, j) != gi_.my_color()) break;
            }
            if(c == -1) return 1;
            
            for(c = i+1; c < board.rows(); ++c){
                // 置いた場所から右に見て行って、角まで自分の石が続いていれば
                if(board.get(c, j) != gi_.my_color()) break;
            }
            if(c == board.rows()) return 1;
        }
        
        // 角の隣。該当する角が自分の石でなければ優先度最低
        int corner_i = -1, corner_j = -1; // 「該当する角」の座標
        
        if(i <= 1) corner_i = 0;
        if(i >= board.rows() - 2) corner_i = board.rows() - 1;
        if(j <= 1) corner_j = 0;
        if(j >= board.cols() - 2) corner_j = board.cols() - 1;
        if(corner_i != -1 && corner_j != -1 && board.get(corner_i, corner_j) != gi_.my_color()){
            return 3;
        }
        
        return 2;
    }
    
    // 同じ優先度のもののなかでの比較に必要な指標。
    // そこに石を置いてみたとして、
    // 自分の石で以下のいずれかに該当するものが一番多くなるものを選ぶ：
    // 「角」「辺」「周囲八方に他の石がある石」
    int count_good_pieces(const Othello::Board & board){
        int good_pieces = 0;
        
        for(int i = 0; i < board.rows(); ++i){
            if(board.get(i, 0) == gi_.my_color()) ++good_pieces;
            if(board.get(i, board.cols()-1) == gi_.my_color()) ++good_pieces;
        }
        for(int j = 1; j < board.cols() - 1; ++j){
            if(board.get(0, j) == gi_.my_color()) ++good_pieces;
            if(board.get(board.rows()-1, j) == gi_.my_color()) ++good_pieces;
        }
        
        for(int i = 1; i < board.rows() - 1; ++i){
            for(int j = 1; j < board.cols() - 1; ++j){
                if(
                    board.get(i-1, j-1) != Othello::Color::EMPTY &&
                    board.get(i,   j-1) != Othello::Color::EMPTY &&
                    board.get(i+1, j-1) != Othello::Color::EMPTY &&
                    board.get(i-1, j)   != Othello::Color::EMPTY &&
                    board.get(i,   j)   == gi_.my_color() &&
                    board.get(i+1, j)   != Othello::Color::EMPTY &&
                    board.get(i-1, j+1) != Othello::Color::EMPTY &&
                    board.get(i,   j+1) != Othello::Color::EMPTY &&
                    board.get(i+1, j+1) != Othello::Color::EMPTY
                ){
                    ++good_pieces;
                }
            }
        }
        
        return good_pieces;
    }
    
public:
    // コンストラクタ（必要ならば）
    // giには対局情報（あなたがどちらの手番か、盤面のサイズ、等）が入る
    OTHELLO_AI(const Othello::GameInfo & gi) : gi_(gi) {}
    
    // 指す手を決める。
    Othello::Coord place(const Othello::Board & board){
        int max_good_pieces = -1;
        int best_priority = 3;
        int best_i = -1, best_j = -1;
        
        for(int i = 0; i < board.rows(); ++i){
            for(int j = 0; j < board.cols(); ++j){
                // 盤面のインスタンスを複製しておく
                Othello::Board board_tmp = Othello::Board(board);
                
                // 石を置いてみる。裏返せないならその手は無視
                if(board_tmp.put_and_flip(i, j, gi_.my_color()) == 0){
                    continue;
                }
                
                // 優先度を調べる
                int priority = calc_priority(board_tmp, i, j);
                
                // いま得られた優先度より高い優先度の手がすでに存在するなら、
                // その候補はそれ以上検討する必要がない
                if(priority <= best_priority){
                    if(priority < best_priority) max_good_pieces = -1;
                    best_priority = priority;
                    
                    int good_pieces = count_good_pieces(board_tmp);
                    if(good_pieces > max_good_pieces){
                        max_good_pieces = good_pieces;
                        best_i = i;
                        best_j = j;
                    }
                }
            }
        }
        
        // 置く場所を返す
        return((best_i == -1 && best_j == -1) ? Othello::Coord() : Othello::Coord(best_i, best_j));
    }
};
