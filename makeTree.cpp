#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <sstream>
#include <filesystem>
#include <algorithm>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TMath.h"
#include "pshape_functions.h"


int main(int argc, char* argv[]){

  //parametri da input
  if (argc!=3){
    std::cout << "USAGE: ./makeTree [folder] [trigger]" << std::endl;
    std::cout << "se il trigger è in auto, metti 0" << std::endl;
    std::cout << "EXAMPLE: ./makeTree data_test 0" << std::endl;
    exit(1);
  }
 
  char* meas  =      argv[1];
  int trigger = atoi(argv[2]);
  
  //unificazioni vari tree già esistenti
  std::stringstream stream;
  
  //elimino tutti i tree singoli inutili
  //stream << "rm -f C1--*" << std::endl;
  //non cosa sia il file DS_Store.root che mi crea
  stream << "rm -f DS_*" << std::endl;
  system(stream.str().c_str());
  
  std::string directory  = "data/root/" + std::string(meas);
  
  std::string file1  = std::string(meas) + "_treeraw.root";
  std::string file2  = std::string(meas) + "_tree.root";
  
  search_and_remove(directory, file1);
  search_and_remove(directory, file2);
  
  stream << "cd data/root/" << std::string(meas) << std::endl;
  stream << "hadd -f " << std::string(meas) << "_treeraw.root C*treeraw.root" << std::endl;
  
  system(stream.str().c_str());
   
  std::cout << "All the trees were unified in one single tree named" << std::string(meas) << "_treeraw.root" << std::endl;
 
  //apertura file con il tree
 TFile run(Form("data/root/%s/%s_treeraw.root",meas,meas));
  TTree *tree_raw = (TTree*)run.Get(Form("treeraw"));
  Long64_t nentries = tree_raw->GetEntries();
  std::cout << nentries << std::endl;

  //preparazione per la lettura del tree_raw
  int   event_raw;
  int   lumi_raw;
  float sampling_raw;
  std::vector<float>* pshape = nullptr;

  tree_raw->SetBranchAddress("event"         , &event_raw    );
  tree_raw->SetBranchAddress("lumi"          , &lumi_raw     );
  tree_raw->SetBranchAddress("sampling_time" , &sampling_raw );
  tree_raw->SetBranchAddress("waveform"      , &pshape       );

  //creazione variabili del nuovo tree
  int   event;
  int   lumi;
  float baseline;
  float baseline_error;
  float charge;
  float amp;
  float sampling;
  Long64_t npoints;

  /*
  Controls ctrl; //classe definita in pshape_functions.h
  int ctrl_double;
  int ctrl_trigger;
  int ctrl_width;
  */
  
  //nuovo tree
  std::string outfile_name = Form("%s_tree.root",meas);
  TFile* outfile = TFile::Open(Form("data/root/%s/%s",meas,outfile_name.c_str()),"recreate");
  TTree* tree = new TTree("tree","tree");

  tree->Branch("event"            , &event            ,             "event/I");
  tree->Branch("lumi"             , &lumi             ,              "lumi/I");
  tree->Branch("trigger"          , &trigger          ,           "trigger/F");
  tree->Branch("baseline"         , &baseline         ,          "baseline/F");
  tree->Branch("baseline_error"   , &baseline_error   ,    "baseline_error/F");
  tree->Branch("charge"           , &charge           ,            "charge/F");
  tree->Branch("amp"              , &amp              ,               "amp/F");
  tree->Branch("sampling"         , &sampling         ,          "sampling/F");
  tree->Branch("npoints"          , &npoints          ,           "npoints/L");
  //tree->Branch("ctrl_double"      , &ctrl_double      ,       "ctrl_double/I");
  //tree->Branch("ctrl_width"       , &ctrl_width       ,        "ctrl_width/I");
  //tree->Branch("ctrl_trigger"     , &ctrl_trigger     ,      "ctrl_trigger/I");
 

  //riempimento del tree
  for(unsigned iEntry=0; iEntry<nentries; iEntry++){

    tree_raw->GetEntry(iEntry);
    npoints = pshape->size();
 
    //functions defined in utility.h
    event           = event_raw;
    lumi            = lumi_raw;
    sampling        = sampling_raw;
    baseline        = GetBaseline(pshape->data(),npoints); //puntatore al primo elemento del vettore
    baseline_error  = GetBaselineError(pshape->data(),npoints, baseline);
    charge          = GetCharge(pshape->data(),npoints, sampling_raw, baseline);
    amp             = GetAmp(pshape->data(),npoints, baseline);
    

    /*
    ctrl         = Ctrl_pshape(pshape, amp, trigger, baseline, baseline_error, CD_number,meas);
    ctrl_double  = ctrl.get_ctrl_double();
    ctrl_trigger = ctrl.get_ctrl_trigger();
    ctrl_width   = ctrl.get_ctrl_width();
    */
    
    tree->Fill();
    
  } //for on tree entries

  
  outfile->cd();
  tree->Write();
  
  std::cout << "tree entries: " << tree->GetEntries() << std::endl;
  std::cout << "tree saved in " << outfile_name << std::endl;
  outfile->Close();
  run.Close();
    
  return(0);
}
	       	       
  
