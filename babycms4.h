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
        CORE2016 core;
        TChain* chain;
        int n_evt_to_process;
        TString core_opt_str;
        TString output_file_name;

        // Functions
        void run();
};

#endif
//eof
