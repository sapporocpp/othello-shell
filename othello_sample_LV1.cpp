// ------------------------------------------------------------
// �I�Z��AI�i�T���v���j
// LV1: �P�Ɂu���[����u����ꏊ��T���A������Βu���v
// ------------------------------------------------------------

#include "othello_ai.hpp"

// �N���X���́uOTHELLO_AI�v�ŌŒ肷�邱�ƁB
// �܂��A���[�U��`�֐��͂��̃N���X�̒��ɒ�`���邱�ƁB
class OTHELLO_AI{
private:
    Othello::GameInfo gi_;
    
public:
    // �R���X�g���N�^�i�K�v�Ȃ�΁j
    // gi�ɂ͑΋Ǐ��i���Ȃ����ǂ���̎�Ԃ��A�Ֆʂ̃T�C�Y�A���j������
    OTHELLO_AI(const Othello::GameInfo & gi) : gi_(gi) {}
    
    // �w��������߂�B
    Othello::Coord place(const Othello::Board & board){
        Othello::Board board_tmp;
        std::size_t flipped;
        
        for(int i = 0; i < board.rows(); ++i){
            for(int j = 0; j < board.cols(); ++j){
                // �Ֆʂ̃C���X�^���X�𕡐����Ă���
                board_tmp = Othello::Board(board);
                // �΂�u���Ă݂�
                flipped = board_tmp.put_and_flip(i, j, gi_.my_color());
                
                if(flipped > 0){
                    // 1�����ł����Ԃ���Ƃ킩�����炻���ɒu��
                    return Othello::Coord(i, j);
                }
            }
        }
        
        // �ǂ��ɂ��u���Ȃ�
        return Othello::Coord();
    }
};
