#include "../../../ANDROMeDa/CommonTools/interface/AndCommon.h"

#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "TCanvas.h"
#include "TH2D.h"
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
void findHoleRuns( const std::map< int, RunData >& run_map, RunData& rd, int& found_before, int& found_after );
float getTimeSeconds( int h, int m );
void addPoints( TGraphErrors* graph, const std::string& fileName, float tzero, int h_i, int m_i, int h_f, int m_f );




int main( int argc, char* argv[] ) {

  if( argc <2 ) {

    std::cout << "USAGE: ./analyzeRate [graphene_run]" << std::endl;
    exit(1);

  }

  AndCommon::setStyle();

  int grap_run       ( atoi(argv[1]) );
  //int hole_run_before( atoi(argv[2]) );
  //int hole_run_after ( atoi(argv[3]) );

  std::map< int, RunData > map_runs;
  map_runs = getRunMap( "./data/runData.dat" );

  RunData rd_grap = map_runs[grap_run];

  int hole_run_before(-1);
  int hole_run_after (-1);
  findHoleRuns( map_runs, rd_grap, hole_run_before, hole_run_after );

  if( hole_run_before<0 ) {
    std::cout << "hole_run_before: " << hole_run_before << std::endl;
    exit(-1);
  }
  if( hole_run_after<0 ) {
    std::cout << "hole_run_after: " << hole_run_after << std::endl;
    exit(-1);
  }

  std::cout << "hole_run_before: " << hole_run_before << std::endl;
  std::cout << "hole_run_after: " << hole_run_after << std::endl;
  
  RunData rd_hole_before = map_runs[hole_run_before];
  RunData rd_hole_after  = map_runs[hole_run_after ];

  float tzero = getTimeSeconds(rd_hole_before.h_i, rd_hole_before.m_i);

  float xmin = 0.;
  float xmax = getTimeSeconds(rd_hole_after.h_f, rd_hole_after.m_f) - tzero;

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600);
  c1->cd();

  TH2D* h2_axes = new TH2D( "axes", "", 10, xmin, xmax, 10, 0., 300. );
  h2_axes->SetXTitle( "t (s)" );
  h2_axes->SetYTitle( "Rate (Hz)" );
  h2_axes->Draw("same");

  std::string file_rate = "data/MM_trigger_rate_10m_data_2026_02_05_15_58_05.csv";

  TGraphErrors* gr_hole = new TGraphErrors(0);
  addPoints( gr_hole, file_rate, tzero, rd_hole_before.h_i, rd_hole_before.m_i, rd_hole_before.h_f, rd_hole_before.m_f );

  gr_hole->SetMarkerSize( 1.3 );
  gr_hole->SetMarkerStyle( 20 );
  gr_hole->SetMarkerColor( 46 );
  gr_hole->SetLineColor  ( 46 );

  gr_hole->Draw("P same");

  c1->SaveAs( Form("rate_run%d.pdf", grap_run) );

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
  


void findHoleRuns( const std::map< int, RunData >& run_map, RunData& rd, int& found_before, int& found_after ) {

  float t_grap = getTimeSeconds( rd.h_i, rd.m_i );

  float min_deltat_before = 99999.;
  float min_deltat_after  = 99999.;

  for( std::map<int, RunData>::const_iterator it = run_map.begin(); it != run_map.end(); it++) {

    float this_t = getTimeSeconds( it->second.h_i, it->second.m_i );
    float this_deltat = fabs(this_t-t_grap);

    if( this_t < t_grap ) {

      if( this_deltat<min_deltat_before ) {

        min_deltat_before = this_deltat;
        found_before = it->first;

      }
   
    } // if before
 

    if( this_t > t_grap ) {

      if( this_deltat<min_deltat_after ) {

        min_deltat_after = this_deltat;
        found_after = it->first;

      }
   
    } // if after
 
  } // for iterator

}





float getTimeSeconds( int h, int m ) {

  float s = 3600.*((float)h) + 60.*((float)m);

  return s;

}


void addPoints( TGraphErrors* graph, const std::string& fileName, float tzero, int h_i, int m_i, int h_f, int m_f ) {

  float t_i = getTimeSeconds( h_i, m_i ) - tzero;
  float t_f = getTimeSeconds( h_f, m_f ) - tzero;

  std::ifstream ifs( fileName.c_str() );

  if( !ifs.good() ) exit(0);

  std::string line;

  int iLine = 0;

  if( ifs.good() ) {

    while( getline(ifs,line) ) {

      if( iLine == 0 ) {

        iLine += 1;

      } else {

        // format: 2026-02-05 12:04:55,25.86 Hz^M

        std::vector<std::string> commaSplit = AndCommon::splitString( line, "," );
        std::string dateTime = commaSplit[0];
        std::vector<std::string> time = AndCommon::splitString( AndCommon::splitString( dateTime, " " )[1], ":" );
        int h = atoi(time[0].c_str());
        int m = atoi(time[1].c_str());
        float this_t = getTimeSeconds( h, m ) - tzero;
 
        float rate = atof(AndCommon::splitString( commaSplit[1], " ")[0].c_str());

        if( this_t > t_i && this_t < t_f )
          graph->SetPoint( graph->GetN(), this_t, rate );

      } // else line not 0 

    } // while get line

  } // if file good

}
