#!/bin/env python

################################
# Job tag and output hadoop path
################################

# NOTE: If you want to resubmit the skimming job, you need to delete $ANALYSIS_BASE/tasks and hadoop_path output path

job_tag = "v1"
hadoop_path = "metis/babycms4/{}".format(job_tag) # The output goes to /hadoop/cms/store/user/$USER/"hadoop_path"

# Dataset names for CMS3 for Moriond 2017 datasets can be found here:
# http://www.t2.ucsd.edu/tastwiki/bin/view/CMS/Run2Samples25nsMoriond17
samples = [
    "/EWK_LNuJJ_MJJ-120_13TeV-madgraph-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM"                             ,
#    "/TTJets_SingleLeptFromT_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM"   ,
#    "/TTJets_SingleLeptFromTbar_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM",
    "/TEST-www/www-Private80X-v1/USER"                                                                                                                               ,
#    "/TEST-www/wwwext-Private80X-v1/USER"                                                                                                                            ,
]

###################################################################################################################
###################################################################################################################
# Below are the Metis submission code that users do not have to care about.
###################################################################################################################
###################################################################################################################

import time
import json
import metis

from time import sleep

from metis.Sample import DirectorySample
from metis.CondorTask import CondorTask

from metis.StatsParser import StatsParser

import sys
import os
import glob
import subprocess

# Will be using dis_client
import sys
sys.path.append('/home/users/namin/public_html/makers/disMaker')
import dis_client
import json

# file/dir paths
babycms4_dir = os.path.dirname(os.path.abspath(__file__))
metis_path = os.path.dirname(os.path.dirname(metis.__file__))
tar_path = os.path.join(metis_path, "package.tar")
tar_gz_path = tar_path + ".gz"
metis_dashboard_path = os.path.join(metis_path, "dashboard")
exec_path = os.path.join(babycms4_dir, "metis.sh")

# Create tarball
os.chdir(babycms4_dir)
os.system("tar -chzf {} setup.sh babycms4 */lib*.so CORE/data COREHelper/configs".format(tar_gz_path))

# Configurations
args = ""

# Change directory to metis
os.chdir(metis_path)

# Loop over datasets to submit
total_summary = {}
while True:

    all_tasks_complete = True

    for sample in samples:

        # Use DIS to parse hadoop path from MINIAOD sample name
        loc = ""
        result = dis_client.query(q=sample, typ="snt")
        status = result["response"]["status"]
        if status == "success":
            payloads = result["response"]["payload"]
            for payload in payloads:
                if payload["cms3tag"].find("CMS4") != -1: continue
                loc = payload["location"]

        # If the location is empty after querying via dis, alert the user and skip
        if loc == "":
            print ">>> [WARNING] DIS did not find hadoop path corresponding to the sample = {}".format(sample)
            continue

        # define the task
        task = CondorTask(
                sample               = DirectorySample(dataset=sample, location=loc),
                tag                  = job_tag,
                arguments            = args,
                executable           = exec_path,
                tarfile              = tar_gz_path,
                special_dir          = hadoop_path,
                output_name          = "merged.root",
                files_per_output     = 1,
                condor_submit_params = {"sites" : "UAF,T2_US_UCSD"},
                open_dataset         = False,
                flush                = True
                )

        # process the job (either submits, checks for resubmit, or finishes etc.)
        task.process()

        # save some information for the dashboard
        total_summary[sample] = task.get_task_summary()

        # Aggregate whether all tasks are complete
        all_tasks_complete = all_tasks_complete and task.complete()

    # parse the total summary and write out the dashboard
    StatsParser(data=total_summary, webdir=metis_dashboard_path).do()

    # Print msummary table so I don't have to load up website
    os.system("msummary | tee summary.txt")
    os.system("chmod -R 755 {}".format(metis_dashboard_path))

    # If all done exit the loop
    if all_tasks_complete:
        print ""
        print "Job={} finished".format(job_tag)
        print ""
        break

    # Neat trick to not exit the script for force updating
    print 'Press Ctrl-C to force update, otherwise will sleep for 30 seconds'
    try:
        for i in range(0,30):
            sleep(1) # could use a backward counter to be preeety :)
    except KeyboardInterrupt:
        raw_input("Press Enter to force update, or Ctrl-C to quit.")
        print "Force updating..."

#eof
