#include "ROOT/RDataFrame.hxx"
#include "ROOT/RVec.hxx"
#include "TCanvas.h"
#include "TH1D.h"
#include "TLorentzVector.h"
#include "TStyle.h"
#include "TLatex.h"

using namespace ROOT::VecOps;

template <typename T> void Draw(T h_result_ptr);

void invariant_dimuon_mass() {
  // Enable implicit multi-threading
  ROOT::EnableImplicitMT();

  // Create dataframe from NanoAOD file
  ROOT::RDataFrame df("Events", "NanoAOD_Example_CMS2011OpenData.root");

  // Baseline selection of more than two Muons
  auto df_filtered = df.Filter("nMuon>=2", "More than two muons");

  // Select Muon pair with highest pt and opposite charge
  auto find_good_pair = [](RVec<double> &pt, RVec<double> &charge) {
    auto idx = Reversed(Argsort(pt));
    RVec<size_t> muon_good;
    for (size_t i = 1; i < idx.size(); i++) {
      const auto i1 = idx[0];
      const auto i2 = idx[i];
      if (charge[i1] != charge[i2]) {
        muon_good.push_back(i1);
        muon_good.push_back(i2);
        break;
      }
    }
    return muon_good;
  };
  auto df_good =
      df_filtered
          .Define("Muon_good", find_good_pair, {"Muon_pt", "Muon_charge"})
          .Filter("Muon_good.size() == 2", "Found valid pair");

  // Calculate invariant mass of Di-Muon systems
  auto compute_mass = [](RVec<double> &pt, RVec<double> &eta, RVec<double> &phi,
                         RVec<double> &mass, RVec<size_t> &idx) {
    TLorentzVector p1;
    TLorentzVector p2;
    const auto i1 = idx[0];
    const auto i2 = idx[1];
    p1.SetPtEtaPhiM(pt[i1], eta[i1], phi[i1], mass[i1]);
    p2.SetPtEtaPhiM(pt[i2], eta[i2], phi[i2], mass[i2]);
    auto p = p1 + p2;
    auto dimuon_mass = p.M();
    return dimuon_mass;
  };
  auto df_mass = df_good.Define(
      "Dimuon_mass", compute_mass,
      {"Muon_pt", "Muon_eta", "Muon_phi", "Muon_mass", "Muon_good"});

  // Plot histogram of Di-Muon mass spectrum
  auto h = df_mass.Histo1D({"Dimuon_mass", "Dimuon_mass", 20000, 0.25, 300},
                           "Dimuon_mass");

  // Draw histogram
  Draw(h);
}

int main() { invariant_dimuon_mass(); }

template <typename T> void Draw(T h_result_ptr) {
  gStyle->SetOptStat(0);
  gStyle->SetTextFont(42);
  TCanvas c("c", "c", 800, 600);
  c.SetLogx();
  c.SetLogy();

  auto h = h_result_ptr.GetValue();
  h.SetTitle("");
  h.GetXaxis()->SetTitle("Invariant Di-Muon mass (GeV)");
  h.GetXaxis()->SetTitleOffset(1.2);;
  h.GetYaxis()->SetTitle("N_{events}");
  h.Draw();

  TLatex label;
  label.DrawLatex(0.50, 7.000, "#bf{CMS Open Data}");
  label.DrawLatex(0.50, 2.500, "#bf{#sqrt{s} = 7 TeV}");
  label.DrawLatex(0.50, 9.0e3, "#eta");
  label.DrawLatex(0.65, 2.0e4, "#rho,#omega");
  label.DrawLatex(1.15, 1.0e4, "#phi");
  label.DrawLatex(3.50, 2.0e4, "J/#psi");
  label.DrawLatex(4.00, 3.0e3, "#psi'");
  label.DrawLatex(8.00, 1.2e4, "Y(1,2,3S)");
  label.DrawLatex(84.0, 1.0e3, "Z");

  c.SaveAs("nanoaod_dimuon_spectrum.pdf");
}
