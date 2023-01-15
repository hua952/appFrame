#include <sstream>
#include <fstream>
#include "makeFileCreator.h"
#include "appMgr.h"

makeFileCreator::makeFileCreator(const char* szFilename, app* pApp)
{
	assert(pApp);
	init(szFilename, pApp);
}

dword makeFileCreator::init(const char* szFilename, app* pApp)
{
	std::ofstream os(szFilename);
	const char* szAppName = pApp->name();

	os<<"DIR_PROJ = $(HOME)/HTML5ChessGAme"<<std::endl
		<<"DIR_COM = $(DIR_PROJ)/libCom"<<std::endl
		<<"LibDir= $(DIR_PROJ)/lib"<<std::endl
		<<"TARGET = $(LibDir)/"<<szAppName<<".so"<<std::endl
		<<"INC = -I. -I$(DIR_COM)/include  -I$(DIR_COM)/src/autogen -I$(DIR_PROJ)/libMsg/src"<<std::endl
		<<"CC = clang"<<std::endl
		<<"CPP = clang++"<<std::endl
		<<"AR = ar"<<std::endl
		<<"LD = ld"<<std::endl
		<<"NM = nm"<<std::endl
		<<"RANLIB = ranlib"<<std::endl
		<<"DBG="<<std::endl
		<<"ARFLAGS = -rc"<<std::endl
		<<std::endl
		<<"OBJS=${shell ls src/*.cpp src/gen/*.cpp";
		if(!pApp->procRpcList().empty())
		{
			os<<" src/procMsg/*.cpp";
		}
		os<<"}"<<std::endl
		<<"CXXFLAGS = -g -std=c++14 -fPIC"<<std::endl
		<<"LibCFL = -L$(LibDir) -lrt -lMsg -llog4cplus -ldl  -lCom -pthread"<<std::endl
		<<"DEPS=$(OBJS:.cpp=.d)"<<std::endl
		<<"all:$(TARGET)"<<std::endl
		<<"$(TARGET):$(OBJS:.cpp=.o) $(LibDir)/libCom.so $(LibDir)/libMsg.so"<<std::endl
		<<"	@echo Build...$@ "<<std::endl
		<<"	$(CPP) $(OBJS:.cpp=.o) $(DBG) -o $@ $(INC) $(CXXFLAGS) $(LibCFL)  -shared -o $(TARGET)"<<std::endl
		<<"sinclude $(DEPS)"<<std::endl
		<<"%.d:%.cpp"<<std::endl
		<<"	@set -e;\\"<<std::endl
		<<"	echo $@;\\"<<std::endl
		<<"	$(CPP) -MM $(INC) $(CXXFLAGS) $< > $@.1;\\"<<std::endl
		<<"	cat $@.1 | sed 's,.*:,$@:,g' > $@.2;\\"<<std::endl
		<<"	echo $(CPP) $(INC) $(CXXFLAGS) -c $< -o $*.o >$@.4;\\"<<std::endl
		<<"	cat $@.1 $@.4 | sed 's/^$(CPP)/\t@$(CPP)/g' | sed 's,.*:,$@:,g' | sed 's,.d:,.o:,g'> $@.5;\\"<<std::endl
		<<"	cat $@.2 $@.5 > $@"<<std::endl
		<<"	@rm -f $@.*"<<std::endl
		<<"clean:"<<std::endl
		<<"	@rm -rf $(OBJS:.cpp=.o)"<<std::endl
		<<"	@rm -rf $(OBJS:.cpp=.d)"<<std::endl
		<<"	@rm -rf $(TARGET)";

		return 0;
}

