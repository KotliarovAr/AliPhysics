AliAnalysisTask *AddTaskHFECal(Bool_t MassConst, Bool_t MassWidthCut, Bool_t MassNonlinear)
{
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  if (!mgr) {
    Error("AddTaskHFEECal", "No analysis manager found.");
    return NULL;
  }

  if (!mgr->GetInputEventHandler()) {
    ::Error("AddTaskHFECal", "This task requires an input event handler");
    return NULL;
  }
  TString type = mgr->GetInputEventHandler()->GetDataType(); // can be "ESD" or "AOD"
  if (type=="AOD"){
    ::Error("AddTaskHFECal", "The tasks exits because AODs are in input");
    return NULL;
  }
  Bool_t MCthere=kFALSE;
  AliMCEventHandler *mcH = dynamic_cast<AliMCEventHandler*>(mgr->GetMCtruthEventHandler());
  if(!mcH){
    MCthere=kFALSE;
  }else{
    MCthere=kTRUE;
  }
  
  Bool_t MassCal = kTRUE;
  Double_t masscut = 0.05;
  if(!MassConst)masscut = 0.1;
  //analysis task 
  gROOT->LoadMacro("$ALICE_PHYSICS/PWGHF/hfe/macros/configs/PbPb/ConfigHFECal.C");

  AliAnalysisTaskHFECal *hfetaskCent = ConfigHFECal(MCthere,MassConst,MassWidthCut,MassCal,MassNonlinear,0.5,0.1,masscut,-5,1);
  AliAnalysisTaskHFECal *hfetaskTrig= ConfigHFECal(MCthere,MassConst,MassWidthCut,MassCal,MassNonlinear,0.5,0.1,masscut,-5,1);
  AliAnalysisTaskHFECal *hfetaskTrig2= ConfigHFECal(MCthere,MassConst,MassWidthCut,MassCal,MassNonlinear,0.5,0.1,masscut,-5,1);
 
  mgr->AddTask(hfetaskCent);
  mgr->AddTask(hfetaskTrig);
  mgr->AddTask(hfetaskTrig2);
  
  
  // central trigger
  hfetaskCent->SelectCollisionCandidates(AliVEvent::kCentral);
  
  TString containerName = mgr->GetCommonFileName();
  containerName += ":PWGHF_hfeCalCentral";
  
  AliAnalysisDataContainer *cinput = mgr->GetCommonInputContainer();
  AliAnalysisDataContainer *coutput1 = mgr->CreateContainer("HFE_Results_EMCalCentral", TList::Class(),AliAnalysisManager::kOutputContainer, containerName.Data());
  mgr->ConnectInput(hfetaskCent, 0, cinput);
  mgr->ConnectOutput(hfetaskCent, 1, coutput1);

  //L1 gamma trigger
  hfetaskTrig->SelectCollisionCandidates(AliVEvent::kEMCEGA);
  
  TString containerName2 = mgr->GetCommonFileName();
  containerName2 += ":PWGHF_hfeCalTrigEGA";
  
  AliAnalysisDataContainer *cinput = mgr->GetCommonInputContainer();
  AliAnalysisDataContainer *coutput1 = mgr->CreateContainer("HFE_Results_EMCalTrigEGA", TList::Class(),AliAnalysisManager::kOutputContainer, containerName2.Data());
  mgr->ConnectInput(hfetaskTrig, 0, cinput);
  mgr->ConnectOutput(hfetaskTrig, 1, coutput1);
  
  
  // semi-central
  //hfetaskTrig2->SelectCollisionCandidates(AliVEvent::kSemiCentral | AliVEvent::kCentral);
  hfetaskTrig2->SelectCollisionCandidates(AliVEvent::kAny);
  
  TString containerName3 = mgr->GetCommonFileName();
  containerName3 += ":PWGHF_hfeCalSemiCentral";
  
  AliAnalysisDataContainer *cinput = mgr->GetCommonInputContainer();
  AliAnalysisDataContainer *coutput1 = mgr->CreateContainer("HFE_Results_SemiCentral", TList::Class(),AliAnalysisManager::kOutputContainer, containerName3.Data());
  mgr->ConnectInput(hfetaskTrig2, 0, cinput);
  mgr->ConnectOutput(hfetaskTrig2, 1, coutput1);

 
  //if(MCthere)
    //{
    //MB trigger
    AliAnalysisTaskHFECal *hfetaskMB = ConfigHFECal(MCthere,MassConst,MassWidthCut,MassCal,MassNonlinear,0.5,0.1,masscut,-5,1);
    mgr->AddTask(hfetaskMB);
    hfetaskMB->SelectCollisionCandidates(AliVEvent::kMB);

    TString containerName4 = mgr->GetCommonFileName();
    containerName4 += ":PWGHF_hfeCalkMB";

     AliAnalysisDataContainer *cinput = mgr->GetCommonInputContainer();
     AliAnalysisDataContainer *coutput1 = mgr->CreateContainer("HFE_Results_EMCalMB", TList::Class(),AliAnalysisManager::kOutputContainer, containerName4.Data());
     mgr->ConnectInput(hfetaskMB, 0, cinput);
     mgr->ConnectOutput(hfetaskMB, 1, coutput1);
     //}

  return NULL;
}
