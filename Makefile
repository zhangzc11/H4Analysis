CXX = g++
CXXFLAGS = -std=c++11 
INCLUDE = -I"./"

ROOT_LIB := `root-config --libs --glibs`
ROOT_FLAGS := `root-config --cflags --ldflags`

DEPS = interface/CfgManager.h interface/WFClass.h interface/RecoTree.h interface/WFTree.h interface/H4Tree.h interface/HodoUtils.h
DEPS_OBJ = lib/CfgManager.o lib/WFClass.o lib/HodoUtils.o

lib/%.o: interface/%.cc $(DEPS)
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(INCLUDE) $(ROOT_LIB) $(ROOT_FLAGS)

all: bin/H4Reco

bin/H4Reco: src/H4Reco.cpp $(DEPS_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(INCLUDE) $(ROOT_LIB) $(ROOT_FLAGS)

clean:
	rm -f tmp/*
	rm -f lib/*.o
	rm -f bin/*
