import FWCore.ParameterSet.Config as cms

# Set the HLT paths
import HLTrigger.HLTfilters.hltHighLevel_cfi
hltFilterForSiPixelStatus = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone(
    andOr = True, ## choose logical OR between Triggerbits
##     HLTPaths = [
##     #Minimum Bias
##     ??"HLT_ZeroBias_v*",'HLT_ZeroBias_AfterAbortGap_v*' 
##     ],
    eventSetupPathsKey = 'PromptCalibProdSiPixel',
    throw = False # tolerate triggers stated above, but not available
    )

# ------------------------------------------------------------------------------
# configure the beam-spot production
from CalibTracker.SiPixelQuality.SiPixelStatusProducer_cff import *
# fit as function of lumi sections
siPixelStatusProducer.SiPxielStatusProducerParameters.resetEveryNLumi = 1
                     
seqALCARECOPromptCalibProdSiPixel = cms.Sequence(hltFilterForSiPixelStatus*siPixelStatusProducer) 
