// ------------------------------------------------------------
// オセロAI（先読みあり）
// ------------------------------------------------------------

#include "othello_ai.hpp"
#include <stack> // 使いたいヘッダがあればここでincludeする

// クラス名は「OTHELLO_AI」で固定すること。
// また、ユーザ定義関数はこのクラスの中に定義すること。
class OTHELLO_AI{
private:
    Othello::GameInfo gi_;
    
public:
    // 「石を置いた場所」と「その結果の盤面」の組
    struct SimulatedBoard{
        Othello::Coord coord;
        Othello::Board board;
        Othello::Color next_player;
    };
    typedef std::stack<SimulatedBoard> SimulatedBoardList;
    
private:
    // ------------------------------------------------------------
    // 自分と相手の手をシミュレーションし、最善の手を選ぶ
    // （ミニマックス法）
    // ------------------------------------------------------------
    
    Othello::Coord chosen_place_;
    // 最終的に置くことになった場所。
    // 本当はtry_placeの返り値にしたかったのだが、再帰呼び出しされる関数であるため、
    // スタックの余計な消費を防ぐためメンバ変数に。
    
    // 盤面に対してスコアを付ける。
    // 今回の評価値：
    // - 「角に置いた回数」ならびに「角の隣に置いた回数」を集計し、
    //   以下の回数の順で決定する。
    //   [1] (自分が角に置いた回数 - 相手が角に置いた回数) が最多のもの
    //   [2] 自分が角に置いた回数 が最多のもの
    //   [3] (相手が角の隣に置いた回数 - 自分が角の隣に置いた回数) が最多のもの
    //   [4] 相手が角の隣に置いた回数 が最多のもの
    // 評価値は ([1] * 262144 + [2] * 4096 + [3] * 64 + [4]) + 2097152 を返している。
    // 最後「+ 2097152」しているのは、正の値を返すことを保証するため。
    struct PlacementCounter{
        int corner[2];
        int next_to_corner[2];
        
        PlacementCounter(){
            corner[0] = corner[1] = 0;
            next_to_corner[0] = next_to_corner[1] = 0;
        }
    };
    
    int board_score(const PlacementCounter & pc){
        return(
            ((pc.corner[static_cast<size_t>(gi_.my_color())] - pc.corner[static_cast<size_t>(gi_.opponent_color())]) * 262144 +
            pc.corner[static_cast<size_t>(gi_.my_color())] * 4096 +
            (pc.next_to_corner[static_cast<size_t>(gi_.opponent_color())] - pc.next_to_corner[static_cast<size_t>(gi_.my_color())]) * 64 +
            pc.next_to_corner[static_cast<size_t>(gi_.opponent_color())]) + 2097152
        );
    }
    
    // 手の探索を行う（この関数を再帰呼び出ししてゲーム木とみなす）
    int try_place(const Othello::Board & board, const PlacementCounter & pc, Othello::Color player, int depth){
        // ----------
        // 一定の深さまで到達したら、盤面を見て評価値を付ける
        // ----------
        if(depth >= 4) return board_score(pc);
        
        // ----------
        // 一定の深さまで到達していない場合、探索を進める
        // ----------
        int best_score = -1;
        Othello::Coord best_place;
        
        // すべての盤面上の位置について繰り返す
        for(int i = 0; i < board.rows(); ++i){
            for(int j = 0; j < board.cols(); ++j){
                Othello::Board board_tmp(board);
                PlacementCounter pc_tmp = pc;
                
                if((i == 0 || i == board.rows() - 1) && (j == 0 || j == board.cols() - 1)){
                    ++(pc_tmp.corner[static_cast<size_t>(player)]);
                }else if((i <= 1 || i >= board.rows() - 2) && (j <= 1 || j >= board.cols() - 2)){
                    ++(pc_tmp.next_to_corner[static_cast<size_t>(player)]);
                }
                
                if(board_tmp.put_and_flip(i, j, player) == 0) continue;
                
                int score = try_place(board_tmp, pc_tmp, Othello::get_opponent_color(player), depth+1);
                if(score == -1){
                    // 何も指せる手がなかった場合、本来指そうとしてたプレイヤーの相手に指させる
                    // それで指せる手があるなら探索継続、なければ盤面からのスコア計算に入る
                    int score2 = try_place(board_tmp, pc_tmp, player, depth+1);
                    if(score2 == -1){
                        score = board_score(pc_tmp);
                    }else{
                        score = score2;
                    }
                }
                
                // 指せる手があった場合、そのスコアが
                // - 自分の手番をシミュレーションするならば、自分にとって最善の手
                // - 相手の手番をシミュレーションするならば、自分にとって最悪の手
                // であるならばその手を残す。
                if(best_score == -1 ||
                   (player == gi_.my_color() && score > best_score) ||
                   (player != gi_.my_color() && score < best_score)){
                    best_score = score;
                    best_place = Othello::Coord(i, j);
                }
            }
        }
        if(depth == 0) chosen_place_ = best_place;
        return best_score;
    }
    
public:
    // コンストラクタ（必要ならば）
    // giには対局情報（あなたがどちらの手番か、盤面のサイズ、等）が入る
    OTHELLO_AI(const Othello::GameInfo & gi) : gi_(gi) {}
    
    // 指す手を決める。
    Othello::Coord place(const Othello::Board & board){
        PlacementCounter pc_empty;
        
        chosen_place_ = Othello::Coord();
        try_place(board, pc_empty, gi_.my_color(), 0);
        return chosen_place_;
    }
};

