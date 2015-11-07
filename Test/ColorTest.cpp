#define CATCH_CONFIG_MAIN
#include<catch.hpp>
#include<vector>
#include"othello_ai.hpp"

TEST_CASE("get_opponent_color", "[lib][Color]")
{
	WARN("NO TEST");
}

TEST_CASE("get_piece_name", "[lib][Color]")
{
	CHECK('.' == Othello::get_piece_name(Othello::Color::EMPTY));
	CHECK('B' == Othello::get_piece_name(Othello::Color::BLACK));
	CHECK('W' == Othello::get_piece_name(Othello::Color::WHITE));
}

