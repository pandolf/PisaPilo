CC = g++
CFLAGS = -Wall -c -g -fpermissive 

#ROOFIT_INCLUDE := $(shell cd $(CMSSW_BASE); scram tool info roofitcore | grep INCLUDE= | sed 's|INCLUDE=||')
#ROOFIT_LIBDIR := $(shell cd $(CMSSW_BASE); scram tool info roofitcore | grep LIBDIR= | sed 's|LIBDIR=||')

INCLUDES = -I. -I.. -I$(ROOTSYS)/include  -I../../../CommonTools/ -I../../../CommonTools/interface/ -I../interface/  #-I$(ROOFIT_INCLUDE)/ C-I$(CMSSW_BASE)/src 

ROOTSYS ?= ERROR_RootSysIsNotDefined

ROOTFLAG = `${ROOTSYS}/bin/root-config --cflags --libs`

EXTRALIBS := -L$(ROOTSYS)/lib -lGenVector -lMinuit -lRooFitCore -lRooFit #-L$(ROOFIT_LIBDIR)/ 


convertCsvToHisto: convertCsvToHisto.cpp AndCommon.o 
	$(CC) -Wall $(INCLUDES) -o convertCsvToHisto convertCsvToHisto.cpp AndCommon.o $(ROOTFLAG) $(EXTRALIBS)

drawHistos: drawHistos.cpp AndCommon.o 
	$(CC) -Wall $(INCLUDES) -o drawHistos drawHistos.cpp AndCommon.o $(ROOTFLAG) $(EXTRALIBS)

analyzeRate: analyzeRate.cpp AndCommon.o 
	$(CC) -Wall $(INCLUDES) -o analyzeRate analyzeRate.cpp AndCommon.o $(ROOTFLAG) $(EXTRALIBS)



AndCommon.o: ../../../ANDROMeDa/CommonTools/src/AndCommon.cc 
	$(CC) -Wall $(INCLUDES) -c ../../../ANDROMeDa/CommonTools/src/AndCommon.cc $(ROOTFLAG) $(EXTRALIBS)



clean:
	rm *.o 
