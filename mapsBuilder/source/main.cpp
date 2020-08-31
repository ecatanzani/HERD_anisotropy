#include "main.h"
#include "event_map.h"
#include "assets.h"
#include "unique.h"

int main(int argc, char **argv)
{
	AnyOption opt;

	opt.addUsage("Usage: ");
	opt.addUsage("");
	opt.addUsage(" -h  --help                                                   Prints this help");
	opt.addUsage(" -a  --acceptance     <path_to_acceptance_TFile>          (*) Input acceptance TFile");
	opt.addUsage(" -e  --event          <path_to_event_dist_TFile>          (*) Input angular event distribution TFile");
	opt.addUsage(" -f  --flux           <path_to_flux_TFile>                (*) Input flux TFile");
	opt.addUsage(" -t  --telemetry      <path_to_telemetry_TFile>           (*) Input telemetry TFile");
	opt.addUsage(" -r  --rate           <path_to_acq_rate_TFile>            (*) Input DAMPE acquisition rate TFile");
	opt.addUsage(" -o  --output         <path_to_output_TFile>                  Output ROOT TFile");
	opt.addUsage(" -d  --outputDir      <path_to_output_TFile_dir>              Output ROOT TFile directory");
	opt.addUsage(" -v  --verbose                                                Verbose output");
	opt.addUsage("");

	opt.setFlag("help", 'h');
	opt.setOption("acceptance", 'a');
	opt.setOption("event", 'e');
	opt.setOption("flux", 'f');
	opt.setOption("telemetry", 't');
	opt.setOption("rate", 'r');
	opt.setOption("output", 'o');
	opt.setOption("outputDir", 'd');
	opt.setFlag("verbose", 'v');

	opt.processCommandArgs(argc, argv);

	std::string accPath;
	std::string evtPath;
	std::string fluxPath;
	std::string telPath;
	std::string acqPath;
	std::string outFilePath;
	bool verbose = false;
	std::time_t ctime = std::time(0);

	if (!opt.hasOptions())
		opt.printUsage();

	if (opt.getFlag("help") || opt.getFlag('h'))
	{
		opt.printUsage();
		return 0;
	}
	if (opt.getValue("acceptance") || opt.getValue('a'))
		accPath = opt.getValue('a');
	if (opt.getValue("event") || opt.getValue('e'))
		evtPath = opt.getValue('e');
	if (opt.getValue("flux") || opt.getValue('f'))
		fluxPath = opt.getValue('f');
	if (opt.getValue("telemetry") || opt.getValue('t'))
		telPath = opt.getValue('t');
	if (opt.getValue("rate") || opt.getValue('r'))
		acqPath = opt.getValue('r');
	if (opt.getFlag("verbose") || opt.getFlag('v'))
		verbose = opt.getFlag('v');

	auto outRootFile =
		getOutRootFile(
			ctime,
			outFilePath,
			opt);
	assets deps;
	deps.computeAssets(
		outRootFile,
		outFilePath,
		fluxPath,
		accPath,
		acqPath,
		evtPath,
		telPath,
		verbose);

	evaluateEventMap(
		deps.getFluxF(),
		deps.getAcceptance(),
		deps.getLiveTime(),
		deps.getRTItree(),
		deps.getEveDistributionVector(),
		opt,
		ctime,
		verbose);

	return 0;
}