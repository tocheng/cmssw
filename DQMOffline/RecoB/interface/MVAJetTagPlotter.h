#ifndef MVAJetTagPlotter_H
#define MVAJetTagPlotter_H

#include <vector>
#include <string>

#include "DQMOffline/RecoB/interface/BaseTagInfoPlotter.h"
#include "DQMOffline/RecoB/interface/TaggingVariablePlotter.h"
#include "DataFormats/BTauReco/interface/BaseTagInfo.h"
#include "RecoBTau/JetTagComputer/interface/GenericMVAJetTagComputer.h"
#include "DQMOffline/RecoB/interface/FlavourHistorgrams.h"
// #include "RecoBTag/MCTools/interface/JetFlavour.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"


class MVAJetTagPlotter: public BaseTagInfoPlotter {

 public:

  MVAJetTagPlotter (const std::string & tagName, const EtaPtBin & etaPtBin,
		    const edm::ParameterSet& pSet, const std::string& folderName, 
		    unsigned int mc, bool willFinalize, DQMStore::IBooker & ibook);

  ~MVAJetTagPlotter ();

  virtual void analyzeTag (const std::vector<const reco::BaseTagInfo *> & baseTagInfos, double jec, int jetFlavour, float w=1);

  virtual void finalize (DQMStore::IBooker & ibook_, DQMStore::IGetter & igetter_);

  void epsPlot(const std::string & name);

  void psPlot(const std::string & name);

  virtual void setEventSetup (const edm::EventSetup & setup);
  virtual std::vector<std::string> tagInfoRequirements () const;

 private:

  std::string jetTagComputer;
  const GenericMVAJetTagComputer *computer;

  reco::TaggingVariableName categoryVariable;
  std::vector<std::unique_ptr<TaggingVariablePlotter>> categoryPlotters;
};

#endif
