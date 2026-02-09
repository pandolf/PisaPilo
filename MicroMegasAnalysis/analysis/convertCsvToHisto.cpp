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
  name_tstr.ReplaceAll("--", "_");
  name = name_tstr;

  std::string outdir = "./data/Spectra";

  TFile* outfile = TFile::Open( Form("%s/%s.root", outdir.c_str(), name.c_str()), "recreate" );
  outfile->cd();

  TH1D* histo = new TH1D( "histo", "", 500, -0.057, 0.442 );


  std::ifstream ifs( fileName.c_str() );

  if( !ifs.good() ) return 0;

  std::string line;

  int iLine = 0;

  if( ifs.good() ) {

    while( getline(ifs,line) ) {

      if( iLine < 5 ) {

        iLine += 1;

      } else {

        std::vector<std::string> words = AndCommon::splitString(line, ",");
        float binStart = std::atof(words[0].c_str());
        int entries = std::atoi(words[1].c_str());

        histo->SetBinContent( histo->FindBin(binStart), entries );

      } // if iLine >= 5

    } // while get line

  } // if ifs good

  outfile->cd();

  histo->Write();

  outfile->Close();

  std::cout << "-> Saved histo in: " << outfile->GetName() << std::endl;

  return 0;

}
  
