# Auto generated configuration file
# using: 
# Revision: 1.381.2.28 
# Source: /local/reps/CMSSW/CMSSW/Configuration/PyReleaseValidation/python/ConfigBuilder.py,v 
# with command line options: step4 --data --conditions auto:com10 --scenario pp -s ALCAHARVEST:SiStripGains --filein file:PromptCalibProdSiStripGains.root -n -1 --no_exec
import FWCore.ParameterSet.Config as cms

process = cms.Process('ALCAHARVEST')

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.AlCaHarvesting_cff')
process.load('Configuration.Geometry.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

# Input source
process.source = cms.Source("PoolSource",
    secondaryFileNames = cms.untracked.vstring(),
    fileNames = cms.untracked.vstring(),
    processingMode = cms.untracked.string('RunsAndLumis')
)

process.source.fileNames.extend(['file:SiPixelCalZeroBias.root'])

process.options = cms.untracked.PSet(
    Rethrow = cms.untracked.vstring('ProductNotFound'),
    fileMode = cms.untracked.string('FULLMERGE')
)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    version = cms.untracked.string('$Revision: 1.381.2.28 $'),
    annotation = cms.untracked.string('step4 nevts:-1'),
    name = cms.untracked.string('PyReleaseValidation')
)

# Output definition

# Additional output definition

# Other statements
process.metadata = cms.PSet(record = cms.untracked.string('SiPixelQualityFromDbRcd'))
process.dbOutput = cms.PSet(record = cms.string('SiPixelQualityFromDbRcd'),
                                              tag = cms.string('SiPixelQualityFromDbRcd_ByLumi'),
                                              timetype   = cms.untracked.string('lumiid')
)

process.PoolDBOutputService.toPut.append(process.dbOutput)
process.pclMetadataWriter.recordsToMap.append(process.metadata)

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_data', '')

# Path and EndPath definitions
process.ALCAHARVESTSiPixelQuality = cms.EDAnalyzer("SiPixelStatusHarvester",

    SiPixelStatusHarvesterParameters = cms.PSet(
        outputBase = cms.untracked.string("nLumibased"), #runbased #dynamicLumibased
        resetEveryNLumi = cms.untracked.int32(1),
        moduleName = cms.untracked.string("siPixelStatusProducer"),
        label      = cms.untracked.string("siPixelStatus"),
        recordName   = cms.untracked.string("SiPixelQualityFromDbRcd"),
        dumpTxt            = cms.untracked.bool(True),
        txtFileName        = cms.untracked.string("SiPixelBadComponent"),
    )
)

process.SiPixelQuality = cms.Path(process.ALCAHARVESTSiPixelQuality)
process.MetadataWriter = cms.Path(process.pclMetadataWriter)

# Schedule definition
process.schedule = cms.Schedule(process.SiPixelQuality,
                                process.MetadataWriter)
