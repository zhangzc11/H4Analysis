CXX = g++
CXXFLAGS = -std=c++1y -fPIC
SOFLAGS = -shared -O3
INCLUDE = -I"./"
LIB = -L"$(pwd)/lib/"

ROOT_LIB := `root-config --libs --glibs`
ROOT_FLAGS := `root-config --cflags --ldflags` -lMathCore -lMathMore


DEPS = interface/CfgManager.h interface/CfgManagerT.h \
	interface/WFClass.h interface/WFClassNINO.h interface/WFViewer.h \
	interface/InfoTree.h interface/DigiTree.h interface/WFTree.h interface/PositionTree.h interface/H4Tree.h 
DEPS_OBJS = lib/CfgManager.o lib/WFClass.o lib/WFClassNINO.o lib/WFViewer.o \
	lib/H4Tree.o lib/InfoTree.o lib/DigiTree.o lib/WFTree.o lib/PositionTree.o lib/H4Dict.so
PLUG_DEPS = plugin/PluginBase.h plugin/DigitizerReco.h \
	plugin/HodoReco.h plugin/HodoBTFReco.h plugin/HodoReco.h plugin/WireChamberReco.h
PLUG_OBJS = lib/plugin/PluginBase.o lib/plugin/DigitizerReco.o \
	lib/plugin/HodoReco.o lib/plugin/HodoBTFReco.o lib/plugin/WireChamberReco.o
DICT_OBJS = lib/CfgManager.o lib/WFViewer.o

MAIN = bin/H4Reco

all: $(DEPS_OBJS) $(PLUG_OBJS) $(MAIN) lib/LinkDef.cxx 

lib/%.o: src/%.cc $(DEPS)
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(INCLUDE) $(ROOT_LIB) $(ROOT_FLAGS)

lib/plugin/%.o: plugin/%.cc $(PLUG_DEPS)
	mkdir -p lib/plugin
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(INCLUDE) $(ROOT_LIB) $(ROOT_FLAGS)

lib/LinkDef.cxx: interface/CfgManager.h interface/WFViewer.h interface/LinkDef.h 
	rootcling -f $@ -c $^

lib/H4Dict.so: lib/LinkDef.cxx $(DICT_OBJS)
	$(CXX) $(CXXFLAGS) $(SOFLAGS) -o $@ $^ $(INCLUDE) $(ROOT_LIB) $(ROOT_FLAGS) $(LIB)

bin/%: main/%.cpp $(DEPS_OBJS) $(PLUG_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(INCLUDE) $(ROOT_LIB) $(ROOT_FLAGS) $(LIB)

clean:
	rm -fr tmp/*
	rm -fr lib/*
	rm -fr bin/*
