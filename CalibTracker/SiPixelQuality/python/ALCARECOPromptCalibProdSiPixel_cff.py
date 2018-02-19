import FWCore.ParameterSet.Config as cms

# Set the HLT paths
import HLTrigger.HLTfilters.hltHighLevel_cfi
hltFilterForSiPixelStatus = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone(
    andOr = True, ## choose logical OR between Triggerbits
    eventSetupPathsKey = 'PromptCalibProdSiPixel',
    throw = False # tolerate triggers stated above, but not available
    )

seqALCARECOPromptCalibProdSiPixel = cms.Sequence(hltFilterForSiPixelStatus) 
