import FWCore.ParameterSet.Config as cms
from RecoMuon.TrackingTools.MuonServiceProxy_cff import *
import FWCore.PythonUtilities.LumiList as LumiList
import FWCore.ParameterSet.Types as CfgTypes
process = cms.Process("AOD2NanoAOD")

process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.threshold = 'INFO'
process.MessageLogger.categories.append('AOD2NanoAOD')
process.MessageLogger.cerr.INFO = cms.untracked.PSet(
    limit=cms.untracked.int32(-1))
process.options = cms.untracked.PSet(wantSummary=cms.untracked.bool(True))

# Set the maximum number of events to be processed (-1 processes all events)
process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(-1))

# Define files of dataset
import FWCore.Utilities.FileUtils as FileUtils
files2011data = FileUtils.loadListFromFile(
    'datasets/CMS_Run2011A_DoubleMu_AOD_12Oct2013-v1_merged.txt')
process.source = cms.Source(
    "PoolSource", fileNames=cms.untracked.vstring(*files2011data))

# Apply JSON file (needs to be done after the process.source definition)
goodJSON = 'datasets/Cert_160404-180252_7TeV_ReRecoNov08_Collisions11_JSON.txt'
myLumis = LumiList.LumiList(filename=goodJSON).getCMSSWString().split(',')
process.source.lumisToProcess = CfgTypes.untracked(
    CfgTypes.VLuminosityBlockRange())
process.source.lumisToProcess.extend(myLumis)

# Number of events to be skipped (0 by default)
process.source.skipEvents = cms.untracked.uint32(0)

# Register fileservice for output file
process.aod2nanoaod = cms.EDAnalyzer('AOD2NanoAOD')
process.TFileService = cms.Service(
    "TFileService", fileName=cms.string('NanoAOD.root'))

process.p = cms.Path(process.aod2nanoaod)
