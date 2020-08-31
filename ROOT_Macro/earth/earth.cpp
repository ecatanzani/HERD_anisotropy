#include "TTree.h"
#include "TFile.h"
#include "TH2F.h"
#include "TStyle.h"
#include "TString.h"
#include "TCanvas.h"
#include "TRoot.h"

TCanvas *earth(const char* AMS_tree)
{   
    gStyle->SetOptTitle(1);
    gStyle->SetOptStat(0);

    TCanvas*e c1 = new TCanvas("c1","earth_projections",700,700);

    TString dat = gROOT->GetTutorialDir();
    dat.Append("/graphics/earth.dat");
    dat.ReplaceAll("/./","/");

    TFile *inFile = TFile::Open(AMS_tree, "READ");
    TTree *tree = static_cast<TTree*>(inFile->Get("RTI_tree"));
    double geo_lat, geo_lon;
    tree->SetBranchAddress("geo_lat", &geo_lat);
    tree->SetBranchAddress("geo_lon", &geo_lon);

    TH2F *hm = new TH2F("hm","Mercator",  180, -180, 180, 161, -80.5, 80.5);

    for (int idx=0; idx<tree->GetEntries(); ++idx)
    {
        tree->GetEntry(idx);
        geo_lon -= 180;
        hm->Fill(geo_lon, geo_lat, 1);
    }

    hm->Draw("mercator");

    return c1;
}