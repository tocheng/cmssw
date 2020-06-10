import FWCore.ParameterSet.Config as cms

process = cms.Process("READ")

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.destinations = ['cout', 'cerr']
process.MessageLogger.cerr.FwkReport.reportEvery = 1000000                            # do not clog output with IO

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100000) )       # large number of events is needed since we probe 5000LS for run (see below)

####################################################################
# Empty source 
####################################################################
#import FWCore.PythonUtilities.LumiList as LumiList
#DCSJson='/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/DCSOnly/json_DCSONLY.txt'

process.source = cms.Source("EmptySource",
                            firstRun = cms.untracked.uint32(294927),
                            firstLuminosityBlock = cms.untracked.uint32(1),           # probe one LS after the other
                            numberEventsInLuminosityBlock = cms.untracked.uint32(1),  # probe one event per LS
                            numberEventsInRun = cms.untracked.uint32(1),           # a number of events > the number of LS possible in a real run (5000 s ~ 32 h)
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
    tag = cms.string("TrackerAlignment_PCL_byRun_v2_express"),
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
                  rawFileName = cms.untracked.string('prompt_BeamSpotObjects_PCL_byLumi_v0_prompt.txt')
                  )

####################################################################
# Output file
####################################################################
process.TFileService = cms.Service("TFileService",
                                   fileName=cms.string("PixelBaryCentre_Prompt.root")
                                   ) 

# Put module in path:
process.p = cms.Path(process.PixelBaryCentreAnalyzer)
