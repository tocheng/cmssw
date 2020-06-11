// -*- C++ -*-
//
// Class:      PixelBaryCentreAnalyzer
/*

 Description: simple emd::one::EDAnalyzer to retrieve and ntuplize tracker alignment from the conditions database

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Marco Musich
//         Created:  Tue, 18 Oct 2016 11:00:44 GMT
//
//

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ESWatcher.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

// Phase-1 Pixel
#include "DQM/SiPixelPhase1Common/interface/SiPixelCoordinates.h"
#include "FWCore/Utilities/interface/ESGetToken.h"
#include "CondFormats/SiPixelObjects/interface/SiPixelFedCablingMap.h"
#include "CondFormats/DataRecord/interface/SiPixelFedCablingMapRcd.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
#include "Geometry/Records/interface/TrackerTopologyRcd.h"

// pixel quality
#include "CondFormats/SiPixelObjects/interface/SiPixelQuality.h"
#include "CondFormats/DataRecord/interface/SiPixelQualityFromDbRcd.h"
// global postion
#include "CondFormats/Alignment/interface/DetectorGlobalPosition.h"
#include "CondFormats/AlignmentRecord/interface/GlobalPositionRcd.h"
// tracker alignment
#include "CondFormats/AlignmentRecord/interface/TrackerAlignmentRcd.h"
#include "CondFormats/Alignment/interface/Alignments.h"
// beamspot
#include "CondFormats/DataRecord/interface/BeamSpotObjectsRcd.h"
#include "CondFormats/BeamSpotObjects/interface/BeamSpotObjects.h"

// For ROOT
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include <TTree.h>
#include <TVector3.h>

#include <sstream>
#include <fstream>

//
// class declaration
//

class PixelBaryCentreAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
   public:
      explicit PixelBaryCentreAnalyzer(const edm::ParameterSet&);
      ~PixelBaryCentreAnalyzer() override;

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      void beginJob() override;
      void analyze(const edm::Event&, const edm::EventSetup&) override;
      void endJob() override;

      void initBC();
      void initBS();

      bool usePixelQuality_; 

      SiPixelCoordinates coord_;

      int   run_;
      int   ls_;

      float BSx0_;
      float BSy0_;
      float BSz0_;

      float PIXx0_,  PIXy0_,  PIXz0_;
      float BPIXx0_, BPIXy0_, BPIXz0_;
      float FPIXx0_, FPIXy0_, FPIXz0_;

      TVector3 PIX_, BPIX_, FPIX_;

      std::vector<float> vBPIXLY1x0_, vBPIXLY1y0_, vBPIXLY1z0_;
      std::vector<float> vBPIXLY2x0_, vBPIXLY2y0_, vBPIXLY2z0_;
      std::vector<float> vBPIXLY3x0_, vBPIXLY3y0_, vBPIXLY3z0_;
      std::vector<float> vBPIXLY4x0_, vBPIXLY4y0_, vBPIXLY4z0_;

      float BPIXLY1x0_, BPIXLY1y0_, BPIXLY1z0_;
      float BPIXLY1_Flippedx0_, BPIXLY1_Flippedy0_, BPIXLY1_Flippedz0_;
      float BPIXLY1_NonFlippedx0_, BPIXLY1_NonFlippedy0_, BPIXLY1_NonFlippedz0_;

      float BPIXLY2x0_, BPIXLY2y0_, BPIXLY2z0_;
      float BPIXLY2_Flippedx0_, BPIXLY2_Flippedy0_, BPIXLY2_Flippedz0_;
      float BPIXLY2_NonFlippedx0_, BPIXLY2_NonFlippedy0_, BPIXLY2_NonFlippedz0_;

      float BPIXLY3x0_, BPIXLY3y0_, BPIXLY3z0_;
      float BPIXLY3_Flippedx0_, BPIXLY3_Flippedy0_, BPIXLY3_Flippedz0_;
      float BPIXLY3_NonFlippedx0_, BPIXLY3_NonFlippedy0_, BPIXLY3_NonFlippedz0_;

      float BPIXLY4x0_, BPIXLY4y0_, BPIXLY4z0_;
      float BPIXLY4_Flippedx0_, BPIXLY4_Flippedy0_, BPIXLY4_Flippedz0_;
      float BPIXLY4_NonFlippedx0_, BPIXLY4_NonFlippedy0_, BPIXLY4_NonFlippedz0_;

      edm::Service<TFileService> tFileService; 
      TTree * bctree_;

      // ----------member data ---------------------------
      edm::ESWatcher<BeamSpotObjectsRcd> watcherBS_;
      edm::ESWatcher<TrackerAlignmentRcd> watcherTkAlign_;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
PixelBaryCentreAnalyzer::PixelBaryCentreAnalyzer(const edm::ParameterSet& iConfig) :
  usePixelQuality_(iConfig.getUntrackedParameter<bool>("usePixelQuality")),
  bctree_(nullptr)
{
  usesResource("TFileService");
}


PixelBaryCentreAnalyzer::~PixelBaryCentreAnalyzer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

void PixelBaryCentreAnalyzer::initBS(){

  float dummy_float = 999999.0;

  BSx0_  = dummy_float;	  
  BSy0_  = dummy_float;	  
  BSz0_  = dummy_float;	  

}

void PixelBaryCentreAnalyzer::initBC(){

  float dummy_float = 999900.0;

  PIXx0_ = dummy_float; 
  PIXy0_ = dummy_float; 
  PIXz0_ = dummy_float;
  BPIXx0_ = dummy_float; 
  BPIXy0_ = dummy_float; 
  BPIXz0_ = dummy_float;
  FPIXx0_ = dummy_float; 
  FPIXy0_ = dummy_float; 
  FPIXz0_ = dummy_float;

  PIX_  =  TVector3(dummy_float,dummy_float,dummy_float);
  BPIX_ =  TVector3(dummy_float,dummy_float,dummy_float);
  FPIX_ =  TVector3(dummy_float,dummy_float,dummy_float);

  vBPIXLY1x0_.clear(); vBPIXLY1y0_.clear(); vBPIXLY1z0_.clear();
  vBPIXLY2x0_.clear(); vBPIXLY2y0_.clear(); vBPIXLY2z0_.clear();
  vBPIXLY3x0_.clear(); vBPIXLY3y0_.clear(); vBPIXLY3z0_.clear();
  vBPIXLY4x0_.clear(); vBPIXLY4y0_.clear(); vBPIXLY4z0_.clear();

}


// ------------ method called for each event  ------------
void PixelBaryCentreAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{

   if (!watcherBS_.check(iSetup) && !watcherTkAlign_.check(iSetup)) return; 

   // Pixel Phase-1 helper class
   coord_.init(iSetup);

   run_  = iEvent.id().run();
   ls_   = iEvent.id().luminosityBlock();

   if (watcherTkAlign_.check(iSetup)) { // check for new IOV for TKAlign
     
     // pixel quality
     edm::ESHandle<SiPixelQuality> qualityInfo;
     iSetup.get<SiPixelQualityFromDbRcd>().get( qualityInfo );
     const SiPixelQuality* badPixelInfo_ = qualityInfo.product();

     PixelBaryCentreAnalyzer::initBC();     

     edm::ESHandle<Alignments> globalAlignments;
     iSetup.get<GlobalPositionRcd>().get(globalAlignments);
     std::unique_ptr<const Alignments> globalPositions = std::make_unique<Alignments>(*globalAlignments);
     const AlignTransform& globalCoordinates = align::DetectorGlobalPosition(*globalPositions, DetId(DetId::Tracker));
     TVector3 globalTkPosition(globalCoordinates.translation().x(), 
                               globalCoordinates.translation().y(),
                               globalCoordinates.translation().z());

     //std::cout << globalTkPosition.X() <<" "<<globalTkPosition.Y()<<" "<<globalTkPosition.Z()<<std::endl;

     // Get TkAlign from EventSetup:
     edm::ESHandle<Alignments> alignments;
     iSetup.get<TrackerAlignmentRcd>().get(alignments);
     std::vector<AlignTransform> tkAlignments = alignments->m_align;

     // per-ladder barycentre

     std::map<int, std::map<int, float>> nmodules;
     std::map<int, std::map<int, TVector3>> barycentre;

     TVector3 barycentre_BPIX;
     float nmodules_BPIX(0.);

     TVector3 barycentre_FPIX;
     float nmodules_FPIX(0.);

     // loop over tracker module
     for (const auto &ali : tkAlignments) {

        // remove bad module
        if(usePixelQuality_ && badPixelInfo_->IsModuleBad(DetId(ali.rawId())) ) continue;

        TVector3 ali_translation(ali.translation().x(),ali.translation().y(),ali.translation().z());

        int subid = DetId(ali.rawId()).subdetId();
        // BPIX
        if (subid == PixelSubdetector::PixelBarrel){
           nmodules_BPIX += 1; 
           barycentre_BPIX += ali_translation;
           
           int layer  = coord_.layer( DetId(ali.rawId()) );
           int ladder = coord_.ladder( DetId(ali.rawId()) );

           nmodules[layer][ladder] += 1;
           barycentre[layer][ladder] += ali_translation;

        } // BPIX

        // FPIX
        if (subid == PixelSubdetector::PixelEndcap){
           nmodules_FPIX += 1; 
           barycentre_FPIX += ali_translation;
        } // FPIX

     }// loop over tracker module

     //PIX
     TVector3 barycentre_PIX = barycentre_BPIX + barycentre_FPIX; 
     float nmodules_PIX = nmodules_BPIX + nmodules_FPIX;
     PIX_  = (1/nmodules_PIX)*barycentre_PIX   + globalTkPosition;
     BPIX_ = (1/nmodules_BPIX)*barycentre_BPIX + globalTkPosition;
     FPIX_ = (1/nmodules_FPIX)*barycentre_FPIX + globalTkPosition;

     PIXx0_ = barycentre_PIX.X()/nmodules_PIX + globalTkPosition.X();
     PIXy0_ = barycentre_PIX.Y()/nmodules_PIX + globalTkPosition.Y();
     PIXz0_ = barycentre_PIX.Z()/nmodules_PIX + globalTkPosition.Z();

     //BPIX
     BPIXx0_ = barycentre_BPIX.X()/nmodules_BPIX + globalTkPosition.X();
     BPIXy0_ = barycentre_BPIX.Y()/nmodules_BPIX + globalTkPosition.Y() ;
     BPIXz0_ = barycentre_BPIX.Z()/nmodules_BPIX + globalTkPosition.Z();

     //FPIX
     FPIXx0_ = barycentre_FPIX.X()/nmodules_FPIX + globalTkPosition.X();
     FPIXy0_ = barycentre_FPIX.Y()/nmodules_FPIX + globalTkPosition.Y();
     FPIXz0_ = barycentre_FPIX.Z()/nmodules_FPIX + globalTkPosition.Z();

     // BPix barycentre per-ladder per-layer
     // !!! Based on assumption : each ladder has the same number of modules in the same layer
     // inner =  flipped; outer = non-flipped
     /*
       // Phase 0: Outer ladders are odd for layer 1,3 and even for layer 2
       // Phase 1: Outer ladders are odd for layer 4 and even for layer 1,2,3
     */

     BPIXLY1x0_ = 0; BPIXLY1x0_ = 0; BPIXLY1z0_ = 0;
     BPIXLY1_Flippedx0_ = 0; BPIXLY1_Flippedx0_ = 0; BPIXLY1_Flippedz0_ = 0;
     BPIXLY1_NonFlippedx0_ = 0; BPIXLY1_NonFlippedx0_ = 0; BPIXLY1_NonFlippedz0_ = 0;

     std::map<int, TVector3> barycentre_L1 = barycentre[1];
     for (std::map<int, TVector3>::iterator it = barycentre_L1.begin(); it != barycentre_L1.end(); ++it) {
         int ladder = it->first;

         barycentre_L1[ladder] = (1/nmodules[1][ladder])*it->second;

         BPIXLY1x0_ += (barycentre_L1[ladder].X() + globalTkPosition.X())/barycentre_L1.size();
         BPIXLY1y0_ += (barycentre_L1[ladder].Y() + globalTkPosition.Y())/barycentre_L1.size();
         BPIXLY1z0_ += (barycentre_L1[ladder].Z() + globalTkPosition.Z())/barycentre_L1.size();

         if(ladder%2) { // odd ladder = inner = flipped 
              BPIXLY1_Flippedx0_ += 2*(barycentre_L1[ladder].X() + globalTkPosition.X())/barycentre_L1.size();
              BPIXLY1_Flippedy0_ += 2*(barycentre_L1[ladder].Y() + globalTkPosition.Y())/barycentre_L1.size();
              BPIXLY1_Flippedz0_ += 2*(barycentre_L1[ladder].Z() + globalTkPosition.Z())/barycentre_L1.size();
         }
         else { // even ladder = outer = non-flipped
              BPIXLY1_NonFlippedx0_ += 2*(barycentre_L1[ladder].X() + globalTkPosition.X())/barycentre_L1.size();
              BPIXLY1_NonFlippedy0_ += 2*(barycentre_L1[ladder].Y() + globalTkPosition.Y())/barycentre_L1.size();
              BPIXLY1_NonFlippedz0_ += 2*(barycentre_L1[ladder].Z() + globalTkPosition.Z())/barycentre_L1.size();
         }

         vBPIXLY1x0_.push_back(barycentre_L1[ladder].X() + globalTkPosition.X());
         vBPIXLY1y0_.push_back(barycentre_L1[ladder].Y() + globalTkPosition.Y());
         vBPIXLY1z0_.push_back(barycentre_L1[ladder].Z() + globalTkPosition.Z());
     }


     BPIXLY2x0_ = 0; BPIXLY2x0_ = 0; BPIXLY2z0_ = 0;
     BPIXLY2_Flippedx0_ = 0; BPIXLY2_Flippedx0_ = 0; BPIXLY2_Flippedz0_ = 0;
     BPIXLY2_NonFlippedx0_ = 0; BPIXLY2_NonFlippedx0_ = 0; BPIXLY2_NonFlippedz0_ = 0;

     std::map<int, TVector3> barycentre_L2 = barycentre[2];
     for (std::map<int, TVector3>::iterator it = barycentre_L2.begin(); it != barycentre_L2.end(); ++it) {
         int ladder = it->first;
         barycentre_L2[ladder] = (1/nmodules[2][ladder])*it->second;

         BPIXLY2x0_ += (barycentre_L2[ladder].X() + globalTkPosition.X())/barycentre_L2.size();
         BPIXLY2y0_ += (barycentre_L2[ladder].Y() + globalTkPosition.Y())/barycentre_L2.size();
         BPIXLY2z0_ += (barycentre_L2[ladder].Z() + globalTkPosition.Z())/barycentre_L2.size();

         if(ladder%2) { // odd ladder = inner = flipped
              BPIXLY2_Flippedx0_ += 2*(barycentre_L2[ladder].X() + globalTkPosition.X())/barycentre_L2.size();
              BPIXLY2_Flippedy0_ += 2*(barycentre_L2[ladder].Y() + globalTkPosition.Y())/barycentre_L2.size();
              BPIXLY2_Flippedz0_ += 2*(barycentre_L2[ladder].Z() + globalTkPosition.Z())/barycentre_L2.size();
         }
         else { // even ladder = outer = non-flipped
              BPIXLY2_NonFlippedx0_ += 2*(barycentre_L2[ladder].X() + globalTkPosition.X())/barycentre_L2.size();
              BPIXLY2_NonFlippedy0_ += 2*(barycentre_L2[ladder].Y() + globalTkPosition.Y())/barycentre_L2.size();
              BPIXLY2_NonFlippedz0_ += 2*(barycentre_L2[ladder].Z() + globalTkPosition.Z())/barycentre_L2.size();
         }

         vBPIXLY2x0_.push_back(barycentre_L2[ladder].X() + globalTkPosition.X());
         vBPIXLY2y0_.push_back(barycentre_L2[ladder].Y() + globalTkPosition.Y());
         vBPIXLY2z0_.push_back(barycentre_L2[ladder].Z() + globalTkPosition.Z());
     }


     BPIXLY3x0_ = 0; BPIXLY3x0_ = 0; BPIXLY3z0_ = 0;
     BPIXLY3_Flippedx0_ = 0; BPIXLY3_Flippedx0_ = 0; BPIXLY3_Flippedz0_ = 0;
     BPIXLY3_NonFlippedx0_ = 0; BPIXLY3_NonFlippedx0_ = 0; BPIXLY3_NonFlippedz0_ = 0;

     std::map<int, TVector3> barycentre_L3 = barycentre[3];
     for (std::map<int, TVector3>::iterator it = barycentre_L3.begin(); it != barycentre_L3.end(); ++it) {
         int ladder = it->first;
         barycentre_L3[ladder] = (1/nmodules[3][ladder])*it->second;

         BPIXLY3x0_ += (barycentre_L3[ladder].X() + globalTkPosition.X())/barycentre_L3.size();
         BPIXLY3y0_ += (barycentre_L3[ladder].Y() + globalTkPosition.Y())/barycentre_L3.size();
         BPIXLY3z0_ += (barycentre_L3[ladder].Z() + globalTkPosition.Z())/barycentre_L3.size();

         if(ladder%2) { // odd ladder = inner = flipped
              BPIXLY3_Flippedx0_ += 2*(barycentre_L3[ladder].X() + globalTkPosition.X())/barycentre_L3.size();
              BPIXLY3_Flippedy0_ += 2*(barycentre_L3[ladder].Y() + globalTkPosition.Y())/barycentre_L3.size();
              BPIXLY3_Flippedz0_ += 2*(barycentre_L3[ladder].Z() + globalTkPosition.Z())/barycentre_L3.size();
         }
         else { // even ladder = outer = non-flipped
              BPIXLY3_NonFlippedx0_ += 2*(barycentre_L3[ladder].X() + globalTkPosition.X())/barycentre_L3.size();
              BPIXLY3_NonFlippedy0_ += 2*(barycentre_L3[ladder].Y() + globalTkPosition.Y())/barycentre_L3.size();
              BPIXLY3_NonFlippedz0_ += 2*(barycentre_L3[ladder].Z() + globalTkPosition.Z())/barycentre_L3.size();
         }

         vBPIXLY3x0_.push_back(barycentre_L3[ladder].X() + globalTkPosition.X());
         vBPIXLY3y0_.push_back(barycentre_L3[ladder].Y() + globalTkPosition.Y());
         vBPIXLY3z0_.push_back(barycentre_L3[ladder].Z() + globalTkPosition.Z());
     }

     BPIXLY4x0_ = 0; BPIXLY4x0_ = 0; BPIXLY4z0_ = 0;
     BPIXLY4_Flippedx0_ = 0; BPIXLY4_Flippedx0_ = 0; BPIXLY4_Flippedz0_ = 0;
     BPIXLY4_NonFlippedx0_ = 0; BPIXLY4_NonFlippedx0_ = 0; BPIXLY4_NonFlippedz0_ = 0;

     std::map<int, TVector3> barycentre_L4 = barycentre[4];
     for (std::map<int, TVector3>::iterator it = barycentre_L4.begin(); it != barycentre_L4.end(); ++it) {
         int ladder = it->first;
         barycentre_L4[ladder] = (1/nmodules[4][ladder])*it->second;

         BPIXLY4x0_ += (barycentre_L4[ladder].X() + globalTkPosition.X())/barycentre_L4.size();
         BPIXLY4y0_ += (barycentre_L4[ladder].Y() + globalTkPosition.Y())/barycentre_L4.size();
         BPIXLY4z0_ += (barycentre_L4[ladder].Z() + globalTkPosition.Z())/barycentre_L4.size();

         if(ladder%2==0) { // even ladder = inner = flipped
              BPIXLY4_Flippedx0_ += 2*(barycentre_L4[ladder].X() + globalTkPosition.X())/barycentre_L4.size();
              BPIXLY4_Flippedy0_ += 2*(barycentre_L4[ladder].Y() + globalTkPosition.Y())/barycentre_L4.size();
              BPIXLY4_Flippedz0_ += 2*(barycentre_L4[ladder].Z() + globalTkPosition.Z())/barycentre_L4.size();
         }
         else { // odd ladder = outer = non-flipped
              BPIXLY4_NonFlippedx0_ += 2*(barycentre_L4[ladder].X() + globalTkPosition.X())/barycentre_L4.size();
              BPIXLY4_NonFlippedy0_ += 2*(barycentre_L4[ladder].Y() + globalTkPosition.Y())/barycentre_L4.size();
              BPIXLY4_NonFlippedz0_ += 2*(barycentre_L4[ladder].Z() + globalTkPosition.Z())/barycentre_L4.size();
         }

         vBPIXLY4x0_.push_back(barycentre_L4[ladder].X() + globalTkPosition.X());
         vBPIXLY4y0_.push_back(barycentre_L4[ladder].Y() + globalTkPosition.Y());
         vBPIXLY4z0_.push_back(barycentre_L4[ladder].Z() + globalTkPosition.Z());
     }

   } // check for new IOV for TKAlign

   // beamspot
   if ( watcherBS_.check(iSetup) ) {

     PixelBaryCentreAnalyzer::initBS();

     // Get BeamSpot from EventSetup
     edm::ESHandle< BeamSpotObjects > beamhandle;
     iSetup.get<BeamSpotObjectsRcd>().get(beamhandle);
     const BeamSpotObjects *mybeamspot = beamhandle.product();
      
     BSx0_  = mybeamspot->GetX();
     BSy0_  = mybeamspot->GetY();
     BSz0_  = mybeamspot->GetZ();

   } // check for new IOV for BS

   bctree_->Fill();

}


// ------------ method called once each job just before starting event loop  ------------
void 
PixelBaryCentreAnalyzer::beginJob()
{
  bctree_ = tFileService->make<TTree>("PixelBarycentre","PixelBarycentre analyzer ntuple");
  
  //Tree Branches
  bctree_->Branch("run",&run_,"run/I");
  bctree_->Branch("ls",&ls_,"ls/I");
  bctree_->Branch("BSx0",&BSx0_,"BSx0/F");
  bctree_->Branch("BSy0",&BSy0_,"BSy0/F");	   
  bctree_->Branch("BSz0",&BSz0_,"BSz0/F");	   

  bctree_->Branch("PIX",&PIX_);
  bctree_->Branch("BPIX",&BPIX_);
  bctree_->Branch("FPIX",&FPIX_);

  bctree_->Branch("PIXx0",&PIXx0_,"PIXx0/F");
  bctree_->Branch("PIXy0",&PIXy0_,"PIXy0/F");
  bctree_->Branch("PIXz0",&PIXz0_,"PIXz0/F");

  bctree_->Branch("BPIXx0",&BPIXx0_,"BPIXx0/F");
  bctree_->Branch("BPIXy0",&BPIXy0_,"BPIXy0/F");
  bctree_->Branch("BPIXz0",&BPIXz0_,"BPIXz0/F");

  //L1
  bctree_->Branch("BPIXLY1x0",&BPIXLY1x0_,"BPIXLY1x0/F");
  bctree_->Branch("BPIXLY1y0",&BPIXLY1y0_,"BPIXLY1y0/F");
  bctree_->Branch("BPIXLY1z0",&BPIXLY1z0_,"BPIXLY1z0/F");

  bctree_->Branch("BPIXLY1_Flippedx0",&BPIXLY1_Flippedx0_,"BPIXLY1_Flippedx0/F");
  bctree_->Branch("BPIXLY1_Flippedy0",&BPIXLY1_Flippedy0_,"BPIXLY1_Flippedy0/F");
  bctree_->Branch("BPIXLY1_Flippedz0",&BPIXLY1_Flippedz0_,"BPIXLY1_Flippedz0/F");

  bctree_->Branch("BPIXLY1_NonFlippedx0",&BPIXLY1_NonFlippedx0_,"BPIXLY1_NonFlippedx0/F");
  bctree_->Branch("BPIXLY1_NonFlippedy0",&BPIXLY1_NonFlippedy0_,"BPIXLY1_NonFlippedy0/F");
  bctree_->Branch("BPIXLY1_NonFlippedz0",&BPIXLY1_NonFlippedz0_,"BPIXLY1_NonFlippedz0/F");

  //L2
  bctree_->Branch("BPIXLY2x0",&BPIXLY2x0_,"BPIXLY2x0/F");
  bctree_->Branch("BPIXLY2y0",&BPIXLY2y0_,"BPIXLY2y0/F");
  bctree_->Branch("BPIXLY2z0",&BPIXLY2z0_,"BPIXLY2z0/F");

  bctree_->Branch("BPIXLY2_Flippedx0",&BPIXLY2_Flippedx0_,"BPIXLY2_Flippedx0/F");
  bctree_->Branch("BPIXLY2_Flippedy0",&BPIXLY2_Flippedy0_,"BPIXLY2_Flippedy0/F");
  bctree_->Branch("BPIXLY2_Flippedz0",&BPIXLY2_Flippedz0_,"BPIXLY2_Flippedz0/F");

  bctree_->Branch("BPIXLY2_NonFlippedx0",&BPIXLY2_NonFlippedx0_,"BPIXLY2_NonFlippedx0/F");
  bctree_->Branch("BPIXLY2_NonFlippedy0",&BPIXLY2_NonFlippedy0_,"BPIXLY2_NonFlippedy0/F");
  bctree_->Branch("BPIXLY2_NonFlippedz0",&BPIXLY2_NonFlippedz0_,"BPIXLY2_NonFlippedz0/F");

  //L3
  bctree_->Branch("BPIXLY3x0",&BPIXLY3x0_,"BPIXLY3x0/F");
  bctree_->Branch("BPIXLY3y0",&BPIXLY3y0_,"BPIXLY3y0/F");
  bctree_->Branch("BPIXLY3z0",&BPIXLY3z0_,"BPIXLY3z0/F");

  bctree_->Branch("BPIXLY3_Flippedx0",&BPIXLY3_Flippedx0_,"BPIXLY3_Flippedx0/F");
  bctree_->Branch("BPIXLY3_Flippedy0",&BPIXLY3_Flippedy0_,"BPIXLY3_Flippedy0/F");
  bctree_->Branch("BPIXLY3_Flippedz0",&BPIXLY3_Flippedz0_,"BPIXLY3_Flippedz0/F");

  bctree_->Branch("BPIXLY3_NonFlippedx0",&BPIXLY3_NonFlippedx0_,"BPIXLY3_NonFlippedx0/F");
  bctree_->Branch("BPIXLY3_NonFlippedy0",&BPIXLY3_NonFlippedy0_,"BPIXLY3_NonFlippedy0/F");
  bctree_->Branch("BPIXLY3_NonFlippedz0",&BPIXLY3_NonFlippedz0_,"BPIXLY3_NonFlippedz0/F");

  //L4
  bctree_->Branch("BPIXLY4x0",&BPIXLY4x0_,"BPIXLY4x0/F");
  bctree_->Branch("BPIXLY4y0",&BPIXLY4y0_,"BPIXLY4y0/F");
  bctree_->Branch("BPIXLY4z0",&BPIXLY4z0_,"BPIXLY4z0/F");

  bctree_->Branch("BPIXLY4_Flippedx0",&BPIXLY4_Flippedx0_,"BPIXLY4_Flippedx0/F");
  bctree_->Branch("BPIXLY4_Flippedy0",&BPIXLY4_Flippedy0_,"BPIXLY4_Flippedy0/F");
  bctree_->Branch("BPIXLY4_Flippedz0",&BPIXLY4_Flippedz0_,"BPIXLY4_Flippedz0/F");

  bctree_->Branch("BPIXLY4_NonFlippedx0",&BPIXLY4_NonFlippedx0_,"BPIXLY4_NonFlippedx0/F");
  bctree_->Branch("BPIXLY4_NonFlippedy0",&BPIXLY4_NonFlippedy0_,"BPIXLY4_NonFlippedy0/F");
  bctree_->Branch("BPIXLY4_NonFlippedz0",&BPIXLY4_NonFlippedz0_,"BPIXLY4_NonFlippedz0/F");
 

  // per-ladder
  bctree_->Branch("vBPIXLY1y0",&vBPIXLY1y0_);
  bctree_->Branch("vBPIXLY1z0",&vBPIXLY1z0_);

  bctree_->Branch("vBPIXLY2x0",&vBPIXLY2x0_);
  bctree_->Branch("vBPIXLY2y0",&vBPIXLY2y0_);
  bctree_->Branch("vBPIXLY2z0",&vBPIXLY2z0_);

  bctree_->Branch("vBPIXLY3x0",&vBPIXLY3x0_);
  bctree_->Branch("vBPIXLY3y0",&vBPIXLY3y0_);
  bctree_->Branch("vBPIXLY3z0",&vBPIXLY3z0_);

  bctree_->Branch("vBPIXLY4x0",&vBPIXLY4x0_);
  bctree_->Branch("vBPIXLY4y0",&vBPIXLY4y0_);
  bctree_->Branch("vBPIXLY4z0",&vBPIXLY4z0_);

}

// ------------ method called once each job just after ending the event loop  ------------
void 
PixelBaryCentreAnalyzer::endJob() 
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
PixelBaryCentreAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(PixelBaryCentreAnalyzer);
