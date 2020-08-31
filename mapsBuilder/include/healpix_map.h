#ifndef HEALPIX_MAP_H
#define HEALPIX_MAP_H

#include <vector>
#include <ctime>

#include "TRandom3.h"
#include "TF1.h"
#include "TH1D.h"
#include "TH2D.h"

#include "anyoption.h"

class healpix_map
{
public:
	healpix_map(long side = 32, UInt_t seed = 2);
	~healpix_map(){};
	void initDataMap(int energy_bins);
	void buildDataMap(
		const std::vector<std::shared_ptr<TH2D>> h_event_distribution,
		const std::vector<double> old_pointing,
		const std::vector<double> pointing,
		const double geo_lat,
		const std::shared_ptr<TF1> fitFunc,
		const std::shared_ptr<TH1D> acceptance,
		const std::shared_ptr<TH1D> live_time);
	void buildPointingMap(const std::vector<double> poiunting);
	void writePointingMap(
		const std::string mtitle,
		const char *coordsys);
	void writeDataMaps(
		AnyOption &opt,
		const std::time_t ctime);

private:
	unsigned int GetNumberToExtract(
		const std::shared_ptr<TF1> fitFunc,
		const std::shared_ptr<TH1D> acceptance,
		const std::shared_ptr<TH1D> live_time,
		const int binIdx,
		const double geo_lat);
	std::vector<std::vector<double>> getEvtsCoordinate(
		const unsigned int events_to_extract,
		const double base_angle,
		const std::shared_ptr<TH2D> evDist,
		const std::vector<double> pointing);
	void sumDataMaps();
	void writeDataMap(
		const unsigned int map_idx,
		const std::string mtitle,
		const char *coordsys);

	std::vector<std::vector<float>> pixel_dataMap;
	std::vector<std::vector<float>> pixel_dataMap_sum;
	std::vector<float> iss_pointing;
	long nside;
	long npix;
	UInt_t rand_seed;
	std::shared_ptr<TRandom3> rgen;
};

#endif