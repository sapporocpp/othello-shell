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
    // 今回の評価値：石の総数の差
    // （ただし0以上の数を返すことを保証するため、盤面のマスの総数を加算している）
    int board_score(const Othello::Board & board){
        int piece_num_diff = board.rows() * board.cols();
        for(int i = 0; i < board.rows(); ++i){
            for(int j = 0; j < board.cols(); ++j){
                Othello::Color piece = board.get(i, j);
                if(piece == Othello::Color::EMPTY){
                    // do nothing
                }else if(piece == gi_.my_color()){
                    ++piece_num_diff;
                }else{
                    --piece_num_diff;
                }
            }
        }
        return piece_num_diff;
    }
    
    // 手の探索を行う（この関数を再帰呼び出ししてゲーム木とみなす）
    int try_place(const Othello::Board & board, Othello::Color player, int depth){
        // ----------
        // 一定の深さまで到達したら、盤面を見て評価値を付ける
        // ----------
        if(depth >= 2) return board_score(board);
        
        // ----------
        // 一定の深さまで到達していない場合、探索を進める
        // ----------
        int best_score = -1;
        Othello::Coord best_place;
        
        // すべての盤面上の位置について繰り返す
        for(int i = 0; i < board.rows(); ++i){
            for(int j = 0; j < board.cols(); ++j){
                Othello::Board board_tmp(board);
                if(board_tmp.put_and_flip(i, j, player) == 0) continue;
                
                int score = try_place(board_tmp, Othello::get_opponent_color(player), depth+1);
                if(score == -1){
                    // 何も指せる手がなかった場合、本来指そうとしてたプレイヤーの相手に指させる
                    // それで指せる手があるなら探索継続、なければ盤面からのスコア計算に入る
                    int score2 = try_place(board_tmp, player, depth+1);
                    if(score2 == -1){
                        score = board_score(board_tmp);
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
        chosen_place_ = Othello::Coord();
        try_place(board, gi_.my_color(), 0);
        return chosen_place_;
    }
};

