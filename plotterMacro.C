#include "Plotter.cc"

void plotterMacro()
{
	// *** USAGE:
	//  1) construct Plot<TYPE>(Object, legName, legType, option)
	//  2) construct Canvas::Canvas(lumi, logy, ratioFactor, extra, cWidth, cHeight)
	//  3) call Canvas::makeLegend(lWidth, lHeight, pos, lOffset)
	//  4) call Canvas::addMainPlot(Plot, isFirst, addToLegend)
	// *5) call Canvas::addLegendEntry(Plot)
	// *6) call Canvas::makeRatioPlot(top, bottom, plusminus, option, ytit, xtit)
	//  7) call Canvas::finishCanvas()
	//
	// * = optional; if addToLegend is always true, and/or if no ratio plot needed (ratioFactor = 0), neither of these steps are required
	//
	TFile *file = TFile::Open("output.root");
	TH1F *hist1 = (TH1F*) file->Get("hist1");
	TH1F *hist2 = (TH1F*) file->Get("hist2");
	hist1->UseCurrentStyle();
	hist2->UseCurrentStyle();
	hist1->SetFillColor(kBlue-9);
	hist2->SetFillColor(kRed-3);

	// Step 1
	Plot<TH1F> hist1plot(hist1, "hist1LegName", "felp");

	// Step 2
	Canvas canvas("41.8 pb^{-1} (13 TeV)", true, 1/3., "Preliminary", 800, 700);

	// Step 3
	canvas.makeLegend();

	// Step 4
	canvas.addMainPlot(hist1plot,true,false);
	hist1->GetYaxis()->SetTitle("Events / 1 GeV");

	// Step 5
	canvas.addLegendEntry(hist1plot);

	// Step 6
	canvas.makeRatioPlot(hist1, hist2);

	// Step 7
	canvas.finishCanvas();
}
