CXX = g++
CXXFLAGS = -std=c++1y -fPIC
SOFLAGS = -shared -O3
INCLUDE = -I"./" 
LIB = -L"./lib/" -L"./DynamicTTree/lib" -Wl,-rpath=/home/pigo/Work/FastTiming/H4Analysis/DynamicTTree/lib/ -lDTT 

ROOT_LIB := `root-config --libs --glibs`
ROOT_FLAGS := `root-config --cflags --ldflags` -lMathCore -lMathMore

DEPS = DynamicTTree/interface/DynamicTTreeBase.h DynamicTTree/interface/DynamicTTreeInterface.h \
	interface/CfgManager.h interface/CfgManagerT.h \
	interface/WFClass.h interface/WFClassNINO.h interface/WFViewer.h interface/MCPAnalyzer.h \
	interface/RecoTree.h interface/DigiTree.h interface/WFTree.h interface/PositionTree.h interface/H4Tree.h 
DEPS_OBJS = lib/CfgManager.o lib/WFClass.o lib/WFClassNINO.o lib/WFViewer.o lib/MCPAnalyzer.o \
	lib/H4Tree.o lib/RecoTree.o lib/DigiTree.o lib/WFTree.o lib/PositionTree.o lib/H4Dict.so
PLUG_DEPS = plugins/PluginBase.h plugins/DigitizerReco.h plugins/MakeCovarianceMatrix.h \
	plugins/HodoReco.h plugins/HodoBTFReco.h plugins/HodoReco.h plugins/WireChamberReco.h \
	plugins/InfoTreeMaker.h
PLUG_OBJS = lib/plugins/PluginBase.o lib/plugins/DigitizerReco.o lib/plugins/MakeCovarianceMatrix.o \
	lib/plugins/HodoReco.o lib/plugins/HodoBTFReco.o lib/plugins/WireChamberReco.o \
	lib/plugins/InfoTreeMaker.o
DICT_OBJS = lib/CfgManager.o lib/WFViewer.o lib/MCPAnalyzer.o

MAIN = bin/H4Reco

all: dev $(DEPS_OBJS) $(PLUG_OBJS) $(MAIN) lib/LinkDef.cxx 

lib/%.o: src/%.cc $(DEPS)
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(INCLUDE) $(ROOT_LIB) $(ROOT_FLAGS)

lib/plugins/%.o: plugins/%.cc $(PLUG_DEPS)
	mkdir -p lib/plugins
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(INCLUDE) $(ROOT_LIB) $(ROOT_FLAGS)

lib/LinkDef.cxx: interface/CfgManager.h interface/WFViewer.h interface/MCPAnalyzer.h interface/LinkDef.h 
	rootcling -f $@ -c $^

lib/H4Dict.so: lib/LinkDef.cxx $(DICT_OBJS)
	$(CXX) $(CXXFLAGS) $(SOFLAGS) -o $@ $^ $(INCLUDE) $(ROOT_LIB) $(ROOT_FLAGS) $(LIB)

bin/%: main/%.cpp $(DEPS_OBJS) $(PLUG_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(INCLUDE) $(ROOT_LIB) $(ROOT_FLAGS) $(LIB)

dev:
	cd DynamicTTree && $(MAKE)

clean:
	rm -fr tmp/*
	rm -fr lib/*
	rm -fr bin/*
	cd DynamicTTree && $(MAKE) clean
