#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TString.h>
#include <TF1.h>
#include <TCanvas.h>
#include <iostream>


void normalize(TH1D* hist, TH1D* hist_mc, TH1D* hist_mc2)
{
    hist->Scale(1./hist->Integral());
    hist_mc->Scale(1./hist_mc->Integral());
    hist_mc2->Scale(1./hist_mc2->Integral());

    double xmax = 1.5* hist->GetMaximum();
    hist->SetMaximum(xmax);
}

void plots(TH1D* h1, TH1D* h2, TString fname)
{
    auto can = new TCanvas(fname.Data(), fname.Data(), 800, 800);

    h1->SetFillColorAlpha(kTeal+1, 0.3);

    h2->SetMarkerColor(kViolet+1);
    h2->SetMarkerStyle(20);

//    double xmax = 1.5* h2->GetMaximum();
//    h2->SetMaximum(xmax);

//    h3->SetMarkerColor(kViolet +1);
//    h3->SetMarkerStyle(20);

    h1->Draw("HIST");
    h2->Draw("E1 SAME");
//    h3->Draw("E1 SAME");

    TString save_name = Form("imgs/%s.png", fname.Data());

    can->SaveAs(save_name.Data());
}


void ratio_plots(TH1D* hist, TH1D* hist_mc, TString var, TString units, TString fname)
{
    hist_mc->Divide(hist);

    TString hname = Form("ratio_%s_%s", var.Data(), fname.Data());
    TString htitle = Form("; %s [%s]; #frac{MC}{Real}", var.Data(), units.Data());

    hist_mc->SetNameTitle(hname.Data(), htitle.Data());
    hist_mc->SetMaximum(3.0);
    hist_mc->SetMinimum(-1.0);

    hist_mc->SetMarkerColor(kAzure+1);
    hist_mc->SetMarkerStyle(20);

    TF1* fit = new TF1("fit", "[0]* x+ [1]");
    fit->SetParName(0, "a");
    fit->SetParName(1, "b");
//    fit->SetParName(2, "c");

    TString cname = Form("can_%s", hname.Data());

    auto c = new TCanvas(cname.Data(), cname.Data(), 800, 800);
    hist_mc->Fit(fit);
    hist_mc->Draw("E1");

    TString pname = Form("imgs/%s_ratio_%s.png", fname.Data(), var.Data());
    c->SaveAs(pname.Data());

    delete fit;
}


void compare_plots()
{
    TH1::SetDefaultSumw2();

    gStyle->SetOptStat(0);
    gStyle->SetOptFit(1);

    int bins = 20;

    auto infile = TFile::Open("../e906-LH2-data/output.root", "READ");
    auto tree = (TTree*)infile->Get("tree");
    auto tree_mc = (TTree*)infile->Get("tree_mc");

    int nevents = tree->GetEntries();
    int nevents_mc = tree_mc->GetEntries();

    std::cout << "---> mc events " << nevents_mc << " real events " <<  nevents <<std::endl;

    double mass, pT, xT, xB, xF, weight;
    double mass_mc, pT_mc, xT_mc, xB_mc, xF_mc, weight_mc, weight2_mc;

    tree->SetBranchAddress("mass", &mass);
    tree->SetBranchAddress("pT", &pT);
    tree->SetBranchAddress("xT", &xT);
    tree->SetBranchAddress("xB", &xB);
    tree->SetBranchAddress("xF", &xF);
    tree->SetBranchAddress("weight", &weight);

    tree_mc->SetBranchAddress("mass", &mass_mc);
    tree_mc->SetBranchAddress("pT", &pT_mc);
    tree_mc->SetBranchAddress("xT", &xT_mc);
    tree_mc->SetBranchAddress("xB", &xB_mc);
    tree_mc->SetBranchAddress("xF", &xF_mc);
    tree_mc->SetBranchAddress("weight", &weight_mc);
    tree_mc->SetBranchAddress("weight2", &weight2_mc);

    auto hmass = new TH1D("hmass", "; mass [GeV]; counts", bins, 4.5, 9.0);
    auto hpT = new TH1D("hpT", "; pT [GeV]; counts", bins, 0.0, 2.5);
    auto hxT = new TH1D("hxT", "; xT; counts", bins, 0.1, 0.6);
    auto hxB = new TH1D("hxB", "; xB; counts", bins, 0.3, 1.0);
    auto hxF = new TH1D("hxF", "; xF; counts", bins, -0.1, 1.0);

    auto hmass_mc = new TH1D("hmass_mc", "; mass [GeV]; counts", bins, 4.5, 9.0);
    auto hpT_mc = new TH1D("hpT_mc", "; pT [GeV]; counts", bins, 0.0, 2.5);
    auto hxT_mc = new TH1D("hxT_mc", "; xT; counts", bins, 0.1, 0.6);
    auto hxB_mc = new TH1D("hxB_mc", "; xB; counts", bins, 0.3, 1.0);
    auto hxF_mc = new TH1D("hxF_mc", "; xF; counts", bins, -0.1, 1.0);

    auto hmass_mc2 = new TH1D("hmass_mc2", "; mass [GeV]; counts", bins, 4.5, 9.0);
    auto hpT_mc2 = new TH1D("hpT_mc2", "; pT [GeV]; counts", bins, 0.0, 2.5);
    auto hxT_mc2 = new TH1D("hxT_mc2", "; xT; counts", bins, 0.1, 0.6);
    auto hxB_mc2 = new TH1D("hxB_mc2", "; xB; counts", bins, 0.3, 1.0);
    auto hxF_mc2 = new TH1D("hxF_mc2", "; xF; counts", bins, -0.1, 1.0);


    for(int ii = 0; ii < nevents; ii++)
    {
        tree->GetEntry(ii);
        tree_mc->GetEntry(ii);

        hmass->Fill(mass, weight);
        hpT->Fill(pT, weight);
        hxB->Fill(xB, weight);
        hxT->Fill(xT, weight);
        hxF->Fill(xF, weight);

        hmass_mc->Fill(mass_mc, weight_mc);
        hpT_mc->Fill(pT_mc, weight_mc);
        hxB_mc->Fill(xB_mc, weight_mc);
        hxT_mc->Fill(xT_mc, weight_mc);
        hxF_mc->Fill(xF_mc, weight_mc);

        hmass_mc2->Fill(mass_mc, weight2_mc);
        hpT_mc2->Fill(pT_mc, weight2_mc);
        hxB_mc2->Fill(xB_mc, weight2_mc);
        hxT_mc2->Fill(xT_mc, weight2_mc);
        hxF_mc2->Fill(xF_mc, weight2_mc);

//        std::cout << weight2_mc << " " << weight_mc << std::endl;
    }

    normalize(hmass, hmass_mc, hmass_mc2);
    normalize(hpT, hpT_mc, hpT_mc2);
    normalize(hxB, hxB_mc, hxB_mc2);
    normalize(hxT, hxT_mc, hxT_mc2);
    normalize(hxF, hxF_mc, hxF_mc2);

    plots(hmass, hmass_mc, "before_mass");
    plots(hpT, hpT_mc, "before_pT");
    plots(hxB, hxB_mc, "before_xB");
    plots(hxT, hxT_mc, "before_xT");
    plots(hxF, hxF_mc, "before_xF");

    plots(hmass, hmass_mc2, "after_mass");
    plots(hpT, hpT_mc2, "after_pT");
    plots(hxB, hxB_mc2, "after_xB");
    plots(hxT, hxT_mc2, "after_xT");
    plots(hxF, hxF_mc2, "after_xF");

    ratio_plots(hmass, hmass_mc, "mass", "GeV", "before");
    ratio_plots(hpT, hpT_mc, "pT", "GeV", "before");
    ratio_plots(hxB, hxB_mc, "xB", " ", "before");
    ratio_plots(hxT, hxT_mc, "xT", " ", "before");
    ratio_plots(hxF, hxF_mc, "xF", " ", "before");

    ratio_plots(hmass, hmass_mc2, "mass", "GeV", "after");
    ratio_plots(hpT, hpT_mc2, "pT", "GeV", "after");
    ratio_plots(hxB, hxB_mc2, "xB", " ", "after");
    ratio_plots(hxT, hxT_mc2, "xT", " ", "after");
    ratio_plots(hxF, hxF_mc2, "xF", " ", "after");
}