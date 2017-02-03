#include "PFCalibration/PFChargedHadronAnalyzer/plugins/PFChargedHadronAnalyzer.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlock.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementTrack.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementCluster.h"

#include "DataFormats/ParticleFlowReco/interface/PFCluster.h"
#include "DataFormats/ParticleFlowReco/interface/PFRecHit.h"
#include "DataFormats/ParticleFlowReco/interface/PFRecHitFraction.h"

#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h" 
#include "DataFormats/EcalRecHit/interface/EcalUncalibratedRecHit.h" 
#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h" 
#include "DataFormats/HcalRecHit/interface/HcalRecHitFwd.h"

#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include "SimDataFormats/CaloHit/interface/PCaloHit.h"

#include <TROOT.h>
#include <TVector3.h>

using namespace std;
using namespace edm;
using namespace reco;

PFChargedHadronAnalyzer::PFChargedHadronAnalyzer(const edm::ParameterSet& iConfig) {
  
  nCh = std::vector<unsigned int>(10,static_cast<unsigned int>(0));
  nEv = std::vector<unsigned int>(2,static_cast<unsigned int>(0));

  inputTagPFCandidates_ 
    = iConfig.getParameter<InputTag>("PFCandidates");
  tokenPFCandidates_ = consumes<PFCandidateCollection>(inputTagPFCandidates_); 

  inputTagPFSimParticles_ 
    = iConfig.getParameter<InputTag>("PFSimParticles");
  tokenPFSimParticles_ =consumes<PFSimParticleCollection>(inputTagPFSimParticles_);

  inputTagEcalPFClusters_ 
    = iConfig.getParameter<InputTag>("EcalPFClusters");
  tokenEcalPFClusters_ = consumes<reco::PFClusterCollection>(inputTagEcalPFClusters_);

  // Smallest track pt
  ptMin_ = iConfig.getParameter<double>("ptMin");

  // Smallest track p
  pMin_ = iConfig.getParameter<double>("pMin");

  // Smallest raw HCAL energy linked to the track
  hcalMin_ = iConfig.getParameter<double>("hcalMin");

  // Largest ECAL energy linked to the track to define a MIP
  ecalMax_ = iConfig.getParameter<double>("ecalMax");

  // Smallest number of pixel hits
  nPixMin_ = iConfig.getParameter<int>("nPixMin");

  // Smallest number of track hits in different eta ranges
  nHitMin_ = iConfig.getParameter< std::vector<int> > ("nHitMin");
  nEtaMin_ = iConfig.getParameter< std::vector<double> > ("nEtaMin");

  //Is minbias from simulation
  isMBMC_ = iConfig.getUntrackedParameter<bool>("isMinBiasMC",false);

  verbose_ = 
    iConfig.getUntrackedParameter<bool>("verbose",false);

  LogDebug("PFChargedHadronAnalyzer")
    <<" input collection : "<<inputTagPFCandidates_ ;
   

  // The root tuple
  outputfile_ = iConfig.getParameter<std::string>("rootOutputFile"); 
  tf1 = new TFile(outputfile_.c_str(), "RECREATE");  
  s = new TTree("s"," PFCalibration");

  s->Branch("true",&true_,"true/F");  
  s->Branch("p",&p_,"p/F");  
  s->Branch("ecal",&ecal_,"ecal/F");  
  s->Branch("hcal",&hcal_,"hcal/F");  
  s->Branch("ho",&ho_,"ho/F");  
  s->Branch("eta",&eta_,"eta/F");  
  s->Branch("phi",&phi_,"phi/F");
  s->Branch("charge",&charge_,"charge/I");
  

  s->Branch("pfcs",&pfcsID);

  //Track position at ECAL entrance
  // s->Branch("etaAtEcal",&etaEcal_,"eta/F");  
  // s->Branch("phiAtEcal",&phiEcal_,"phi/F");
  
  // s->Branch("cluEcalE",&cluEcalE);
  // s->Branch("cluEcalEta",&cluEcalEta);
  // s->Branch("cluEcalPhi",&cluEcalPhi);

  // s->Branch("distEcalTrk",&distEcalTrk);

  // s->Branch("cluHcalE",&cluHcalE);
  // s->Branch("cluHcalEta",&cluHcalEta);
  // s->Branch("cluHcalPhi",&cluHcalPhi);

  // s->Branch("distHcalTrk",&distHcalTrk);
  // s->Branch("distHcalEcal",&distHcalEcal);

  // s->Branch("addDr",&addDr );
  // s->Branch("addPdgId",&addPdgId );
  // s->Branch("addEmE",&addEmE );
  // s->Branch("addHadE",&addHadE );
  // s->Branch("addEta",&addEta );
  // s->Branch("addPhi",&addPhi );

  // s->Branch("genDr",&genDr );
  // s->Branch("genPdgId",&genPdgId );
  // s->Branch("genE",&genE );
  // s->Branch("genEta",&genEta );
  // s->Branch("genPhi",&genPhi );

  // s->Branch("emHitX",&emHitX );
  // s->Branch("emHitY",&emHitY );
  // s->Branch("emHitZ",&emHitZ );
  // s->Branch("emHitE",&emHitE );
  // s->Branch("emHitF",&emHitF );

  // s->Branch("hadHitX",&hadHitX );
  // s->Branch("hadHitY",&hadHitY );
  // s->Branch("hadHitZ",&hadHitZ );
  // s->Branch("hadHitE",&hadHitE );
  // s->Branch("hadHitF",&hadHitF );

  //BasicCluster ECAL

  // s->Branch("bcEcalE",&bcEcalE);
  // s->Branch("bcEcalEta",&bcEcalEta);
  // s->Branch("bcEcalPhi",&bcEcalPhi);


  s->Branch("run",&orun,"run/I");
  s->Branch("evt",&oevt,"evt/l");
  s->Branch("lumiBlock",&olumiBlock,"lumi/I");
  //s->Branch("time",&otime,"orun/l");

  //simHits
   // s->Branch("EcalSimHits",&EcalSimHits);
   // s->Branch("ESSimHits",&ESSimHits);
   // s->Branch("HcalSimHits",&HcalSimHits);

   //recHits
   // s->Branch("EcalRecHits",&EcalRecHits);
   // //s->Branch("ESSimHits",&ESSimHits);
   // s->Branch("HcalRecHits",&HcalRecHits);

   // s->Branch("EcalRecHitsDr",&EcalRecHitsDr);
   // //s->Branch("ESSimHitsDr",&ESSimHitsDr);
   // s->Branch("HcalRecHitsDr",&HcalRecHitsDr);
   

}



PFChargedHadronAnalyzer::~PFChargedHadronAnalyzer() { 

  std::cout << "Total number of events .............. " << nEv[0] << std::endl;
  std::cout << "Number of events with 1 Sim Particle  " << nEv[1] << std::endl;


  std::cout << "Number of PF candidates ............. " << nCh[0] << std::endl;
  std::cout << "Number of PF Charged Hadrons......... " << nCh[1] << std::endl;
  std::cout << " - With pt > " << ptMin_ << " GeV/c ................ " << nCh[2] << std::endl;
  std::cout << " - With E_HCAL > " << hcalMin_ << " GeV .............. " << nCh[3] << std::endl;
  std::cout << " - With only 1 track in the block ... " << nCh[4] << std::endl;
  std::cout << " - With p > " << pMin_ << " GeV/c ................. " << nCh[5] << std::endl;
  std::cout << " - With at least " << nPixMin_ << " pixel hits ....... " << nCh[6] << std::endl;
  std::cout << " - With more than "<< nHitMin_[0] << " track hits ..... " << nCh[7] << std::endl;
  std::cout << " - With E_ECAL < " << ecalMax_ << " GeV ............ " << nCh[8] << std::endl;

  tf1->cd();
  s->Write();
  tf1->Write();
  tf1->Close();  


}



void 
PFChargedHadronAnalyzer::beginRun(const edm::Run& run, 
				  const edm::EventSetup & es) { }


void 
PFChargedHadronAnalyzer::analyze(const Event& iEvent, 
				 const EventSetup& iSetup) {
  
  LogDebug("PFChargedHadronAnalyzer")<<"START event: "<<iEvent.id().event()
			 <<" in run "<<iEvent.id().run()<<endl;

   edm::ESHandle<CaloGeometry> pCalo;
   iSetup.get<CaloGeometryRecord>().get( pCalo );
   theCaloGeom = pCalo.product();

  run  = iEvent.id().run();
  evt  = iEvent.id().event();
  lumiBlock = iEvent.id().luminosityBlock();
  time = iEvent.time();

  orun = (size_t)run;
  oevt = (size_t)evt;
  olumiBlock = (size_t)lumiBlock;
  otime = (size_t)((iEvent.time().value())>>32);

  // get PFCandidates
  Handle<PFCandidateCollection> pfCandidates;
  iEvent.getByToken(tokenPFCandidates_, pfCandidates);
  
  //get Ecal PFClusters
  Handle<reco::PFClusterCollection> pfClustersEcal;
  iEvent.getByToken(tokenEcalPFClusters_,pfClustersEcal);

  Handle<PFSimParticleCollection> trueParticles;
  bool isSimu = iEvent.getByToken(tokenPFSimParticles_,trueParticles);

  //simHits
  EcalSimHits.clear();
  ESSimHits.clear();
  HcalSimHits.clear();
  
  //recHits
  EcalRecHits.clear();
  ESRecHits.clear();
  HcalRecHits.clear();
  EcalRecHitsDr.clear();
  ESRecHitsDr.clear();
  HcalRecHitsDr.clear();

  pfcsID.clear();

  if(isMBMC_)
    isSimu=false;

  if ( isSimu ) { 
    nEv[0]++;//  cout<<" True part size "<<(*trueParticles).size()<<"    "
// 		  <<(*trueParticles)[0].pdgCode()<<"   "<<(*trueParticles)[1].pdgCode()<<endl;
    if ( (*trueParticles).size() != 1 ) return;
    nEv[1]++;
    
    
    // Check if there is a reconstructed track
    bool isCharged = false;
    for( CI ci  = pfCandidates->begin(); 
	 ci!=pfCandidates->end(); ++ci)  {
      const reco::PFCandidate& pfc = *ci;
      //if ( pfc.particleId() == 5 )
	pfcsID.push_back( pfc.particleId() );
      // std::cout << "Id = " << pfc.particleId() << std::endl;
      if ( pfc.particleId() < 4 ) { 
	isCharged = true;
	break;
      }
    }

    //to clean a bit the neutral hadrons
    //if(pfcsID.size()!=1) return;

    //std::cout << "isCharged ? " << isCharged << std::endl;
    //cout<<" =============================> "<<ecal_<<"     "<<hcal_<<endl;
    //SaveSimHit(iEvent, eta_, phi_ );
    // Case of no reconstructed tracks (and neutral single particles)
    //isCharged=true;//manual bypass
    if ( !isCharged ) { // || fabs((*trueParticles)[0].charge()) < 1E-10 ) {
      //cout<<"=====>"<<(*trueParticles)[0].energy()<<"   "<<(*trueParticles)[0].eta()<<"   "<<(*trueParticles)[0].phi()<<endl;
      reco::PFTrajectoryPoint::LayerType ecalEntrance = reco::PFTrajectoryPoint::ECALEntrance;
      const reco::PFTrajectoryPoint& tpatecal = ((*trueParticles)[0]).extrapolatedPoint( ecalEntrance );
      eta_ = tpatecal.positionREP().Eta();
      if ( fabs(eta_) < 1E-10 ) return; 
      phi_ = tpatecal.positionREP().Phi();
      true_ = std::sqrt(tpatecal.momentum().Vect().Mag2());
      p_ = 0.;
      
      ecal_ = 0.;
      hcal_ = 0.;
      for( CI ci  = pfCandidates->begin(); 
	   ci!=pfCandidates->end(); ++ci)  {
	const reco::PFCandidate& pfc = *ci;
	double deta = eta_ - pfc.eta();
	double dphi = phi_ - pfc.phi();
	double dR = std::sqrt(deta*deta+dphi*dphi);
	if ( pfc.particleId() == 4 && dR < 0.04 ) ecal_ += pfc.rawEcalEnergy();
	if ( pfc.particleId() == 5 && dR < 0.2 ) hcal_ += pfc.rawHcalEnergy();
      }
      
      //cout<<"======> "<<true_<<"  "<<eta_<<"  "<<phi_<<endl;
    
      //SaveRecHits(iEvent, eta_, phi_ );
      
      s->Fill();
      
      return;
    }
    



  }
  
  //cout<<" Track case !!! "<<endl;

  // Case of a reconstructed track.
  // Loop on pfCandidates
  for( CI ci  = pfCandidates->begin(); 
       ci!=pfCandidates->end(); ++ci)  {

    // The pf candidate
    const reco::PFCandidate& pfc = *ci;
    nCh[0]++;

    //MM
    //cout<< pfc.particleId()<<"    "<<pfc.pt()<<"    "<<pfc.rawEcalEnergy()<<"   "<<pfc.rawHcalEnergy()<<endl;



    // Only charged hadrons (no PF muons, no PF electrons) 1 / 5
    if ( pfc.particleId() != 1 ) continue;
    nCh[1]++;

    // Charged hadron minimum pt (the track pt, to an excellent approximation)
    if ( pfc.pt() < ptMin_ ) continue;
    nCh[2]++;

    // At least 1 GeV in HCAL
    double ecalRaw = pfc.rawEcalEnergy();
    double hcalRaw = pfc.rawHcalEnergy();
    double hoRaw = pfc.rawHoEnergy();
    if ( ecalRaw + hcalRaw < hcalMin_ ) continue;
    nCh[3]++;

    //cout<<endl<<endl<<" new event "<<endl;
    // Find the corresponding PF block elements
    const PFCandidate::ElementsInBlocks& theElements = pfc.elementsInBlocks();
    if( theElements.empty() ) continue;
    const reco::PFBlockRef blockRef = theElements[0].first;
    PFBlock::LinkData linkData =  blockRef->linkData();
   

    //cout<<endl<<endl<<" new event "<<endl;

    const edm::OwnVector<reco::PFBlockElement>& elements = blockRef->elements();
    // Check that there is only one track in the block.
    unsigned int nTracks = 0;
    unsigned int nEcal = 0;
    unsigned int nHcal = 0;
    unsigned iTrack = 999;
    vector<unsigned> iECAL;// =999;
    vector<unsigned> iHCAL;// =999;
    for(unsigned iEle=0; iEle<elements.size(); iEle++) {
    

      // Find the tracks in the block
      PFBlockElement::Type type = elements[iEle].type();


      //test distance ====================================
      // for(unsigned iEle2=0; iEle2<elements.size(); iEle2++) {
      // 	PFBlockElement::Type type2 = elements[iEle2].type();
      // 	double d = blockRef->dist(iEle, iEle2, linkData);
      // 	//cout<<iEle<<"     "<<iEle2<<" ---> "<<type<<"    "<<type2<<" ---------> "<<d<<endl;

      // }
      //==================================================


  
   
        
      switch( type ) {
      case PFBlockElement::TRACK:
	iTrack = iEle;
	nTracks++;
	break;
      case PFBlockElement::ECAL:
	iECAL.push_back( iEle );
	//cout<<"iEle "<<iEle<<endl;
	nEcal++;
	break;
      case PFBlockElement::HCAL:
	iHCAL.push_back( iEle );
	nHcal++;
	break;
      default:
	continue;
      }

    }
    //bypass for neutrals
    if ( nTracks != 1 ) continue;
    nCh[4]++;

    // Characteristics of the track
    const reco::PFBlockElementTrack& et =
      dynamic_cast<const reco::PFBlockElementTrack &>( elements[iTrack] );
    double p =/*pfc.energy();//*/ et.trackRef()->p();  
    double pt =/*pfc.pt();//*/ et.trackRef()->pt(); 
    double eta =/*pfc.eta();//*/ et.trackRef()->eta();
    double phi =/*pfc.phi();//*/ et.trackRef()->phi();
    
    //cout<<nEcal<<"   "<<nHcal<<endl;
    //ECAL element
    for(unsigned int ii=0;ii<nEcal;ii++) {
      const reco::PFBlockElementCluster& eecal =
	dynamic_cast<const reco::PFBlockElementCluster &>( elements[ iECAL[ii] ] );
      double E_ECAL = eecal.clusterRef()->energy();  
      double eta_ECAL = eecal.clusterRef()->eta();
      double phi_ECAL = eecal.clusterRef()->phi();

      cluEcalE.push_back( E_ECAL );
      cluEcalEta.push_back( eta_ECAL );
      cluEcalPhi.push_back( phi_ECAL );
      
      double d = blockRef->dist(iTrack, iECAL[ii], linkData);	
      distEcalTrk.push_back( d );
      //cout<<" ecal loop -> "<<iECAL[ii]<<"  "<<d<<" eta "<<eta_ECAL<<"   "<<phi_ECAL<<" <==>  "<<eta<<"   "<<phi<<endl;
      vector<float> tmp;
      emHitF.push_back( tmp );
      emHitE.push_back( tmp );
      emHitX.push_back( tmp );
      emHitY.push_back( tmp );
      emHitZ.push_back( tmp );

      if(isMBMC_ || isSimu) {
	const std::vector< reco::PFRecHitFraction > erh=eecal.clusterRef()->recHitFractions();
	for(unsigned int ieh=0;ieh<erh.size();ieh++) {
	  
	  emHitF[ii].push_back( erh[ieh].fraction() );
	  
	  emHitE[ii].push_back(  erh[ieh].recHitRef()->energy() );
	  

	  // cout<<" rechit "<<ieh<<" =====> "<<erh[ieh].recHitRef()->energy()<<"  "<<
	  //   erh[ieh].fraction()<<" / "<<erh[ieh].recHitRef()->position().Eta()
	  //     <<"  "<<erh[ieh].recHitRef()->position().Phi()<<endl;
	  bool isEB= erh[ieh].recHitRef()->layer()==-1;
	  emHitX[ii].push_back( isEB?erh[ieh].recHitRef()->position().eta() :erh[ieh].recHitRef()->position().x() );
	  emHitY[ii].push_back( isEB?erh[ieh].recHitRef()->position().phi() :erh[ieh].recHitRef()->position().y() );
	  emHitZ[ii].push_back( isEB?0:erh[ieh].recHitRef()->position().z() );
	  
	}
      }

    }
    //HCAL element
      for(unsigned int ii=0;ii<nHcal;ii++) {
	const reco::PFBlockElementCluster& ehcal =
	  dynamic_cast<const reco::PFBlockElementCluster &>( elements[iHCAL[ii] ] );
	double E_HCAL = ehcal.clusterRef()->energy();  
	double eta_HCAL = ehcal.clusterRef()->eta();
	double phi_HCAL = ehcal.clusterRef()->phi();

	cluHcalE.push_back( E_HCAL );
	cluHcalEta.push_back( eta_HCAL );
	cluHcalPhi.push_back( phi_HCAL );

	double d = blockRef->dist(iTrack, iHCAL[ii], linkData);	
	distHcalTrk.push_back( d );


	//ECAL-HCAL distance
	vector<float> tmp;
	distHcalEcal.push_back(tmp);
	for(unsigned int ij=0;ij<nEcal;ij++) {
	  d = blockRef->dist(iECAL[ij], iHCAL[ii], linkData);	
	  distHcalEcal[ii].push_back( d );
	}
	//==================
	//cout<<" hcal loop -> "<<iHCAL[ii]<<"  "<<d<<" eta "<<eta_HCAL<<"   "<<phi_HCAL<<" <==>  "<<eta<<"   "<<phi<<endl;
	//	vector<float> tmp;
	hadHitF.push_back( tmp );
	hadHitE.push_back( tmp );
	hadHitX.push_back( tmp );
	hadHitY.push_back( tmp );
	hadHitZ.push_back( tmp );

	 if(isMBMC_ || isSimu) {
	  const std::vector< reco::PFRecHitFraction > erh=ehcal.clusterRef()->recHitFractions();
	  for(unsigned int ieh=0;ieh<erh.size();ieh++) {

	    hadHitF[ii].push_back( erh[ieh].fraction() );
      
	    hadHitE[ii].push_back(  erh[ieh].recHitRef()->energy() );
      
	    // cout<<" rechit "<<ieh<<" =====> "<<erh[ieh].recHitRef()->energy()<<"  "<<
	    //   erh[ieh].fraction()<<" / "<<erh[ieh].recHitRef()->position().Eta()
	    // 	<<"  "<<erh[ieh].recHitRef()->position().Phi()<<endl;

	    bool isHB= erh[ieh].recHitRef()->layer()==1;
	    hadHitX[ii].push_back( isHB?erh[ieh].recHitRef()->position().eta() :erh[ieh].recHitRef()->position().x() );
	    hadHitY[ii].push_back( isHB?erh[ieh].recHitRef()->position().phi() :erh[ieh].recHitRef()->position().y() );
	    hadHitZ[ii].push_back( isHB?0:erh[ieh].recHitRef()->position().z() );
	  
	  }
	}

      }

    
    // A minimum p and pt
      if ( p < pMin_ || pt < ptMin_ ) continue;
      nCh[5]++;
    
    // Count the number of valid hits (first three iteration only)
    //unsigned int nHits = et.trackRef()->found();
    unsigned int tobN = 0;
    unsigned int tecN = 0;
    unsigned int tibN = 0;
    unsigned int tidN = 0;
    unsigned int pxbN = 0;
    unsigned int pxdN = 0;
    const reco::HitPattern& hp = et.trackRef()->hitPattern();
    switch ( et.trackRef()->algo() ) {
    case TrackBase::initialStep:
    case TrackBase::lowPtTripletStep:
    case TrackBase::pixelPairStep:
    case TrackBase::detachedTripletStep:
      tobN += hp.numberOfValidStripTOBHits();
      tecN += hp.numberOfValidStripTECHits();
      tibN += hp.numberOfValidStripTIBHits();
      tidN += hp.numberOfValidStripTIDHits();
      pxbN += hp.numberOfValidPixelBarrelHits(); 
      pxdN += hp.numberOfValidPixelEndcapHits(); 
      break;
    case TrackBase::mixedTripletStep:
    case TrackBase::pixelLessStep:
    case TrackBase::tobTecStep:
    case TrackBase::jetCoreRegionalStep:
    case TrackBase::muonSeededStepInOut:
    case TrackBase::muonSeededStepOutIn:
    default:
      break;
    }
    int inner = pxbN+pxdN;
    int outer = tibN+tobN+tidN+tecN;
    
    // // Number of pixel hits
      if ( inner < nPixMin_ ) continue;
      nCh[6]++;
    
    // Number of tracker hits (eta-dependent cut)
    bool trackerHitOK = false;
    double etaMin = 0.;
    for ( unsigned int ieta=0; ieta<nEtaMin_.size(); ++ieta ) { 
      if ( fabs(eta) < etaMin ) break;
      double etaMax = nEtaMin_[ieta];
      trackerHitOK = 
    	fabs(eta)>etaMin && fabs(eta)<etaMax && inner+outer>nHitMin_[ieta]; 
      if ( trackerHitOK ) break;
      etaMin = etaMax;
    }
      if ( !trackerHitOK ) continue;
      nCh[7]++;
    
    // Selects only ECAL MIPs
    if ( ecalRaw > ecalMax_ ) continue;
    nCh[8]++;

    
    //extrapolate track to ECAL --> impact position
    etaEcal_ = et.positionAtECALEntrance().Eta();
    phiEcal_ = et.positionAtECALEntrance().Phi();

    
   //  std::cout <<endl<< "Selected track : p = " << p << "; pt = " << pt 
// 	      << "; eta/phi = " << eta << " " << phi << std::endl
// 	      << "PF Ch. hadron  : p = " << pfc.p() << "; pt = " << pfc.pt()
// 	      << "; eta/phi = " << pfc.eta() << " " << pfc.phi() << std::endl
// 	      << "Nb of hits (pix/tot) " << inner << " " << inner+outer << std::endl;
//     std::cout << "Raw Ecal and HCAL energies : ECAL = " << ecalRaw 
// 	      << "; HCAL = " << hcalRaw << std::endl;
    

    // Fill the root-tuple
    p_ = p;
    ecal_ = ecalRaw;
    hcal_ = hcalRaw;
    ho_ = hoRaw;

//    cout<<ecal_<<"   "<<hcal_<<

    charge_ = pfc.charge();
    //cout<<pfc.charge()<<"   "<<pfc.particleId()<<"   "<<(*trueParticles)[0].charge()<<"  "<<pfCandidates->size()<<endl;

    //Cluster characteristics



//     float dr; //bool addP=false;
//     for(CI ci2 = pfCandidates->begin(); 
// 	ci2!=pfCandidates->end(); ++ci2) {
      
//       dr = dR(eta, ci2->eta(), phi ,ci2->phi() );
//       if( dr < 0.3 && dr!=0 ) {//addP=true; continue;
// // 	std::cout<<" -----> close particle :"<<dr<<std::endl;
// // 	std::cout<<" \t\t ==> p   : "<<ci2->p()<<" ; pt : "<<ci2->pt()<<std::endl;
// // 	std::cout<<" \t\t ==> eta : "<<ci2->eta()<<" ; ph : "<<ci2->phi()<<std::endl;
// // 	std::cout<<" \t\t ==> EM  : "<<ci2->rawEcalEnergy()<<" ; Ha : "<<ci2->rawHcalEnergy()<<std::endl;

// 	addDr.push_back( dr );
// 	addPdgId.push_back( ci2->translateTypeToPdgId( ci2->particleId() ) );
// 	addEmE.push_back( ci2->rawEcalEnergy() );
// 	addHadE.push_back( ci2->rawHcalEnergy() );
// 	addEta.push_back( ci2->eta() );
// 	addPhi.push_back( ci2->phi() );

//       }

//     }
    
			    //  if(addP) { std::cout<<" !!!!===> No additionnal reconstructed particles "<<std::endl; }

//     if( isMBMC_ ) {
//       float etaG, phiG, trueG; 
//       iEvent.getByLabel(inputTagPFSimParticles_,trueParticles);
//       reco::PFTrajectoryPoint::LayerType ecalEntrance = reco::PFTrajectoryPoint::ECALEntrance;
//         reco::PFSimParticleCollection::const_iterator genCI;
// 	for(genCI = trueParticles->begin(); genCI != trueParticles->end(); genCI++ ) {
// 	  const reco::PFSimParticle& genpfc = *genCI;

// 	  const reco::PFTrajectoryPoint& tpatecal = genpfc.extrapolatedPoint( ecalEntrance );
// 	  etaG = tpatecal.positionREP().Eta();
// 	  phiG = tpatecal.positionREP().Phi();
// 	  trueG = std::sqrt(tpatecal.momentum().Vect().Mag2());


// 	  dr = dR(etaG, etaEcal_, phiG ,phiEcal_ );
// 	  if( dr < 0.2 ) {
// 	  //   std::cout<<" -----> close generated particle :"<<dr<<" \t -->   "<<genpfc.pdgCode()<<std::endl;
// // 	    std::cout<<" \t\t ==> energy   : "<<trueG<<std::endl;
// // 	    std::cout<<" \t\t ==> eta : "<<etaG<<" ; ph : "<<phiG<<std::endl;

// // 	    std::cout<<" \t\t ==> etaI: "<<(*(genpfc.innermostMeasurement())).momentum().Eta();
// // 	    std::cout<<" ; phI: "<<(*(genpfc.innermostMeasurement())).momentum().Phi()<<std::endl;

// 	    genDr.push_back( dr );
// 	    genPdgId.push_back( genpfc.pdgCode() );
// 	    genE.push_back( trueG );
// 	    genEta.push_back( etaG );
// 	    genPhi.push_back( phiG );

// 	    //  std::cout<<" \t\t ==> EM  : "<<genpfc->rawEcalEnergy()<<" ; Ha : "<<genpfc->rawHcalEnergy()<<std::endl;
// 	  }
	  
// 	} //loop
//     } //isMBMC
   

    if( isSimu ) {
      reco::PFTrajectoryPoint::LayerType ecalEntrance = reco::PFTrajectoryPoint::ECALEntrance;
      const reco::PFTrajectoryPoint& tpatecal = ((*trueParticles)[0]).extrapolatedPoint( ecalEntrance );
      eta_ = tpatecal.positionREP().Eta();
      phi_ = tpatecal.positionREP().Phi();
      true_ = std::sqrt(tpatecal.momentum().Vect().Mag2());
      //cout<<" Etrapol "<<eta_<<"   "<<phi_<<endl;
    }
    else {
      //const reco::PFTrajectoryPoint& tpatecal = et.trackRef().extrapolatedPoint( ecalEntrance );

    //   edm::ESHandle<TrackerGeometry> trackerGeomHandle;
//       edm::ESHandle<MagneticField> magFieldHandle;
//       iSetup.get<TrackerDigiGeometryRecord>().get( trackerGeomHandle );
//       iSetup.get<IdealMagneticFieldRecord>().get( magFieldHandle );
//       const TrackerGeometry* trackerGeom = trackerGeomHandle.product();
//       const MagneticField* magField = magFieldHandle.product();

      eta_ = eta; // tpatecal.positionREP().Eta();
      phi_ = phi; //tpatecal.positionREP().Phi();
      true_ = p; //std::sqrt(tpatecal.momentum().Vect().Mag2());
    }




    //Basic Ecal PF Clusters
   
//     for( size_t ibc=0; ibc<pfClustersEcal->size(); ++ibc )
//       {
// 	reco::PFClusterRef bcRef( pfClustersEcal, ibc );
// 	bcEcalE.push_back( bcRef->energy() );
// 	bcEcalEta.push_back( bcRef->eta() );
// 	bcEcalPhi.push_back( bcRef->phi() );

//  	//float dr = dR(eta_, bcRef->eta(), phi_ ,bcRef->phi() );
// // 	if( dr < 0.3 ) {
// // 	  //std::cout<<ecal_<<"   "<<bcRef->energy()<<"  -->  "<<ecal_-bcRef->energy()<<" :::  "<<"   "<<bcRef->eta()<<"   "<<bcRef->phi()<<endl;
	  
// // 	  for(int unsigned kk=0;kk<distEcalTrk.size();kk++) {
// // 	    if(distEcalTrk[kk]<0.1)
// // 	      std::cout<<"\t ---> "<<cluEcalE[kk]<<"    "<<ecal_-cluEcalE[kk]<<endl;
	    
// // 	  }
	    
// // 	}

//       }




    s->Fill();

    addDr.clear();
    addPdgId.clear();
    addEmE.clear();
    addHadE.clear();
    addEta.clear();
    addPhi.clear();
    
    cluEcalE.clear();
    cluEcalEta.clear();
    cluEcalPhi.clear();

    distEcalTrk.clear();

    cluHcalE.clear();
    cluHcalEta.clear();
    cluHcalPhi.clear();

    distHcalTrk.clear();
    distHcalEcal.clear();

    genDr.clear();
    genPdgId.clear();
    genE.clear();
    genEta.clear();
    genPhi.clear();
  
    emHitF.clear();
    emHitE.clear();
    emHitX.clear();
    emHitY.clear();
    emHitZ.clear();
    hadHitF.clear();
    hadHitE.clear();
    hadHitX.clear();
    hadHitY.clear();
    hadHitZ.clear();
    
    bcEcalE.clear();
    bcEcalEta.clear();
    bcEcalPhi.clear();
    
    
  }
}


float PFChargedHadronAnalyzer::dR(float eta1, float eta2, float phi1, float phi2 ) {

  TVector3 v1(0,0,0),v2(0,0,0);
  
  v1.SetPtEtaPhi(1, eta1, phi1);
  v2.SetPtEtaPhi(1, eta2, phi2);

  return v1.DrEtaPhi( v2 );
  
}


void PFChargedHadronAnalyzer::SaveSimHit(const edm::Event& iEvent,  float eta_, float phi_) {

  //Access to simHits informations
  Handle<PCaloHitContainer> h_PCaloHitsEB;
  iEvent.getByLabel("g4SimHits","EcalHitsEB", h_PCaloHitsEB);

  Handle<PCaloHitContainer> h_PCaloHitsEE;
  iEvent.getByLabel("g4SimHits","EcalHitsEE", h_PCaloHitsEE);

  Handle<PCaloHitContainer> h_PCaloHitsES;
  iEvent.getByLabel("g4SimHits","EcalHitsES", h_PCaloHitsES);
  
  Handle<PCaloHitContainer> h_PCaloHitsH;
  iEvent.getByLabel("g4SimHits","HcalHits", h_PCaloHitsH);

  //iterator
  PCaloHitContainer::const_iterator genSH;

  //match hits... dR 0.2, should contains all simHits
  
  //ECAL
  if( fabs(eta_) <1.5 ) { //barrel
    
    for(genSH = h_PCaloHitsEB->begin(); genSH != h_PCaloHitsEB->end(); genSH++) {
      // float theta = genSH->thetaAtEntry();
      // float phi = genSH->phiAtEntry();
      // float eta = Eta( theta );
      // float dr = dR( eta, eta_, phi, phi_ );
      
      // if(dr > 0.2 ) continue;
      //cout<<" ecal hit : "<<genSH->energy()<<endl;
      EcalSimHits.push_back( genSH->energy() );
    }
  }
  else {
    
    for(genSH = h_PCaloHitsEE->begin(); genSH != h_PCaloHitsEE->end(); genSH++) {
      // float theta = genSH->thetaAtEntry();
      // float phi = genSH->phiAtEntry();
      // float eta = Eta( theta );
      // float dr = dR( eta, eta_, phi, phi_ );

      // if(dr > 0.2 ) continue;
      EcalSimHits.push_back( genSH->energy() );
    }    

    for(genSH = h_PCaloHitsES->begin(); genSH != h_PCaloHitsES->end(); genSH++) {
       // float theta = genSH->thetaAtEntry();
       // float phi = genSH->phiAtEntry();
       // float eta = Eta( theta );
       // float dr = dR( eta, eta_, phi, phi_ );

       // if(dr > 0.2 ) continue;
       ESSimHits.push_back( genSH->energy() );
    }    
  }
  
  //Hcal
  float sH=0; 
  for(genSH = h_PCaloHitsH->begin(); genSH != h_PCaloHitsH->end(); genSH++) {
    // float theta = genSH->thetaAtEntry();
    // float phi = genSH->phiAtEntry();
    // float eta = Eta( theta );
    // float dr = dR( eta, eta_, phi, phi_ );
       // if(dr > 0.2 ) continue;
    sH += genSH->energy();
    //cout<<" ecal hit : "<<genSH->energy()<<"    "<<genSH->energyEM()<<"   "<<genSH->energyHad()<<"   "<<sH<<endl;
       HcalSimHits.push_back( genSH->energy() );
    }
}


float PFChargedHadronAnalyzer::Eta( float theta_ ) {
  if( sin(theta_/2.)==0 ) return 10000.*cos(theta_/2.);
  return -log(tan(theta_/2.0));
}


void PFChargedHadronAnalyzer::SaveRecHits(const edm::Event& iEvent, float eta_, float phi_) {

  //get rechits
  edm::Handle< EcalRecHitCollection > ebRecHits_h;
  edm::Handle< EcalRecHitCollection > eeRecHits_h;
  edm::Handle< EcalRecHitCollection > esRecHits_h;
 // Barrel
  iEvent.getByLabel( "ecalRecHit","EcalRecHitsEB", ebRecHits_h );
  // Endcaps
  iEvent.getByLabel( "ecalRecHit","EcalRecHitsEE", eeRecHits_h );
  // Preshower
  iEvent.getByLabel( "ecalRecHit","EcalRecHitsES", esRecHits_h );
  // Hcal
  edm::Handle< HBHERecHitCollection > hbheRecHits_h;
  iEvent.getByLabel( "hbhereco","", hbheRecHits_h );
  

  for( size_t ii =0; ii < ebRecHits_h->size(); ++ii )
    {
      EcalRecHitRef recHitRef( ebRecHits_h, ii );
      EBDetId id = recHitRef->id();

      const  GlobalPoint & rhPos = theCaloGeom->getPosition( id );
      float eta = rhPos.eta();
      float phi = rhPos.phi();
      float dr = dR( eta, eta_, phi, phi_ );
      if(dr > 0.1 ) continue;
      //cout<<"EB : "<<dr<<"  "<<recHitRef->energy()<<endl;
      EcalRecHits.push_back( recHitRef->energy() );
      EcalRecHitsDr.push_back( dr );
    }

  for( size_t ii =0; ii < eeRecHits_h->size(); ++ii )
    {
      EcalRecHitRef recHitRef( eeRecHits_h, ii );
      EEDetId id = recHitRef->id();

      const  GlobalPoint & rhPos = theCaloGeom->getPosition( id );
      float eta = rhPos.eta();
      float phi = rhPos.phi();
      float dr = dR( eta, eta_, phi, phi_ );
      if(dr > 0.1 ) continue;
      EcalRecHits.push_back( recHitRef->energy() );
      EcalRecHitsDr.push_back( dr );
    }


  for( size_t ii =0; ii < hbheRecHits_h->size(); ++ii )
    {
      HBHERecHitRef recHitRef( hbheRecHits_h, ii );
      HcalDetId id = recHitRef->id();

      const  GlobalPoint & rhPos = theCaloGeom->getPosition( id );
      float eta = rhPos.eta();
      float phi = rhPos.phi();
      float dr = dR( eta, eta_, phi, phi_ );
      if(dr > 0.15 ) continue;
      //cout<<"Hcal : "<<dr<<"  "<<recHitRef->energy()<<endl;
      HcalRecHits.push_back( recHitRef->energy() );
      HcalRecHitsDr.push_back( dr );
    }
}


DEFINE_FWK_MODULE(PFChargedHadronAnalyzer);
