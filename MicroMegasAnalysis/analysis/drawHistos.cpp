#include "../../../ANDROMeDa/CommonTools/interface/AndCommon.h"

#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TMath.h"
#include "TFile.h"
#include "TLegend.h"
#include "TLine.h"




void drawHistos( const std::string& name, const std::vector<int> runs );



int main( int argc, char* argv[] ) {

  //if( argc <2 ) {

  //  std::cout << "USAGE: ./convertCsvToHisto [fileName]" << std::endl;
  //  exit(1);

  //}


  std::vector<int> runs_graphene;
  runs_graphene.push_back(8);
  runs_graphene.push_back(10);
  runs_graphene.push_back(13);
  runs_graphene.push_back(15);
  runs_graphene.push_back(17);
  runs_graphene.push_back(21);
  runs_graphene.push_back(24);
  runs_graphene.push_back(30);
  runs_graphene.push_back(32);

  drawHistos( "Graphene", runs_graphene );

  std::vector<int> runs_hole;
  runs_hole.push_back(7);
  runs_hole.push_back(9);
  runs_hole.push_back(12);
  runs_hole.push_back(14);
  runs_hole.push_back(16);
  runs_hole.push_back(20);
  runs_hole.push_back(23);
  runs_hole.push_back(29);
  runs_hole.push_back(31);
  //runs_hole.push_back(35);

  drawHistos( "Hole", runs_hole );

  //std::string fileName(argv[1]);
  //std::string name = AndCommon::removePathAndSuffix( fileName );
  //TString name_tstr( name );
  //name_tstr.ReplaceAll("--", "_");
  //name = name_tstr;

  //std::string outdir = "./data/Spectra";

  //TFile* outfile = TFile::Open( Form("%s/%s.root", outdir.c_str(), name.c_str()), "recreate" );
  //outfile->cd();

  //TH1D* histo = new TH1D( "histo", "", 500, -0.057, 0.442 );


  //outfile->cd();

  //histo->Write();

  //outfile->Close();

  //std::cout << "-> Saved histo in: " << outfile->GetName() << std::endl;

  return 0;

}
  

void drawHistos( const std::string& name, const std::vector<int> runs ) {

  TH1D* h1_bg_frac = new TH1D(Form("bg_frac_%s", name.c_str()), "", 100, 0., 0.3 );

  TCanvas* c1 = new TCanvas( Form("c1_%s", name.c_str()), "c1", 600, 600 );
  c1->cd();

  float bg_cut = 0.018;

  float ymax = 0.01;

  TH2D* h2_axes = new TH2D( Form("axes_%s", name.c_str()), "", 10, -0.1, 0.5, 10, 0., ymax );
  h2_axes->Draw();

  for( unsigned i = 0; i<runs.size(); ++i ) {

    int run = runs[i];

    std::string fileName;
    if( run<10 ) fileName = std::string(Form("./data/Spectra/F4_Trace_0000%d.root", run));
    else         fileName = std::string(Form("./data/Spectra/F4_Trace_000%d.root" , run)); 

    TFile* file = TFile::Open(fileName.c_str(), "read");

    TH1D* histo = (TH1D*)file->Get("histo");

    c1->cd()->cd();
    histo->SetLineColor(i+1);
    histo->DrawNormalized("same");

    float s(0.), bg(0.);
    for( unsigned iBin=1; iBin<histo->GetNbinsX(); ++iBin ) {
      if( histo->GetBinCenter(iBin) < bg_cut ) bg += histo->GetBinContent( iBin ); 
      else                                     s  += histo->GetBinContent( iBin ); 
    }

    float bg_frac = bg / ( s + bg );

    //gr_bg_vs_run->SetPoint( i, run, bg_frac );
    h1_bg_frac->Fill( bg_frac );

  } // for runs

  TLine* line_bg = new TLine( bg_cut, 0., bg_cut, ymax);
  line_bg->Draw("same");

  c1->SaveAs(Form("histos_%s.pdf", name.c_str()));

  c1->Clear();
  c1->cd();

  h1_bg_frac->Draw("");

  c1->SaveAs(Form("bg_frac_%s.pdf", name.c_str()));

} // drawHistos
