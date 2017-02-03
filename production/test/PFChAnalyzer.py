# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: Configuration/Generator/python/SinglePi0E10_cfi.py -s GEN,SIM,DIGI,L1,DIGI2RAW,RAW2DIGI,RECO -n 10 --no_exec --conditions=auto:mc
import FWCore.ParameterSet.Config as cms

process = cms.Process('ANA')

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
#process.load('Configuration.Geometry.GeometrySimDB_cff')
process.load("Configuration.Geometry.GeometryIdeal_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

# Input source
process.source = cms.Source("PoolSource",
    #fileNames = cms.untracked.vstring(['root://eoscms.cern.ch//store/group/dpg_hcal/comm_hcal/RecoAlgos/privatePionGun/PFjetMET/step3_pi500_2017_realistic_PF_PhotoStatUnc.root'])
    #fileNames = cms.untracked.vstring(['root://eoscms.cern.ch//store/group/dpg_hcal/comm_hcal/RecoAlgos/privatePionGun/PFjetMET/step3_pi500_2017_realistic_PF.root'])
    #fileNames = cms.untracked.vstring(['root://eoscms.cern.ch//store/group/dpg_hcal/comm_hcal/RecoAlgos/privatePionGun/PFjetMET/step3_pi-20-30_10k_2017_realistic_PF.root'])
    #fileNames = cms.untracked.vstring(['root://eoscms.cern.ch//store/group/dpg_hcal/comm_hcal/RecoAlgos/privatePionGun/PFjetMET/step3_pi-20-30_10k_2017_realistic_PF_PhotoStatUnc.root'])
    #fileNames = cms.untracked.vstring(['root://eoscms.cern.ch//store/group/dpg_hcal/comm_hcal/RecoAlgos/privatePionGun/PFjetMET/step3_pi500_10k_2017_realistic_PF.root'])
    #fileNames = cms.untracked.vstring(['root://eoscms.cern.ch//store/group/dpg_hcal/comm_hcal/RecoAlgos/privatePionGun/PFjetMET/step3_pi500_10k_2017_realistic_PF_PhotoStatUnc.root'])
    #fileNames = cms.untracked.vstring(['root://eoscms.cern.ch//store/group/dpg_hcal/comm_hcal/RecoAlgos/privatePionGun/PFjetMET/step3_pi-90-100_10k_2017_realistic_PF.root'])
    fileNames = cms.untracked.vstring(['root://eoscms.cern.ch//store/group/dpg_hcal/comm_hcal/RecoAlgos/privatePionGun/PFjetMET/step3_pi-90-100_10k_2017_realistic_PF_PhotoStatUnc.root'])

)

#'root://eoscms.cern.ch//store/group/dpg_hcal/comm_hcal/RecoAlgos/privatePionGun/PFjetMET/step3_pi500_2017_realistic_PF_PhotoStatUnc.root'
#'root://eoscms.cern.ch//store/group/dpg_hcal/comm_hcal/RecoAlgos/privatePionGun/PFjetMET/step3_pi500_2017_realistic_PF.root'

process.options = cms.untracked.PSet(
)

#from Configuration.AlCa.GlobalTag import GlobalTag
#process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:mc', '')

process.pfChargedHadronAnalyzer = cms.EDAnalyzer(
    "PFChargedHadronAnalyzer",
    PFCandidates = cms.InputTag("particleFlow"),
    PFSimParticles = cms.InputTag("particleFlowSimParticle"),
    EcalPFClusters = cms.InputTag("particleFlowClusterECAL"),
    HcalPFClusters = cms.InputTag("particleFlowClusterHCAL"),
    ptMin = cms.double(1.),                     # Minimum pt
    pMin = cms.double(1.),                      # Minimum p
    nPixMin = cms.int32(2),                     # Nb of pixel hits
    nHitMin = cms.vint32(14,17,20,17,10),       # Nb of track hits
    nEtaMin = cms.vdouble(1.4,1.6,2.0,2.4,2.6), # in these eta ranges
    hcalMin = cms.double(0.5),                   # Minimum hcal energy
    ecalMax = cms.double(1E9),                  # Maximum ecal energy 
    verbose = cms.untracked.bool(True),         # not used.
    rootOutputFile = cms.string("PGun.root"),# the root tree
#    IsMinBias = cms.untracked.bool(False)
)



process.bla = cms.EndPath(process.pfChargedHadronAnalyzer)

# Schedule definition
process.schedule = cms.Schedule(process.bla)
