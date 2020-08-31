#include "acceptance_reader.h"

std::shared_ptr<TH1D> GetAcceptanceHisto(
    const std::string accPath,
    const bool verbose)
{
    if (verbose)
        std::cout << "\n\nReading acceptance file... [" << accPath << "]\n";
    TFile* inFile = TFile::Open(accPath.c_str(), "READ");
    if (inFile->IsZombie())
    {
        std::cerr << "\n\nError reading input file: " << accPath << std::endl;
        exit(100);
    }
    if (verbose)
        std::cout << "\nExtracting acceptance...\n";
    std::shared_ptr<TH1D> h_acceptance (static_cast<TH1D*>(inFile->Get("acceptance")));
    h_acceptance->SetDirectory(0);
    inFile->Close();

    return h_acceptance;
}