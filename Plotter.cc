#ifndef PLOTTER_C
#define PLOTTER_C

#include "TFile.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TH1.h"
#include "TLegend.h"
#include "TGraph.h"
#include <string>

// holds a plot object; expected to be a hist, graph, or hstack
// legName is the legend display name, legType is the legend symbol draw option, option is the draw option
template <typename T>
class Plot
{
	public:
		T *plot;
		std::string legName;
		std::string legType;
		std::string option;
	
		Plot(T *plot_, std::string legName_="hist", std::string legType_="felp", std::string option_="") :
			plot(plot_),
			legName(legName_),
			legType(legType_),
			option(option_)
		{
		}
};

// canvas class
// lumi is lumi text (top right), logy is if canvas should be log y scale, ratiofactor is fraction of canvas devoted to ratio plot
// extra is text that follows CMS (top left), cWidth is canvas width, cHeight is canvas height, font is font
class Canvas
{
	public:
		TCanvas *c;
		TPad *mainPad;
		TPad *ratPad;
		int cWidth;
		int cHeight;
		int font;
		float tsize;
		std::string lumi;
		std::string extra;
		bool logy;
		float ratioFactor;
		TLegend *leg;
		TH1F *rat;
		TGraph *gr;

	// the user calls this constructor
	Canvas(std::string lumi_="40 pb^{-1} (13 TeV)", bool logy_=0, float ratioFactor_=0, std::string extra_="Preliminary", int cWidth_=800, int cHeight_=600, int font_=42, float tsize_=0.05) :
		cWidth(cWidth_),
		cHeight(cHeight_),
		font(font_),
		tsize(tsize_),
		lumi(lumi_),
		extra(extra_),
		logy(logy_),
		ratioFactor(ratioFactor_)
	{
		setStyle(cWidth,cHeight,font,tsize);
		c = new TCanvas("c","Canvas",cWidth,cHeight);

		prepareCanvas();	
	}

	// the user doesn't call this. I should really make some of these things private
	void prepareCanvas()
	{
		float tMargin = c->GetTopMargin();
		float lMargin = c->GetLeftMargin();
		float rMargin = c->GetRightMargin();

		mainPad = new TPad("main","Main",0,ratioFactor,1,1);
		
		if (ratioFactor != 0)
		{
			mainPad->SetBottomMargin(0.04);
			mainPad->SetRightMargin(tMargin * (cHeight * (1 - ratioFactor)) / cWidth);
			ratPad = new TPad("ratio","Ratio",0,0,1,ratioFactor);
			ratPad->SetTopMargin(0.04);
			ratPad->SetBottomMargin(lMargin * (1 - ratioFactor)/ratioFactor);
			ratPad->SetRightMargin(tMargin * (cHeight * (1 - ratioFactor)) / cWidth);
			ratPad->Draw();
		}

		mainPad->Draw();
		mainPad->SetLogy(logy);

	}

	// the user calls this; it "initializes" the legend
	// lWidth is width as fraction of pad, lHeight is height as fraction of pad, lOffset is offset from corner as fraction of pad
	// pos can be tr, tl, br, bl for each of the four corners. if it doesn't suit, just grab it and move it
	// fontsize defaults to 0.04
	void makeLegend(float lWidth=0.125, float lHeight=0.2, std::string pos="tr", float lOffset=0.02, float fontsize=0.04)
	{
		float tMargin = mainPad->GetTopMargin();
		float lMargin = mainPad->GetLeftMargin();
		float rMargin = mainPad->GetRightMargin();
		float bMargin = mainPad->GetBottomMargin();

		c->cd();
		mainPad->cd();

			 if (pos == "tr"){ leg = new TLegend(1-rMargin-lOffset-lWidth, 1-tMargin-lOffset-lHeight, 1-rMargin-lOffset,		1-tMargin-lOffset);			}
		else if (pos == "tl"){ leg = new TLegend(  lMargin+lOffset,		   1-tMargin-lOffset-lHeight,   lMargin+lOffset+lWidth, 1-tMargin-lOffset);			}
		else if (pos == "br"){ leg = new TLegend(1-rMargin-lOffset-lWidth,	 bMargin+lOffset,		  1-rMargin-lOffset,		  bMargin+lOffset+lHeight); }
		else if (pos == "bl"){ leg = new TLegend(  lMargin+lOffset,			 bMargin+lOffset,			lMargin+lOffset+lWidth,	  bMargin+lOffset+lHeight); }
		else
		{
			std::cout << "Invalid legend position string; defaulting to top-right" << std::endl;
			leg = new TLegend(1-rMargin-lOffset-lWidth, 1-tMargin-lOffset-lHeight, 1-rMargin-lOffset, 1-tMargin-lOffset);
		}

		leg->SetTextSize(fontsize);
		leg->SetFillStyle(0);
	}

	// the user may call this, but only if addToLegend=0 in addMainPlot (see below)
	template <typename T>
	void addLegendEntry(Plot<T> plot)
	{
		leg->AddEntry(plot.plot, plot.legName.c_str(), plot.legType.c_str());
	}
	
	// the user calls this; adds a plot Plot to the main pad
	// isFirst is if the main pad should get its draw information from this plot (axis titles and ranges)
	// addToLegend is if this plot should be added to the legend right away
	// I added this option because perhaps the "first" plot isn't intended to also be the first legend entry
	template <typename T>
	void addMainPlot(Plot<T> plot, bool isFirst=1, bool addToLegend=1)
	{
		plot.plot->UseCurrentStyle();
		c->cd();
		mainPad->cd();

		if (isFirst)
		{
			plot.plot->Draw(plot.option.c_str());
			plot.plot->GetXaxis()->CenterTitle();
			plot.plot->GetYaxis()->CenterTitle();
		}
		else
		{
			plot.plot->Draw((plot.option+" same").c_str());
		}

		if (ratioFactor != 0)
		{
			plot.plot->GetXaxis()->SetLabelSize(0);
		}

		if (addToLegend) { addLegendEntry(plot); }
	}

	// the user calls this; makes ratio plot given a top hist and a bottom hist
	// plusminus is the window around 1, i.e. 0.5 means plot from 0.5 to 1.5
	// ytit is the y axis title, xtit is the x axis title, option is the draw option
	void makeRatioPlot(TH1F *topHist, TH1F *bottomHist, float plusminus=0.5, std::string option="", std::string ytit="Data/MC", std::string xtit="")
	{
		c->cd();
		ratPad->cd();
		ratPad->SetGridy(true);

		rat = (TH1F*) topHist->Clone();
		TH1F *bot = (TH1F*) bottomHist->Clone();
		rat->Divide(bot);

		float factor = (1 - ratioFactor)/ratioFactor;

		if (xtit != "")
		{
			rat->GetXaxis()->SetTitle(xtit.c_str());
		}
		rat->GetXaxis()->SetTitleSize(rat->GetXaxis()->GetTitleSize() * factor);
		rat->GetXaxis()->SetLabelSize(rat->GetYaxis()->GetLabelSize() * factor);
		rat->GetXaxis()->SetTickLength(rat->GetXaxis()->GetTickLength() * factor);
		rat->GetXaxis()->CenterTitle();

		rat->GetYaxis()->SetTitle(ytit.c_str());
		rat->GetYaxis()->SetTitleOffset(rat->GetYaxis()->GetTitleOffset() / factor);
		rat->GetYaxis()->SetTitleSize(rat->GetYaxis()->GetTitleSize() * factor);
		rat->GetYaxis()->SetLabelSize(rat->GetYaxis()->GetLabelSize() * factor);
		rat->GetYaxis()->SetNdivisions(5);
		rat->GetYaxis()->SetRangeUser(1-plusminus,1+plusminus);
		rat->GetYaxis()->SetTickLength(0.01);
		rat->GetYaxis()->CenterTitle();
		rat->Draw(option.c_str());

		float low = rat->GetXaxis()->GetXmin();
		float up = rat->GetXaxis()->GetXmax();
		float x[2] = { low, up };
		float y[2] = { 1, 1 };
		gr = new TGraph(2,x,y);
		gr->SetLineColor(kRed);
		gr->SetLineStyle(3);
		gr->SetLineWidth(2);
		gr->Draw("C same");

		rat->Draw((option+" same").c_str());
		rat->SetMarkerColor(kBlack);
		rat->SetLineColor(kBlack);

		ratPad->RedrawAxis();

	}

	// the user calls this last; it draws the lumi text, "CMS", extra text, and legend 
	void finishCanvas()
	{
		float tMargin = mainPad->GetTopMargin();
		float lMargin = mainPad->GetLeftMargin();
		float rMargin = mainPad->GetRightMargin();

		float tOffset = 0.02;
		float fontsize = 0.04;
		if (ratioFactor != 0) { fontsize /= (1-ratioFactor); }

		c->cd();
		mainPad->cd();

		TLatex latex;
		latex.SetTextFont(font);
		latex.SetTextSize(fontsize);
		latex.SetTextAlign(31);
		latex.DrawLatexNDC(1-rMargin,1-tMargin+tOffset,lumi.c_str());
		latex.SetTextFont(font+20);
		latex.SetTextSize(fontsize*1.25);
		latex.SetTextAlign(11);
		latex.DrawLatexNDC(lMargin,1-tMargin+tOffset,"CMS");
		latex.SetTextFont(font+10);
		latex.SetTextSize(fontsize);
		latex.DrawLatexNDC(lMargin + fontsize*cHeight*(1-ratioFactor)*2.75/cWidth,1-tMargin+tOffset,extra.c_str());

		leg->Draw();
		mainPad->RedrawAxis();
	}

};

#endif
