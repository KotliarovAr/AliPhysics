// TODO LIST
// TODO: You're all set!

AliAnalysisTaskPhiCount* AddAnalysisTaskPhiCount( Bool_t MCFlag, Bool_t PhiFlag, Bool_t KaonFlag, Int_t fAnalysisOption = 0, TString fName = "name" )
{
    // Analysis Manager
    AliAnalysisManager         *fAliAnlManager      =   AliAnalysisManager::GetAnalysisManager();
    
    // Filename
    TString fileName                                =   AliAnalysisManager::GetCommonFileName();
    
    // Task initialisation
    AliAnalysisTaskPhiCount    *fAliAnlTask         =   new AliAnalysisTaskPhiCount(fName.Data());
    
    // Checks
    if (!fAliAnlManager)                            return 0x0;
    if (!fAliAnlManager->GetInputEventHandler())    return 0x0;
    if (!fAliAnlTask)                               return 0x0;
    
    // task Selection
    fAliAnlTask ->  SelectCollisionCandidates(AliVEvent::kAnyINT);
    
    // Task options
    fAliAnlTask ->  SetMCFlag(MCFlag);
    fAliAnlTask ->  SetPhiFlag(PhiFlag);
    fAliAnlTask ->  SetKaonFlag(KaonFlag);
    
    switch ( fAnalysisOption ) {
        case 1:
            fAliAnlTask -> SetFilterBit(7);
            break;
            
        default:
            fAliAnlTask -> SetFilterBit(5);
            break;
    }
    
    // Input
    fAliAnlManager->ConnectInput(fAliAnlTask,0,fAliAnlManager->GetCommonInputContainer());
    
    // Output
    // - // TLists
    
    fAliAnlManager->ConnectOutput(fAliAnlTask,1,fAliAnlManager->CreateContainer(Form("fAnalysisOutputList_%i_%s",fAnalysisOption,fName.Data()),     TList::Class(), AliAnalysisManager::kOutputContainer, fileName.Data()));
    fAliAnlManager->ConnectOutput(fAliAnlTask,2,fAliAnlManager->CreateContainer(Form("fQCOutputList"),                                              TList::Class(), AliAnalysisManager::kOutputContainer, fileName.Data()));
    
    // - // TTrees
    if ( PhiFlag )              fAliAnlManager->ConnectOutput(fAliAnlTask,3,fAliAnlManager->CreateContainer("OutContainer3", TTree::Class(), AliAnalysisManager::kOutputContainer, fileName.Data()));
    if ( KaonFlag )             fAliAnlManager->ConnectOutput(fAliAnlTask,4,fAliAnlManager->CreateContainer("OutContainer4", TTree::Class(), AliAnalysisManager::kOutputContainer, fileName.Data()));
    if ( MCFlag && PhiFlag )    fAliAnlManager->ConnectOutput(fAliAnlTask,5,fAliAnlManager->CreateContainer("OutContainer5", TTree::Class(), AliAnalysisManager::kOutputContainer, fileName.Data()));
    if ( MCFlag && KaonFlag )   fAliAnlManager->ConnectOutput(fAliAnlTask,6,fAliAnlManager->CreateContainer("OutContainer6", TTree::Class(), AliAnalysisManager::kOutputContainer, fileName.Data()));
    
    // Add task
    fAliAnlManager->AddTask(fAliAnlTask);
    
    return fAliAnlTask;
}
