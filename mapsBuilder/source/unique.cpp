#include "unique.h"

const std::string uniquePointingMapOutFile(
    AnyOption &opt,
    const std::time_t ctime)
{
    std::stringstream fPath;
    if (opt.getValue("outputDir") || opt.getValue('d'))
        fPath << opt.getValue('d') << "/mapsOutFile_isspointing_" << ctime << ".fits";
    else if (opt.getValue("output") || opt.getValue('o'))
        fPath << opt.getValue('o');
    else
        fPath << "mapsOutFile_isspointing_" << ctime << ".fits";
    return fPath.str();
}

const std::string uniqueDataMapOutFile(
    AnyOption &opt,
    const std::time_t ctime,
    const int binIdx)
{
    std::stringstream fPath;
    if (opt.getValue("outputDir") || opt.getValue('d'))
        fPath << opt.getValue('d') << "/mapsOutFile_energyBin_" << binIdx << "_" << ctime << ".fits";
    else if (opt.getValue("output") || opt.getValue('o'))
        fPath << opt.getValue('o');
    else
        fPath << "mapsOutFile_energyBin_" << binIdx << "_" << ctime << ".fits";

    return fPath.str();
}

const std::string uniqueROOTOutFile(
    AnyOption &opt,
    const std::time_t ctime)
{
    std::stringstream fPath;
    if (opt.getValue("outputDir") || opt.getValue('d'))
        fPath << opt.getValue('d') << "/mapsOutFile_" << ctime << ".root";
    else if (opt.getValue("output") || opt.getValue('o'))
        fPath << opt.getValue('o');
    else
        fPath << "mapsOutFile_" << ctime << ".root";

    return fPath.str();
}

std::shared_ptr<TFile> getOutRootFile(
    const std::time_t ctime,
    std::string &outFilePath,
    AnyOption &opt)
{
    outFilePath = uniqueROOTOutFile(opt, ctime);
    std::shared_ptr<TFile> outFile = std::make_shared<TFile>(outFilePath.c_str(), "RECREATE");
    if (outFile->IsZombie())
    {
        std::cerr << "\n\nError writing ROOT output file [" << outFilePath << "]\n\n";
        exit(100);
    }
    else
        return outFile;
}