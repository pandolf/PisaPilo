#include "../../../ANDROMeDa/CommonTools/interface/AndCommon.h"

#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "TCanvas.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TMath.h"
#include "TF1.h"
#include "TLine.h"
#include "TPaveText.h"
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
float getTimeSeconds( int h, int m, int s=0 );
float get_sFrac( int run );
void addPoints( TGraphErrors* graph, const std::string& fileName, float tzero, int h_i, int m_i, int h_f, int m_f, float bg_frac = 1. );




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

  std::cout << std::endl << std::endl;
  std::cout << "-> Staring analysis of run " << grap_run << std::endl;

  RunData rd_grap = map_runs[grap_run];
  float sFrac_grap  = get_sFrac( grap_run  );
  std::cout << "-> signal frac run " << grap_run << " : " << sFrac_grap  << std::endl;
  std::cout << std::endl;

  int hole_run_before(-1);
  int hole_run_after (-1);
  findHoleRuns( map_runs, rd_grap, hole_run_before, hole_run_after );

  std::cout << "hole_run_before: " << hole_run_before << std::endl;
  std::cout << "hole_run_after: " << hole_run_after << std::endl;
  
  RunData rd_hole_before = map_runs[hole_run_before];
  RunData rd_hole_after  = map_runs[hole_run_after ];

  float sFrac_before = get_sFrac( hole_run_before );
  float sFrac_after  = get_sFrac( hole_run_after  );

  std::cout << "-> signal frac run before: " << sFrac_before << std::endl;
  std::cout << "-> signal frac run after : " << sFrac_after  << std::endl;
  std::cout << std::endl;

  float tzero = getTimeSeconds(rd_hole_before.h_i, rd_hole_before.m_i);

  float xmin = 0.;
  float xmax = getTimeSeconds(rd_hole_after.h_f, rd_hole_after.m_f) - tzero;

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600);
  c1->cd();

  TH2D* h2_axes = new TH2D( "axes", "", 10, xmin, xmax, 10, 0., 330. );
  h2_axes->SetXTitle( "t (s)" );
  h2_axes->SetYTitle( "Rate (Hz)" );
  h2_axes->Draw("same");


  std::string file_rate = "data/MM_trigger_rate_10m_data_2026_02_05_15_58_05.csv";

  TGraphErrors* gr_grap = new TGraphErrors(0);
  std::cout << std::endl;
  addPoints( gr_grap, file_rate, tzero, rd_grap.h_i, rd_grap.m_i, rd_grap.h_f, rd_grap.m_f, sFrac_grap );

  TGraphErrors* gr_hole = new TGraphErrors(0);
  addPoints( gr_hole, file_rate, tzero, rd_hole_before.h_i, rd_hole_before.m_i, rd_hole_before.h_f, rd_hole_before.m_f, sFrac_before );
  addPoints( gr_hole, file_rate, tzero, rd_hole_after .h_i, rd_hole_after .m_i, rd_hole_after .h_f, rd_hole_after .m_f, sFrac_after  );

  gr_hole->SetMarkerSize( 1.3 );
  gr_hole->SetMarkerStyle( 20 );
  gr_hole->SetMarkerColor( 46 );
  gr_hole->SetLineColor  ( 46 );

  TF1* f1_line = new TF1( "line", "[0] + [1]*x", xmin, xmax );
  f1_line->SetLineColor( 46 );
  gr_hole->Fit( f1_line, "QR" );

  gr_hole->Draw("P same");

  gr_grap->SetMarkerSize( 1.3 );
  gr_grap->SetMarkerStyle( 20 );
  gr_grap->SetMarkerColor( 38 );
  gr_grap->SetLineColor  ( 38 );

  gr_grap->Draw("P same");

  TLegend* legend = new TLegend( 0.6, 0.4, 0.9, 0.55 );
  legend->SetFillColor(0);
  legend->SetBorderSize(0);
  legend->SetFillStyle(3003);
  legend->SetTextSize(0.03);
  legend->AddEntry( gr_grap, Form("Graphene run %d", grap_run), "P" );
  legend->AddEntry( gr_hole, Form("Hole runs %d and %d", hole_run_before, hole_run_after), "P" );
  legend->AddEntry( f1_line, "Linear fit", "L" );
  legend->Draw("same");

  c1->SaveAs( Form("rate_run%d.pdf", grap_run) );

  TH1D* h1_rateCorr = new TH1D( "rateCorr", "", 100, 0., 1000. );
  TH1D* h1_effRaw   = new TH1D( "effRaw"  , "", 100, 0., 1. );
  TH1D* h1_transp   = new TH1D( "transp"  , "", 100, 0., 1. );

  float eff_geom = 0.433; // from Martina's thesis

  for( unsigned iPoint=0; iPoint<gr_grap->GetN(); ++iPoint ) {

    Double_t x, y;
    gr_grap->GetPoint( iPoint, x, y );

    float rate0 = f1_line->Eval( x );
    float rate1 = y;
    float eff_raw = rate1/rate0;
    float transp = eff_raw/eff_geom;

    h1_rateCorr->Fill( rate1 );
    h1_effRaw  ->Fill( eff_raw );
    h1_transp  ->Fill( transp );

  }
   
  std::cout << std::endl << std::endl;
  std::cout << "-> Corrected Rate: "        << h1_rateCorr->GetMean() << " +/- " << h1_rateCorr->GetMeanError() << std::endl; 
  std::cout << "-> Raw Efficiency: "        << h1_effRaw  ->GetMean() << " +/- " << h1_effRaw  ->GetMeanError() << std::endl; 
  std::cout << "-> Graphene Transparency: " << h1_transp  ->GetMean() << " +/- " << h1_transp  ->GetMeanError() << std::endl; 
  std::cout << "(using eff_geom = " << eff_geom << ")" << std::endl;
  std::cout << std::endl;


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

    if( it->second.type != "Hole" ) continue;

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





float getTimeSeconds( int h, int m, int s ) {

  float t = 3600.*((float)h) + 60.*((float)m) + ((float)s);

  return t;

}


float get_sFrac( int run ) {

  std::string fileName;
  if( run < 10 ) fileName = (std::string)(Form("./data/Spectra/F4_Trace_0000%d.root", run) );
  else           fileName = (std::string)(Form("./data/Spectra/F4_Trace_000%d.root" , run) );

  TFile* file = TFile::Open( fileName.c_str() );
  TH1D* histo = (TH1D*)file->Get("histo");

  float bg_cut = 0.018;
  
  float s(0.), bg(0.);
  for( unsigned iBin=1; iBin<histo->GetNbinsX(); ++iBin ) {
    if( histo->GetBinCenter(iBin) < bg_cut ) bg += histo->GetBinContent( iBin );
    else                                     s  += histo->GetBinContent( iBin );
  }

  float sFrac = s / ( s + bg );


  TCanvas* c2 = new TCanvas(Form("c2_run%d", run), "", 600, 600 );
  c2->cd();

  float ymax = 0.01;
  TH2D* h2_axes = new TH2D( Form("axes_%d", run), "", 10, -0.1, 0.5, 10, 0., ymax );
  h2_axes->SetYTitle( "Normalized to Unity" );
  h2_axes->SetXTitle( "Amplitude (V)" );
  h2_axes->Draw("");

  histo->SetLineColor(46);
  histo->SetFillColor(46);
  histo->SetFillStyle(3004);
  histo->DrawNormalized("same");

  TLine* line_bg = new TLine( bg_cut, 0., bg_cut, ymax);
  line_bg->Draw("same");
  
  TPaveText* label_sFrac = new TPaveText( 0.6, 0.75, 0.9, 0.9, "brNDC" );
  label_sFrac->SetTextSize( 0.035 );
  label_sFrac->SetFillColor( 0 );
  label_sFrac->AddText( Form("Run %d", run) );
  label_sFrac->AddText( Form("s / (s+b) = %.3f", sFrac) );
  label_sFrac->Draw("same");
  
  gPad->RedrawAxis();

  c2->SaveAs( Form("data/Spectra/histo_run%d.pdf", run) );

  return sFrac;

}




void addPoints( TGraphErrors* graph, const std::string& fileName, float tzero, int h_i, int m_i, int h_f, int m_f, float sFrac ) {

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
        int s = atoi(time[2].c_str());
        float this_t = getTimeSeconds( h, m, s ) - tzero;
 
        float rate = atof(AndCommon::splitString( commaSplit[1], " ")[0].c_str());

        if( this_t > t_i + 60. && this_t < t_f - 30. ) {
          graph->SetPoint( graph->GetN(), this_t, sFrac*rate );
        }

      } // else line not 0 

    } // while get line

  } // if file good

}
