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
  std::string type; // "Hole/hole", "Grap/1LG", "Cent/wall"
  int h_i; // hour start
  int m_i; // min start
  int h_f; // hour end
  int m_f; // min end
  //int run_number;

};



std::map< int, RunData > getRunMap( const std::string& rundatafile );
void findHoleRuns( const std::map< int, RunData >& run_map, RunData& rd, int& found_before, int& found_after );
float getTimeSeconds( int h, int m, int s=0 );
float get_sFrac( const std::string& dataset, int run, float bg_cut );
void addPoints( TGraphErrors* graph, const std::string& fileName, float tzero, int h_i, int m_i, int h_f, int m_f, float bg_frac = 1., float bg_rate = 0. );




int main( int argc, char* argv[] ) {

//if( argc <2 ) {

//  std::cout << "USAGE: ./analyzeRate [graphene_run]" << std::endl;
//  exit(1);

//}

  std::string dataset = "20260227";
  std::string file_rate = "data/" + dataset + "/MM_trigger_rate.csv";

  AndCommon::setStyle();

  int hole_run = 12;
  int cosmics_run = 1;

  std::vector<int> grap_runs;
  grap_runs.push_back( 3);
  grap_runs.push_back( 4);
  grap_runs.push_back( 5);
  //grap_runs.push_back( 6);
  //grap_runs.push_back( 7);
  //grap_runs.push_back( 8);
  //grap_runs.push_back( 9);
  //grap_runs.push_back(10);
  //grap_runs.push_back(11);
  //grap_runs.push_back(13);
  //grap_runs.push_back(15);
  //grap_runs.push_back(16);
  //grap_runs.push_back(17);
  //grap_runs.push_back(18);
  //grap_runs.push_back(19);
  //grap_runs.push_back(20);
  //grap_runs.push_back(21);
  //grap_runs.push_back(22);
  //grap_runs.push_back(23);
  //grap_runs.push_back(25);
  //grap_runs.push_back(26);
  //grap_runs.push_back(27);
  //grap_runs.push_back(28);
  //grap_runs.push_back(29);
  //grap_runs.push_back(30);
  //grap_runs.push_back(31);
  //grap_runs.push_back(33);
  //grap_runs.push_back(34);
  //grap_runs.push_back(37);
  //grap_runs.push_back(39);
  //grap_runs.push_back(40);
  //grap_runs.push_back(41);

  std::map< int, RunData > map_runs;
  map_runs = getRunMap( "./data/"+dataset+"/runData.dat" );
  if( map_runs.size()==0 ) {
    std::cout << "-> Failed to create run map!" << std::endl;
    exit(5);
  }

  float bg_cut = 0.07;

  std::vector<float> sFrac_grap;
  std::vector<RunData> rd_grap;
  for( unsigned i=0; i<grap_runs.size(); ++i ) {
    std::cout << "-> Getting sFrac for run: " << grap_runs[i] << std::endl;
    sFrac_grap.push_back( get_sFrac (dataset, grap_runs[i], bg_cut) );
    rd_grap.push_back( map_runs[grap_runs[i]] );
  }


  float sFrac_cosmics = get_sFrac (dataset, cosmics_run, bg_cut);

  RunData rd_cosmics = map_runs[cosmics_run];
  TGraphErrors* gr_cosmics = new TGraphErrors(0);
  addPoints( gr_cosmics, file_rate, 0., rd_cosmics.h_i, rd_cosmics.m_i, rd_cosmics.h_f, rd_cosmics.m_f, sFrac_cosmics );
  TH1D* h1_rate_cosmics = new TH1D( "rate_comics", "", 10, 0., 1000. );
  for( unsigned iPoint=0; iPoint<gr_cosmics->GetN(); ++iPoint ) {
    Double_t x, y;
    gr_cosmics->GetPoint( iPoint, x, y );
    h1_rate_cosmics->Fill(y);
  }
  float rate_cosmics = h1_rate_cosmics->GetMean();
  std::cout << std::endl << "-> Cosmics rate: " << rate_cosmics << " Hz" << std::endl;



  float sFrac_hole = get_sFrac (dataset, hole_run, bg_cut);
  RunData rd_hole = map_runs[hole_run];

  float tzero = getTimeSeconds(rd_hole.h_i, rd_hole.m_i);

  TGraphErrors* gr_hole = new TGraphErrors(0);
  addPoints( gr_hole, file_rate, tzero, rd_hole.h_i, rd_hole.m_i, rd_hole.h_f, rd_hole.m_f, sFrac_hole, rate_cosmics );
  TH1D* h1_rate_hole = new TH1D( "rate_comics", "", 10, 0., 1000. );
  for( unsigned iPoint=0; iPoint<gr_hole->GetN(); ++iPoint ) {
    Double_t x, y;
    gr_hole->GetPoint( iPoint, x, y );
    h1_rate_hole->Fill(y);
  }
  float rate_hole = h1_rate_hole->GetMean();
  std::cout << std::endl << "-> Hole rate: " << rate_hole << " Hz" << std::endl;




  float xmin = 0.;
  float xmax = getTimeSeconds(rd_grap[rd_grap.size()-1].h_f, rd_grap[rd_grap.size()-1].m_f) - tzero;

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600);
  c1->cd();

  TH2D* h2_axes = new TH2D( "axes", "", 10, xmin, xmax, 10, 0., 330. );
  h2_axes->SetXTitle( "t (s)" );
  h2_axes->SetYTitle( "Rate (Hz)" );
  h2_axes->Draw("same");


  TGraphErrors* gr_grap = new TGraphErrors(0);
  for( unsigned i=0; i<rd_grap.size(); ++i ) 
    addPoints( gr_grap, file_rate, tzero, rd_grap[i].h_i, rd_grap[i].m_i, rd_grap[i].h_f, rd_grap[i].m_f, sFrac_grap[i], rate_cosmics );

  gr_hole->SetMarkerSize( 1.3 );
  gr_hole->SetMarkerStyle( 20 );
  gr_hole->SetMarkerColor( 46 );
  gr_hole->SetLineColor  ( 46 );

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
  legend->AddEntry( gr_hole, Form("Hole run %d", hole_run), "P" );
  legend->AddEntry( gr_grap, Form("Graphene runs %d - %d", grap_runs[0], grap_runs[grap_runs.size()-1]), "P" );
  legend->Draw("same");

  c1->SaveAs( Form("data/%s/rate_heatmap.pdf", dataset.c_str()) );

  std::vector<TH1D*> vh1_rateCorr;
  std::vector<TH1D*> vh1_effRaw  ;
  std::vector<TH1D*> vh1_transp  ;

  for( unsigned i=0; i<grap_runs.size(); ++i ) {

    TH1D* h1_rateCorr = new TH1D( Form("rateCorr_%d", grap_runs[i]), "", 100, 0., 1000. );
    TH1D* h1_effRaw   = new TH1D( Form("effRaw_%d"  , grap_runs[i]), "", 100, 0., 1.    );
    TH1D* h1_transp   = new TH1D( Form("transp_%d"  , grap_runs[i]), "", 100, 0., 1.    );

    vh1_rateCorr.push_back( h1_rateCorr );
    vh1_effRaw.push_back( h1_effRaw );
    vh1_transp.push_back( h1_transp );

  }

  float eff_geom = 0.433; // from Martina's thesis
  float eff_slit = 1.; // 0.245; // slit area fraction

  for( unsigned iPoint=0; iPoint<gr_grap->GetN(); ++iPoint ) {

    Double_t x, y;
    gr_grap->GetPoint( iPoint, x, y );

    float time = tzero + x;

    int this_grap_run = -1;
    for( unsigned i=0; i<grap_runs.size(); ++i ) {
      if( time > getTimeSeconds( rd_grap[i].h_i, rd_grap[i].m_i) 
       && time < getTimeSeconds( rd_grap[i].h_f, rd_grap[i].m_f) ) {
        this_grap_run = i;
      } // if
    } // for
    
    if( this_grap_run >= 0 ) {
      float eff_raw = y/rate_hole;
      float transp = eff_raw/(eff_geom*eff_slit);
      vh1_rateCorr[this_grap_run]->Fill( y );
      vh1_effRaw  [this_grap_run]->Fill( eff_raw );
      vh1_transp  [this_grap_run]->Fill( transp );
    }

  }


  TCanvas* c2 = new TCanvas( "c2", "", 600, 600 );
  c2->cd();

  TH2D* h2_heatMap = new TH2D( "heatMap", "", 40, -10.5, -6.5, 20, -10.5, -8.5 );
  h2_heatMap->SetXTitle( "x position [cm]" );
  h2_heatMap->SetYTitle( "z position [cm]" );

  for( unsigned i=0; i<grap_runs.size(); ++i ) {
    
    int binx = h2_heatMap->GetXaxis()->FindBin( rd_grap[i].x );
    int biny = h2_heatMap->GetYaxis()->FindBin( rd_grap[i].z );

    h2_heatMap->SetBinContent( binx, biny, vh1_transp[i]->GetMean() );

  }

  h2_heatMap->Draw();

  c2->SaveAs( Form("./data/%s/transp_heatMap.pdf", dataset.c_str()) );

  return 0;

}





std::map< int, RunData > getRunMap( const std::string& rundatafile ) {

  std::map< int, RunData > map_runs;

  std::ifstream ifs( rundatafile.c_str() );

  if( !ifs.good() ) {
    std::cout << "-> Can't find rundata file: " << rundatafile << " !! Exiting!" << std::endl;
    exit(7);
  }

  std::string line;

  if( ifs.good() ) {

    while( getline(ifs,line) ) {

      TString line_tstr(line);

      if( line_tstr.BeginsWith( "//" ) ) continue;

      std::vector<std::string> data = AndCommon::splitString( line, "\t" );

      float x = std::atof(data[0].c_str());
      float z = std::atof(data[1].c_str());
      std::string type = data[2]; // "Hole/hole", "Grap/1LG", "Cent/wall"
      int hour_i     = std::atoi(data[3].c_str()); // hour start
      int min_i      = std::atoi(data[4].c_str()); // min start
      int hour_f     = std::atoi(data[5].c_str()); // hour end
      int min_f      = std::atoi(data[6].c_str()); // min end
      int run_number = std::atoi(data[7].c_str());

      RunData rd;
      rd.x = x;
      rd.z = z;
      rd.type = type; // "Hole/hole", "Grap/1LG", "Cent/wall"
      rd.h_i = hour_i; // hour start
      rd.m_i = min_i; // min start
      rd.h_f = hour_f; // hour end
      rd.m_f = min_f; // min end
      //rd.h_i run_number;

      if( run_number > 0 ) {
        map_runs[run_number] = rd;
        std::cout << "-> Added " << type << " run: " << run_number << std::endl;
      }

    } // while

  } // if ifs good

  return map_runs;

}
  


void findHoleRuns( const std::map< int, RunData >& run_map, RunData& rd, int& found_before, int& found_after ) {

  float t_grap = getTimeSeconds( rd.h_i, rd.m_i );

  float min_deltat_before = 99999.;
  float min_deltat_after  = 99999.;

  for( std::map<int, RunData>::const_iterator it = run_map.begin(); it != run_map.end(); it++) {

    if( it->second.type == "Hole" || it->second.type == "hole" ) {

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

    } // if is hole
 
  } // for iterator

}





float getTimeSeconds( int h, int m, int s ) {

  float t = 3600.*((float)h) + 60.*((float)m) + ((float)s);

  return t;

}


float get_sFrac( const std::string& dataset, int run, float bg_cut ) {

  std::string fileName;
  if( run < 10 ) fileName = (std::string)(Form("./data/%s/Spectra/F4_Trace_0000%d.root", dataset.c_str(), run) );
  else           fileName = (std::string)(Form("./data/%s/Spectra/F4_Trace_000%d.root" , dataset.c_str(), run) );

  TFile* file = TFile::Open( fileName.c_str() );
  TH1D* histo = (TH1D*)file->Get("histo");

  
  float s(0.), bg(0.);
  for( unsigned iBin=1; iBin<histo->GetNbinsX(); ++iBin ) {
    if( histo->GetBinCenter(iBin) < bg_cut ) bg += histo->GetBinContent( iBin );
    else                                     s  += histo->GetBinContent( iBin );
  }

  float sFrac = s / ( s + bg );


  TCanvas* c2 = new TCanvas(Form("c2_run%d", run), "", 600, 600 );
  c2->cd();

  float ymax = (histo->GetMaximum()/histo->Integral()<0.01) ? 0.01 : 0.1;
  TH2D* h2_axes = new TH2D( Form("axes_%d", run), "", 10, 0., 0.5, 10, 0., ymax );
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

  c2->SaveAs( Form("data/%s/Spectra/histo_run%d.pdf", dataset.c_str(), run) );

  return sFrac;

}




void addPoints( TGraphErrors* graph, const std::string& fileName, float tzero, int h_i, int m_i, int h_f, int m_f, float sFrac, float rate_bg ) {

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
          graph->SetPoint( graph->GetN(), this_t, sFrac*rate - rate_bg );
        }

        } // else line not 0 

    } // while get line

  } // if file good

}
