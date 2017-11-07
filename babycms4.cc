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
            ("n,nevt", "Number of events to process.", cxxopts::value<int>()->default_value("-1"), "NEVENTS")
            ("treename", "TTree name in each root file.", cxxopts::value<std::string>()->default_value("Events"), "TREENAME")
            ("help", "Print help")
            ;

        options.parse(argc, argv);

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
BabyCMS4::BabyCMS4()
{
}

//_________________________________________________________________________________________________
BabyCMS4::~BabyCMS4()
{
}

//_________________________________________________________________________________________________
void BabyCMS4::run()
{
    RooUtil::Looper<CMS3> looper(chain, &cms3, n_evt_to_process);
    while (looper.nextEvent())
    {
    }
}

//eof
