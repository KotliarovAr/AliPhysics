/**
 * @file   QATrain.C
 * @author Christian Holm Christensen <cholm@master.hehi.nbi.dk>
 * @date   Fri Jun  1 13:55:50 2012
 * 
 * @brief  
 * 
 * 
 * @ingroup pwglf_forward_trains_examples
 */

#include "TrainSetup.C"
#include <AliESDInputHandlerRP.h>
#include <AliCDBManager.h>

//====================================================================
/**
 * Analysis train to do full Quality Assurance train
 * 
 * @ingroup pwglf_forward_trains_examples
 */
class QATrain : public TrainSetup
{
public:
  enum { 
    kCDBConnect = 0x1, 
    kEventStats = 0x2,  // Event Statistics (Jan Fiete)
    kCentrality = 0x4,  // Centrality (A. Toia)
    kDefaultFlags = (kCDBConnect|kEventStats|kCentrality)
  };
  enum { 
    kVertex     = 0x000001,  // Vertexing (A. Dainese)
    kSymmetric  = 0x000002,  // TPC QA (E. Sicking)
    kVZERO      = 0x000004,  // VZERO QA  (C. Cheshkov)
    kTPC        = 0x000008,  // TPC (Jacek Otwinowski & Michael Knichel)
    kSPD        = 0x000010,  // SPD (A. Mastroserio) - Needs RP
    kSDD        = 0x000020,  // SDD (F. Prino) Needs RP
    kSSD        = 0x000040,  // SSD dEdx (Marek Chojnacki)
    kITS        = 0x000080,  // 
    kITSSA      = 0x000100,  // ITS saTracks (F.Prino)
    kITSAlign   = 0x000200,  // ITS align (F.Prino)
    kTRD        = 0x000400,  // TRD (Alex Bercuci, M. Fasel) 
    kZDC        = 0x000800,  // ZDC (Chiara Oppedisano) 
    kCALO       = 0x001000,  // Calorimetry (Gustavo Conesa)
    kMUONTRG    = 0x002000,  // Muon Trigger
    kMUONEff    = 0x004000,  // Muon Efficiency (not used) Need geo 
    kV0         = 0x008000,  // V0-Decay Reconstruction (Ana Marin)
			     // (not used) Need MC truth 
    kBRes       = 0x010000,  // Impact parameter resolution
			     // (xianbao.yuan@pd.infn.it,
			     // andrea.dainese@pd.infn.it) 
    kMUON       = 0x020000,  // MUON QA (Philippe Pillot)
    kTOF        = 0x040000,  // TOF (Francesca Bellini)
    kPIDRes     = 0x080000,  // PIDResponse (Jens)
    kPID        = 0x100000,  // PIDqa (Jens)
    kHMPID      = 0x200000,  // HMPID QA (Giacomo Volpe)
    kT0         = 0x400000,  // T0 QA (Alla Mayevskaya)
    kFMD        = 0x800000,  // FMD QA (Christian Holm Christiansen)
    kDefaultModules  = (kVertex|kSymmetric|kVZERO|kTPC|kSPD|kSDD|kSSD|kITS|
			kITSSA|kITSAlign|kTRD|kZDC|kCALO|kMUONTRG|kBRes|
			kMUON|kTOF|kPIDRes|kPID|kHMPID|kT0|kFMD)
  };
    
    

  /** 
   * Constructor.  Date and time must be specified when running this
   * in Termiante mode on Grid
   * 
   * @param name     Name of train 
   */
  QATrain(const char* name="PilotAnalysis")
    : TrainSetup(name, false, 0, 0, 0, 0, 0), 
      fRun(0),
      fFlags(kDefaultFlags), 
      fModules(kDefaultModules), 
      fTriggerMask(AliVEvent::kAnyINT), 
      fTriggerHM(AliVEvent::kHighMult),
      fTriggerEMC(AliVEvent::kEMC7), 
      fTriggerMUONBarrel(AliVEvent::kMUU7),
      fCollisionType(0) // 0: pp, 1: PbPb
  {}
  void SetFlags(UShort_t flags) { fFlags = flags; }
  void SetRun(UInt_t run) { fRun = run; }
  void SetModules(UInt_t m) { fModules = m; }
protected:
  AliVEventHandler* CreateInputHandler(EType type)
  {
    if (type != kESD) return 0;
    AliAnalysisManager::GetAnalysisManager()->SetRunFromPath(fRun);
    
    AliESDInputHandlerRP* ih = new AliESDInputHandlerRP();
    ih->SetReadFriends(kTRUE);
    ih->SetActiveBranches("ESDfriend");
    return ih;
  }
  AliAnalysisTaskSE* CreateTaskAndSetCollisionCandidates(const char* macro,
							 const char* args="")
  {
    AliAnalysisTaskSE* task = CoupleSECar(macro, args);
    if (task) task->SelectCollisionCandidates(fTriggerMask);
    return task;
  }
  void CreateCDBConnect()
  {                                 
    ::Info("CreateCDBConnect", "Loading CDB connect w/run=%d", fRun);
    AliAnalysisTask* task = CoupleCar("AddTaskCDBconnect.C", Form("%d", fRun));
    ::Info("CreateCDBConnect", "Loaded %p", task);
    if (!task) return;
    AliCDBManager::Instance()->SetDefaultStorage("raw://");
  }
  void CreatePhysicsSelection(Bool_t mc,
			      AliAnalysisManager* mgr)
  {
    // Event Statistics (Jan Fiete)
    if (!(fFlags & kEventStats)) return;
    TrainSetup::CreatePhysicsSelection(mc, mgr);
  }
  void CreateCentralitySelection(Bool_t mc, AliAnalysisManager* mgr)
  {
    // Centrality (A. Toia)
    if (!(fFlags & kCentrality)) return;
    TrainSetup::CreateCentralitySelection(mc, mgr);
  } 
  void CreateVertex()
  {
    // Vertexing (A. Dainese)
    CoupleCar("AddTaskVertexESD.C", Form("kFALSE,0x%x", fTriggerMask));
  }
  void CreateSymmetric()
  {
    // TPC QA (E. Sicking)
    CoupleCar("AddTaskQAsym.C", Form("0,0x%x,0x%x,0x%x,0x%x",
				     fTriggerMask, fTriggerHM, fTriggerEMC, 
				     fTriggerMUONBarrel));
  }
  void CreateVZERO()
  {
    //  VZERO QA  (C. Cheshkov)
    CoupleCar("AddTaskVZEROQA.C", "0");
  }
  void CreateTPC()
  {
    // TPC (Jacek Otwinowski & Michael Knichel)
    //
    // Optionally MC information can be used by setting the 1st
    // argument to true 
    // 
    // Optionally friends information can be switched off by setting
    // the 2st argument to false
    // 
    // Optionally highMult axis can be used by setting the 3st
    // argument to true (for PbPb)
    gROOT->SetMacroPath(Form("%s:$(ALICE_PHYSICS)/PWGPP/TPC/macros",
			     gROOT->GetMacroPath()));
    CreateTaskAndSetCollisionCandidates("AddTaskPerformanceTPCdEdxQA.C",
					"kFALSE,kTRUE,kFALSE");
  }
  void CreateSPD()
  {
    // SPD (A. Mastroserio)
    CreateTaskAndSetCollisionCandidates("AddTaskSPDQA.C");
    // AliAnalysisTask* task = 
    //   CreateTaskAndSetCollisionCandidates("AddTaskSPDQA.C");
    // if (!task) return;
    // task->SetOCDBInfo(fRun, "raw://");
  }
  void CreateSDD()
  {
    // SDD (F. Prino)
    CreateTaskAndSetCollisionCandidates("AddSDDPoints.C");
  }
  void CreateSSD()
  {
    // SSD dEdx (Marek Chojnacki)
    CreateTaskAndSetCollisionCandidates("AddTaskdEdxSSDQA.C");
  }
  void CreateITS()
  {
    CoupleCar("AddTaskPerformanceITS.C","false");
    if (fCollisionType == 0) return;

    gROOT->ProcessLine("AddTaskPerformanceITS(false,false,false,3500,10000)");
    gROOT->ProcessLine("AddTaskPerformanceITS(false,false,false,590,1570)");
    gROOT->ProcessLine("AddTaskPerformanceITS(false,false,false,70,310)");
  }
  void CreateITSSA() 
  {
    // ITS saTracks, align (F.Prino)
    CreateTaskAndSetCollisionCandidates("AddTaskITSsaTracks.C",
					"false,false)");
  }
  void CreateITSAlign()
  {
    // ITS saTracks, align (F.Prino)
    CoupleCar("AddTaskITSAlign.C","0,2011");
  }
  void CreateTRD()
  {
    // TRD (Alex Bercuci, M. Fasel) 
    gSystem->AddIncludePath("-I${ALICE_PHYSICS}/PWGPP/TRD");
    CoupleCar("AddTrainPerformanceTRD.C","\"ESD DET EFF RES PID\""); 
  }
  void CreateZDC()
  {
    // ZDC (Chiara Oppedisano)
    gROOT->SetMacroPath(Form("%s:$(ALICE_PHYSICS)/PWGPP/ZDC",
			     gROOT->GetMacroPath()));
    CreateTaskAndSetCollisionCandidates("AddTaskZDCQA.C");
  }
  void CreateCALO(EMode mode, Bool_t par)
  {
    // Calorimetry (Gustavo Conesa)
    LoadLibrary("EMCALUtils", mode, par, true);
    LoadLibrary("PHOSUtils", mode, par, true);
    LoadLibrary("PWG4PartCorrBase", mode, par, true);
    LoadLibrary("PWG4PartCorrDep", mode, par, true);
    
    gROOT->SetMacroPath(Form("%s:$(ALICE_PHYSICS)/PWG4/macros/QA",
			     gROOT->GetMacroPath()));
    CreateTaskAndSetCollisionCandidates("AddTaskCalorimeterQA.C",
					"\"ESD\",20011,false,false)");
    Long_t ret = 
      gROOT->ProcessLine("AddTaskCalorimeterQA(\"ESD\",2011,false,false,\"\",\"EMC7\")");
    if (!ret) return;
    AliAnalysisTaskSE* task = reinterpret_cast<AliAnalysisTaskSE*>(ret);
    task->SelectCollisionCandidates(fTriggerEMC);
  }
  void CreateMUONTRG(EMode mode, Bool_t par)
  {
    // Muon Trigger
    LoadLibrary("PWG3base", mode, par, true);
    LoadLibrary("PWG3muon", mode, par, true);
    LoadLibrary("PWG3muondep", mode, par, true);
    
    CoupleCar("AddTaskMTRchamberEfficiency.C");
  }
  void CreateMUONEff()
  {
    gROOT->SetMacroPath(Form("%s:$(ALICE_PHYSICS)/PWG3/muondep",
			     gROOT->GetMacroPath()));
    CoupleCar("AddTaskMUONTrackingEfficiency.C");
  }
  void CreateV0()
  {
    // V0-Decay Reconstruction (Ana Marin) (not used)
    CoupleCar("AddTaskV0QA.C","false");
  }
  void CreateBRes()
  {
    // Impact parameter resolution (xianbao.yuan@pd.infn.it,
    // andrea.dainese@pd.infn.it) 
    CreateTaskAndSetCollisionCandidates(Form("AddTaskImpParRes.C", 
					     fCollisionType == 0 ? 
					     "" : 
					     "false,-1,false,false"));
  }
  void CreateMUON(EMode mode, Bool_t par)
  {
    // MUON QA (Philippe Pillot)
    LoadLibrary("PWG3base", mode, par, true);
    LoadLibrary("PWG3muon", mode, par, true);
    LoadLibrary("PWG3muondep", mode, par, true);
    gROOT->SetMacroPath(Form("%s:$(ALICE_PHYSICS)/PWG3/muon",
			     gROOT->GetMacroPath()));
    CoupleCar("AddTaskMuonQA.C");
  }
  void CreateTOF()
  {
    // TOF (Francesca Bellini)
    gROOT->SetMacroPath(Form("%s:$(ALICE_PHYSICS)/PWGPP/TOF",
			     gROOT->GetMacroPath()));
    CreateTaskAndSetCollisionCandidates("AddTaskTOFQA.C");
  }
  void CreatePIDRes()
  {
    // PIDResponse (Jens)
    CreateTaskAndSetCollisionCandidates("AddTaskPIDResponse.C");
  }

  void CreatePID()
  {
    // PIDqa (Jens)
    CreateTaskAndSetCollisionCandidates("AddTaskPIDqa.C");
  }
  void CreateHMPID()
  {
    // HMPID QA (Giacomo Volpe)
    gROOT->SetMacroPath(Form("%s:$(ALICE_PHYSICS)/PWGPP/HMPID",
			     gROOT->GetMacroPath()));
    CreateTaskAndSetCollisionCandidates("AddTaskHmpidQA.C");
  }
  void CreateT0()
  {
    // T0 QA (Alla Mayevskaya)
    gROOT->SetMacroPath(Form("%s:$(ALICE_PHYSICS)/PWGPP/T0",
			     gROOT->GetMacroPath()));
    CreateTaskAndSetCollisionCandidates("AddTaskT0QA.C");
  }
  void CreateFMD(EMode mode, Bool_t par)
  {
    // FMD QA (Christian Holm Christiansen)
    LoadLibrary("PWGLFforward2", mode, par, true);
    Bool_t mc = AliAnalysisManager::GetAnalysisManager()
      ->GetMCtruthEventHandler() != 0;
    CoupleCar("AddTaskForwardQA.C", Form("(%d,%d)",mc,(fFlags & kCentrality)));
  }
  //__________________________________________________________________
  /** 
   * Create the tasks 
   * 
   * @param mode Processing mode
   * @param par  Whether to use par files 
   * @param mgr  Analysis manager 
   */
  void CreateTasks(EMode mode, Bool_t par, AliAnalysisManager* mgr)
  {
    // --- Output file name ------------------------------------------
    AliAnalysisManager::SetCommonFileName("QAResults.root");

    LoadLibrary("CORRFW", mode, par);
    LoadLibrary("TENDER", mode, par);
    LoadLibrary("PWG0base", mode, par);
    LoadLibrary("PWG0dep", mode, par);
    LoadLibrary("PWG0selectors", mode, par);
    LoadLibrary("PWGPP", mode, par);    

    gROOT->SetMacroPath(Form("%s:$(ALICE_PHYSICS)/PWGPP/PilotTrain"
			     ":$(ALICE_PHYSICS)/PWGPP/macros",
			     gROOT->GetMacroPath()));   
 
    mgr->AddStatisticsTask(fTriggerMask);
    if (fFlags   & kCDBConnect) CreateCDBConnect();
    if (fModules & kVertex)     CreateVertex();
    if (fModules & kSymmetric)  CreateSymmetric();
    if (fModules & kVZERO)      CreateVZERO();
    if (fModules & kTPC)        CreateTPC();
    if (fModules & kSPD)        CreateSPD();
    if (fModules & kSDD)        CreateSDD();
    if (fModules & kSSD)        CreateSSD();
    if (fModules & kITS)        CreateITS();
    if (fModules & kITSSA)      CreateITSSA();
    if (fModules & kITSAlign)   CreateITSAlign();
    if (fModules & kTRD)        CreateTRD(); 
    if (fModules & kZDC)        CreateZDC(); 
    if (fModules & kCALO)       CreateCALO(mode, par); 
    if (fModules & kMUONTRG)    CreateMUONTRG(mode, par); 
    if (fModules & kMUONEff)    CreateMUONEff(); 
    if (fModules & kV0)         CreateV0(); 
    if (fModules & kBRes)       CreateBRes(); 
    if (fModules & kMUON)       CreateMUON(mode, par); 
    if (fModules & kTOF)        CreateTOF(); 
    if (fModules & kPIDRes)     CreatePIDRes(); 
    if (fModules & kPID)        CreatePID(); 
    if (fModules & kHMPID)      CreateHMPID(); 
    if (fModules & kT0)         CreateT0(); 
    if (fModules & kFMD)        CreateFMD(mode, par); 
  }
  /** 
   * Crete output handler - we don't want one here. 
   * 
   * @return 0
   */
  AliVEventHandler* CreateOutputHandler(EType) { return 0; }
  UInt_t   fRun; // Run number 
  UShort_t fFlags; // Flags 
  UInt_t   fModules; // Modules to load 
  UInt_t   fTriggerMask; 
  UInt_t   fTriggerHM;
  UInt_t   fTriggerEMC;
  UInt_t   fTriggerMUONBarrel;
  UShort_t fCollisionType; // 0: pp, 1: PbPb 
  
  
  Bool_t fUseCent; // Whether to use centrality or not 
};

//
// EOF
//
