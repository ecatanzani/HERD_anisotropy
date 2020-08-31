#ifndef ASSETS_H
#define ASSETS_H

#include <string>
#include <vector>
#include <memory>

#include "TF1.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"
#include "TTree.h"
#include "TMultiGraph.h"

class assets
{
public:
	assets(){};
	~assets(){};
	void computeAssets(
		std::shared_ptr<TFile> outRootFile,
		const std::string outFilePath,
		const std::string fluxPath,
		const std::string accPath,
		const std::string acqPath,
		const std::string evtPath,
		const std::string telPath,
		const bool verbose);
	std::shared_ptr<TMultiGraph> getFluxMG();
	std::shared_ptr<TF1> getFluxF();
	std::shared_ptr<TF1> getFluxF3();
	std::shared_ptr<TH1D> getAcceptance();
	std::shared_ptr<TH2D> getDampeAcqRate();
	std::shared_ptr<TH1D> getLiveTime();
	std::shared_ptr<TTree> getRTItree();
	std::vector<std::shared_ptr<TH2D>>getEveDistributionVector();

private:
	void extractEvtDistributionInfo(
		const std::string evtPath,
		const bool verbose);
	void extractRTItreeInfo(
		const std::string telPath,
		const bool verbose);

	std::shared_ptr<TMultiGraph> mg;
	std::shared_ptr<TF1> fitFunc;
	std::shared_ptr<TF1> fitFuncE3;
	std::shared_ptr<TH1D> acceptance;
	std::shared_ptr<TH2D> dampe_acq_rate;
	std::shared_ptr<TH1D> live_time;
	std::shared_ptr<TTree> RTItree;
	std::vector<std::shared_ptr<TH2D>> h_event_distribution;
};

#endif