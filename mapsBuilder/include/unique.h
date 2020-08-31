#ifndef UNIQUE_H
#define UNIQUE_H

#include <string>
#include <iostream>
#include <sstream>

#include "TFile.h"

#include "anyoption.h"

extern const std::string uniquePointingMapOutFile(
    AnyOption &opt,
    const std::time_t ctime);

extern const std::string uniqueDataMapOutFile(
    AnyOption &opt,
    const std::time_t ctime,
    const int binIdx);

extern const std::string uniqueROOTOutFile(
    AnyOption &opt,
    const std::time_t ctime);

extern std::shared_ptr<TFile> getOutRootFile(
    const std::time_t ctime,
    std::string &outFilePath,
    AnyOption &opt);

#endif