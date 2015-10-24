// ------------------------------------------------------------
// オセロAI（サンプル）
// LV3: 「自分が置く→相手が置く、という操作をシミュレーションし
//        （それぞれ一手ずつぶん）、自分の石が最も多くなる手を選ぶ。
//        ただし、相手はなるべく自分の石を少なくする手を選ぶとする。」
// ------------------------------------------------------------

#include "othello_ai.hpp"
#include <deque> // 使いたいヘッダがあればここでincludeする

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
    };
    
    typedef std::deque<SimulatedBoard> SimulatedBoardList;
    
    // 置ける場所の候補を列挙する
    void enum_possible_placements(Othello::Color color, const Othello::Board & board, SimulatedBoardList & result){
        SimulatedBoard simboard;
        std::size_t flipped;
        
        result.clear();
        
        for(int i = 0; i < board.rows(); ++i){
            for(int j = 0; j < board.cols(); ++j){
                // 盤面のインスタンスを複製しておく
                simboard.coord = Othello::Coord(i, j);
                simboard.board = Othello::Board(board);
                // 石を置いてみる
                flipped = simboard.board.put_and_flip(simboard.coord, gi_.my_color());
                
                if(flipped > 0){
                    // 1か所でも裏返せるとわかったら候補に加える
                    result.push_back(simboard);
                }
            }
        }
    }
    
    // コンストラクタ（必要ならば）
    // giには対局情報（あなたがどちらの手番か、盤面のサイズ、等）が入る
    OTHELLO_AI(const Othello::GameInfo & gi) : gi_(gi) {}
    
    // 指す手を決める。
    Othello::Coord place(const Othello::Board & board){
        std::size_t flipped;
        
        SimulatedBoardList candidates_me, candidates_opponent;
        
        // 自分が置ける場所をすべて試し、結果をdequeとして返す
        enum_possible_placements(gi_.my_color(), board, candidates_me);
        
        // そのそれぞれに対して、相手が置ける場所をすべて試す
        std::size_t max_pieces_m = 0;
        Othello::Coord max_pieces_m_place;
        
        for(SimulatedBoardList::iterator it_m = candidates_me.begin(); it_m != candidates_me.end(); ++it_m){
            // 相手が置ける場所を列挙する
            enum_possible_placements(gi_.my_color(), it_m->board, candidates_opponent);
            
            // 相手が置いた結果の、自分の残る石の数。
            // 相手が置く前の数で初期化しておく。
            std::size_t min_pieces_o = (it_m->board.pieces())[gi_.my_color()];
            
            // 相手が置ける場所のうち、
            // 自分の石が最も少なくなるときの個数を数える
            // （相手が置ける場所がなければ、上記「相手が置く前の数」が使われる）
            std::size_t pieces;
            for(SimulatedBoardList::iterator it_o = candidates_opponent.begin(); it_o != candidates_opponent.end(); ++it_o){
                pieces = (it_o->board.pieces())[gi_.my_color()];
                if(pieces < min_pieces_o){
                    min_pieces_o = pieces;
                }
            }
            
            // 相手の手によって「自分の石が一番少なくなる」ようにされた
            // 結果において、自分の石が他の候補よりも多くなるのであれば、
            // その手を選ぶ
            if(min_pieces_o > max_pieces_m){
                max_pieces_m = min_pieces_o;
                max_pieces_m_place = it_m->coord;
            }
        }
        
        // 置く場所を返す
        return max_pieces_m_place;
    }
};
