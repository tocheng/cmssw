import FWCore.ParameterSet.Config as cms

process = cms.Process("READ")

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

import FWCore.ParameterSet.VarParsing as VarParsing

options = VarParsing.VarParsing()
options.register('lumisPerRun',
                1,
                VarParsing.VarParsing.multiplicity.singleton,
                VarParsing.VarParsing.varType.int,
                "the number of lumis to be processed per-run.")
options.register('firstRun',
                294927,
                VarParsing.VarParsing.multiplicity.singleton,
                VarParsing.VarParsing.varType.int,
                "the first run number be processed")

options.parseArguments()

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.destinations = ['cout', 'cerr']
process.MessageLogger.cerr.FwkReport.reportEvery = options.lumisPerRun*100   # do not clog output with I/O


process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.lumisPerRun*40000) )       # large number of events is needed since we probe 5000LS for run (see below)

####################################################################
# Empty source 
####################################################################
#import FWCore.PythonUtilities.LumiList as LumiList
#DCSJson='/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/DCSOnly/json_DCSONLY.txt'

process.source = cms.Source("EmptySource",
                            firstRun = cms.untracked.uint32(options.firstRun),
                            firstLuminosityBlock = cms.untracked.uint32(1),           # probe one LS after the other
                            numberEventsInLuminosityBlock = cms.untracked.uint32(1),  # probe one event per LS
                            numberEventsInRun = cms.untracked.uint32(options.lumisPerRun),           # a number of events > the number of LS possible in a real run (5000 s ~ 32 h)
                            )

####################################################################
# Connect to conditions DB
####################################################################

# either from Global Tag
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag,"auto:run2_data")

process.GlobalTag.toGet.append(
  cms.PSet(
    record = cms.string("TrackerAlignmentRcd"),
    label = cms.untracked.string("prompt"),
    tag = cms.string("TrackerAlignment_PCL_byRun_v2_express"),
    connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS")
  )
)

process.GlobalTag.toGet.append(
  cms.PSet(
    record = cms.string("BeamSpotObjectsRcd"),
    label = cms.untracked.string("prompt"),
    tag = cms.string("BeamSpotObjects_PCL_byLumi_v0_prompt"),
    connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS")
  )
)

# ...or specify database connection and tag:  
#from CondCore.CondDB.CondDB_cfi import *
#CondDBBeamSpotObjects = CondDB.clone(connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'))
#process.dbInput = cms.ESSource("PoolDBESSource",
#                               CondDBBeamSpotObjects,
#                               toGet = cms.VPSet(cms.PSet(record = cms.string('BeamSpotObjectsRcd'),
#                                                          tag = cms.string('BeamSpotObjects_PCL_byLumi_v0_prompt') #choose your own favourite
#                                                          )
#                                                 )
#                               )

####################################################################
# Load and configure analyzer
####################################################################
process.PixelBaryCentreAnalyzer = cms.EDAnalyzer("PixelBaryCentreAnalyzer",
                    usePixelQuality = cms.untracked.bool(False)
                  )

process.PixelBaryCentreAnalyzerWithPixelQuality = cms.EDAnalyzer("PixelBaryCentreAnalyzer",
                    usePixelQuality = cms.untracked.bool(True)
                  )



####################################################################
# Output file
####################################################################
process.TFileService = cms.Service("TFileService",
                                   fileName=cms.string("PixelBaryCentre_Rereco.root")
                                   ) 

# Put module in path:
process.p = cms.Path(process.PixelBaryCentreAnalyzer*process.PixelBaryCentreAnalyzerWithPixelQuality)
