# PFCalibration  
For Single Pion sample Generation:

(For tcsh)  
```
setenv SCRAM_ARCH slc6_amd64_gcc530 </br>
```
(For bash)
```
export SCRAM_ARCH=slc6_amd64_gcc530
cmsrel CMSSW_10_0_2
cd CMSSW_10_0_2/src
git clone -b PFHadCalib_NoEtaDep git@github.com:bkansal/cmssw.git
scram b -j 40
cd PFCalibration/PFChargedHadronAnalyzer/test/
cmsenv
cmsRun PGUnWithGeneration.py
```
