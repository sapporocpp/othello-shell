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
    // - [1] 角の石は2^25点
    // - [2] 辺の石は、そこから当該プレイヤーの石のみが繋がって角まで
    //       到達できるならば2^19点、それ以外は[4]で判定
    // - [3] 角に隣接する石（[2]に該当するものは除く）は、当該角に
    //       自分の石があるならば[4]で判定、それ以外は-2^14点
    // - [4] それ以外の石は、八方すべてを別の石に囲まれているなら
    //       2^7点、他は1点
    // 2のべき乗の倍率は、それぞれ下位の優先順位のものが上位のそれを超過しない
    // ように設定している。
    // これらの結果に対し、[自分のスコア]-[相手のスコア]を計算し、さらに
    // 2^28を加算して正の値となることを保証する。
    
    // [3], [4]
    int board_score_ordinary(const Othello::Board & board, int i, int j){
        const Othello::Color color = board.get(i, j);
        if(color == Othello::Color::EMPTY) return 0;
        
        int corner_i = -1, corner_j = -1;
        if(i <= 1) corner_i = 0;
        if(i >= board.rows() - 2) corner_i = board.rows() - 1;
        if(j <= 1) corner_j = 0;
        if(j >= board.cols() - 2) corner_j = board.cols() - 1;
        
        if(corner_i != -1 && corner_j != -1){
            // 角の隣
            if(board.get(corner_i, corner_j) != color) return -(1 << 14);
        }
        
        // 周りがすべて石があるか判定
        bool surrounded = true;
        
        for(int ip = -1; ip <= 1; ++ip){
            for(int jp = -1; jp <= 1; ++jp){
                if(ip == 0 && jp == 0) continue;
                
                if(board.get(i + ip, j + jp) == Othello::Color::EMPTY){
                    // ここで、board.get(i + ip, j + jp) が
                    // Othello::Color::INVALID になる（盤面をはみ出す）可能性もあるが
                    // そうなっても「周りに石がない」とはみなさないのでこれでよい
                    surrounded = false;
                    break;
                }
            }
            if(!surrounded) break;
        }
        
        if(surrounded) return(1 << 7);
        return 1;
    }
    
    // スコア計算のメイン
    int board_score(const Othello::Board & board){
        int score[3] = {0, 0, 0};
        
        // [1]
        for(int i = 0; i < board.rows(); i += board.rows() - 1){
            for(int j = 0; j < board.cols(); j += board.cols() - 1){
                const Othello::Color corner_color = board.get(i, j);
                if(corner_color != Othello::Color::EMPTY){
                    score[static_cast<std::size_t>(corner_color)] += (1 << 25);
                }
            }
        }
        
        // [2]の縦列
        for(int i = 0; i < board.rows(); i += board.rows() - 1){
            // 上端の角に石があれば、それを起点に数える
            const Othello::Color corner_color_upper = board.get(i, 0);
            int seq_upper = 0; // 上端から見てここまで同じ色が続いている
            if(corner_color_upper != Othello::Color::EMPTY){
                ++seq_upper;
                while(seq_upper <= board.cols() - 2 && board.get(i, seq_upper) == corner_color_upper){
                    ++seq_upper;
                }
                --seq_upper;
            }
            
            // 下端の角に石があれば、それを起点に数える
            const Othello::Color corner_color_lower = board.get(i, board.cols() - 1);
            int seq_lower = board.cols() - 1; // 下端から見てここまで同じ色が続いている
            if(corner_color_lower != Othello::Color::EMPTY){
                --seq_lower;
                while(seq_lower >= 1 && board.get(i, seq_lower) == corner_color_lower){
                    --seq_lower;
                }
                ++seq_lower;
            }
            
            // 連続した領域によって場合分け
            if(seq_lower < seq_upper){
                // 端一列がすべて同じ色
                score[static_cast<std::size_t>(corner_color_upper)] += (1 << 19) * 6;
            }else{
                // 角から連続する端の部分はそのスコアを加算
                if(corner_color_upper != Othello::Color::EMPTY){
                    score[static_cast<std::size_t>(corner_color_upper)] += (1 << 19) * seq_upper;
                }
                if(corner_color_lower != Othello::Color::EMPTY){
                    score[static_cast<std::size_t>(corner_color_lower)] += (1 << 19) * (board.cols() - 1 - seq_lower);
                }
                
                // それ以外の部分は[3], [4]の条件で加算
                for(int j = seq_upper + 1; j < seq_lower; ++j){
                    const Othello::Color color = board.get(i, j);
                    if(color != Othello::Color::EMPTY){
                        score[static_cast<std::size_t>(color)] += board_score_ordinary(board, i, j);
                    }
                }
            }
        }
        // [2]の横列
        for(int j = 0; j < board.cols(); j += board.cols() - 1){
            // 左端の角に石があれば、それを起点に数える
            const Othello::Color corner_color_upper = board.get(0, j);
            int seq_upper = 0; // 左端から見てここまで同じ色が続いている
            if(corner_color_upper != Othello::Color::EMPTY){
                ++seq_upper;
                while(seq_upper <= board.rows() - 2 && board.get(seq_upper, j) == corner_color_upper){
                    ++seq_upper;
                }
                --seq_upper;
            }
            
            // 右端の角に石があれば、それを起点に数える
            const Othello::Color corner_color_lower = board.get(board.rows() - 1, j);
            int seq_lower = board.rows() - 1; // 右端から見てここまで同じ色が続いている
            if(corner_color_lower != Othello::Color::EMPTY){
                --seq_lower;
                while(seq_lower >= 1 && board.get(seq_lower, j) == corner_color_lower){
                    --seq_lower;
                }
                ++seq_lower;
            }
            
            // 連続した領域によって場合分け
            if(seq_lower < seq_upper){
                // 端一列がすべて同じ色
                score[static_cast<std::size_t>(corner_color_upper)] += (1 << 19) * 6;
            }else{
                // 角から連続する端の部分はそのスコアを加算
                if(corner_color_upper != Othello::Color::EMPTY){
                    score[static_cast<std::size_t>(corner_color_upper)] += (1 << 19) * seq_upper;
                }
                if(corner_color_lower != Othello::Color::EMPTY){
                    score[static_cast<std::size_t>(corner_color_lower)] += (1 << 19) * (board.cols() - 1 - seq_lower);
                }
                
                // それ以外の部分は[3], [4]の条件で加算
                for(int i = seq_upper + 1; i < seq_lower; ++i){
                    const Othello::Color color = board.get(i, j);
                    if(color != Othello::Color::EMPTY){
                        score[static_cast<std::size_t>(color)] += board_score_ordinary(board, i, j);
                    }
                }
            }
        }
        
        // ほかのマス
        for(int i = 1; i <= board.rows() - 2; ++i){
            for(int j = 1; j <= board.cols() - 2; ++j){
                const Othello::Color color = board.get(i, j);
                score[static_cast<std::size_t>(color)] += board_score_ordinary(board, i, j);
            }
        }
        
        return score[static_cast<std::size_t>(gi_.my_color())] - score[static_cast<std::size_t>(gi_.opponent_color())] + (1 << 28);
    }
    
    // 手の探索を行う（この関数を再帰呼び出ししてゲーム木とみなす）
    int try_place(const Othello::Board & board, Othello::Color player, int depth){
        // ----------
        // 一定の深さまで到達したら、盤面を見て評価値を付ける
        // ----------
        if(depth >= 4) return board_score(board);
        
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
        //try_place(board, gi_.my_color(), 0);
        int score = try_place(board, gi_.my_color(), 0) - (1 << 28);
        std::cerr << "<Player " << get_piece_name(gi_.my_color()) << " placed with score ";
        if(score < 0){
            std::cerr << '-';
            score = -score;
        }
        std::cerr << 'l'; // 下位ビット
        while(score > 0){
            std::cerr << (score % 2);
            score /= 2;
        }
        std::cerr << 'u'; // 上位ビット
        std::cerr << std::endl;
        
        return chosen_place_;
    }
};

