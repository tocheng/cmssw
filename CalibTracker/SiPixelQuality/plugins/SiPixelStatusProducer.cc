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
  resetNLumi_   = iConfig.getParameter<edm::ParameterSet>("SiPixelStatusProducerParameters").getUntrackedParameter<int>("resetEveryNLumi",1);

  ftotalevents = 0;
  countLumi_ = 0;

  beginLumi_ = endLumi_ = -1;
  endLumi_ = endRun_ = -1;
  
  produces<SiPixelDetectorStatus, edm::Transition::EndLuminosityBlock>("siPixelStatus");
}

//--------------------------------------------------------------------------------------------------
SiPixelStatusProducer::~SiPixelStatusProducer(){

}


//--------------------------------------------------------------------------------------------------
void SiPixelStatusProducer::beginLuminosityBlock(edm::LuminosityBlock const& lumiSeg, const edm::EventSetup& iSetup){

  edm::LogInfo("SiPixelStatusProducer")
                 << "beginlumi setup "<<endl;

  const edm::TimeValue_t fbegintimestamp = lumiSeg.beginTime().value();
  const std::time_t ftmptime = fbegintimestamp >> 32;

  if ( countLumi_ == 0 && resetNLumi_ > 0 ) {

    beginLumi_ = lumiSeg.luminosityBlock();
    beginRun_ = lumiSeg.run();
    refTime_[0] = ftmptime;

  }
    
  countLumi_++;

  if(siPixelFedCablingMapWatcher_.check(iSetup)||trackerDIGIGeoWatcher_.check(iSetup)||trackerTopoWatcher_.check(iSetup)){

    coord_.init(iSetup);

    iSetup.get<SiPixelFedCablingMapRcd>().get(fCablingMap);
    fCablingMap_ = fCablingMap.product();

    iSetup.get<TrackerDigiGeometryRecord>().get(fTG);
    // -- Initialize fIndices for offline/online conversion
    for (TrackerGeometry::DetContainer::const_iterator it = fTG->dets().begin(); it != fTG->dets().end(); it++){

        const PixelGeomDetUnit *pgdu = dynamic_cast<const PixelGeomDetUnit*>((*it));
        if (0 == pgdu) continue;

        DetId detId = (*it)->geographicalId();

        int nrocs = pgdu->specificTopology().ncolumns()/52*pgdu->specificTopology().nrows()/80;
        fDet.addModule(detId, nrocs);
        map<pair<int, int>, triplet> a;
        fIndices.insert(make_pair(detId, a));
    }

  } // if conditionWatcher_.check(iSetup)
  

}

//--------------------------------------------------------------------------------------------------
void SiPixelStatusProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup){

  ftotalevents++;

  edm::LogInfo("SiPixelStatusProducer")
                 << "start cluster analyzer "<<endl;

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

       // loop over clusters on a pixel module (DetId)
       for (edmNew::DetSet<SiPixelCluster>::const_iterator  di = isearch->begin(); di != isearch->end(); ++di) {

         int roc(-1), rocC(-1), rocR(-1);

         const vector<SiPixelCluster::Pixel>& pixvector = di->pixels();
         // loop over pixels of a cluster
         for (unsigned int i = 0; i < pixvector.size(); ++i) {

             int mr0 = pixvector[i].x;
             int mc0 = pixvector[i].y;
             pair<int, int> offline(mr0, mc0);
             map<pair<int, int>, triplet>::iterator pI = fIndices[detid].find(offline);

             if (pI != fIndices[detid].end()) {
                 roc  = pI->second.roc;
                 rocR = pI->second.row;
                 rocC = pI->second.column;
             } else {

                 //onlineRocColRow(detId, mr0, mc0, roc, rocR, rocC);
                 //cout<<"roc from coord - roc from cabling map "<<coord_.roc(detId,offline)<<"-"<<roc<<endl;

                 roc = coord_.roc(detId,offline);
                 triplet a;
                 a.roc = roc;
                 rocR = 39; rocC = 25;
                 a.row = rocR; a.column = rocC;
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

  edm::LogInfo("SiPixelStatusProducer")
                 << "endlumi producer "<<endl;

  const edm::TimeValue_t fendtimestamp = lumiSeg.endTime().value();
  const std::time_t fendtime = fendtimestamp >> 32;
  refTime_[1] = fendtime;
    
  endLumi_ = lumiSeg.luminosityBlock();
  endRun_  = lumiSeg.run();  

  if ( resetNLumi_ == -1 ) return;
  if ( countLumi_ < resetNLumi_ ) return;

  fDet.setRunRange(beginRun_,endRun_);
  fDet.setLSRange(beginLumi_,endLumi_);
  fDet.setRefTime(refTime_[0],refTime_[1]);

  // save result
  auto result = std::make_unique<SiPixelDetectorStatus>();
  *result = fDet;
  lumiSeg.put(std::move(result), std::string("siPixelStatus"));
  edm::LogInfo("SiPixelStatusProducer")
                 << "new lumi-based data stored for run "<<beginRun_<<" lumi from "<<beginLumi_<<" to "<<endLumi_<<std::endl;

  // reset detector status and lumi-counter
  fDet.resetDetectorStatus();
  countLumi_=0;
}

//--------------------------------------------------------------------------------------------------
void SiPixelStatusProducer::onlineRocColRow(const DetId &detId, int offlineRow, int offlineCol, int &roc, int &row, int &col) {

  int fedId = coord_.fedid(detId);

  // from detector to cabling
  sipixelobjects::ElectronicIndex cabling;
  sipixelobjects::DetectorIndex detector = {detId, offlineRow, offlineCol};

  SiPixelFrameConverter converter(fCablingMap_, fedId);
  converter.toCabling(cabling, detector);

  // then one can construct local pixel
  sipixelobjects::LocalPixel::DcolPxid loc;
  loc.dcol = cabling.dcol;
  loc.pxid = cabling.pxid;
  // and get local(online) row/column
  sipixelobjects::LocalPixel locpixel(loc);
  col = locpixel.rocCol();
  row = locpixel.rocRow();

  //sipixelobjects::CablingPathToDetUnit path = {(unsigned int) fedId, (unsigned int)cabling.link, (unsigned int)cabling.roc};
  //const sipixelobjects::PixelROC *theRoc = fCablingMap->findItem(path);
  const sipixelobjects::PixelROC *theRoc = converter.toRoc(cabling.link, cabling.roc);
  roc = theRoc->idInDetUnit();
  // has to be BPIX; has to be minus side; has to be half module
  if (detId.subdetId() == PixelSubdetector::PixelBarrel && coord_.side(detId)==1 && coord_.half(detId))     
  {
      roc += 8;
  }

}


DEFINE_FWK_MODULE(SiPixelStatusProducer);
