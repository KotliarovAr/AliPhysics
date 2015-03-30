/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

//______________________________________________________________________________
// Analysis task for high pt particle correlations 
// author: R.Diaz, J. Rak,  D.J. Kim
// ALICE Group University of Jyvaskyla 
// Finland 
// Fill the analysis containers for ESD or AOD
// Adapted for AliAnalysisTaskSE and AOD objects  
//////////////////////////////////////////////////////////////////////////////

#include "TChain.h"
//#include "TList.h"
//#include "TTree.h"
#include "TFile.h"
#include "AliAnalysisUtils.h"
#include "AliAnalysisTaskSE.h"
#include "AliAODHandler.h"
#include "AliAODMCParticle.h"
#include "AliJFFlucTask.h" 
#include "AliAnalysisManager.h"
#include "AliVEvent.h"
#include "AliAODEvent.h" 
#include "AliJTrack.h"
#include "AliJMCTrack.h"
//#include "AliJPhoton.h"
#include "AliJEventHeader.h"
#include "AliJHistManager.h"
#include "AliInputEventHandler.h"

//______________________________________________________________________________
AliJFFlucTask::AliJFFlucTask():
	fInputList(0),
    AliAnalysisTaskSE(), 
    fFFlucAna(0x0),
	fOutput()
{
 fEvtNum=0;
 fFilterBit = 0;
 fEta_min = 0;
 fEta_max = 0;
//  DefineOutput(1, TDirectory::Class());
}


//______________________________________________________________________________
AliJFFlucTask::AliJFFlucTask(const char *name, int CollisionCandidates, Bool_t IsMC, Bool_t IsExcludeWeakDecay):
	fInputList(0),
    AliAnalysisTaskSE(name), 
    fFFlucAna(0x0),
	fOutput()
{
 DefineOutput(1, TDirectory::Class());
  fEvtNum=0;
 fFilterBit = 0;
 fEta_min = 0;
 fEta_max = 0;
 fTaskName = name;
}

//____________________________________________________________________________
AliJFFlucTask::AliJFFlucTask(const AliJFFlucTask& ap) :
	fInputList(ap.fInputList),
	AliAnalysisTaskSE(ap.GetName()), 
    fFFlucAna(ap.fFFlucAna),
	fOutput(ap.fOutput)
{ 
  AliInfo("----DEBUG AliJFFlucTask COPY ----");
}

//_____________________________________________________________________________
AliJFFlucTask& AliJFFlucTask::operator = (const AliJFFlucTask& ap)
{
  // assignment operator
  AliInfo("----DEBUG AliJFFlucTask operator= ----");
  this->~AliJFFlucTask();
  new(this) AliJFFlucTask(ap);
  return *this;
}

//______________________________________________________________________________
AliJFFlucTask::~AliJFFlucTask()
{
		// destructor 
		delete fFFlucAna;
		delete fInputList;
		delete fOutput;
}

//________________________________________________________________________
void AliJFFlucTask::UserCreateOutputObjects()
{ 
  fFFlucAna =  new AliJFFlucAnalysis( fTaskName );
  fFFlucAna->SetDebugLevel(fDebugLevel); 
  //=== create the jcorran outputs objects

  //=== Get AnalysisManager
  /*
  AliAnalysisManager *man = AliAnalysisManager::GetAnalysisManager();
  if(!man->GetOutputEventHandler()) {
    Fatal("UserCreateOutputObjects", "This task needs an AOD handler");
    return;
  }
  */
  fInputList = new TClonesArray("AliJBaseTrack" , 2500);
  fInputList->SetOwner(kTRUE);


  OpenFile(1);
  fOutput = gDirectory;
  fOutput->cd();
  fFFlucAna->UserCreateOutputObjects(); 

  PostData(1, fOutput);
}

//______________________________________________________________________________
void AliJFFlucTask::UserExec(Option_t* /*option*/) 
{
	// Processing of one event
	if(!((Entry()-1)%100))  AliInfo(Form(" Processing event # %lld",  Entry()));
	// initiaizing variables from last event 
	fInputList->Clear();
	
	float fCent = -999;
	double fvertex[3] = {-999,-999,-999};	

	fEvtNum++;
//	if(fEvtNum % 100 == 0){ cout << "evt : " << fEvtNum <<endl ;}

	// load current event and save track, event info 
	AliAODEvent *currentEvent = dynamic_cast<AliAODEvent*>(InputEvent());

	if( IsGoodEvent( currentEvent )){

			ReadAODTracks( currentEvent, fInputList ) ; // read tracklist
			ReadVertexInfo( currentEvent, fvertex); // read vertex info
			fCent = 			ReadAODCentrality( currentEvent, "V0M"  ) ; 
			Int_t Ntracks = fInputList->GetEntriesFast();

			// Analysis Part 
			fFFlucAna->Init();
			fFFlucAna->SetInputList( fInputList ); 
			fFFlucAna->SetEventCentrality( fCent );
			fFFlucAna->SetEventVertex( fvertex );
			fFFlucAna->SetEtaRange( fEta_min, fEta_max ) ;
			fFFlucAna->UserExec(""); // doing some analysis here. 
			// 
	}
}

//______________________________________________________________________________
void AliJFFlucTask::ReadVertexInfo ( AliAODEvent *aod, double*  fvtx )
{
	fvtx[0] = aod->GetPrimaryVertex()->GetX();
	fvtx[1] = aod->GetPrimaryVertex()->GetY();
	fvtx[2] = aod->GetPrimaryVertex()->GetZ();
}
//______________________________________________________________________________
float AliJFFlucTask::ReadAODCentrality( AliAODEvent *aod, TString Trig )
{
	AliCentrality *cent = aod->GetCentrality();
	if(!cent){ Printf("No Cent event"); return -999; };
	return cent->GetCentralityPercentile(Trig.Data()); 
}
//______________________________________________________________________________
void AliJFFlucTask::Init()
{
	// initiationg all parameters and variables

	// Intialisation of parameters
	AliInfo("Doing initialization") ; 
//
}
//______________________________________________________________________________
void AliJFFlucTask::ReadAODTracks( AliAODEvent *aod , TClonesArray *TrackList)
{
		//aod->Print();
	if( IsMC == kTRUE ){  // how to get a flag to check  MC or not ! 

		TClonesArray *mcArray = (TClonesArray*) aod->FindListObject(AliAODMCParticle::StdBranchName());
		if(!mcArray){ Printf("Error not a proper MC event"); };  // check mc array
		
		Int_t nt = mcArray->GetEntriesFast();
		Int_t ntrack =0;
		for( int it=0; it< nt ; it++){
				AliAODMCParticle *track = (AliAODMCParticle*)mcArray->At(it);
				if(!track) { Error("ReadEventAODMC", "Could not receive particle %d",(int) it); continue; };
				if( track->IsPhysicalPrimary() ){
						// insert AMTP weak decay switch here
						if(IsExcludeWeakDecay == kTRUE){
								Bool_t kExcludeParticle = kFALSE;
								Int_t gMotherIndex = track->GetMother(); // check and ask about this to DJ changed to mother from firstmother
								if(gMotherIndex != -1) {
										AliAODMCParticle *motherParticle = (AliAODMCParticle*)mcArray->At(gMotherIndex);
										if(motherParticle) {
												if(IsThisAWeakDecayingParticle(motherParticle)){
													kExcludeParticle = kTRUE;
												}
										}
								}
								//Exclude from the analysis decay products of weakly decaying particles
								if(kExcludeParticle) continue;
						} // weak decay particles are excluded
						double track_abs_eta = TMath::Abs( track->Eta() );
						//if( track_abs_eta > fEta_min && track_abs_eta < fEta_max){ // eta cut
						//if( track->Pt() < 0.2 || track->Pt() > 5 ) continue ; // test pt cut
						Int_t pdg = track->GetPdgCode();
						Char_t ch = (Char_t) track->Charge();
						Int_t label = track->GetLabel();
						AliJBaseTrack *itrack = new ((*TrackList)[ntrack++])AliJBaseTrack;
						itrack->SetLabel( label );
						itrack->SetParticleType( pdg);
						itrack->SetPxPyPzE( track->Px(), track->Py(), track->Pz(), track->E() );
						itrack->SetCharge(ch) ;
						//}; no eta cut in task file
				}
		}
	} // read mc track done.
	else if( IsMC == kFALSE){
		Int_t nt = aod->GetNumberOfTracks();
		Int_t ntrack =0;
		for( int it=0; it<nt ; it++){
				AliAODTrack *track = dynamic_cast<AliAODTrack*>(aod->GetTrack(it));
				if(!track) { Error("ReadEventAOD", "Could not receive partice %d", (int) it); continue; };
				if(track->TestFilterBit( fFilterBit )){ // hybrid cut
//						double track_abs_eta = TMath::Abs( track->Eta() );
//						if( track_abs_eta > fEta_min && track_abs_eta < fEta_max){ // eta cut
								AliJBaseTrack *itrack = new( (*TrackList)[ntrack++])AliJBaseTrack; 
								//itrack->SetID( track->GetID() );
								itrack->SetID( TrackList->GetEntriesFast() ) ;
								itrack->SetPxPyPzE( track->Px(), track->Py(), track->Pz(), track->E() );
								itrack->SetParticleType(kJHadron);
								itrack->SetCharge(track->Charge() );
								itrack->SetStatus(track->GetStatus() );
//						} no eta cut in task file
				}
		}
	} //read aod reco track done.
}
//______________________________________________________________________________
Bool_t AliJFFlucTask::IsGoodEvent( AliAODEvent *event){

	Bool_t Event_status = kFALSE;
	// check vertex 
	AliVVertex *vtx = event->GetPrimaryVertex();
	if(vtx){
			if( vtx->GetNContributors() > 0 ){
					double zVert = vtx->GetZ();
					if( zVert > -10 && zVert < 10) Event_status = kTRUE;
			}
	}
	return Event_status;
/*
//	Bool_t Trigger_kCentral = kFALSE; // init
//	Trigger_kCentral =(((AliInputEventHandler*)(AliAnalysisManager::GetAnalysisManager()->GetInputEventHandler()))
->IsEventSelected() & AliVEvent::kCentral);
//	return Trigger_kCentral;
//	*/
}
//
//______________________________________________________________________________
void AliJFFlucTask::Terminate(Option_t *)
{
//    fFFlucAna->Terminate("");
	// Processing when the event loop is ended
	cout<<"AliJFFlucTask Analysis DONE !!"<<endl; 
}


//______________________________________________________________________________
Bool_t AliJFFlucTask::IsThisAWeakDecayingParticle(AliAODMCParticle *thisGuy)
{
  // In order to prevent analyzing daughters from weak decays 
  // - AMPT does not only strong decays, so IsPhysicalPrimary does not catch it
Int_t pdgcode = TMath::Abs( thisGuy->GetPdgCode() );
 Int_t myWeakParticles[7] = { 3322, 3312, 3222, // Xi0 Xi+- Sigma-+
                              3122, 3112, // Lambda0 Sigma+-
                              130, 310 // K_L0 K_S0
 };
 Bool_t found = kFALSE;
 for(Int_t i=0; i!=7; ++i)
   if( myWeakParticles[i] == pdgcode ) {
     found = kTRUE;
     break;
   }
 return found;
}
