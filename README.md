# 2015.10.31 Sapporo.cpp もくもく会 お題「オセロ」

作：H.Hiro <main@hhiro.net>

これは、2015.10.17の勉強会向けに作成していたものを、その際のフィードバックをもとに書き直したものです。

# ファイル構成

- `Makefile`
  - Unix系OS向けのMakefileです。デフォルトでは `othello_match` を生成します。
- `othello_ai.hpp`
  - 共通の挙動を定義するヘッダです。
- `othello_match.cpp`
  - 対戦状況を管理するプログラムです。
- `othello_sample_LV****.cpp`
  - 指す手を決めるプログラムです。これに相当するプログラムを皆様に作成していただくことになります。

なお、C++11で新規に規格化された機能も利用しているため、対応したコンパイラの用意をお願いします。

# 動かしてみる

## 対局させるAIを決める

`othello_match.cpp`の上部に、

    #define OTHELLO_AI OthelloAI1
    #include "othello_sample_LV1.cpp"
    #define OTHELLO_AI_1_NAME "othello_sample_LV1.cpp"
    #undef OTHELLO_AI
    #define OTHELLO_AI OthelloAI2
    #include "othello_sample_LV2.cpp"
    #define OTHELLO_AI_2_NAME "othello_sample_LV2.cpp"
    #undef OTHELLO_AI

という部分があるので、そのファイル名の部分を対局させたいものに差し替えてください。

`make`でビルドができたら、コマンドライン上で

    ./othello_match

とだけすれば動きます（Windows環境では`./`は不要）。

# 指す手を決める方法

## 概説

まず、最も簡単な例である `othello_sample_LV1.cpp` について解説します。これが行いたいことは、「ルール上、石を置いてよい場所（少なくとも一つ相手の石を裏返せるような場所）を探し、最初に見つかった場所に置く」というものです。  
これを実現するために、当プログラムで用意されている `Othello::Board::put_and_flip` メソッドを用います。これは、盤面にその石を置き、裏返す操作を行い、裏返した石の数を返すものです。ただし、受け取る`Othello::Board`クラスのインスタンス（プログラム中では`board`）はconst参照であり、直接そのインスタンスに対して裏返す操作（より一般には、変更を加える操作）はできないので、一度コピーしたものを`board_tmp`変数に代入してから`put_and_flip`メソッドを適用しています。

## 改良の方針

多くのものが考えられますが、まずはこのようなところから取り組んではいかがでしょうか。

- `othello_sample_LV2A.cpp`では、「角に置けるときには優先的に置く」「角の隣には極力置かない」という戦術を採用しているが、これをさらに改善できるか。
- `othello_sample_LV3.cpp`では、「自分の手を一手＋相手の手を一手」まで判断して自分の石が最も多くなる手を選ぶことにしたものの、これを何手にでも増やせるようにするにはどうすればよいか。

また、上記のサンプルプログラムを動かしてみるとわかるのですが、先手・後手の差で結果がかなり大きく変わります。作成したプログラムを試す際は、先手・後手の両方で、また色々な相手と試してみるようにしましょう。

# API

## enum class Othello::Color;

石を表す列挙型です。

- `Color BLACK;` // 黒石を表す定数
- `Color WHITE;` // 白石を表す定数
- `Color EMPTY;` // 石は置かれていない
- `Color INVALID;` // 無効な座標を指定した場合などの値

## class Othello::GameInfo;

対局情報を表すクラスです。AIのクラスのコンストラクタに渡ります（`othello_sample_LV****.cpp`のそれぞれのコンストラクタを参照）。

- `Color GameInfo::my_color()` // 自分の石の色
- `Color GameInfo::opponent_color()` // 相手の石の色
- `int GameInfo::rows()` // 盤面の行数
- `int GameInfo::cols()` // 盤面の列数

## class Othello::Coord;

盤面上の座標を表すクラスです。AI中の`place`関数がこのインスタンスを返すと、指す手を決めたことになります。

- `Coord(int i, int j)` // 盤面上の座標
- `Coord()` // 空座標。パスをしたい場合にこれを返す。
- `int Coord::row()` // 行番号
- `int Coord::col()` // 列番号
- `bool Coord::is_valid()` // 空座標ならfalse、そうでないときはtrue

## class Othello::Board;

盤面の石の配置を表すクラスです。

- `Board::Board(const Board & other)`  
  // 他の盤面をコピー。  
  // これ以外にもコンストラクタはあるものの割愛する  
  // （AIのコードを書く上ではまず用いないため）。
- `int Board::rows()` // 盤の行数
- `int Board::cols()` // 盤の列数
- `std::map<Color, std::size_t> Board::pieces()`  
  // 盤上の石の数。例えば  
  // `(board.pieces())[Othello::Color::WHITE]`
  // で盤上の白石の数を得ることができる。
- `Color Board::get(const Coord & coord)`  
  `Color Board::get(int i, int j)`  
  // その座標に置かれている石を取得する。  
  // なければ`Color::EMPTY`、座標が盤面外だと`Color::INVALID`。
- `bool Board::put_only(const Coord & coord, Color color)`  
- `bool Board::put_only(int i, int j, Color color)`  
  // 座標を指定して`color`の石を配置する（裏返しはしない）。  
  // 石が置けると（＝座標が適正だと）true、座標が盤面外だとfalse。
- `std::size_t Board::put_and_flip(const Coord & coord, Color color)`  
- `std::size_t Board::put_and_flip(int i, int j, Color color)`  
  // 座標を指定して`color`の石を配置し、裏返す。  
  // インスタンスの内容が変更されるので、元のものを残したい場合は  
  // 事前にインスタンスをコピーする必要がある。  
  // 裏返した石の数を返す。
- `void Board::display(void)`  
  // 盤面を表示する。
- `void Board::display(int i, int j)`  
  // 上記と似るが、座標(i, j)を強調表示する。  
  // `othello_match.cpp`では最後に指された手を表示するのに利用している。
