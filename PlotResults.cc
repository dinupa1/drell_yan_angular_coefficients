//
// dinupa3@gmail.com
//

#include <TFile.h>
#include <TTree.h>
#include <TH2D.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TString.h>
#include <TF2.h>
#include <TFitResultPtr.h>

#include "FitHist.h"

FitHist::FitHist()
{;}

void FitHist::Init()
{
    TFile* file = TFile::Open("results0.root", "READ");
    tree = (TTree*)file->Get("tree");
    n_events = tree->GetEntries();

    tree->SetBranchAddress("true_hist",          true_hist);
    tree->SetBranchAddress("reco_hist",          reco_hist);
    tree->SetBranchAddress("pred_hist",          pred_hist);
    tree->SetBranchAddress("lambda",             &lambda);
    tree->SetBranchAddress("mu",                 &mu);
    tree->SetBranchAddress("nu",                 &nu);
}

void FitHist::DrawFits()
{
    double pi = TMath::Pi();

    // draw 5 fit histograms
    for(int i = 0; i < 5; i++)
    {
        tree->GetEntry(i);

        TString hist_name = Form("#lambda = %.3f, #mu = %.3f, #nu = %.3f ; #phi [rad]; cos#theta [a.u.]", lambda, mu, nu);

        TH2D* hist_true = new TH2D("hist_true", hist_name.Data(), 12, -pi, pi, 12, -0.6, 0.6);
        TH2D* hist_reco = new TH2D("hist_reco", hist_name.Data(), 12, -pi, pi, 12, -0.6, 0.6);
        TH2D* hist_pred = new TH2D("hist_pred", hist_name.Data(), 12, -pi, pi, 12, -0.6, 0.6);

        for(int j = 0; j < 12; j++)
        {
            for(int k = 0; k < 12; k++)
            {
                double true_cnt = (true_hist[0][j][k] + true_hist[1][j][k])/2.;
                double true_err = (true_hist[1][j][k] - true_hist[0][j][k])/2.;

                double reco_cnt = (reco_hist[0][j][k] + reco_hist[1][j][k])/2.;
                double reco_err = (reco_hist[1][j][k] - reco_hist[0][j][k])/2.;

                double pred_cnt = (pred_hist[0][j][k] + pred_hist[1][j][k])/2.;
                double pred_err = (pred_hist[1][j][k] - pred_hist[0][j][k])/2.;

                hist_true->SetBinContent(j+1, k+1, true_cnt);
                hist_true->SetBinError(j+1, k+1, true_err);

                hist_reco->SetBinContent(j+1, k+1, reco_cnt);
                hist_reco->SetBinError(j+1, k+1, reco_err);

                hist_pred->SetBinContent(j+1, k+1, pred_cnt);
                hist_pred->SetBinError(j+1, k+1, pred_err);
            }
        }

        TF2* true_fit = new TF2("true_fit", "[0]* (1. + [1]* y* y + 2.* [2]* y* sqrt(1. - y* y) *cos(x) + 0.5* [3]* (1. - y* y)* cos(2.* x))");

        //true_fit->SetParLimits(0, 0., 1.);
        true_fit->SetParLimits(1, -1., 1.);
        true_fit->SetParLimits(2, -0.5, 0.5);
        true_fit->SetParLimits(3, -0.5, 0.5);
        true_fit->SetParNames("A", "#lambda", "#mu", "#nu");

        TF2* pred_fit = new TF2("pred_fit", "[0]* (1. + [1]* y* y + 2.* [2]* y* sqrt(1. - y* y) *cos(x) + 0.5* [3]* (1. - y* y)* cos(2.* x))");

        //pred_fit->SetParLimits(0, 0., 1.);
        pred_fit->SetParLimits(1, -1., 1.);
        pred_fit->SetParLimits(2, -0.5, 0.5);
        pred_fit->SetParLimits(3, -0.5, 0.5);
        pred_fit->SetParNames("A", "#lambda", "#mu", "#nu");


        TCanvas* can = new TCanvas();

        TFitResultPtr true_rp = hist_true->Fit(true_fit, "S");

        TString true_name = Form("imgs/true_fit_%d.png", i);
        hist_true->Draw("COLZ");
        can->SaveAs(true_name.Data());

        TString reco_name = Form("imgs/reco_fit_%d.png", i);
        hist_reco->Draw("COLZ");
        can->SaveAs(reco_name.Data());

        // covariace matrix
        // auto true_cov = true_rp->GetCovarianceMatrix();
        //
        // TString true_cov_name = Form("imgs/true_cov_%d.png", i);
        // true_cov.Draw("COLZ");
        // can->SaveAs(true_cov_name.Data());

        // corelation matrix
        // auto true_cor = true_rp->GetCorrelationMatrix();

        // TString true_cor_name = Form("imgs/true_cor_%d.png", i);
        // true_cor.Draw("COLZ");
        // can->SaveAs(true_cor_name.Data());

        TFitResultPtr pred_rp = hist_pred->Fit(pred_fit, "S");

        TString pred_name = Form("imgs/pred_fit_%d.png", i);
        hist_pred->Draw("COLZ");
        can->SaveAs(pred_name.Data());

        // covariace matrix
        // auto pred_cov = pred_rp->GetCovarianceMatrix();

        // TString pred_cov_name = Form("imgs/pred_cov_%d.png", i);
        // pred_cov.Draw("COLZ");
        // can->SaveAs(pred_cov_name.Data());

        // corelation matrix
        // auto pred_cor = pred_rp->GetCorrelationMatrix();
        //
        // TString pred_cor_name = Form("imgs/pred_cor_%d.png", i);
        // pred_cor.Draw("COLZ");
        // can->SaveAs(pred_cor_name.Data());

        delete hist_true;
        delete hist_reco;
        delete hist_pred;
    }
}

void FitHist::DrawResults()
{
    double pi = TMath::Pi();

    TH1D* lambda_true = new TH1D("lambda_true", "; #Delta #lambda [a.u.]; counts", 50, -0.1, 0.15);
    TH1D* mu_true = new TH1D("mu_true", "; #Delta #mu [a.u.]; counts", 50, -0.04, 0.04);
    TH1D* nu_true = new TH1D("nu_true", "; #Delta #nu [a.u.]; counts", 50, -0.06, 0.06);

    TH1D* lambda_pred = new TH1D("lambda_pred", "; #Delta #lambda [a.u.]; counts", 50, -0.15, 0.15);
    TH1D* mu_pred = new TH1D("mu_pred", "; #Delta #mu [a.u.]; counts", 50, -0.1, 0.1);
    TH1D* nu_pred = new TH1D("nu_pred", "; #Delta #nu [a.u.]; counts", 50, -0.1, 0.1);

    for(int i = 0; i < n_events; i++)
    {
        tree->GetEntry(i);

        TH2D* hist_true = new TH2D("hist_true", "hist", 12, -pi, pi, 12, -0.6, 0.6);
        TH2D* hist_pred = new TH2D("hist_pred", "hist", 12, -pi, pi, 12, -0.6, 0.6);

        for(int j = 0; j < 12; j++)
        {
            for(int k = 0; k < 12; k++)
            {
                double true_cnt = (true_hist[0][j][k] + true_hist[1][j][k])/2.;
                double true_err = (true_hist[1][j][k] - true_hist[0][j][k])/2.;

                double pred_cnt = (pred_hist[0][j][k] + pred_hist[1][j][k])/2.;
                double pred_err = (pred_hist[1][j][k] - pred_hist[0][j][k])/2.;

                hist_true->SetBinContent(j+1, k+1, true_cnt);
                hist_true->SetBinError(j+1, k+1, true_err);

                hist_pred->SetBinContent(j+1, k+1, pred_cnt);
                hist_pred->SetBinError(j+1, k+1, pred_err);
            }
        }

        TF2* true_fit = new TF2("true_fit", "[0]* (1. + [1]* y* y + 2.* [2]* y* sqrt(1. - y* y) *cos(x) + 0.5* [3]* (1. - y* y)* cos(2.* x))");

        //true_fit->SetParLimits(0, 0., 0.2);
        true_fit->SetParLimits(1, -1., 1.);
        true_fit->SetParLimits(2, -0.5, 0.5);
        true_fit->SetParLimits(3, -0.5, 0.5);
        true_fit->SetParNames("A", "#lambda", "#mu", "#nu");

        TF2* pred_fit = new TF2("pred_fit", "[0]* (1. + [1]* y* y + 2.* [2]* y* sqrt(1. - y* y) *cos(x) + 0.5* [3]* (1. - y* y)* cos(2.* x))");

        //pred_fit->SetParLimits(0, 0., 0.2);
        pred_fit->SetParLimits(1, -1., 1.);
        pred_fit->SetParLimits(2, -0.5, 0.5);
        pred_fit->SetParLimits(3, -0.5, 0.5);
        pred_fit->SetParNames("A", "#lambda", "#mu", "#nu");

        TFitResultPtr true_rp = hist_true->Fit(true_fit, "S");

        //double true_A = fit_true->GetParameter(0);
        double true_lambda = true_rp->Parameter(1);
        double true_mu = true_rp->Parameter(2);
        double true_nu = true_rp->Parameter(3);

        lambda_true->Fill(lambda-true_lambda);
        mu_true->Fill(mu-true_mu);
        nu_true->Fill(nu-true_nu);


       TFitResultPtr pred_rp = hist_pred->Fit(pred_fit, "S");

        //double true_A = fit_true->GetParameter(0);
        double pred_lambda = pred_rp->Parameter(1);
        double pred_mu = pred_rp->Parameter(2);
        double pred_nu = pred_rp->Parameter(3);

        lambda_pred->Fill(lambda-pred_lambda);
        mu_pred->Fill(mu-pred_mu);
        nu_pred->Fill(nu-pred_nu);

        delete hist_true;
        // delete hist_reco;
        delete hist_pred;
    }

    TCanvas* can = new TCanvas();

    int b_max;
    double x_max, y_max, rms;

    // lambda_true->Fit("gaus");
    // lambda_true->Draw();
    // can->SaveAs("imgs/lambda_true.png");
    //
    // mu_true->Fit("gaus");
    // mu_true->Draw();
    // can->SaveAs("imgs/mu_true.png");
    //
    // nu_true->Fit("gaus");
    // nu_true->Draw();
    // can->SaveAs("imgs/nu_true.png");

    auto gaus_fit = new TF1("gaus_fit", "gaus", -0.04, 0.08);
    b_max = lambda_pred->GetMaximumBin();
    y_max = lambda_pred->GetBinContent(b_max);
    x_max = lambda_pred->GetBinCenter(b_max);
    rms = lambda_pred->GetRMS();
    gaus_fit->SetParameter(0, y_max);
    gaus_fit->SetParLimits(1, x_max-rms, x_max+rms);

    lambda_pred->Fit(gaus_fit, "R");
    lambda_pred->Draw();
    can->SaveAs("imgs/lambda_pred.png");

    auto gaus_fit1 = new TF1("gaus_fit1", "gaus", -0.02, 0.03);
    b_max = mu_pred->GetMaximumBin();
    y_max = mu_pred->GetBinContent(b_max);
    x_max = mu_pred->GetBinCenter(b_max);
    rms = mu_pred->GetRMS();
    gaus_fit1->SetParameter(0, y_max);
    gaus_fit1->SetParLimits(1, x_max-rms, x_max+rms);

    mu_pred->Fit(gaus_fit1, "R");
    mu_pred->Draw();
    can->SaveAs("imgs/mu_pred.png");

    auto gaus_fit2 = new TF1("gaus_fit2", "gaus", -0.03, 0.04);
    b_max = nu_pred->GetMaximumBin();
    y_max = nu_pred->GetBinContent(b_max);
    x_max = nu_pred->GetBinCenter(b_max);
    rms = nu_pred->GetRMS();
    gaus_fit2->SetParameter(0, y_max);
    gaus_fit2->SetParLimits(1, x_max-rms, x_max+rms);

    nu_pred->Fit(gaus_fit2, "R");
    nu_pred->Draw();
    can->SaveAs("imgs/nu_pred.png");
}

void PlotResults()
{
    gStyle->SetPalette(71);
    gStyle->SetOptStat(0);
    gStyle->SetOptFit();
    gStyle->SetTitleSize(.05, "X");

    FitHist* fh = new FitHist();
    fh->Init();
    //fh->DrawFits();
    fh->DrawResults();
}
