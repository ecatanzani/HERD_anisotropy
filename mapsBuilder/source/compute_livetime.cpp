#include "compute_livetime.h"

std::shared_ptr<TH1D> GetLiveTime(
    const std::shared_ptr<TH2D> dampe_acquition_rate,
    const bool verbose)
{
    if (verbose)
        std::cout << "\nExtracting live-time...\n";
    std::shared_ptr<TH1D> live_time = std::make_shared<TH1D>(
        "live_time", 
        "live_time", 
        dampe_acquition_rate->GetNbinsY(),
        -90, 90);

    // Get mean acquisition rate for a certain latitude
    for (int bYidx=1; bYidx<=dampe_acquition_rate->GetNbinsY(); ++bYidx)
    {
        double myrate = 0;
        for (int bXidx=1; bXidx<=dampe_acquition_rate->GetNbinsX(); ++bXidx)
            myrate += dampe_acquition_rate->GetBinContent(bXidx, bYidx);
        myrate /= dampe_acquition_rate->GetNbinsX();
        auto tmp_livetime = exp(-deadtime * myrate);
        live_time->SetBinContent(bYidx, tmp_livetime);
    }
    
    return live_time;
}