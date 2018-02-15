/**_________________________________________________________________
   class:   SiPixelStatusProducer.cc
   package: CalibTracker/SiPixelQuality/SiPixelStatusProducer

   author:

   ________________________________________________________________**/


// C++ standard
#include <string>
// ROOT
#include "TMath.h"

// CMSSW FW
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/LuminosityBlock.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/ESWatcher.h"

// CMSSW DataFormats
#include "DataFormats/Common/interface/ConditionsInEdm.h"
#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/FEDRawData/interface/FEDNumbering.h"
#include "DataFormats/SiPixelCluster/interface/SiPixelCluster.h"
#include "DataFormats/SiPixelDetId/interface/PXBDetId.h"
#include "DataFormats/SiPixelDetId/interface/PXFDetId.h"
#include "DataFormats/SiPixelDetId/interface/PixelBarrelName.h"
#include "DataFormats/SiPixelDetId/interface/PixelEndcapName.h"
#include "DataFormats/SiPixelDigi/interface/PixelDigi.h"

// CMSSW CondFormats
#include "CondFormats/RunInfo/interface/RunSummary.h"
#include "CondFormats/RunInfo/interface/RunInfo.h"
#include "CondFormats/DataRecord/interface/RunSummaryRcd.h"
#include "CondFormats/DataRecord/interface/SiPixelFedCablingMapRcd.h"
#include "CondFormats/SiPixelObjects/interface/SiPixelFrameConverter.h"
#include "CondFormats/SiPixelObjects/interface/SiPixelFedCablingMap.h"
#include "Geometry/TrackerGeometryBuilder/interface/PixelGeomDetUnit.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"

// Producer related dataformat
#include "DQM/SiPixelPhase1Common/interface/SiPixelCoordinates.h"
#include "CalibTracker/SiPixelQuality/interface/SiPixelDetectorStatus.h"

// header file
#include "CalibTracker/SiPixelQuality/plugins/SiPixelStatusProducer.h"

using namespace std;

//--------------------------------------------------------------------------------------------------
SiPixelStatusProducer::SiPixelStatusProducer(const edm::ParameterSet& iConfig){
  // get parameter
  fPixelClusterLabel_   = iConfig.getParameter<edm::ParameterSet>("SiPixelStatusProducerParameters").getUntrackedParameter<edm::InputTag>("pixelClusterLabel");
  fSiPixelClusterToken_ = consumes<edmNew::DetSetVector<SiPixelCluster>>(fPixelClusterLabel_);
  resetNLumi_   = iConfig.getParameter<edm::ParameterSet>("SiPixelStatusProducerParameters").getUntrackedParameter<int>("resetEveryNLumi",-1);

  ftotalevents = 0;
  countLumi_ = 0;

  beginLumi_ = endLumi_ = -1;
  endLumi_ = endRun_ = -1;
  
  produces<SiPixelDetectorStatus, edm::Transition::EndLuminosityBlock>("siPixelStatusProducer");
}

//--------------------------------------------------------------------------------------------------
SiPixelStatusProducer::~SiPixelStatusProducer(){
}

//--------------------------------------------------------------------------------------------------
void SiPixelStatusProducer::beginJob(){

  MINFEDID = FEDNumbering::MINSiPixeluTCAFEDID;
  MAXFEDID = FEDNumbering::MAXSiPixeluTCAFEDID;

}

//--------------------------------------------------------------------------------------------------
void SiPixelStatusProducer::beginLuminosityBlock(edm::LuminosityBlock const& lumiSeg, const edm::EventSetup& iSetup){
  const edm::TimeValue_t fbegintimestamp = lumiSeg.beginTime().value();
  const std::time_t ftmptime = fbegintimestamp >> 32;

  if ( countLumi_ == 0 || (resetNLumi_ > 0 && countLumi_<=resetNLumi_) ) {

    beginLumi_ = lumiSeg.luminosityBlock();
    beginRun_ = lumiSeg.run();
    refTime_[0] = ftmptime;
  }
    
  countLumi_++;

  // -- Setup cabling map and its map to detIDs
  iSetup.get<SiPixelFedCablingMapRcd>().get(fCablingMap);
  for (int i = MINFEDID; i < MAXFEDID; ++i) {
      fPFC[i] = new SiPixelFrameConverter(fCablingMap.product(), i);
  }

  // -- Initialize FEDs for offline/online conversion" << endl;
  iSetup.get<TrackerDigiGeometryRecord>().get(fTG);
  // -- Initialize fIndices"
  //int nmod(0);
  for (TrackerGeometry::DetContainer::const_iterator it = fTG->dets().begin(); it != fTG->dets().end(); it++){

      const PixelGeomDetUnit *pgdu = dynamic_cast<const PixelGeomDetUnit*>((*it));
      if (0 == pgdu) continue;

      DetId detId = (*it)->geographicalId();
      uint32_t newDetId = detId;

      for (int fedid = MINFEDID; fedid < MAXFEDID; ++fedid) {
          if (fPFC[fedid]->hasDetUnit(newDetId)) {
            fFEDID.insert(make_pair(newDetId, fedid));
            break;
          }
      }

      int nrocs = pgdu->specificTopology().ncolumns()/52*pgdu->specificTopology().nrows()/80;
      fDet.addModule(detId, nrocs);
      map<pair<int, int>, triplet> a;
      fIndices.insert(make_pair(detId, a));

        /*
        int nrocs = pgdu->specificTopology().ncolumns()/52*pgdu->specificTopology().nrows()/80;
        cout << "detid = " << detId.rawId()
             << " columns =  " << pgdu->specificTopology().ncolumns()
             << " rows = " << pgdu->specificTopology().nrows()
             << " nrocs = " << nrocs
             << endl;
        */
        //++nmod;
  }
  
}

//--------------------------------------------------------------------------------------------------
void SiPixelStatusProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup){

  ftotalevents++;

  // ----------------------------------------------------------------------
  // -- Pixel cluster analysis
  // ----------------------------------------------------------------------
  const edmNew::DetSetVector<SiPixelCluster> *clustColl(0);
  edm::Handle<edmNew::DetSetVector<SiPixelCluster> > hClusterColl;
  iEvent.getByToken(fSiPixelClusterToken_, hClusterColl);
  clustColl = hClusterColl.product();

  // loop over tracker DIGI geo
  for (TrackerGeometry::DetContainer::const_iterator it = fTG->dets().begin(); it != fTG->dets().end(); it++) { 
       
       if (0 == dynamic_cast<const PixelGeomDetUnit*>((*it))) continue;
       DetId detId = (*it)->geographicalId();
       int detid = detId.rawId();
       // -- clusters on this det
       edmNew::DetSetVector<SiPixelCluster>::const_iterator isearch = clustColl->find(detId);
       if (isearch == clustColl->end()) continue;
       int roc(0), rocC(0), rocR(0);
       int mr0(-1), mc0(-1);

       // loop over clusters on a pixel module (DetId)
       for (edmNew::DetSet<SiPixelCluster>::const_iterator  di = isearch->begin(); di != isearch->end(); ++di) {
        const vector<SiPixelCluster::Pixel>& pixvector = di->pixels();
        // loop over pixels of a cluster
        for (unsigned int i = 0; i < pixvector.size(); ++i) {

             mr0 = pixvector[i].x;
             mc0 = pixvector[i].y;
             pair<int, int> offline(mc0, mr0);

             map<pair<int, int>, triplet>::iterator pI = fIndices[detid].find(offline);

             if (pI != fIndices[detid].end()) {
                 roc  = pI->second.roc;
                 rocC = pI->second.col;
                 rocR = pI->second.row;
             } else {
                onlineRocColRow(detId, mr0, mc0, roc, rocC, rocR);
                triplet a;
                a.roc = roc;
                a.col = rocC;
                a.row = rocR;
                fIndices[detid].insert(make_pair(offline, a));
             }

             fDet.fill(detId, roc, rocC/2);

           } // -- loop over pixels of cluster

        } // -- loop over clusters on a pixel module (DetId)

    } // loop over tracker DIGI Geo
 
}


//--------------------------------------------------------------------------------------------------
void SiPixelStatusProducer::endLuminosityBlock(edm::LuminosityBlock const& lumiSeg, const edm::EventSetup& iSetup){
}

//--------------------------------------------------------------------------------------------------
void SiPixelStatusProducer::endLuminosityBlockProduce(edm::LuminosityBlock& lumiSeg, const edm::EventSetup& iSetup){

  const edm::TimeValue_t fendtimestamp = lumiSeg.endTime().value();
  const std::time_t fendtime = fendtimestamp >> 32;
  refTime_[1] = fendtime;
    
  endLumi_ = lumiSeg.luminosityBlock();
  endRun_  = lumiSeg.run();  

  if ( resetNLumi_ == -1 ) return;
  if ( countLumi_ <= resetNLumi_ ) return;

  fDet.setRunRange(beginRun_,endRun_);
  fDet.setLSRange(beginLumi_,endLumi_);
  fDet.setRefTime(refTime_[0],refTime_[1]);

  // save result
  auto result = std::make_unique<SiPixelDetectorStatus>();
  *result = fDet;
  lumiSeg.put(std::move(result), std::string("siPixelStatus"));

  // reset detector status and lumi-counter
  fDet.resetDetectorStatus();
  countLumi_=0;
}


void SiPixelStatusProducer::endJob() {

   // FED counter
   for (int i = MINFEDID; i < MAXFEDID; ++i) {
        delete fPFC[i];
   }

}

//--------------------------------------------------------------------------------------------------
void SiPixelStatusProducer::onlineRocColRow(const DetId &pID, int offlineRow, int offlineCol, int &roc, int &col, int &row) {

  int realfedID = -1;
  uint32_t newDetId = pID;

  sipixelobjects::ElectronicIndex cabling;
  sipixelobjects::DetectorIndex detector = {pID, offlineRow, offlineCol};

  realfedID = fFEDID[newDetId];
  fPFC[realfedID]->toCabling(cabling, detector);

  sipixelobjects::LocalPixel::DcolPxid loc;
  loc.dcol = cabling.dcol;
  loc.pxid = cabling.pxid;

  sipixelobjects::LocalPixel locpixel(loc);
  sipixelobjects::CablingPathToDetUnit path = {(unsigned int) realfedID, (unsigned int)cabling.link, (unsigned int)cabling.roc};
  const sipixelobjects::PixelROC *theRoc = fCablingMap->findItem(path);

  roc = theRoc->idInDetUnit();
  uint32_t detSubId = pID.subdetId();
  if (detSubId == 1) { // ForBPIX
    PixelBarrelName nameworker(pID);
    std::string outputname = nameworker.name();
    bool HalfModule = nameworker.isHalfModule();
    if ((outputname.find("mO") != string::npos || outputname.find("mI") != string::npos) && (HalfModule)) {
      // has to be BPIX; has to be minus side; has to be half module
      roc = theRoc->idInDetUnit() + 8;
      }
  }
  col = locpixel.rocCol();
  row = locpixel.rocRow();

}


DEFINE_FWK_MODULE(SiPixelStatusProducer);
