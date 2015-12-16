// ------------------------------------------------------------
// �I�Z��AI�i��ǂ݂���j
// ------------------------------------------------------------

#include "othello_ai.hpp"
#include <stack> // �g�������w�b�_������΂�����include����

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
        Othello::Color next_player;
    };
    typedef std::stack<SimulatedBoard> SimulatedBoardList;
    
private:
    // ------------------------------------------------------------
    // �����Ƒ���̎���V�~�����[�V�������A�őP�̎��I��
    // �i�~�j�}�b�N�X�@�j
    // ------------------------------------------------------------
    
    Othello::Coord chosen_place_;
    // �ŏI�I�ɒu�����ƂɂȂ����ꏊ�B
    // �{����try_place�̕Ԃ�l�ɂ����������̂����A�ċA�Ăяo�������֐��ł��邽�߁A
    // �X�^�b�N�̗]�v�ȏ����h�����߃����o�ϐ��ɁB
    
    // �Ֆʂɑ΂��ăX�R�A��t����B
    // ����̕]���l�F
    // - �u�p�ɒu�����񐔁v�Ȃ�тɁu�p�ׂ̗ɒu�����񐔁v���W�v���A
    //   �ȉ��̉񐔂̏��Ō��肷��B
    //   [1] (�������p�ɒu������ - ���肪�p�ɒu������) ���ő��̂���
    //   [2] �������p�ɒu������ ���ő��̂���
    //   [3] (���肪�p�ׂ̗ɒu������ - �������p�ׂ̗ɒu������) ���ő��̂���
    //   [4] ���肪�p�ׂ̗ɒu������ ���ő��̂���
    // �]���l�� ([1] * 262144 + [2] * 4096 + [3] * 64 + [4]) + 2097152 ��Ԃ��Ă���B
    // �Ō�u+ 2097152�v���Ă���̂́A���̒l��Ԃ����Ƃ�ۏ؂��邽�߁B
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
    
    // ��̒T�����s���i���̊֐����ċA�Ăяo�����ăQ�[���؂Ƃ݂Ȃ��j
    int try_place(const Othello::Board & board, const PlacementCounter & pc, Othello::Color player, int depth){
        // ----------
        // ���̐[���܂œ��B������A�Ֆʂ����ĕ]���l��t����
        // ----------
        if(depth >= 4) return board_score(pc);
        
        // ----------
        // ���̐[���܂œ��B���Ă��Ȃ��ꍇ�A�T����i�߂�
        // ----------
        int best_score = -1;
        Othello::Coord best_place;
        
        // ���ׂĂ̔Ֆʏ�̈ʒu�ɂ��ČJ��Ԃ�
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
                    // �����w����肪�Ȃ������ꍇ�A�{���w�����Ƃ��Ă��v���C���[�̑���Ɏw������
                    // ����Ŏw����肪����Ȃ�T���p���A�Ȃ���ΔՖʂ���̃X�R�A�v�Z�ɓ���
                    int score2 = try_place(board_tmp, pc_tmp, player, depth+1);
                    if(score2 == -1){
                        score = board_score(pc_tmp);
                    }else{
                        score = score2;
                    }
                }
                
                // �w����肪�������ꍇ�A���̃X�R�A��
                // - �����̎�Ԃ��V�~�����[�V��������Ȃ�΁A�����ɂƂ��čőP�̎�
                // - ����̎�Ԃ��V�~�����[�V��������Ȃ�΁A�����ɂƂ��čň��̎�
                // �ł���Ȃ�΂��̎���c���B
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
    // �R���X�g���N�^�i�K�v�Ȃ�΁j
    // gi�ɂ͑΋Ǐ��i���Ȃ����ǂ���̎�Ԃ��A�Ֆʂ̃T�C�Y�A���j������
    OTHELLO_AI(const Othello::GameInfo & gi) : gi_(gi) {}
    
    // �w��������߂�B
    Othello::Coord place(const Othello::Board & board){
        PlacementCounter pc_empty;
        
        chosen_place_ = Othello::Coord();
        try_place(board, pc_empty, gi_.my_color(), 0);
        return chosen_place_;
    }
};

