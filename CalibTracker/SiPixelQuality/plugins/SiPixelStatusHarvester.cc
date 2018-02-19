
/*
 *  See header file for a description of this class.
 *
 *  author
 */

// CMSSW FW
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/LuminosityBlock.h"
#include "FWCore/Framework/interface/FileBlock.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

// Condition Format
#include "CondFormats/SiPixelObjects/interface/SiPixelQuality.h"
#include "CondFormats/DataRecord/interface/SiPixelQualityFromDbRcd.h"
// CondOutput
#include "CondCore/DBOutputService/interface/PoolDBOutputService.h"

// Dataformat of SiPixel status in ALCAPROMPT data
#include "CalibTracker/SiPixelQuality/interface/SiPixelDetectorStatus.h"
//#include "CondCore/Utilities/bin/cmscond_export_iov.cpp"
//#include "CondCore/Utilities/interface/Utilities.h"

// harvest helper class
#include "CalibTracker/SiPixelQuality/interface/SiPixelStatusManager.h"
// header file
#include "CalibTracker/SiPixelQuality/plugins/SiPixelStatusHarvester.h"

#include <iostream> 
#include <cstring>

using namespace edm;

//--------------------------------------------------------------------------------------------------
SiPixelStatusHarvester::SiPixelStatusHarvester(const edm::ParameterSet& iConfig) :
  outputBase_    (iConfig.getParameter<ParameterSet>("SiPixelStatusHarvesterParameters").getUntrackedParameter<std::string>("outputBase")),
  recordName_      (iConfig.getParameter<ParameterSet>("SiPixelStatusHarvesterParameters").getUntrackedParameter<std::string>("recordName", "SiPixelQualityFromDbRcd")),
  dumpTxt_               (iConfig.getParameter<ParameterSet>("SiPixelStatusHarvesterParameters").getUntrackedParameter<bool>("dumpTxt")),
  outTxtFileName_        (iConfig.getParameter<ParameterSet>("SiPixelStatusHarvesterParameters").getUntrackedParameter<std::string>("txtFileName")),
  siPixelStatusManager_(iConfig, consumesCollector()) {  

}

//--------------------------------------------------------------------------------------------------
SiPixelStatusHarvester::~SiPixelStatusHarvester(){}

//--------------------------------------------------------------------------------------------------
void SiPixelStatusHarvester::beginJob() {}

//--------------------------------------------------------------------------------------------------
void SiPixelStatusHarvester::endJob() {}  

//--------------------------------------------------------------------------------------------------
void SiPixelStatusHarvester::analyze(const edm::Event& iEvent, const edm::EventSetup&) {
}

//--------------------------------------------------------------------------------------------------
void SiPixelStatusHarvester::beginRun(const edm::Run&, const edm::EventSetup&){
  siPixelStatusManager_.reset();
}

//--------------------------------------------------------------------------------------------------
void SiPixelStatusHarvester::endRun(const edm::Run& iRun, const edm::EventSetup&){

  siPixelStatusManager_.createPayloads();
  std::map<edm::LuminosityBlockNumber_t,SiPixelDetectorStatus> siPixelStatusMap = siPixelStatusManager_.getPayloads();

  edm::Service<cond::service::PoolDBOutputService> poolDbService;


  if(poolDbService.isAvailable() ) {

    // also means looping over IOV of the payloads
    for(SiPixelStatusManager::siPixelStatusMap_iterator it=siPixelStatusMap.begin(); it!=siPixelStatusMap.end();it++){

          SiPixelDetectorStatus tmpSiPixelStatus = it->second;
          // drop the IOV if the statistics is too low
          //if(tmpSiPixelStatus.perRocOccupancy()<1.e3) continue;

          std::string outTxt = Form("%s_Run%d_Lumi%d_SiPixelStatus.txt", outTxtFileName_.c_str(), iRun.id().run(),it->first);
          std::ofstream outFile;
          outFile.open(outTxt.c_str());

          cond::Time_t thisIOV = 1;

          // run based
          if (outputBase_ == "runbased" ) {
               thisIOV = (cond::Time_t) iRun.id().run();
          }
          else if (outputBase_ == "nLumibased" || outputBase_ == "dynamicLumibased" ) {
	     edm::LuminosityBlockID lu(iRun.id().run(),it->first);
	     thisIOV = (cond::Time_t)(lu.value()); 
          }

          // create the DB object
          SiPixelQuality *siPixelQuality = new SiPixelQuality();
          
          int DetAverage = tmpSiPixelStatus.perRocOccupancy();
          //int DetSigma = tmpSiPixelStatus.perRocOccupancyVariance();
          std::map<int, SiPixelModuleStatus>::iterator itModEnd = tmpSiPixelStatus.end();
          for (std::map<int, SiPixelModuleStatus>::iterator itMod = tmpSiPixelStatus.begin(); itMod != itModEnd; ++itMod) {
    
               // create the bad module list
               SiPixelQuality::disabledModuleType BadModule;

               int detid = itMod->first;
               BadModule.DetID = uint32_t(detid);
               BadModule.errorType = 3;

               BadModule.BadRocs = 0; 
               std::vector<uint32_t> BadRocList;
               SiPixelModuleStatus modStatus = itMod->second;
               for (int iroc = 0; iroc < modStatus.nrocs(); ++iroc) {
                   int rocOccupancy = modStatus.statusROC(iroc);
                   if(rocOccupancy<1.e-4*DetAverage){
                     BadRocList.push_back(uint32_t(iroc));
                   }
               }

               short badrocs = 0;
               for(std::vector<uint32_t>::iterator iter = BadRocList.begin(); iter != BadRocList.end(); ++iter){
                   badrocs +=  1 << *iter; // 1 << *iter = 2^{*iter} using bitwise shift 
               } 
               BadModule.BadRocs = badrocs;

               siPixelQuality->addDisabledModule(BadModule);

         } // end module loop
          
         if (poolDbService->isNewTagRequest(recordName_) ) {
             edm::LogInfo("SiPixelStatusHarvester")
                 << "new tag requested" << std::endl;
	     poolDbService->writeOne<SiPixelQuality>(siPixelQuality, thisIOV, recordName_);
         } 
         else {
            edm::LogInfo("SiPixelStatusHarvester")
               << "no new tag requested, appending IOV" << std::endl;
	    poolDbService->writeOne<SiPixelQuality>(siPixelQuality, thisIOV, recordName_);
         }

         if (dumpTxt_){ 
            tmpSiPixelStatus.dumpToFile(outTxt); 
            outFile.close();
         }

     }// loop over IOV-structured Map (payloads)

  } // if DB serverice is available


}

//--------------------------------------------------------------------------------------------------
void SiPixelStatusHarvester::beginLuminosityBlock(const edm::LuminosityBlock&, const edm::EventSetup&) {}

//--------------------------------------------------------------------------------------------------
void SiPixelStatusHarvester::endLuminosityBlock(const edm::LuminosityBlock& iLumi, const edm::EventSetup&) {

  siPixelStatusManager_.readLumi(iLumi);

}


DEFINE_FWK_MODULE(SiPixelStatusHarvester);
