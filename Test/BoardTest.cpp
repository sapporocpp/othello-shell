#define CATCH_CONFIG_MAIN
#include<catch.hpp>
#include<vector>
#include"othello_ai.hpp"

TEST_CASE("Board::Init", "[lib][Board]")
{
	const auto rows=8, cols=8;
	auto board = Othello::Board::init(rows,cols);
	REQUIRE(board.rows()==rows);
	REQUIRE(board.cols()==cols);

	const auto e = Othello::Color::EMPTY;
	const auto b = Othello::Color::BLACK;
	const auto w = Othello::Color::WHITE;
	const std::vector<Othello::Color> test_place = {
		e,e,e,e,e,e,e,e,
		e,e,e,e,e,e,e,e,
		e,e,e,e,e,e,e,e,
		e,e,e,w,b,e,e,e,
		e,e,e,b,w,e,e,e,
		e,e,e,e,e,e,e,e,
		e,e,e,e,e,e,e,e,
		e,e,e,e,e,e,e,e,
	};

	std::vector<Othello::Color> board_place;
	for(auto i=0;i<cols*rows;++i) {
		board_place.push_back(board.get(i%rows, i/rows));
	}
	CHECK(test_place==board_place);
}


