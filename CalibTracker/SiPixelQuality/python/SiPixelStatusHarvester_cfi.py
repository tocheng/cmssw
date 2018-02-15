import FWCore.ParameterSet.Config as cms

siPixelStatusHarvester = cms.EDAnalyzer("SiPixelStatusHarvester",

    SiPixelStatusHarvesterParameters = cms.PSet(
	OutputBase = cms.untracked.string("nLumibased"), #runbased #dynamicLumibased
        ResetEveryNLumi = cms.untracked.int(10),
	ModuleName = cms.untracked.string("siPixelStatusProducer"),
	Label      = cms.untracked.string("siPixelStatus"),
	RecordName   = cms.untracked.string("SiPixelQualityFromDbRcd"),
	DumpTxt            = cms.untracked.bool(False),
	TxtFileName        = cms.untracked.string("SiPixelBadComponent"),
    )
)

