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
    // - [1] �p�̐΂�2^25�_
    // - [2] �ӂ̐΂́A�������瓖�Y�v���C���[�̐΂݂̂��q�����Ċp�܂�
    //       ���B�ł���Ȃ��2^19�_�A����ȊO��[4]�Ŕ���
    // - [3] �p�ɗאڂ���΁i[2]�ɊY��������̂͏����j�́A���Y�p��
    //       �����̐΂�����Ȃ��[4]�Ŕ���A����ȊO��-2^14�_
    // - [4] ����ȊO�̐΂́A�������ׂĂ�ʂ̐΂Ɉ͂܂�Ă���Ȃ�
    //       2^7�_�A����1�_
    // 2�ׂ̂���̔{���́A���ꂼ�ꉺ�ʂ̗D�揇�ʂ̂��̂���ʂ̂���𒴉߂��Ȃ�
    // �悤�ɐݒ肵�Ă���B
    // �����̌��ʂɑ΂��A[�����̃X�R�A]-[����̃X�R�A]���v�Z���A�����
    // 2^28�����Z���Đ��̒l�ƂȂ邱�Ƃ�ۏ؂���B
    
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
            // �p�̗�
            if(board.get(corner_i, corner_j) != color) return -(1 << 14);
        }
        
        // ���肪���ׂĐ΂����邩����
        bool surrounded = true;
        
        for(int ip = -1; ip <= 1; ++ip){
            for(int jp = -1; jp <= 1; ++jp){
                if(ip == 0 && jp == 0) continue;
                
                if(board.get(i + ip, j + jp) == Othello::Color::EMPTY){
                    // �����ŁAboard.get(i + ip, j + jp) ��
                    // Othello::Color::INVALID �ɂȂ�i�Ֆʂ��͂ݏo���j�\�������邪
                    // �����Ȃ��Ă��u����ɐ΂��Ȃ��v�Ƃ݂͂Ȃ��Ȃ��̂ł���ł悢
                    surrounded = false;
                    break;
                }
            }
            if(!surrounded) break;
        }
        
        if(surrounded) return(1 << 7);
        return 1;
    }
    
    // �X�R�A�v�Z�̃��C��
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
        
        // [2]�̏c��
        for(int i = 0; i < board.rows(); i += board.rows() - 1){
            // ��[�̊p�ɐ΂�����΁A������N�_�ɐ�����
            const Othello::Color corner_color_upper = board.get(i, 0);
            int seq_upper = 0; // ��[���猩�Ă����܂œ����F�������Ă���
            if(corner_color_upper != Othello::Color::EMPTY){
                ++seq_upper;
                while(seq_upper <= board.cols() - 2 && board.get(i, seq_upper) == corner_color_upper){
                    ++seq_upper;
                }
                --seq_upper;
            }
            
            // ���[�̊p�ɐ΂�����΁A������N�_�ɐ�����
            const Othello::Color corner_color_lower = board.get(i, board.cols() - 1);
            int seq_lower = board.cols() - 1; // ���[���猩�Ă����܂œ����F�������Ă���
            if(corner_color_lower != Othello::Color::EMPTY){
                --seq_lower;
                while(seq_lower >= 1 && board.get(i, seq_lower) == corner_color_lower){
                    --seq_lower;
                }
                ++seq_lower;
            }
            
            // �A�������̈�ɂ���ďꍇ����
            if(seq_lower < seq_upper){
                // �[��񂪂��ׂē����F
                score[static_cast<std::size_t>(corner_color_upper)] += (1 << 19) * 6;
            }else{
                // �p����A������[�̕����͂��̃X�R�A�����Z
                if(corner_color_upper != Othello::Color::EMPTY){
                    score[static_cast<std::size_t>(corner_color_upper)] += (1 << 19) * seq_upper;
                }
                if(corner_color_lower != Othello::Color::EMPTY){
                    score[static_cast<std::size_t>(corner_color_lower)] += (1 << 19) * (board.cols() - 1 - seq_lower);
                }
                
                // ����ȊO�̕�����[3], [4]�̏����ŉ��Z
                for(int j = seq_upper + 1; j < seq_lower; ++j){
                    const Othello::Color color = board.get(i, j);
                    if(color != Othello::Color::EMPTY){
                        score[static_cast<std::size_t>(color)] += board_score_ordinary(board, i, j);
                    }
                }
            }
        }
        // [2]�̉���
        for(int j = 0; j < board.cols(); j += board.cols() - 1){
            // ���[�̊p�ɐ΂�����΁A������N�_�ɐ�����
            const Othello::Color corner_color_upper = board.get(0, j);
            int seq_upper = 0; // ���[���猩�Ă����܂œ����F�������Ă���
            if(corner_color_upper != Othello::Color::EMPTY){
                ++seq_upper;
                while(seq_upper <= board.rows() - 2 && board.get(seq_upper, j) == corner_color_upper){
                    ++seq_upper;
                }
                --seq_upper;
            }
            
            // �E�[�̊p�ɐ΂�����΁A������N�_�ɐ�����
            const Othello::Color corner_color_lower = board.get(board.rows() - 1, j);
            int seq_lower = board.rows() - 1; // �E�[���猩�Ă����܂œ����F�������Ă���
            if(corner_color_lower != Othello::Color::EMPTY){
                --seq_lower;
                while(seq_lower >= 1 && board.get(seq_lower, j) == corner_color_lower){
                    --seq_lower;
                }
                ++seq_lower;
            }
            
            // �A�������̈�ɂ���ďꍇ����
            if(seq_lower < seq_upper){
                // �[��񂪂��ׂē����F
                score[static_cast<std::size_t>(corner_color_upper)] += (1 << 19) * 6;
            }else{
                // �p����A������[�̕����͂��̃X�R�A�����Z
                if(corner_color_upper != Othello::Color::EMPTY){
                    score[static_cast<std::size_t>(corner_color_upper)] += (1 << 19) * seq_upper;
                }
                if(corner_color_lower != Othello::Color::EMPTY){
                    score[static_cast<std::size_t>(corner_color_lower)] += (1 << 19) * (board.cols() - 1 - seq_lower);
                }
                
                // ����ȊO�̕�����[3], [4]�̏����ŉ��Z
                for(int i = seq_upper + 1; i < seq_lower; ++i){
                    const Othello::Color color = board.get(i, j);
                    if(color != Othello::Color::EMPTY){
                        score[static_cast<std::size_t>(color)] += board_score_ordinary(board, i, j);
                    }
                }
            }
        }
        
        // �ق��̃}�X
        for(int i = 1; i <= board.rows() - 2; ++i){
            for(int j = 1; j <= board.cols() - 2; ++j){
                const Othello::Color color = board.get(i, j);
                score[static_cast<std::size_t>(color)] += board_score_ordinary(board, i, j);
            }
        }
        
        return score[static_cast<std::size_t>(gi_.my_color())] - score[static_cast<std::size_t>(gi_.opponent_color())] + (1 << 28);
    }
    
    // ��̒T�����s���i���̊֐����ċA�Ăяo�����ăQ�[���؂Ƃ݂Ȃ��j
    int try_place(const Othello::Board & board, Othello::Color player, int depth){
        // ----------
        // ���̐[���܂œ��B������A�Ֆʂ����ĕ]���l��t����
        // ----------
        if(depth >= 4) return board_score(board);
        
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
        //try_place(board, gi_.my_color(), 0);
        int score = try_place(board, gi_.my_color(), 0) - (1 << 28);
        std::cerr << "<Player " << get_piece_name(gi_.my_color()) << " placed with score ";
        if(score < 0){
            std::cerr << '-';
            score = -score;
        }
        std::cerr << 'l'; // ���ʃr�b�g
        while(score > 0){
            std::cerr << (score % 2);
            score /= 2;
        }
        std::cerr << 'u'; // ��ʃr�b�g
        std::cerr << std::endl;
        
        return chosen_place_;
    }
};

