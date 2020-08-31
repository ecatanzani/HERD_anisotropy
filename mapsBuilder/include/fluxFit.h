#ifndef FLUXFIT_H
#define FLUXFIT_H

#include <memory>

#include "TF1.h"
#include "TMultiGraph.h"

class fluxFit
{
    public:
        fluxFit() {};
        ~fluxFit() {};
        void extractFluxInfo(
            const std::string fluxPath,
            const bool verbose);
        std::shared_ptr<TMultiGraph> getFluxMG();
        std::shared_ptr<TF1> getFluxF();
        std::shared_ptr<TF1> getFluxF3();

    private:
        std::shared_ptr<TMultiGraph> mg;
        std::shared_ptr<TF1> fitFunc;
        std::shared_ptr<TF1> fitFuncE3;
};

#endif