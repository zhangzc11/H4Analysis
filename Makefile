CXX = g++
CXXFLAGS = -std=c++1y -fPIC
SOFLAGS = -shared -O3
INCLUDE = -I"./"
LIB = -L"$(pwd)/lib/"

ROOT_LIB := `root-config --libs --glibs`
ROOT_FLAGS := `root-config --cflags --ldflags` -lMathCore -lMathMore


DEPS = interface/CfgManager.h interface/CfgManagerT.h interface/WFClass.h interface/RecoTree.h interface/WFTree.h interface/H4Tree.h interface/HodoUtils.h interface/WFClassNINO.h interface/WireChamber.h
DEPS_OBJ = lib/CfgManager.o lib/WFClass.o lib/WFClassNINO.o lib/HodoUtils.o lib/WireChamber.o lib/H4lib.so 
DICT_OBJ = lib/CfgManager.o 

lib/%.o: interface/%.cc $(DEPS)
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(INCLUDE) $(ROOT_LIB) $(ROOT_FLAGS)

all: lib/LinkDef.cxx lib/H4lib.so bin/H4Reco bin/TemplatesMaker

lib/LinkDef.cxx: interface/CfgManager.h interface/LinkDef.h 
	rootcling -f $@ -c $^

lib/H4lib.so: lib/LinkDef.cxx $(DICT_OBJ)
	$(CXX) $(CXXFLAGS) $(SOFLAGS) -o $@ $^ $(INCLUDE) $(ROOT_LIB) $(ROOT_FLAGS) $(LIB)

bin/%: src/%.cpp $(DEPS_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(INCLUDE) $(ROOT_LIB) $(ROOT_FLAGS) $(LIB)

clean:
	rm -f tmp/*
	rm -f lib/*
	rm -f bin/*
