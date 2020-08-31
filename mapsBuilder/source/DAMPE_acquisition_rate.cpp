#include "DAMPE_acquisition_rate.h"

std::shared_ptr<TH2D> GetDAMPEAcquisitionRate(
    const std::string acqPath,
    const bool verbose)
{
    if (verbose)
        std::cout << "\n\nReading DAMPE acquisitionr rate file... [" << acqPath << "]\n";
    TFile *inFile = TFile::Open(acqPath.c_str(), "READ");
    if (inFile->IsZombie())
    {
        std::cerr << "\n\nError reading DAMPE acqusition rate file: " << acqPath;
        exit(100);
    }
    if (verbose)
        std::cout << "\nExtracting DAMPE acquisitionr rate...\n";
    std::shared_ptr<TH2D> dampe_acq_rate(static_cast<TH2D*>(inFile->Get("rate")));
    dampe_acq_rate->SetDirectory(0);
    // Scale for herd acceptance - factor ~ 10
    dampe_acq_rate->Scale(10);
    inFile->Close();
    
    return dampe_acq_rate;
}