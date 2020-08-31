#include "assets.h"
#include "fluxFit.h"
#include "acceptance_reader.h"
#include "compute_livetime.h"
#include "DAMPE_acquisition_rate.h"

#include "TDirectory.h"

#include <iostream>

void assets::computeAssets(
    std::shared_ptr<TFile> outRootFile,
    const std::string outFilePath,
    const std::string fluxPath,
    const std::string accPath,
    const std::string acqPath,
    const std::string evtPath,
    const std::string telPath,
    const bool verbose)
{
    // Extract all electron flux fit function
    fluxFit fluxInfo;
    fluxInfo.extractFluxInfo(fluxPath, verbose);
    mg = std::shared_ptr<TMultiGraph>(fluxInfo.getFluxMG());
    fitFunc = std::shared_ptr<TF1>(fluxInfo.getFluxF());
    fitFuncE3 = std::shared_ptr<TF1>(fluxInfo.getFluxF3());

    // Extract HERD acceptance
    acceptance = std::shared_ptr<TH1D>(GetAcceptanceHisto(accPath, verbose));
    
    // Extract DAMPE acquisition rate
    dampe_acq_rate = std::shared_ptr<TH2D>(GetDAMPEAcquisitionRate(acqPath, verbose));

    // Compute HERD livetime
    live_time = std::shared_ptr<TH1D>(GetLiveTime(dampe_acq_rate, verbose));

    // Extract event distribution information
    extractEvtDistributionInfo(evtPath, verbose);

    // Extract AMS RTI tree information
    extractRTItreeInfo(telPath, verbose);

    // Save flux information on file
    if (verbose)
        std::cout << "\n\nSavind assets to output ROOT file... [" << outFilePath << "]\n";
    auto fluxDir = outRootFile->mkdir("flux");
    fluxDir->cd();
    mg->Write();
    fitFunc->Write();
    fitFuncE3->Write();
    
    // Save acceptance information on file
    auto accDir = outRootFile->mkdir("acceptance");
    accDir->cd();
    acceptance->Write();

    // Save acquisition rate information on file
    auto acqDir = outRootFile->mkdir("rate");
    acqDir->cd();
    dampe_acq_rate->Write();

    // Save live-time rate information on file
    auto lvtDir = outRootFile->mkdir("ltime");
    lvtDir->cd();
    live_time->Write();
}

void assets::extractEvtDistributionInfo(
    const std::string evtPath,
    const bool verbose)
{
    if (verbose)
        std::cout << "\n\nReading event distribution file... [" << evtPath << "]\n";
    TFile ang_event_file(evtPath.c_str(), "READ");
    if (ang_event_file.IsZombie())
    {
        std::cerr << "\n\nError reading input event distribution file... [" << evtPath <<"]\n";
        exit(100);
    }
    if (verbose)
        std::cout << "\nExtracting event distribution...\n";
    h_event_distribution.resize(acceptance->GetNbinsX());
    for (auto it=h_event_distribution.begin(); it!=h_event_distribution.end(); ++it)
    {   
        std::string histo_tmp_name = "h_angularDistribution_energyBin_" + std::to_string(std::distance(h_event_distribution.begin(), it));
        *it = std::shared_ptr<TH2D>(static_cast<TH2D*>(ang_event_file.Get(histo_tmp_name.c_str())));
        (*it)->SetDirectory(0);
    }
    ang_event_file.Close();
}

void assets::extractRTItreeInfo(
            const std::string telPath,
            const bool verbose)
{
    if (verbose)
        std::cout << "\n\nReading AMS telemetry file... [" << telPath << "]\n";
    TFile tree_file(telPath.c_str(), "READ");
    if (tree_file.IsZombie())
    {
        std::cerr << "\n\nError reading input telemetry file [" << telPath << "]\n";
        exit(100);
    }
    if (verbose)
        std::cout << "\nExtracting AMS telemetry tree...\n";
    RTItree = std::shared_ptr<TTree>(static_cast<TTree*>(tree_file.Get("RTI_tree")));
    tree_file.Close();
}

std::shared_ptr<TMultiGraph> assets::getFluxMG()
{
    return mg;
}

std::shared_ptr<TF1> assets::getFluxF()
{
    return fitFunc;
}

std::shared_ptr<TF1> assets::getFluxF3()
{
    return fitFuncE3;
}

std::shared_ptr<TH1D> assets::getAcceptance()
{
    return acceptance;
}

std::shared_ptr<TH2D> assets::getDampeAcqRate()
{
    return dampe_acq_rate;
}

std::shared_ptr<TH1D> assets::getLiveTime()
{
    return live_time;
}

std::shared_ptr<TTree> assets::getRTItree()
{
    return RTItree;
}

std::vector<std::shared_ptr<TH2D>> assets::getEveDistributionVector()
{
    return h_event_distribution;
}