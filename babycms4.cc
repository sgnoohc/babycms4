//  .
// ..: P. Chang, philip@physics.ucsd.edu

#include "babycms4.h"

int main(int argc, char* argv[])
{

    //---------------------------------------------------------------------------------------------
    // Parsing options to the program
    //---------------------------------------------------------------------------------------------
    try
    {
        cxxopts::Options options(argv[0], "Baby Maker from CMS4");

        options.positional_help("[optional args]");

        options.add_options()
            ("i,input", "Comma separated Input CMS4 root path. Wildcard supported only at the most lower level directory. (e.g. '/path/to/my/ttbar/merged_*.root,/path/to/my/dy/merged_*.root')", cxxopts::value<std::string>(), "file1.root,file2.root")
            ("o,output", "Output ROOT file path.", cxxopts::value<std::string>()->default_value("output.root"), "OUTPUT")
            ("p,option", "Option string to be passed on to COREHelper.", cxxopts::value<std::string>()->default_value("applyJEC"), "OPTION")
            ("n,nevt", "Number of events to process.", cxxopts::value<int>()->default_value("-1"), "NEVENTS")
            ("treename", "TTree name in each root file.", cxxopts::value<std::string>()->default_value("Events"), "TREENAME")
            ("h,help", "Print help")
            ;

        options.parse(argc, argv);

        // Print help message if --help or -h is provided
        if (options.count("help"))
        {
            RooUtil::print(options.help());
            exit(0);
        }

        // Required options
        if (!options.count("input"))
        {
            RooUtil::error("option --input missing");
            exit(1);
        }

        //---------------------------------------------------------------------------------------------
        // Configuring the BabyCMS4 instance
        //---------------------------------------------------------------------------------------------
        BabyCMS4 babycms4;

        // Load all the ttrees from all the input files to the TChain
        babycms4.chain = RooUtil::FileUtil::createTChain(
                options["treename"].as<std::string>().c_str() ,
                options["input"]   .as<std::string>().c_str()
                );

        // Set the total number of events to process
        babycms4.n_evt_to_process = options["nevt"].as<int>();

        // Set the option string to be passed to the COREHelper.
        // The hadoop directory path contains the CMS4 sample name.
        // To first order the CMS4 sample name is enough to configure what we need to do.
        babycms4.core_opt_str = options["input"].as<std::string>().c_str();
        babycms4.core_opt_str += options["option"].as<std::string>().c_str();

        // Set the output root file
        babycms4.output_file_name = options["output"].as<std::string>().c_str();

        //---------------------------------------------------------------------------------------------
        // Run the code
        //---------------------------------------------------------------------------------------------
        babycms4.run();

    }
    catch (const cxxopts::OptionException& e)
    {
        std::cout << "error parsing options: " << e.what() << std::endl;
        exit(1);
    }


    return 0;
}

//_________________________________________________________________________________________________
void BabyCMS4::run()
{
    // Load and set all the configurational stuff. (e.g. JES, TMVA, GRL, and etc.)
    core.initializeCORE(core_opt_str);

    // Set up the looper.
    RooUtil::Looper<CMS3> looper(chain, &cms3, n_evt_to_process);

    // Set up the output.
    TFile* ofile = new TFile(output_file_name, "recreate");
    TTreeX* ttree = new TTreeX("t", "Baby Ntuple");

    // create std branches
    core.createStdBranches(ttree);

    // Loop!
    while (looper.nextEvent())
    {
        // Reset all the branch values.
        core.setStdBranches(ttree);

        if (core.nCount(ttree, "lep_pass_VVV_cutbased_fo_noiso_noip") < 1)
            continue;

        ttree->fill();
    }

    // Save the output.
    ofile->cd();
    ttree->write();
    ofile->Close();
}

//eof
