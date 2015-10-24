// ------------------------------------------------------------
// �I�Z��AI�i�T���v���j
// LV2�̔��C����:
// �u���[����u����ꏊ��T���A�ł������΂𗠕Ԃ���ꏊ�ɒu���v
// �u�������A�p�̃}�X�ɒu����Ƃ��͂����D�悵�A�p�ׂ̗̃}�X��
//   �������Ƃ͋ɗ͍s��Ȃ��v
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
        
        // �u����ꏊ�̂����A�i�e���ނɂ����āj�΂��ł��������Ԃ���Ƃ���
        // ���Ȃ�тɏꏊ�B
        // [0]: �p�̃}�X�i�D��x�ō��j
        // [1]: ����ȊO
        // [2]: �p�ɗאڂ���}�X�i�D��x�Œ�j
        std::size_t max_flipped[3];
        Othello::Coord max_flipped_place[3];
        for(std::size_t i = 0; i < 3; ++i){
            max_flipped[i] = 0;
        }
        
        for(int i = 0; i < board.rows(); ++i){
            for(int j = 0; j < board.cols(); ++j){
                // �Ֆʂ̃C���X�^���X�𕡐����Ă���
                board_tmp = Othello::Board(board);
                // �΂�u���Ă݂�
                flipped = board_tmp.put_and_flip(i, j, gi_.my_color());
                
                // ���܂Ŏ��������ň�ԑ����΂𗠕Ԃ��Ă�����A
                // ���̍��W�� max_flipped_place �ɕۑ����Ă���
                if((i == 0 || i == board.rows() - 1) && (j == 0 || j == board.cols() - 1)){
                    // �p�B�D�悵�Ēu��
                    if(flipped > max_flipped[0]){
                        max_flipped[0] = flipped;
                        max_flipped_place[0] = Othello::Coord(i, j);
                    }
                }else if((i <= 1 || i >= board.rows() - 2) && (j <= 1 || j >= board.cols() - 2)){
                    // �p�ׁ̗B�D��x�Œ�
                    if(flipped > max_flipped[2]){
                        max_flipped[2] = flipped;
                        max_flipped_place[2] = Othello::Coord(i, j);
                    }
                }else{
                    // ���̑�
                    if(flipped > max_flipped[1]){
                        max_flipped[1] = flipped;
                        max_flipped_place[1] = Othello::Coord(i, j);
                    }
                }
            }
        }
        
        // �u���ꏊ��Ԃ�
        if(max_flipped_place[0].is_valid()) return max_flipped_place[0];
        if(max_flipped_place[1].is_valid()) return max_flipped_place[1];
        return max_flipped_place[2];
    }
};
