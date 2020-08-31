#include "fluxFit.h"

#include <iostream>

#include "TGraphAsymmErrors.h"
#include "TFile.h"
#include "TMath.h"
#include "TKey.h"

void fluxFit::extractFluxInfo(
    const std::string fluxPath,
    const bool verbose)
{
    if (verbose)
        std::cout << "\n\nReading flux file... [" << fluxPath << "]\n";
    TFile* inFile = TFile::Open(fluxPath.c_str(), "READ");
    if (inFile->IsZombie())
    {
        std::cerr << "\n\nError reading input file [" << fluxPath << "]\n";
        exit(100);
    }

    // Building TMultiGraph
    if (verbose)
        std::cout << "\nExtracting TMultiGraph...\n";
    mg = std::make_shared<TMultiGraph>();
    mg->SetTitle("All electron flux (AMS, DAMPE, CALET); Energy(GeV); Flux");
    mg->SetName("flux_mg");

    for (auto&& keyAsObj : *inFile->GetListOfKeys())
    {
        auto key = static_cast<TKey*>(keyAsObj);
        if (!strcmp(key->GetClassName(), "TGraphAsymmErrors"))
            mg->Add(static_cast<TGraphAsymmErrors *>(key->ReadObj()));
    }

    // Fitting E3 flux
    if (verbose)
        std::cout << "\nExtracting E3 flux...\n";
    fitFuncE3 = std::make_shared<TF1>("fitFuncE3", "pow(log10(x),3) * pow(10, [0] + [1]*log10(x) + [2]*pow(log10(x),2) +[3]*pow(log10(x),3) + [4]*pow(log10(x),4)) ", 5, 1e+4);
    fitFuncE3->SetNpx(10000);
    mg->Fit("fitFuncE3", "IR");

    // Flitting flux
    if (verbose)
        std::cout << "\nExtracting flux...\n";
    fitFunc = std::make_shared<TF1>("fitFunc", "(pow(log10(x),3) * pow(10, [0] + [1]*log10(x) + [2]*pow(log10(x),2) +[3]*pow(log10(x),3) + [4]*pow(log10(x),4))) / pow(x,3)", 5, 1e+4);
    for (int nPar=0; nPar<fitFuncE3->GetNpar(); ++nPar)
        fitFunc->SetParameter(nPar, fitFuncE3->GetParameter(nPar));
}

std::shared_ptr<TMultiGraph> fluxFit::getFluxMG()
{
    return mg;
}

std::shared_ptr<TF1> fluxFit::getFluxF()
{
    return fitFunc;
}

std::shared_ptr<TF1> fluxFit::getFluxF3()
{
    return fitFuncE3;
}