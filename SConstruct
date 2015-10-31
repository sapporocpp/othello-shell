# SConstruct
import os

VariantDir("build", "source", duplicate=0)
env = Environment(CXX="clang++", CXXFLAGS=["-std=c++11", "-Wall", "-Wextra"])

add_build_path = lambda filename: os.path.join("build",filename)
env.Program(add_build_path("othello_match"), map(add_build_path,["othello_match.cpp"]))
