g++ gixels.cpp $(pkg-config --libs opencv) $(pkg-config --cflags --libs yaml-cpp) -o gixels -std=c++11 -lboost_system -lboost_filesystem -I include
