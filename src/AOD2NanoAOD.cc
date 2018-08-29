// -*- C++ -*-
//
// Package:    AOD2NanoAOD
// Class:      AOD2NanoAOD

// system include files
#include <memory>

// user include files, general
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

//------ EXTRA HEADER FILES--------------------//
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/Common/interface/Ref.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "math.h"

// ROOT headers
#include "TFile.h"
#include "TTree.h"

// for tracking information
#include "DataFormats/TrackReco/interface/HitPattern.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

// class declaration
class AOD2NanoAOD : public edm::EDAnalyzer {
public:
  explicit AOD2NanoAOD(const edm::ParameterSet &);
  ~AOD2NanoAOD();

private:
  virtual void beginJob();
  virtual void analyze(const edm::Event &, const edm::EventSetup &);
  virtual void endJob();
  bool providesGoodLumisection(const edm::Event &iEvent);

  TTree *tree;
  TBranch *branch_mu_pt;
  TBranch *branch_mu_eta;
  TBranch *branch_mu_phi;
  TBranch *branch_mu_mass;
  TBranch *branch_mu_charge;
  TBranch *branch_mu_n;
  TBranch *branch_run;
  TBranch *branch_lumi_block;
  TBranch *branch_event;
  Int_t value_run;
  UInt_t value_lumi_block;
  UInt_t value_mu_n;
  ULong64_t value_event;
  std::vector<float> value_mu_pt;
  std::vector<float> value_mu_eta;
  std::vector<float> value_mu_phi;
  std::vector<float> value_mu_mass;
  std::vector<int> value_mu_charge;
};

AOD2NanoAOD::AOD2NanoAOD(const edm::ParameterSet &iConfig) {
  edm::Service<TFileService> fs;

  tree = fs->make<TTree>("Events", "Events");
  branch_mu_pt = tree->Branch("Muon_pt", &value_mu_pt);
  branch_mu_eta = tree->Branch("Muon_eta", &value_mu_eta);
  branch_mu_phi = tree->Branch("Muon_phi", &value_mu_phi);
  branch_mu_mass = tree->Branch("Muon_mass", &value_mu_mass);
  branch_mu_charge = tree->Branch("Muon_charge", &value_mu_charge);
  branch_mu_n = tree->Branch("nMuon", &value_mu_n);
  branch_run = tree->Branch("run", &value_run);
  branch_lumi_block = tree->Branch("luminosityBlock", &value_lumi_block);
  branch_event = tree->Branch("event", &value_event);
}

AOD2NanoAOD::~AOD2NanoAOD() {}

// method called for each event
void AOD2NanoAOD::analyze(const edm::Event &iEvent,
                          const edm::EventSetup &iSetup) {

  using namespace edm;
  using namespace reco;
  using namespace std;

  // Event is to be analyzed

  /*
  LogInfo("DEBUG")
  << "Starting to analyze \n"
  << "Event number: " << (iEvent.id()).event()
  << ", Run number: " << iEvent.run()
  << ", Lumisection: " << iEvent.luminosityBlock();
  *
  */

  // Load (relevant) Event information
  // https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookChapter4#GetData
  // https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideEDMGetDataFromEvent#get_ByLabel
  // https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideAodDataTable
  // https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideRecoDataTable

  // INFO: globalMuons
  // NB: note that when using keyword "globalMuons" getByLabel-function returns
  //     reco::TrackCollection
  Handle<reco::TrackCollection> gmuons;
  iEvent.getByLabel("globalMuons", gmuons);

  // Analysing Global Muons (gmuons-TrackCollection)

  // Loop over all the Global Muons of current Event
  value_mu_pt.clear();
  value_mu_eta.clear();
  value_mu_phi.clear();
  value_mu_charge.clear();
  value_mu_mass.clear();
  value_mu_n = gmuons->size();
  value_run = iEvent.run();
  value_lumi_block = iEvent.luminosityBlock();
  value_event = iEvent.id().event();

  for (reco::TrackCollection::const_iterator it = gmuons->begin(); it != gmuons->end(); it++) {
    value_mu_pt.push_back(it->pt());
    value_mu_eta.push_back(it->eta());
    value_mu_phi.push_back(it->phi());
    value_mu_charge.push_back(it->charge());
    value_mu_mass.push_back(0.105658);
  }

  tree->Fill();
}

void AOD2NanoAOD::beginJob() {}

void AOD2NanoAOD::endJob() {}

// Define this as a plug-in
DEFINE_FWK_MODULE(AOD2NanoAOD);
