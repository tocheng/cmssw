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
  int roc, col, row;
};

class SiPixelStatusProducer : public edm::one::EDProducer<edm::EndLuminosityBlockProducer,
                                                         edm::one::WatchLuminosityBlocks> {
 public:
  explicit SiPixelStatusProducer(const edm::ParameterSet&);
  ~SiPixelStatusProducer() override;

 private:
  void beginJob() final;
  void beginLuminosityBlock     (edm::LuminosityBlock const& lumiSeg, const edm::EventSetup& iSetup) final;
  void endLuminosityBlock       (edm::LuminosityBlock const& lumiSeg, const edm::EventSetup& iSetup) final;
  void endLuminosityBlockProduce(edm::LuminosityBlock& lumiSeg, const edm::EventSetup& iSetup) final;
  void endJob() final;
  void produce                  (edm::Event& iEvent, const edm::EventSetup& iSetup) final;
  
  virtual void onlineRocColRow(const DetId &pID, int offlineRow, int offlineCol, int &roc, int &col, int &row);

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
  edm::ESHandle<SiPixelFedCablingMap>                     fCablingMap;
  int MINFEDID, MAXFEDID;

  // TrackerDIGIGeo
  edm::ESWatcher<TrackerDigiGeometryRecord> trackerDIGIGeoWatcher_;
  edm::ESHandle<TrackerGeometry>                          fTG;
 
  // SiPixel offline<->online conversion
  // -- map (for each detid) of the map from offline col/row to the online roc/col/row
  std::map<int, std::map<std::pair<int, int>, triplet> >  fIndices;
  SiPixelCoordinates coord_;
  std::map<int, int>                                      fFEDID;
  SiPixelFrameConverter                                  *fPFC[1350];

  // Producer inputs / controls
  int                                                     fVerbose;
  std::string                                             fFileName;
  edm::InputTag                                           fPixelClusterLabel_;
  edm::EDGetTokenT<edmNew::DetSetVector<SiPixelCluster>>  fSiPixelClusterToken_;

  // Producer production (output collection)
  SiPixelDetectorStatus                                    fDet;

};

#endif
