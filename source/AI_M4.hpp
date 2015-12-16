// ------------------------------------------------------------
// �I�Z��AI�iH.Hiro�쐬�j
// �ł�����ƂŌ��߂Ă܂�
// ------------------------------------------------------------

#include "othello_ai.hpp"

// �N���X���́uOTHELLO_AI�v�ŌŒ肷�邱�ƁB
// �܂��A���[�U��`�֐��͂��̃N���X�̒��ɒ�`���邱�ƁB
class OTHELLO_AI{
private:
    Othello::GameInfo gi_;
    
    // (i, j)�ɐ΂�u�������ʂ̔Ֆʂ�board�ƂȂ����Ƃ��A���̎�͂ǂꂾ���D�悷�ׂ����]��
    //
    // [1] ���D��x0���p�͍ŗD��B
    // [2] ���D��x1���p�ȊO�̕ӂɂ��ẮA��������p�Ɍq����S���̐΂������̂ɂȂ�Ȃ�D��B
    //     �Ⴆ�Ή��̐}�ŁA�u�{�v�ɂ��Ă͍��΂͗D��I�ɒu�����A�u�~�v�͂����ł͂Ȃ��B
    //     ���������{�~�E�E
    //     ���E�E�E�E���E�E
    //     ���E�E�E�E���E�E
    //     �{�����E�E�E�E�E
    //     ���E�E�E�E�E�E�E
    //     �~�����E�E�E�E�E
    //     �E�E�E�E�E�E�E�E
    //     �E�E�E�E�E�E�E�E
    // [3] ���D��x3���p�ׂ̗́A�Ή�����p�������̐΂łȂ��Ȃ�΁A�D��x�Œ�
    // [4] ���D��x2���ȏ�ɊY�����Ȃ����̂͗D��x����
    int calc_priority(const Othello::Board & board, int i, int j){
        // �p�B�ŗD��
        if((i == 0 || i == board.rows() - 1) && (j == 0 || j == board.cols() - 1)){
            return 0;
        }
        
        // �ӁB
        // �ǂ��炩�̊p�܂Ŏ����̐΂������Ă���ΗD��(1)
        // �����łȂ���ΐ�ɐi��
        if(i == 0 || i == board.rows() - 1){
            int c;
            
            // �E�[�����[�̗�ɒu�����ꍇ
            for(c = j-1; c >= 0; --c){
                // �u�����ꏊ�����Ɍ��čs���āA�p�܂Ŏ����̐΂������Ă����
                if(board.get(i, c) != gi_.my_color()) break;
            }
            if(c == -1) return 1;
            
            for(c = j+1; c < board.cols(); ++c){
                // �u�����ꏊ���牺�Ɍ��čs���āA�p�܂Ŏ����̐΂������Ă����
                if(board.get(i, c) != gi_.my_color()) break;
            }
            if(c == board.cols()) return 1;
        }
        
        if(j == 0 || j == board.cols() - 1){
            int c;
            
            // ��[�����[�̗�ɒu�����ꍇ
            for(c = i-1; c >= 0; --c){
                // �u�����ꏊ���獶�Ɍ��čs���āA�p�܂Ŏ����̐΂������Ă����
                if(board.get(c, j) != gi_.my_color()) break;
            }
            if(c == -1) return 1;
            
            for(c = i+1; c < board.rows(); ++c){
                // �u�����ꏊ����E�Ɍ��čs���āA�p�܂Ŏ����̐΂������Ă����
                if(board.get(c, j) != gi_.my_color()) break;
            }
            if(c == board.rows()) return 1;
        }
        
        // �p�ׁ̗B�Y������p�������̐΂łȂ���ΗD��x�Œ�
        int corner_i = -1, corner_j = -1; // �u�Y������p�v�̍��W
        
        if(i <= 1) corner_i = 0;
        if(i >= board.rows() - 2) corner_i = board.rows() - 1;
        if(j <= 1) corner_j = 0;
        if(j >= board.cols() - 2) corner_j = board.cols() - 1;
        if(corner_i != -1 && corner_j != -1 && board.get(corner_i, corner_j) != gi_.my_color()){
            return 3;
        }
        
        return 2;
    }
    
    // �����D��x�̂��̂̂Ȃ��ł̔�r�ɕK�v�Ȏw�W�B
    // �����ɐ΂�u���Ă݂��Ƃ��āA
    // �����̐΂ňȉ��̂����ꂩ�ɊY��������̂���ԑ����Ȃ���̂�I�ԁF
    // �u�p�v�u�Ӂv�u���͔����ɑ��̐΂�����΁v
    int count_good_pieces(const Othello::Board & board){
        int good_pieces = 0;
        
        for(int i = 0; i < board.rows(); ++i){
            if(board.get(i, 0) == gi_.my_color()) ++good_pieces;
            if(board.get(i, board.cols()-1) == gi_.my_color()) ++good_pieces;
        }
        for(int j = 1; j < board.cols() - 1; ++j){
            if(board.get(0, j) == gi_.my_color()) ++good_pieces;
            if(board.get(board.rows()-1, j) == gi_.my_color()) ++good_pieces;
        }
        
        for(int i = 1; i < board.rows() - 1; ++i){
            for(int j = 1; j < board.cols() - 1; ++j){
                if(
                    board.get(i-1, j-1) != Othello::Color::EMPTY &&
                    board.get(i,   j-1) != Othello::Color::EMPTY &&
                    board.get(i+1, j-1) != Othello::Color::EMPTY &&
                    board.get(i-1, j)   != Othello::Color::EMPTY &&
                    board.get(i,   j)   == gi_.my_color() &&
                    board.get(i+1, j)   != Othello::Color::EMPTY &&
                    board.get(i-1, j+1) != Othello::Color::EMPTY &&
                    board.get(i,   j+1) != Othello::Color::EMPTY &&
                    board.get(i+1, j+1) != Othello::Color::EMPTY
                ){
                    ++good_pieces;
                }
            }
        }
        
        return good_pieces;
    }
    
public:
    // �R���X�g���N�^�i�K�v�Ȃ�΁j
    // gi�ɂ͑΋Ǐ��i���Ȃ����ǂ���̎�Ԃ��A�Ֆʂ̃T�C�Y�A���j������
    OTHELLO_AI(const Othello::GameInfo & gi) : gi_(gi) {}
    
    // �w��������߂�B
    Othello::Coord place(const Othello::Board & board){
        int max_good_pieces = -1;
        int best_priority = 3;
        int best_i = -1, best_j = -1;
        
        for(int i = 0; i < board.rows(); ++i){
            for(int j = 0; j < board.cols(); ++j){
                // �Ֆʂ̃C���X�^���X�𕡐����Ă���
                Othello::Board board_tmp = Othello::Board(board);
                
                // �΂�u���Ă݂�B���Ԃ��Ȃ��Ȃ炻�̎�͖���
                if(board_tmp.put_and_flip(i, j, gi_.my_color()) == 0){
                    continue;
                }
                
                // �D��x�𒲂ׂ�
                int priority = calc_priority(board_tmp, i, j);
                
                // ���ܓ���ꂽ�D��x��荂���D��x�̎肪���łɑ��݂���Ȃ�A
                // ���̌��͂���ȏ㌟������K�v���Ȃ�
                if(priority <= best_priority){
                    if(priority < best_priority) max_good_pieces = -1;
                    best_priority = priority;
                    
                    int good_pieces = count_good_pieces(board_tmp);
                    if(good_pieces > max_good_pieces){
                        max_good_pieces = good_pieces;
                        best_i = i;
                        best_j = j;
                    }
                }
            }
        }
        
        // �u���ꏊ��Ԃ�
        return((best_i == -1 && best_j == -1) ? Othello::Coord() : Othello::Coord(best_i, best_j));
    }
};
