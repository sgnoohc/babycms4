//  .
// ..: P. Chang, philip@physics.ucsd.edu

#ifndef babycms4_h
#define babycms4_h

#include <cstdlib>

// CORE
#include "CORE/CMS3.h"

// COREHelper
#include "COREHelper/corehelper.h"

// cxxopts
#include "cxxopts/include/cxxopts.hpp"

// RooUtil
#include "rooutil/looper.h"
#include "rooutil/stringutil.h"
#include "rooutil/fileutil.h"

class BabyCMS4
{
    public:
        // Members
        CORE2016 core2016;
        TChain* chain;
        int n_evt_to_process;

        // Functions
        BabyCMS4();
        ~BabyCMS4();
        void run();
};

#endif
//eof
