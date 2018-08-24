/// \file
/// \ingroup tutorial_dataframe
/// \notebook -nodraw
/// This tutorial illustrates how NanoAOD files can be processed with ROOT
/// dataframes. The NanoAOD-like input file is filled with events from
/// CMS OpenData containing Muon candidates from 2011 data. The script matches
/// Muon pairs and produces an histogram of the Di-Muon mass spectrum showing
/// particle resonances up to the Z mass.
///
/// \macro_code
///
/// \date August 2018
/// \author Stefan Wunsch

#include "ROOT/RDataFrame.hxx"
#include "ROOT/RVec.hxx"
#include "TCanvas.h"
#include "TH1D.h"
#include "TLatex.h"
#include "TLorentzVector.h"
#include "TStyle.h"

using namespace ROOT::VecOps;

template <typename T> void Draw(T h_result_ptr);

void nanoaod_dimuon_spectrum() {
  // Enable implicit multi-threading
  ROOT::EnableImplicitMT();

  // Create dataframe from NanoAOD file
  ROOT::RDataFrame df("Events", "http://root.cern.ch/files/NanoAOD_Example_CMS2011OpenData.root");

  // Baseline selection of more than two muons
  auto df_filtered = df.Filter("nMuon>=2", "More than two muons");

  // Fin muon pair with highest pt and opposite charge
  auto find_pair = [](RVec<double> &pt, RVec<double> &charge) {
    // Get indices that sort the muon pts in descending order
    auto idx = Reversed(Argsort(pt));

    // Find muon with second-highest pt and opposite charge
    for (size_t i = 1; i < idx.size(); i++) {
      const auto i1 = idx[0];
      const auto i2 = idx[i];
      if (charge[i1] != charge[i2]) {
        return RVec<size_t>({i1, i2});
      }
    }

    // Return empty selection if no candidate matches
    return RVec<size_t>({});
  };
  auto df_pair =
      df_filtered.Define("Muon_pair", find_pair, {"Muon_pt", "Muon_charge"})
          .Filter("Muon_pair.size() == 2", "Found valid pair");

  // Compute invariant mass of the di-muon system
  auto compute_mass = [](RVec<double> &pt, RVec<double> &eta, RVec<double> &phi,
                         RVec<double> &mass, RVec<size_t> &idx) {
    // Compose four-vectors of both muons
    TLorentzVector p1;
    const auto i1 = idx[0];
    p1.SetPtEtaPhiM(pt[i1], eta[i1], phi[i1], mass[i1]);

    TLorentzVector p2;
    const auto i2 = idx[1];
    p2.SetPtEtaPhiM(pt[i2], eta[i2], phi[i2], mass[i2]);

    // Add four-vectors to build di-muon system and return the invariant mass
    return (p1 + p2).M();
  };
  auto df_mass = df_pair.Define(
      "Dimuon_mass", compute_mass,
      {"Muon_pt", "Muon_eta", "Muon_phi", "Muon_mass", "Muon_pair"});

  // Plot histogram of di-muon mass spectrum
  auto h = df_mass.Histo1D({"Dimuon_mass", "Dimuon_mass", 20000, 0.25, 300},
                           "Dimuon_mass");

  // Draw histogram
  Draw(h);
}

int main() { nanoaod_dimuon_spectrum(); }

template <typename T> void Draw(T h_result_ptr) {
  gStyle->SetOptStat(0);
  gStyle->SetTextFont(42);
  TCanvas c("c", "c", 800, 600);
  c.SetLogx();
  c.SetLogy();

  auto h = h_result_ptr.GetValue();
  h.SetTitle("");
  h.GetXaxis()->SetTitle("Invariant Di-Muon mass (GeV)");
  h.GetXaxis()->SetTitleOffset(1.2);
  ;
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
