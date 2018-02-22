#ifndef CalibTracker_SiPixelQuality_SiPixelStatusProducer_h
#define CalibTracker_SiPixelQuality_SiPixelStatusProducer_h

/**_________________________________________________________________
   class:   SiPixelStatusProducer.h
   package: CalibTracker/SiPixelQuality
   
________________________________________________________________**/


// C++ standard
#include <string>
// CMS
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

struct triplet{
  int roc, row, column;
};

class SiPixelStatusProducer : public edm::one::EDProducer<edm::EndLuminosityBlockProducer,
                                                         edm::one::WatchLuminosityBlocks> {
 public:
  explicit SiPixelStatusProducer(const edm::ParameterSet&);
  ~SiPixelStatusProducer() override;

 private:
  void beginLuminosityBlock     (edm::LuminosityBlock const& lumiSeg, const edm::EventSetup& iSetup) final;
  void endLuminosityBlock       (edm::LuminosityBlock const& lumiSeg, const edm::EventSetup& iSetup) final;
  void endLuminosityBlockProduce(edm::LuminosityBlock& lumiSeg, const edm::EventSetup& iSetup) final;
  void produce                  (edm::Event& iEvent, const edm::EventSetup& iSetup) final;
  
  virtual void onlineRocColRow(const DetId &detId, int offlineRow, int offlineCol, int &roc, int &row, int &col);

  // time granularity control
  int ftotalevents;
  int resetNLumi_;
  int countLumi_;      //counter

  int beginLumi_;
  int endLumi_;
  int beginRun_;
  int endRun_;

  std::time_t refTime_[2];

  // condition watchers
  // CablingMaps
  edm::ESWatcher<SiPixelFedCablingMapRcd> siPixelFedCablingMapWatcher_;
  edm::ESHandle<SiPixelFedCablingMap> fCablingMap;
  const SiPixelFedCablingMap* fCablingMap_;  

  // TrackerDIGIGeo
  edm::ESWatcher<TrackerDigiGeometryRecord> trackerDIGIGeoWatcher_;
  edm::ESHandle<TrackerGeometry> fTG;
  // TrackerTopology 
  edm::ESWatcher<TrackerTopologyRcd> trackerTopoWatcher_;

  // SiPixel offline<->online conversion
  // -- map (for each detid) of the map from offline col/row to the online roc/col/row
  bool monitorOnDoubleColumn_; // whether to use CablingMap to get the roc/ pixel local coordinate
  std::map<int, std::map<std::pair<int, int>, triplet> >  fIndices;
  SiPixelCoordinates coord_;

  // Producer inputs / controls
  int                                                     fVerbose;
  std::string                                             fFileName;
  edm::InputTag                                           fPixelClusterLabel_;
  edm::EDGetTokenT<edmNew::DetSetVector<SiPixelCluster>>  fSiPixelClusterToken_;
  std::vector<edm::EDGetTokenT<PixelFEDChannelCollection> > theBadPixelFEDChannelsTokens_;

  // Producer production (output collection)
  SiPixelDetectorStatus                                    fDet;

};

#endif
