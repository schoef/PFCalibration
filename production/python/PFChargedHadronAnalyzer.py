import FWCore.ParameterSet.Config as cms
process = cms.Process('Ana')

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10) )
process.source = cms.Source(
    'PoolSource',
#    fileNames = cms.untracked.vstring('root://eoscms.cern.ch//store/relval/CMSSW_7_4_12/RelValProdTTbar/GEN-SIM-RECO/74X_mcRun1_realistic_v1-v1/00000/54A3F5C8-3D5C-E511-8C2E-00261894387A.root')
    fileNames = cms.untracked.vstring('root://eoscms.cern.ch//store/data/Run2015D/SingleMuon/RECO/PromptReco-v3/000/256/629/00000/48900836-F35E-E511-BECE-02163E014306.root')
    )


# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
#process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
#process.load('Configuration.Geometry.GeometrySimDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.Generator_cff')
process.load('IOMC.EventVertexGenerators.VtxSmearedNominalCollision2015_cfi')
#process.load('GeneratorInterface.Core.genFilterSummary_cff')
#process.load('Configuration.StandardSequences.SimIdeal_cff')
#process.load('Configuration.StandardSequences.Digi_cff')
#process.load('Configuration.StandardSequences.SimL1Emulator_cff')
#process.load('Configuration.StandardSequences.DigiToRaw_cff')
#process.load('Configuration.StandardSequences.RawToDigi_cff')
#process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(100)
)

process.options = cms.untracked.PSet(

)

# Output definition

process.RECOSIMoutput = cms.OutputModule("PoolOutputModule",
    splitLevel = cms.untracked.int32(0),
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    outputCommands = cms.untracked.vstring("keep *"),
    fileName = cms.untracked.string('SinglePi0E10_cfi_py_GEN_SIM_DIGI_L1_DIGI2RAW_RAW2DIGI_RECO.root'),
    dataset = cms.untracked.PSet(
        filterName = cms.untracked.string(''),
        dataTier = cms.untracked.string('')
    ),
)

# Additional output definition

# Other statements
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:mc', '')

### ===========================================================
### ZS removal
###
#process.simHcalDigis.useConfigZSvalues=cms.int32(1)
#process.simHcalDigis.HBlevel=cms.int32(-999)
#process.simHcalDigis.HElevel=cms.int32(-999)
#process.simHcalDigis.HOlevel=cms.int32(-999)
#process.simHcalDigis.HFlevel=cms.int32(-999)
#process.simEcalDigis.srpBarrelLowInterestChannelZS = cms.double(-1.e9)
#process.simEcalDigis.srpEndcapLowInterestChannelZS = cms.double(-1.e9)

#process.GlobalTag.toGet = cms.VPSet(
#    cms.PSet(record = cms.string("EcalSRSettingsRcd"),
#             tag = cms.string('EcalSRSettings_fullreadout_v01_mc'),
#             connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_34X_ECAL")
#))

###===========================================================

#process.RandomNumberGeneratorService.generator.initialSeed = XXXX

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


# Path and EndPath definitions
process.endjob_step = cms.EndPath(process.endOfProcess)
process.RECOSIMoutput_step = cms.EndPath(process.RECOSIMoutput)
process.bla = cms.EndPath(process.pfChargedHadronAnalyzer)
# Schedule definition
process.schedule = cms.Schedule(process.endjob_step,process.bla)

