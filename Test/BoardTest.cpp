#define CATCH_CONFIG_MAIN
#include<catch.hpp>
#include<string>
#include"othello_ai.hpp"

TEST_CASE("Board::Init", "[lib][Board]")
{
	const auto rows=8, cols=8;
	auto board = Othello::Board::init(rows,cols);
	REQUIRE(board.rows()==rows);
	REQUIRE(board.cols()==cols);

	const std::string test_place = 
		" . . . . . . . . \n"
		" . . . . . . . . \n"
		" . . . . . . . . \n"
		" . . . W B . . . \n"
		" . . . B W . . . \n"
		" . . . . . . . . \n"
		" . . . . . . . . \n"
		" . . . . . . . . \n";

	CHECK(test_place==board.state());
}


