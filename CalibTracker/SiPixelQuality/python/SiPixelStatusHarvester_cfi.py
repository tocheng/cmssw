import FWCore.ParameterSet.Config as cms

siPixelStatusHarvester = cms.EDAnalyzer("SiPixelStatusHarvester",

    SiPixelStatusHarvesterParameters = cms.PSet(
	outputBase = cms.untracked.string("nLumibased"), #runbased #dynamicLumibased
        resetEveryNLumi = cms.untracked.int(10),
	moduleName = cms.untracked.string("siPixelStatusProducer"),
	label      = cms.untracked.string("siPixelStatus"),
	recordName   = cms.untracked.string("SiPixelQualityFromDbRcd"),
	dumpTxt            = cms.untracked.bool(False),
	txtFileName        = cms.untracked.string("SiPixelBadComponent"),
    )
)

