# RecoParticleFlow  
For Single Pion sample Generation:

(For tcsh)  
```
setenv SCRAM_ARCH slc6_amd64_gcc630 </br>
```
(For bash)
```
export SCRAM_ARCH=slc6_amd64_gcc630
cmsrel CMSSW_10_0_2
cd CMSSW_10_0_2/src
cmsenv
git cms-init
git cms-addpkg RecoParticleFlow/PFClusterTools
git cms-merge-topic bkansal:PFHadCalib_NoEtaDep
scram b -j 40

```
