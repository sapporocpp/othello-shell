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
    SimulatedBoardList enum_possible_placements(Othello::Color /*color*/, const Othello::Board & board){
        SimulatedBoardList result;

        for(int i = 0; i < board.rows(); ++i){
            for(int j = 0; j < board.cols(); ++j){
                // 盤面のインスタンスを複製しておく
                SimulatedBoard simboard;
                simboard.coord = Othello::Coord(i, j);
                simboard.board = Othello::Board(board);
                
                // 石を置いてみる
                if(simboard.board.put_and_flip(simboard.coord, gi_.my_color()) > 0){
                    // 1か所でも裏返せるとわかったら候補に加える
                    result.push_back(simboard);
                }
            }
        }

        return result;
    }
    
    // コンストラクタ（必要ならば）
    // giには対局情報（あなたがどちらの手番か、盤面のサイズ、等）が入る
    OTHELLO_AI(const Othello::GameInfo & gi) : gi_(gi) {}
    
    // 指す手を決める。
    Othello::Coord place(const Othello::Board & board){
        // 自分が置ける場所をすべて試し、結果をdequeとして返す
        const auto candidates_me = enum_possible_placements(gi_.my_color(), board);
        
        // そのそれぞれに対して、相手が置ける場所をすべて試す
        std::size_t max_pieces_m = 0;
        Othello::Coord max_pieces_m_place;
        
        for(auto& me : candidates_me){
            // 相手が置ける場所を列挙する
            const auto candidates_opponent = enum_possible_placements(gi_.my_color(), me.board);
            
            // 相手が置いた結果の、自分の残る石の数。
            // 相手が置く前の数で初期化しておく。
            std::size_t min_pieces_o = (me.board.pieces())[gi_.my_color()];
            
            // 相手が置ける場所のうち、
            // 自分の石が最も少なくなるときの個数を数える
            // （相手が置ける場所がなければ、上記「相手が置く前の数」が使われる）
            for(auto& o : candidates_opponent){
                const std::size_t pieces = (o.board.pieces())[gi_.my_color()];
                if(pieces < min_pieces_o) min_pieces_o = pieces;
            }
            
            // 相手の手によって「自分の石が一番少なくなる」ようにされた
            // 結果において、自分の石が他の候補よりも多くなるのであれば、
            // その手を選ぶ
            if(min_pieces_o > max_pieces_m){
                max_pieces_m = min_pieces_o;
                max_pieces_m_place = me.coord;
            }
        }
        
        // 置く場所を返す
        return max_pieces_m_place;
    }
};

