#include "event_map.h"
#include "unique.h"
#include "healpix_map.h"

inline void branchRTItree(
	const std::shared_ptr<TTree> RTItree,
	std::vector<double> &pointing,
	double &geo_lat)
{
	RTItree->SetBranchAddress("glat", &pointing[1]);
	RTItree->SetBranchAddress("glon", &pointing[0]);
	RTItree->SetBranchAddress("geo_lat", &geo_lat);
}

void evaluateEventMap(
	const std::shared_ptr<TF1> fitFunc,
	const std::shared_ptr<TH1D> acceptance,
	const std::shared_ptr<TH1D> live_time,
	const std::shared_ptr<TTree> RTItree,
	const std::vector<std::shared_ptr<TH2D>> h_event_distribution,
	AnyOption &opt,
	const std::time_t ctime,
	const bool verbose)
{
	// initialise healpix map class
	healpix_map datamap;
	int energy_bins = acceptance->GetNbinsX();
	datamap.initDataMap(energy_bins);

	// Branch AMS RTI tree
	std::vector<double> pointing(2, 0);
	std::vector<double> old_pointing(2);
	double geo_lat = 0;
	branchRTItree(
		RTItree,
		pointing,
		geo_lat);

	// Start looping on tree
	for (auto idx = 0; idx < RTItree->GetEntries(); ++idx)
	{
		if ((idx + 1) % 1000 == 0)
			std::cout << "\n[Event Loop]: " << idx + 1;

		RTItree->GetEntry(idx);
		
		// Use healpix convention for pointing
		pointing[1] += 90;
		pointing[1] *= TMath::DegToRad();

		pointing[0] += 180;
		pointing[0] *= TMath::DegToRad();

		// Fill pointing map
		datamap.buildPointingMap(pointing);

		if (!idx)
		{
			old_pointing = pointing;
			continue;
		}
		
		datamap.buildDataMap(
			h_event_distribution,
			old_pointing,
			pointing,
			geo_lat,
			fitFunc,
			acceptance,
			live_time);
	}

	// Get pointing map title
	auto pointing_map_title =
		uniquePointingMapOutFile(
			opt,
			ctime);

	// Write pointign map
	datamap.writePointingMap(
		pointing_map_title, 
		"G");

	// Write data maps
	datamap.writeDataMaps(
		opt,
		ctime);
}
