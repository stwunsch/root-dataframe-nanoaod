void postprocess_nanoaod() {
  TFile input("NanoAOD.root");
  auto ntuple = (TTree *)input.Get("aod2nanoaod/Events");

  TFile output("NanoAOD_Example_CMS2011OpenData.root", "RECREATE");
  output.cd();
  ntuple->CloneTree()->Write();
  output.Close();
}
