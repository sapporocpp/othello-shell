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
    // ����̕]���l�F�΂̑����̍�
    // �i������0�ȏ�̐���Ԃ����Ƃ�ۏ؂��邽�߁A�Ֆʂ̃}�X�̑��������Z���Ă���j
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
    
    // ��̒T�����s���i���̊֐����ċA�Ăяo�����ăQ�[���؂Ƃ݂Ȃ��j
    int try_place(const Othello::Board & board, Othello::Color player, int depth){
        // ----------
        // ���̐[���܂œ��B������A�Ֆʂ����ĕ]���l��t����
        // ----------
        if(depth >= 2) return board_score(board);
        
        // ----------
        // ���̐[���܂œ��B���Ă��Ȃ��ꍇ�A�T����i�߂�
        // ----------
        int best_score = -1;
        Othello::Coord best_place;
        
        // ���ׂĂ̔Ֆʏ�̈ʒu�ɂ��ČJ��Ԃ�
        for(int i = 0; i < board.rows(); ++i){
            for(int j = 0; j < board.cols(); ++j){
                Othello::Board board_tmp(board);
                if(board_tmp.put_and_flip(i, j, player) == 0) continue;
                
                int score = try_place(board_tmp, Othello::get_opponent_color(player), depth+1);
                if(score == -1){
                    // �����w����肪�Ȃ������ꍇ�A�{���w�����Ƃ��Ă��v���C���[�̑���Ɏw������
                    // ����Ŏw����肪����Ȃ�T���p���A�Ȃ���ΔՖʂ���̃X�R�A�v�Z�ɓ���
                    int score2 = try_place(board_tmp, player, depth+1);
                    if(score2 == -1){
                        score = board_score(board_tmp);
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
        chosen_place_ = Othello::Coord();
        try_place(board, gi_.my_color(), 0);
        return chosen_place_;
    }
};

