#ifndef SiPixelStatusHarvester_H
#define SiPixelStatusHarvester_H

/** \class SiPixelStatusHarvester
 *  harvest per-lumi prduced SiPixelDetector status and make the payload for SiPixelQualityFromDB
 *
 *  \author 
 */
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

class SiPixelStatusHarvester : public edm::EDAnalyzer {
 public:

  // Constructor
  SiPixelStatusHarvester(const edm::ParameterSet&);

  // Destructor
  ~SiPixelStatusHarvester() override;
  
  // Operations
  void beginJob            () override;
  void endJob              () override;  
  void analyze             (const edm::Event&          , const edm::EventSetup&) override;
  void beginRun            (const edm::Run&            , const edm::EventSetup&) override;
  void endRun              (const edm::Run&            , const edm::EventSetup&) override;
  void beginLuminosityBlock(const edm::LuminosityBlock&, const edm::EventSetup&) override;
  void endLuminosityBlock  (const edm::LuminosityBlock&, const edm::EventSetup&) override;

 protected:

 private:
  // Parameters
  std::string 	      outputBase_;
  std::string 	      recordName_;
  bool                dumpTxt_;
  std::string 	      outTxtFileName_;
  // harvest helper classs that setup the IOV structure
  SiPixelStatusManager siPixelStatusManager_;

};

#endif
