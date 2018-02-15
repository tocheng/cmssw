/** \class SiPixelStatusManager
 *  helper class that set up IOV strcutre of SiPixelDetectorStatus
 *
 *  \author 
 */

#include "CalibTracker/SiPixelQuality/interface/SiPixelStatusManager.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include <vector>
#include <cmath>
#include <climits>

using namespace edm;
using namespace std;

//--------------------------------------------------------------------------------------------------
SiPixelStatusManager::SiPixelStatusManager(){
}

//--------------------------------------------------------------------------------------------------
SiPixelStatusManager::SiPixelStatusManager(const ParameterSet& iConfig, edm::ConsumesCollector&& iC) :
  outputBase_(iConfig.getParameter<ParameterSet>("SiPixelStatusHarvesterParameters").getUntrackedParameter<std::string>("OutputBase")),
  nLumi_(iConfig.getParameter<edm::ParameterSet>("SiPixelStatusHarvesterParameters").getUntrackedParameter<int>("ResetEveryNLumi")),
  moduleName_(iConfig.getParameter<ParameterSet>("SiPixelStatusHarvesterParameters").getUntrackedParameter<std::string>("ModuleName")),
  label_     (iConfig.getParameter<ParameterSet>("SiPixelStatusHarvesterParameters").getUntrackedParameter<std::string>("Label")){

  edm::InputTag siPixelStatusTag_(moduleName_, label_);
  siPixelStatusToken_ = iC.consumes<SiPixelDetectorStatus,edm::InLumi>(siPixelStatusTag_);

  LogInfo("SiPixelStatusManager") 
    << "Output base: " << outputBase_ 
    << std::endl;
  reset();
}

//--------------------------------------------------------------------------------------------------
SiPixelStatusManager::~SiPixelStatusManager(){
}

//--------------------------------------------------------------------------------------------------
void SiPixelStatusManager::reset(){
     siPixelStatusMap_.clear();
}

//--------------------------------------------------------------------------------------------------
void SiPixelStatusManager::readLumi(const LuminosityBlock& iLumi){

  edm::Handle<SiPixelDetectorStatus> siPixelStatusHandle;
  iLumi.getByToken(siPixelStatusToken_, siPixelStatusHandle);

  if(siPixelStatusHandle.isValid()) { // check the product

    siPixelStatusMap_[iLumi.luminosityBlock()] = *siPixelStatusHandle;
  }
  else {
    LogInfo("SiPixelStatusManager")
        << "Lumi: " << iLumi.luminosityBlock() << std::endl;
    LogInfo("SiPixelStatusManager")
        << " SiPixelDetectorStatus is not valid!" << std::endl;
  }

}

//--------------------------------------------------------------------------------------------------
void SiPixelStatusManager::createPayloads(){

  if(siPixelStatusMap_.size() <= 1){
    return;
  }

  siPixelStatusMap_iterator firstStatus     = siPixelStatusMap_.begin();
  siPixelStatusMap_iterator lastStatus     = siPixelStatusMap_.end();

  if(outputBase_ == "nLumibased" && nLumi_>1){

    std::map<LuminosityBlockNumber_t,SiPixelDetectorStatus> tmpSiPixelStatusMap_;

    unsigned int iterationLumi = 0;
    unsigned int iterationIOV = 0;
    unsigned int nIOV = siPixelStatusMap_.size()/nLumi_;

    LuminosityBlockNumber_t firstLumi;
    SiPixelDetectorStatus tmpSiPixelStatus;
    for (siPixelStatusMap_iterator it = firstStatus; it != lastStatus; it++) {
  
        if(iterationLumi%nLumi_==0 and iterationIOV<nIOV){
           // this is the begining of an IOV 
           firstLumi = it->first;
           tmpSiPixelStatus = it->second; 
           iterationIOV = iterationIOV + 1;
	}
        else{
           // keep update detector status up to nLumi_ lumi sections
           tmpSiPixelStatus.updateDetectorStatus(it->second); 
           if((nLumi_-iterationLumi%nLumi_)==1 || (++it)==lastStatus) // last lumi-section in the IOV
           {
             // fill it into a new map (with IOV structured)
             tmpSiPixelStatusMap_[firstLumi] = tmpSiPixelStatus;
           }
        }

        iterationLumi=iterationLumi+1;
    }

    siPixelStatusMap_.clear();
    siPixelStatusMap_ = tmpSiPixelStatusMap_;

  }
  else if(outputBase_ == "dynamicLumibased"){

    std::map<LuminosityBlockNumber_t,SiPixelDetectorStatus> tmpSiPixelStatusMap_;

    siPixelStatusMap_iterator secondStatus = ++siPixelStatusMap_.begin();

    // initialize the first IOV and SiPixelDetector status (in the first IOV)
    LuminosityBlockNumber_t firstLumi = firstStatus->first;
    SiPixelDetectorStatus tmpSiPixelStatus = firstStatus->second;

    for (siPixelStatusMap_iterator it = secondStatus; it != lastStatus; it++) {

        // keep adding data until the average occupancy is high enough
        if(tmpSiPixelStatus.perRocOccupancy()<1.e3){
             tmpSiPixelStatus.updateDetectorStatus(it->second);
        }
        else{
              
          tmpSiPixelStatusMap_[firstLumi]=tmpSiPixelStatus;
          if((++it)!= lastStatus){
             firstLumi = (++it)->first;
             tmpSiPixelStatus = (++it)->second;   
          }

        }

    } // end of siPixelStatusMap

  }
  else if(outputBase_ == "runbased" || ( (int(siPixelStatusMap_.size()) <= nLumi_ && outputBase_ == "nLumibased")) ){

    LuminosityBlockNumber_t firstLumi = siPixelStatusMap_.begin()->first;
    SiPixelDetectorStatus tmpSiPixelStatus = siPixelStatusMap_.begin()->second;
    
    siPixelStatusMap_iterator nextStatus = ++siPixelStatusMap_.begin();
    for (siPixelStatusMap_iterator it = nextStatus; it != lastStatus; it++) {
          tmpSiPixelStatus.updateDetectorStatus(it->second);
    }

    siPixelStatusMap_.clear();
    siPixelStatusMap_[firstLumi] = tmpSiPixelStatus;

  }
  else{
    LogInfo("SiPixelStatusManager")
      << "Unrecognized payload outputBase parameter: " << outputBase_
      << endl;    
  }

}


