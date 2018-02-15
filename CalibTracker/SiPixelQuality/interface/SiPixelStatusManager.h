#ifndef SiPixelStatusManager_H
#define SiPixelStatusManager_H

/** \class SiPixelStatusManager
 *  
 *
 *  \author 
 */

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/LuminosityBlock.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "DataFormats/Provenance/interface/LuminosityBlockID.h"
#include <string>
#include <map>
#include <utility>

//Data format
#include "CalibTracker/SiPixelQuality/interface/SiPixelDetectorStatus.h"

class SiPixelStatusManager {

 public:
  SiPixelStatusManager        ();
  SiPixelStatusManager         (const edm::ParameterSet&, edm::ConsumesCollector&&);
  virtual ~SiPixelStatusManager();

  void reset();   
  void readLumi(const edm::LuminosityBlock&);   

  void createPayloads();   
  const std::map<edm::LuminosityBlockNumber_t,SiPixelDetectorStatus>& getPayloads(){return siPixelStatusMap_;} 

  typedef std::map<edm::LuminosityBlockNumber_t,SiPixelDetectorStatus>::iterator siPixelStatusMap_iterator;

 private:

  std::map<edm::LuminosityBlockNumber_t, SiPixelDetectorStatus> siPixelStatusMap_;

  std::string outputBase_;
  int nLumi_;
  std::string moduleName_;
  std::string label_;

  edm::InputTag beamSpotTag_;
  edm::EDGetTokenT<SiPixelDetectorStatus> siPixelStatusToken_;

};

#endif
