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

      std::vector<float> BPIXLY1x0_, BPIXLY1y0_, BPIXLY1z0_;
      std::vector<float> BPIXLY2x0_, BPIXLY2y0_, BPIXLY2z0_;
      std::vector<float> BPIXLY3x0_, BPIXLY3y0_, BPIXLY3z0_;
      std::vector<float> BPIXLY4x0_, BPIXLY4y0_, BPIXLY4z0_;

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

  BPIXLY1x0_.clear(); BPIXLY1y0_.clear(); BPIXLY1z0_.clear();
  BPIXLY2x0_.clear(); BPIXLY2y0_.clear(); BPIXLY2z0_.clear();
  BPIXLY3x0_.clear(); BPIXLY3y0_.clear(); BPIXLY3z0_.clear();
  BPIXLY4x0_.clear(); BPIXLY4y0_.clear(); BPIXLY4z0_.clear();

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

     std::map<int, float> nmodules_L1, nmodules_L2, nmodules_L3, nmodules_L4;
     std::map<int, TVector3> barycenter_L1, barycenter_L2, barycenter_L3, barycenter_L4;

     TVector3 barycenter_BPIX;
     float nmodules_BPIX(0.);

     TVector3 barycenter_FPIX;
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
           barycenter_BPIX += ali_translation;
           
           int layer  = coord_.layer( DetId(ali.rawId()) );
           int ladder = coord_.ladder( DetId(ali.rawId()) );

           switch (layer) {
             case 1:
               nmodules_L1[ladder] += 1;
               barycenter_L1[ladder] += ali_translation;
               break;
             case 2:
               nmodules_L2[ladder] += 1;
               barycenter_L2[ladder] += ali_translation;
               break;
             case 3:
               nmodules_L3[ladder] += 1;
               barycenter_L3[ladder] += ali_translation;
               break;
             case 4:
               nmodules_L4[ladder] += 1;
               barycenter_L4[ladder] += ali_translation;
               break;
             default:
               edm::LogError("PixelBaryCentreAnalyzer") << "Unrecognized BPIX layer " << layer << std::endl;
               break;
           }  // switch on layer of BPIX

        } // BPIX

        // FPIX
        if (subid == PixelSubdetector::PixelEndcap){
           nmodules_FPIX += 1; 
           barycenter_FPIX += ali_translation;
        } // FPIX

     }// loop over tracker module

     TVector3 barycenter_PIX = barycenter_BPIX + barycenter_FPIX; 
     float nmodules_PIX = nmodules_BPIX + nmodules_FPIX;
     PIXx0_ = barycenter_PIX.X()/nmodules_PIX + globalTkPosition.X();
     PIXy0_ = barycenter_PIX.Y()/nmodules_PIX + globalTkPosition.Y();
     PIXz0_ = barycenter_PIX.Z()/nmodules_PIX + globalTkPosition.Z();

     BPIXx0_ = barycenter_BPIX.X()/nmodules_BPIX + globalTkPosition.X();
     BPIXy0_ = barycenter_BPIX.Y()/nmodules_BPIX + globalTkPosition.Y() ;
     BPIXz0_ = barycenter_BPIX.Z()/nmodules_BPIX + globalTkPosition.Z();

     FPIXx0_ = barycenter_FPIX.X()/nmodules_FPIX + globalTkPosition.X();
     FPIXy0_ = barycenter_FPIX.Y()/nmodules_FPIX + globalTkPosition.Y();
     FPIXz0_ = barycenter_FPIX.Z()/nmodules_FPIX + globalTkPosition.Z();

     // barycentre per layer/ladder 
     for (std::map<int, TVector3>::iterator it = barycenter_L1.begin(); it != barycenter_L1.end(); ++it) {
         int ladder = it->first;
         barycenter_L1[ladder] = (1/nmodules_L1[ladder])*it->second;
         BPIXLY1x0_.push_back(barycenter_L1[ladder].X() + globalTkPosition.X());
         BPIXLY1y0_.push_back(barycenter_L1[ladder].Y() + globalTkPosition.Y());
         BPIXLY1z0_.push_back(barycenter_L1[ladder].Z() + globalTkPosition.Z());
     }
     for (std::map<int, TVector3>::iterator it = barycenter_L2.begin(); it != barycenter_L2.end(); ++it) {
         int ladder = it->first;
         barycenter_L2[ladder] = (1/nmodules_L2[ladder])*it->second;
         BPIXLY2x0_.push_back(barycenter_L2[ladder].X() + globalTkPosition.X());
         BPIXLY2y0_.push_back(barycenter_L2[ladder].Y() + globalTkPosition.Y());
         BPIXLY2z0_.push_back(barycenter_L2[ladder].Z() + globalTkPosition.Z());
     }
     for (std::map<int, TVector3>::iterator it = barycenter_L3.begin(); it != barycenter_L3.end(); ++it) {
         int ladder = it->first;
         barycenter_L3[ladder] = (1/nmodules_L3[ladder])*it->second;
         BPIXLY3x0_.push_back(barycenter_L3[ladder].X() + globalTkPosition.X());
         BPIXLY3y0_.push_back(barycenter_L3[ladder].Y() + globalTkPosition.Y());
         BPIXLY3z0_.push_back(barycenter_L3[ladder].Z() + globalTkPosition.Z());
     }
     for (std::map<int, TVector3>::iterator it = barycenter_L4.begin(); it != barycenter_L4.end(); ++it) {
         int ladder = it->first;
         barycenter_L4[ladder] = (1/nmodules_L4[ladder])*it->second;
         BPIXLY4x0_.push_back(barycenter_L4[ladder].X() + globalTkPosition.X());
         BPIXLY4y0_.push_back(barycenter_L4[ladder].Y() + globalTkPosition.Y());
         BPIXLY4z0_.push_back(barycenter_L4[ladder].Z() + globalTkPosition.Z());
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

  bctree_->Branch("PIXx0",&PIXx0_,"PIXx0/F");
  bctree_->Branch("PIXy0",&PIXy0_,"PIXy0/F");
  bctree_->Branch("PIXz0",&PIXz0_,"PIXz0/F");

  bctree_->Branch("BPIXx0",&BPIXx0_,"BPIXx0/F");
  bctree_->Branch("BPIXy0",&BPIXy0_,"BPIXy0/F");
  bctree_->Branch("BPIXz0",&BPIXz0_,"BPIXz0/F");

  bctree_->Branch("BPIXLY1x0",&BPIXLY1x0_);
  bctree_->Branch("BPIXLY1y0",&BPIXLY1y0_);
  bctree_->Branch("BPIXLY1z0",&BPIXLY1z0_);

  bctree_->Branch("BPIXLY2x0",&BPIXLY2x0_);
  bctree_->Branch("BPIXLY2y0",&BPIXLY2y0_);
  bctree_->Branch("BPIXLY2z0",&BPIXLY2z0_);

  bctree_->Branch("BPIXLY3x0",&BPIXLY3x0_);
  bctree_->Branch("BPIXLY3y0",&BPIXLY3y0_);
  bctree_->Branch("BPIXLY3z0",&BPIXLY3z0_);

  bctree_->Branch("BPIXLY4x0",&BPIXLY4x0_);
  bctree_->Branch("BPIXLY4y0",&BPIXLY4y0_);
  bctree_->Branch("BPIXLY4z0",&BPIXLY4z0_);

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
