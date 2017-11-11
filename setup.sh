source /cvmfs/cms.cern.ch/cmsset_default.sh > /dev/null 2>&1
export SCRAM_ARCH=slc6_amd64_gcc530
export CMSSW_VERSION=CMSSW_9_2_0
echo Setting up CMSSW for CMSSW_9_2_0 for slc6_amd64_gcc530
cd /cvmfs/cms.cern.ch/slc6_amd64_gcc530/cms/cmssw/CMSSW_9_2_0/src
eval `scramv1 runtime -sh`
cd -

for PACKAGE in $(ls -d -- *); do
    LD_LIBRARY_PATH=$PWD/$PACKAGE:${LD_LIBRARY_PATH}
done

