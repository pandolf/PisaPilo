#include "../../../ANDROMeDa/CommonTools/interface/AndCommon.h"

#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "TCanvas.h"
#include "TH1D.h"
#include "TMath.h"
#include "TFile.h"
#include "TLegend.h"



struct RunData {

  float x;
  float z;
  std::string type; // "Hole", "Grap", "Cent"
  int h_i; // hour start
  int m_i; // min start
  int h_f; // hour end
  int m_f; // min end
  //int run_number;

};



std::map< int, RunData > getRunMap( const std::string& rundatafile );



int main( int argc, char* argv[] ) {

  if( argc <4 ) {

    std::cout << "USAGE: ./analyzeRate [graphene_run] [hole_run_before] [hole_run_after]" << std::endl;
    exit(1);

  }

  int grap_run       ( atoi(argv[1]) );
  int hole_run_before( atoi(argv[2]) );
  int hole_run_after ( atoi(argv[3]) );

  std::map< int, RunData > map_runs;
  map_runs = getRunMap( "./data/runData.dat" );

  RunData rd = map_runs[grap_run];
  
  std::cout << "RunData for run " << grap_run << std::endl;
  std::cout << "x: " <<    rd.x << std::endl;
  std::cout << "z: " <<    rd.z << std::endl;
  std::cout << "type: " <<  rd.type << std::endl;
  std::cout << "h_i: " <<  rd.h_i << std::endl;
  std::cout << "m_i: " <<  rd.m_i << std::endl;
  std::cout << "h_f: " <<  rd.h_f << std::endl;
  std::cout << "m_f: " <<  rd.m_f << std::endl;

  return 0;

}



std::map< int, RunData > getRunMap( const std::string& rundatafile ) {

  std::map< int, RunData > map_runs;

  std::ifstream ifs( rundatafile.c_str() );

  if( !ifs.good() ) exit(0);

  std::string line;

  int iLine = 0;

  if( ifs.good() ) {

    while( getline(ifs,line) ) {

      if( iLine == 0 ) {

        iLine += 1;

      } else {

        float x;
        float z;
        std::string type; // "Hole", "Grap", "Cent"
        float rate_i;
        int hour_i; // hour start
        int min_i; // min start
        int hour_f; // hour end
        int min_f; // min end
        float rate_f;
        int run_number;

        ifs >> x >> z >> type >> rate_i >> hour_i >> min_i >> hour_f >> min_f >> rate_f >> run_number;

        RunData rd;
        rd.x = x;
        rd.z = z;
        rd.type = type; // "Hole", "Grap", "Cent"
        rd.h_i = hour_i; // hour start
        rd.m_i = min_i; // min start
        rd.h_f = hour_f; // hour end
        rd.m_f = min_f; // min end
        //rd.h_i run_number;

        map_runs[run_number] = rd;

      } // if iLine == 0

    } // while get line

  } // if ifs good

  return map_runs;

}
  
