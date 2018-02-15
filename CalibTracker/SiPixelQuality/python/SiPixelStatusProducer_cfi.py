import FWCore.ParameterSet.Config as cms

siPixelStatusProducer = cms.EDProducer("SiPixelStatusProducer",
    SiPixelStausProducerParameters = cms.PSet(
        pixelClusterLabel = cms.untracked.InputTag("siPixelClusters"),
	resetEveryNLumi = cms.untracked.int32( 1 )
    )
)

