from WMCore.Configuration import Configuration
config = Configuration()

config.section_("General")
config.General.requestName = 'PGun76X_M1-1p_2'
config.General.workArea = 'PGun76X_M1-1p_2'
config.General.transferLogs = True
#optional
#config.General.transferOutputs
#config.General.transferLogs
#config.General.failureLimit = 

#Expert use
#config.General.instance
#config.General.activity

config.section_("JobType")
config.JobType.pluginName = 'PrivateMC'
config.JobType.psetName = '../test/PGunWithGeneration.py'
config.JobType.outputFiles = ['PGun.root']
config.JobType.eventsPerLumi = 5000

config.section_("Data")
#config.Data.inputDataset = '/Single_Pion_gun_13TeV_pythia8/Fall14DR73-NoPU_MCRUN2_73_V9-v1/GEN-SIM-RAW-RECO'
#config.Data.primaryDataset = ''
config.Data.splitting = 'EventBased'
config.Data.unitsPerJob = 500
config.Data.totalUnits = 1000000
config.Data.publication = False
#config.Data.publishDBS = '' default for the moment

config.section_("Site")
config.Site.storageSite = 'T2_AT_Vienna'
#config.Site.storageSite = 'T2_CH_CERN'
#config.Site.blacklist = ['T1_US_FNAL']
#config.Site.whitelist = ['T2_CH_CERN']

#config.section_("User")
#config.section_("Debug")
