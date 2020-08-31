#include "healpix_map.h"
#include "unique.h"

#include "healpix_base.h"
#include "chealpix.h"

#include <string>

#include "TMath.h"

healpix_map::healpix_map(long side, UInt_t seed)
{
	nside = side;
	npix = nside2npix(nside);
	rand_seed = seed;
	rgen = std::make_shared<TRandom3>();
}

void healpix_map::initDataMap(int energy_bins)
{
	pixel_dataMap.resize(energy_bins);
	for (auto it = pixel_dataMap.begin(); it != pixel_dataMap.end(); ++it)
	{
		(*it).resize(npix);
		for (auto s_it = (*it).begin(); s_it != (*it).end(); ++s_it)
			(*s_it) = 0;
	}
	iss_pointing.resize(npix);
	for (auto it = iss_pointing.begin(); it != iss_pointing.end(); ++it)
		(*it) = 0;
}

void healpix_map::buildDataMap(
	const std::vector<std::shared_ptr<TH2D>> h_event_distribution,
	const std::vector<double> old_pointing,
	const std::vector<double> pointing,
	const double geo_lat,
	const std::shared_ptr<TF1> fitFunc,
	const std::shared_ptr<TH1D> acceptance,
	const std::shared_ptr<TH1D> live_time)
{
	// Compute trajectory base angle
	auto base_angle = TMath::ATan2((pointing[1] - old_pointing[1]), (pointing[0] - old_pointing[0]));

	// Extract events from energy bin map
	for (auto it = h_event_distribution.begin(); it != h_event_distribution.end(); ++it)
	{
		// For each time interval compute the number of points to extract
		auto events_to_extract =
			GetNumberToExtract(
				fitFunc,
				acceptance,
				live_time,
				(int)std::distance(h_event_distribution.begin(), it),
				geo_lat);

		// Compute events coordinates
		auto coord = getEvtsCoordinate(events_to_extract, base_angle, *it, pointing);

		// Add events coordinates to the data map
		for (unsigned int idx = 0; idx < coord.size(); ++idx)
		{
			long hpix = 0;
			ang2pix_ring(nside, coord[idx][1], coord[idx][0], &hpix);
			++pixel_dataMap[std::distance(h_event_distribution.begin(), it)][hpix];
		}
	}
}

unsigned int healpix_map::GetNumberToExtract(
	const std::shared_ptr<TF1> fitFunc,
	const std::shared_ptr<TH1D> acceptance,
	const std::shared_ptr<TH1D> live_time,
	const int binIdx,
	const double geo_lat)
{
	std::unique_ptr<TH1D> h_acqRate(static_cast<TH1D *>(acceptance->Clone("h_acqRate")));
	auto multiplyStatus = h_acqRate->Multiply(fitFunc.get());
	if (!multiplyStatus)
		std::cout << "\nERROR during TF1 multiplication" << std::endl;
	double energyWidth = acceptance->GetBinWidth(binIdx);
	double bin_rate = h_acqRate->GetBinContent(binIdx) * energyWidth;
	double live_time_value = live_time->GetBinContent(live_time->GetXaxis()->FindBin(geo_lat));
	double mean_number_of_events = bin_rate * live_time_value;
	unsigned int events = rgen->Poisson(mean_number_of_events);
	return events;
}

std::vector<std::vector<double>> healpix_map::getEvtsCoordinate(
	const unsigned int events_to_extract,
	const double base_angle,
	const std::shared_ptr<TH2D> evDist,
	const std::vector<double> pointing)
{
	std::vector<std::vector<double>> coord(events_to_extract, std::vector<double>(2, 0));
	double costheta, phi;
	for (unsigned int evt = 0; evt < events_to_extract; ++evt)
	{
		evDist->GetRandom2(costheta, phi);
		phi += base_angle;

		auto local_lat = TMath::ACos(costheta) * TMath::Sin(phi);
		auto local_lon = TMath::ACos(costheta) * TMath::Cos(phi);

		coord[evt][1] = pointing[1] + local_lat;
		coord[evt][0] = pointing[0] + local_lon;

		while (coord[evt][1] > TMath::Pi())
			coord[evt][1] -= TMath::Pi();

		while (coord[evt][1] < 0)
			coord[evt][1] += TMath::Pi();

		while (coord[evt][0] > 2 * TMath::Pi())
			coord[evt][0] -= 2 * TMath::Pi();

		while (coord[evt][0] < 0)
			coord[evt][0] += 2 * TMath::Pi();
	}

	return coord;
}

void healpix_map::buildPointingMap(const std::vector<double> pointing)
{
	long hpix;
	ang2pix_ring(nside, pointing[1], pointing[0], &hpix);
	++iss_pointing[hpix];
}

void healpix_map::writePointingMap(
	const std::string mtitle,
	const char *coordsys)
{
	write_healpix_map(
		&(iss_pointing[0]),
		nside,
		mtitle.c_str(),
		0,
		coordsys);
}

void healpix_map::writeDataMaps(
	AnyOption &opt,
	const std::time_t ctime)
{
	// Sum data maps
	sumDataMaps();

	// Write sum maps
	for(auto it=pixel_dataMap_sum.begin(); it!=pixel_dataMap_sum.end(); ++it)
	{
		auto binIdx = std::distance(pixel_dataMap_sum.begin(), it);
		auto dataMap_title = 
		uniqueDataMapOutFile(
            opt, 
            ctime, 
            binIdx);
		writeDataMap(
			binIdx, 
			dataMap_title, 
			"G");
	}
}

void healpix_map::sumDataMaps()
{
	auto n_dataMaps = pixel_dataMap.size();
	pixel_dataMap_sum.resize(n_dataMaps);
	for (unsigned int idx_map=0; idx_map<n_dataMaps; ++idx_map)
    {
		auto dataMap_size = pixel_dataMap[idx_map].size();
        std::vector<float> tmp_sum_map (dataMap_size, 0);
        for (unsigned int idx_map_to_sum = idx_map; idx_map_to_sum<n_dataMaps; ++idx_map_to_sum)
            for(unsigned int single_map_idx=0; single_map_idx<dataMap_size; ++single_map_idx)
                tmp_sum_map[single_map_idx] += pixel_dataMap[idx_map_to_sum][single_map_idx];
        pixel_dataMap_sum[idx_map] = tmp_sum_map;
    }
}

void healpix_map::writeDataMap(
	const unsigned int map_idx,
	const std::string mtitle,
	const char *coordsys)
{
	if (map_idx >= 0 && map_idx < pixel_dataMap_sum.size())
		write_healpix_map(
			&(pixel_dataMap_sum[map_idx][0]),
			nside,
			mtitle.c_str(),
			0,
			coordsys);
}