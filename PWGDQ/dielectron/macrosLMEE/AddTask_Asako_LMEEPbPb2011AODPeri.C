AliAnalysisTask *AddTask_Asako_LMEEPbPb2011AODPeri(Bool_t runAll=kFALSE,Bool_t setMC=kFALSE,Bool_t getFromAlien=kFALSE, Bool_t PIDbaseline=kFALSE){

  Bool_t bESDANA=kFALSE; //Autodetect via InputHandler
  //get the current analysis manager
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  if (!mgr) {
	Error("AddTask_Asako_LMEEPbPb2011AODPeri", "No analysis manager found.");
	return 0;
  }


//  create task and add it to the manager
//	gSystem->AddIncludePath("$ALICE_ROOT/PWGDQ/dielectron/macrosLMEE");


  TString configBasePath("$TRAIN_ROOT/cbaumann_dielectron/");
  TString trainRoot=gSystem->Getenv("TRAIN_ROOT");

        if (trainRoot.IsNull()) configBasePath= "$ALICE_ROOT/PWGDQ/dielectron/macrosLMEE/";
		//  if (trainRoot.IsNull()) configBasePath= "/home/tsuji/nfs/AliceAna/pol2/Cent/";
 

  if (getFromAlien &&
      (!gSystem->Exec("alien_cp alien:///alice/cern.ch/user/c/cbaumann/PWGDQ/dielectron/macrosLMEE/ConfigAsakoLMEEPbPb2011AOD.C .")) &&
      (!gSystem->Exec("alien_cp alien:///alice/cern.ch/user/c/cbaumann/PWGDQ/dielectron/macrosLMEE/LMEECutLibAsako.C ."))
     ) {
        configBasePath=Form("%s/",gSystem->pwd());
  }

  TString configFile("ConfigAsakoLMEEPbPb2011AOD.C");
  TString configLMEECutLib("LMEECutLibAsako.C");

  TString configFilePath(configBasePath+configFile);
  TString configLMEECutLibPath(configBasePath+configLMEECutLib);

  //AOD Usage currently tested with separate task, to be merged
  if (mgr->GetInputEventHandler()->IsA()==AliAODInputHandler::Class()){
	::Info("AddTaskLMEEPbPb2011", "no dedicated AOD configuration");
  }
  else if (mgr->GetInputEventHandler()->IsA()==AliESDInputHandler::Class()){
	::Info("AddTaskLMEEPbPb2011AOD","switching on ESD specific code");
	bESDANA=kTRUE;
  }


  //Do we have an MC handler?
  Bool_t hasMC=setMC;
  if (AliAnalysisManager::GetAnalysisManager()->GetMCtruthEventHandler()!=0x0)
	hasMC=kTRUE;



  if (!gROOT->GetListOfGlobalFunctions()->FindObject(configLMEECutLib.Data()))
	gROOT->LoadMacro(configLMEECutLibPath.Data());
  if (!gROOT->GetListOfGlobalFunctions()->FindObject(configFile.Data()))
	gROOT->LoadMacro(configFilePath.Data());


  LMEECutLibAsako* cutlib = new LMEECutLibAsako();
  AliAnalysisTaskMultiDielectron *task=new AliAnalysisTaskMultiDielectron("MultiDiEData");
  if (!hasMC) task->UsePhysicsSelection();
  task->SetTriggerMask(AliVEvent::kMB+AliVEvent::kCentral+AliVEvent::kSemiCentral);
//  task->SelectCollisionCandidates(AliVEvent::kMB+AliVEvent::kCentral+AliVEvent::kSemiCentral);
//  task->SetRejectPileup();
  task->SelectCollisionCandidates(AliVEvent::kAny);  
  task->SetEventFilter(cutlib->GetEventCuts(LMEECutLibAsako::kPbPb2011TPCandTOF)); //
	

  //load dielectron configuration file

  //add dielectron analysis with different cuts to the task

  
  AliDielectron *lowmass4=ConfigAsakoLMEEPbPb2011AOD(4,hasMC,bESDANA);
  task->AddDielectron(lowmass4);
  printf("add: %s\n",lowmass4->GetName());
  
  AliDielectron *lowmass8=ConfigAsakoLMEEPbPb2011AOD(8,hasMC,bESDANA);
  task->AddDielectron(lowmass8);
  printf("add: %s\n",lowmass8->GetName());
  
  AliDielectron *lowmass12=ConfigAsakoLMEEPbPb2011AOD(12,hasMC,bESDANA);
  task->AddDielectron(lowmass12);
  printf("add: %s\n",lowmass12->GetName());
  
  //if (PIDbaseline) {
  //AliDielectron *lowmass7=ConfigLMEEPbPb2011AOD(7,hasMC,bESDANA);
  //	task->AddDielectron(lowmass7);
  //	printf("add: %s\n",lowmass7->GetName());
  //}

  mgr->AddTask(task);

  //create output container
  AliAnalysisDataContainer *coutput1 =
	mgr->CreateContainer("asako_LMEEPbPb2011Peri_tree",
		TTree::Class(),
		AliAnalysisManager::kExchangeContainer,
		"LMEEoutput.root");

  AliAnalysisDataContainer *cOutputHist1 =
	mgr->CreateContainer("asako_LMEEPbPb2011Peri_out",
		TList::Class(),
		AliAnalysisManager::kOutputContainer,
		"LMEEoutput.root");

  /*  AliAnalysisDataContainer *cOutputHist2 =
	  mgr->CreateContainer("cbaumann_lowmass_CF",
	  TList::Class(),
	  AliAnalysisManager::kOutputContainer,
	  "cbaumann_lowmass_CF.root");
	  */
  AliAnalysisDataContainer *cOutputHist2 =
	mgr->CreateContainer("asako_LMEEPbPb2011Peri_CF",
		TList::Class(),
		AliAnalysisManager::kOutputContainer,
		"LMEEoutput.root");

  AliAnalysisDataContainer *cOutputHist3 =
	mgr->CreateContainer("asako_EventStatPbPb2011Peri",
		TH1D::Class(),
		AliAnalysisManager::kOutputContainer,
		"LMEEoutput.root");


  mgr->ConnectInput(task,  0, mgr->GetCommonInputContainer());
  mgr->ConnectOutput(task, 0, coutput1 );
  mgr->ConnectOutput(task, 1, cOutputHist1);
  mgr->ConnectOutput(task, 2, cOutputHist2);
  mgr->ConnectOutput(task, 3, cOutputHist3);

  return task;
}
