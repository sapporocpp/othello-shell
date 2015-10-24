// ------------------------------------------------------------
// �I�Z��AI�i�T���v���j
// LV3: �u�������u�������肪�u���A�Ƃ���������V�~�����[�V������
//        �i���ꂼ���肸�Ԃ�j�A�����̐΂��ł������Ȃ���I�ԁB
//        �������A����͂Ȃ�ׂ������̐΂����Ȃ�������I�ԂƂ���B�v
// ------------------------------------------------------------

#include "othello_ai.hpp"
#include <deque> // �g�������w�b�_������΂�����include����

// �N���X���́uOTHELLO_AI�v�ŌŒ肷�邱�ƁB
// �܂��A���[�U��`�֐��͂��̃N���X�̒��ɒ�`���邱�ƁB
class OTHELLO_AI{
private:
    Othello::GameInfo gi_;
    
public:
    // �u�΂�u�����ꏊ�v�Ɓu���̌��ʂ̔Ֆʁv�̑g
    struct SimulatedBoard{
        Othello::Coord coord;
        Othello::Board board;
    };
    
    typedef std::deque<SimulatedBoard> SimulatedBoardList;
    
    // �u����ꏊ�̌���񋓂���
    void enum_possible_placements(Othello::Color color, const Othello::Board & board, SimulatedBoardList & result){
        SimulatedBoard simboard;
        std::size_t flipped;
        
        result.clear();
        
        for(int i = 0; i < board.rows(); ++i){
            for(int j = 0; j < board.cols(); ++j){
                // �Ֆʂ̃C���X�^���X�𕡐����Ă���
                simboard.coord = Othello::Coord(i, j);
                simboard.board = Othello::Board(board);
                // �΂�u���Ă݂�
                flipped = simboard.board.put_and_flip(simboard.coord, gi_.my_color());
                
                if(flipped > 0){
                    // 1�����ł����Ԃ���Ƃ킩��������ɉ�����
                    result.push_back(simboard);
                }
            }
        }
    }
    
    // �R���X�g���N�^�i�K�v�Ȃ�΁j
    // gi�ɂ͑΋Ǐ��i���Ȃ����ǂ���̎�Ԃ��A�Ֆʂ̃T�C�Y�A���j������
    OTHELLO_AI(const Othello::GameInfo & gi) : gi_(gi) {}
    
    // �w��������߂�B
    Othello::Coord place(const Othello::Board & board){
        std::size_t flipped;
        
        SimulatedBoardList candidates_me, candidates_opponent;
        
        // �������u����ꏊ�����ׂĎ����A���ʂ�deque�Ƃ��ĕԂ�
        enum_possible_placements(gi_.my_color(), board, candidates_me);
        
        // ���̂��ꂼ��ɑ΂��āA���肪�u����ꏊ�����ׂĎ���
        std::size_t max_pieces_m = 0;
        Othello::Coord max_pieces_m_place;
        
        for(SimulatedBoardList::iterator it_m = candidates_me.begin(); it_m != candidates_me.end(); ++it_m){
            // ���肪�u����ꏊ��񋓂���
            enum_possible_placements(gi_.my_color(), it_m->board, candidates_opponent);
            
            // ���肪�u�������ʂ́A�����̎c��΂̐��B
            // ���肪�u���O�̐��ŏ��������Ă����B
            std::size_t min_pieces_o = (it_m->board.pieces())[gi_.my_color()];
            
            // ���肪�u����ꏊ�̂����A
            // �����̐΂��ł����Ȃ��Ȃ�Ƃ��̌��𐔂���
            // �i���肪�u����ꏊ���Ȃ���΁A��L�u���肪�u���O�̐��v���g����j
            std::size_t pieces;
            for(SimulatedBoardList::iterator it_o = candidates_opponent.begin(); it_o != candidates_opponent.end(); ++it_o){
                pieces = (it_o->board.pieces())[gi_.my_color()];
                if(pieces < min_pieces_o){
                    min_pieces_o = pieces;
                }
            }
            
            // ����̎�ɂ���āu�����̐΂���ԏ��Ȃ��Ȃ�v�悤�ɂ��ꂽ
            // ���ʂɂ����āA�����̐΂����̌����������Ȃ�̂ł���΁A
            // ���̎��I��
            if(min_pieces_o > max_pieces_m){
                max_pieces_m = min_pieces_o;
                max_pieces_m_place = it_m->coord;
            }
        }
        
        // �u���ꏊ��Ԃ�
        return max_pieces_m_place;
    }
};
