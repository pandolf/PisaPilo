#include "../../../ANDROMeDa/CommonTools/interface/AndCommon.h"

#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "TCanvas.h"
#include "TH1D.h"
#include "TMath.h"
#include "TFile.h"
#include "TLegend.h"



int main( int argc, char* argv[] ) {

  if( argc <2 ) {

    std::cout << "USAGE: ./convertCsvToHisto [fileName]" << std::endl;
    exit(1);

  }


  std::string fileName(argv[1]);
  std::string name = AndCommon::removePathAndSuffix( fileName );
  TString name_tstr( name );
  name_tstr.ReplaceAll("-", "_");

  std::cout << "name: " << name << std::endl;
  std::cout << "name_tstr: " << name_tstr << std::endl;


  return 0;

}
  
