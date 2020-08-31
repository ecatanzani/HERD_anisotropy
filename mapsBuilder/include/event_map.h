#ifndef EVENT_MAP_H
#define EVENT_MAP_H

#include <memory>
#include <ctime>

#include "TF1.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TTree.h"

#include "anyoption.h"

extern void evaluateEventMap(
	const std::shared_ptr<TF1> fitFunc,
	const std::shared_ptr<TH1D> acceptance,
	const std::shared_ptr<TH1D> live_time,
	const std::shared_ptr<TTree> RTItree,
    const std::vector<std::shared_ptr<TH2D>> h_event_distribution,
	AnyOption &opt,
	const std::time_t ctime,
	const bool verbose);

#endif