# SConstruct
import os

VariantDir("build", "source", duplicate=0)
env = Environment(CXX="clang++", CXXFLAGS=["-std=c++11", "-Wall", "-Wextra"])

add_build_path = lambda filename: os.path.join("build",filename)
env.Program(add_build_path("othello_match"), map(add_build_path,["othello_match.cpp"]))

# `scons test`
Tests = [
	env.Program("board_test", "Test/BoardTest.cpp", CPPPATH=["Catch/include","source"]),
	env.Program("color_test", "Test/ColorTest.cpp", CPPPATH=["Catch/include","source"]),
]
test_alias= Alias("test", Tests, [t[0].abspath for t in Tests ])
AlwaysBuild(test_alias)
