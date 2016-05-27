#include "Plotter.cc"

void plotterMacro()
{
	// *** USAGE:
	//  1) construct Plot<TYPE>(Object, legName, legType="felp", option)
	//  2) construct Canvas::Canvas(lumi, logy, ratioFactor, extra, cWidth=800, cHeight=600)
	//  3) call Canvas::makeLegend(lWidth=0.125, lHeight=0.2, pos="tr", lOffset=0.02, fontsize=0.04)
	//  4) call Canvas::addMainPlot(Plot, isFirst, addToLegend)
	//  5) apply any cosmetic commands here
	// *6) call Canvas::addLegendEntry(Plot)
	// *7) call Canvas::makeRatioPlot(top, bottom, plusminus, option, ytit, xtit)
	//  8) call Canvas::finishCanvas()
	//
	// * = optional; if addToLegend is always true, and/or if no ratio plot needed (ratioFactor = 0), neither of these steps are required
	//
	// Canvas class member pointers to c, mainPad, ratPad, leg, rat, and gr are available
	//
	// Note: If TYPE is a TGraph and option="P", a draw option of "AP" is required for the FIRST plot (first addMainPlot)
	// So change plot.option, either to "P" after (if option="AP"), or change plot.option to "AP" before and "P" after (if option="P")
	//
	
	TH1F *hist1 = new TH1F("h1","Test1;X Axis;Y Axis",100,-10,10);
	hist1->FillRandom("gaus",10000);
	TH1F *hist2 = new TH1F("h2","Test2;X Axis;Y Axis",100,-10,10);
	hist2->FillRandom("gaus",10000);

	// Step 1
	Plot<TH1F> hist1plot(hist1, "hist1LegName", "fl","hist");
	Plot<TH1F> hist2plot(hist2, "hist2LegName", "fl","hist");

	// Step 2
	Canvas canvas("1 fb^{-1} (13 TeV)", false, 1/3., "Preliminary", 800, 700);

	// Step 3
	canvas.makeLegend(.2);

	// Step 4
	canvas.addMainPlot(hist1plot,true,false);
	canvas.addMainPlot(hist2plot,false,false);

	// Step 5
	hist1->GetYaxis()->SetTitle("Events / 1 GeV");
	hist1->SetLineColor(kBlue-9);
	hist2->SetLineColor(kRed-3);

	// Step 6
	canvas.addLegendEntry(hist1plot);
	canvas.addLegendEntry(hist2plot);

	// Step 7
	canvas.makeRatioPlot(hist1, hist2);

	// Step 8
	canvas.finishCanvas();
}
