#include "PACKAGENAME/interface/CLASSNAME.h"

// constructor: input file, output file, path to tree
common::CLASSNAME::CLASSNAME(TChain* chain, std::string outpath)
{
	fileout = new TFile(outpath.c_str(), "RECREATE");
	tree = chain;
	nEntries = tree->GetEntries();

	// BRANCH INITIALIZATIONS HERE


	// SET BRANCH ADDRESSES HERE


}

// looks for "dirname" in internal list; if not found, make it; then cd
void common::CLASSNAME::setCurrDir(const std::string &dirname)
{
	size_t npos = -1;
	bool matchedDirName = false;
	for (const auto &currDirName : dirnames)
	{
		if (currDirName.find(dirname) != npos) { matchedDirName = true; break; }
	}
	if (!matchedDirName) { dirnames.push_back(dirname); fileout->mkdir(dirname.c_str()); }
	fileout->cd(dirname.c_str());
}

// makes histogram "name" in directory "dirname" with temporary parameters, then sets user-defined properties
void common::CLASSNAME::makeHist(std::string name, std::string dirname)
{
	setCurrDir(dirname);
	std::string tmpHistName = "h" + std::to_string(hists.size() + 1);
	hists[name] = new TH1F(tmpHistName.c_str(),"",1,0,1);
	setHistProperties(name);
}

// makes several histograms in a single directory
void common::CLASSNAME::makeHist(std::vector<std::string> names, std::string dirname)
{
	for (std::string &name : names) { makeHist(name, dirname); }
}

// makes several histograms in several directories
void common::CLASSNAME::makeHist(std::map<std::string,std::string> namesdirs)
{
	for (auto &val : namesdirs) { makeHist(val.first, val.second); }
}

// makes graph "name" in directory "dirname", then sets user-defined properties
void common::CLASSNAME::makeGraph(std::string name, std::string dirname)
{
	setCurrDir(dirname);
	graphs[name] = new TGraphAsymmErrors;
	setGraphProperties(name);
}

// makes several graphs in a single directory
void common::CLASSNAME::makeGraph(std::vector<std::string> names, std::string dirname)
{
	for (std::string &name : names) { makeGraph(name, dirname); }
}

// makes several graphs in several directories
void common::CLASSNAME::makeGraph(std::map<std::string,std::string> namesdirs)
{
	for (auto &val : namesdirs) { makeGraph(val.first, val.second); }
}


// wrapper for filling histogram "name" with "expr" and optional "weight"
// "weight" defaults to 1
void common::CLASSNAME::histFill(std::string name, float expr, float weight)
{
	hists[name]->Fill(expr,weight);
}

// fills a histogram "name" and its cut version "name+cutName" if "cutCondition" is true
// "cutName" defaults to "Cut"
void common::CLASSNAME::histFillAndCut(std::string name, float expr, bool cutCondition, float weight, std::string cutName)
{
	hists[name]->Fill(expr, weight);
	if (cutCondition) { hists[name+cutName]->Fill(expr, weight); }
}

// fills a histogram "name" and several cut versions given several conditions
void common::CLASSNAME::histFillAndCut(std::string name, float expr, std::map<std::string,bool> cutNamesConditions, float weight)
{
	hists[name]->Fill(expr, weight);
	for (auto &val : cutNamesConditions)
	{
		if (val.second) { hists[name+val.first]->Fill(expr, weight); }
	}
}

// wrapper for looping through tree entries and doing something user-defined
// a progress bar is printed every 100 entries (it seems to help with the cursor jumping around); after the loop, a newline
void common::CLASSNAME::eventLoop()
{
	preEventLoop();
	printf("\e[1;31mStarting tree loop: \e[0;34m%lu\e[1;31m events to be processed.\n",nEntries);
	for (unsigned i=0;i<nEntries;i++)
	{
		if (i%100 == 0 || i == nEntries-1) { printProgress(0,i,nEntries-1); }
		tree->GetEntry(i);
		perEventLoop();
	}
	printf("\n");
	postEventLoop();
}

// prints a progress bar given a first, current, and last value
// This function carriage returns and overwrites its own output; preferably use it only for loops that don't print out information
// In any case, a newline MUST be printed after the function is called, otherwise the output may be unexpected
void common::CLASSNAME::printProgress(unsigned first, unsigned current, unsigned last)
{
	// fraction completed
	float frac = (current - first) / float(last);
	// "arrow" is the progress bar, printed in bold red
	// the full progress bar is 25 characters wide, not that it really matters
	std::string arrow = "\e[0;0m|\e[1;31m";
	for (int j=0;j<=int(frac*25);j++) { arrow += "="; }
	arrow += ">\e[0;0m";
	for (int j=25;j>int(frac*25);j--) { arrow += "."; }
	arrow += "|";
	// print percent with 2 decimals (in blue) and the progress bar, carriage returns, and flushes the buffer
	std::cout << "\e[1;31mCompleted: \e[0;34m" << std::fixed << std::setprecision(2) << std::setw(6) << frac*100 << " %  " << arrow << "\r" << std::flush;
}

// computes efficiency given two histograms
float common::CLASSNAME::intEfficiency(std::string pass, std::string total)
{
    TH1F *passHist = getHist(pass);
    TH1F *totalHist = getHist(total);
    int nPassBins = passHist->GetSize() - 2;
    int nTotalBins = totalHist->GetSize() - 2;
    return float(passHist->Integral(0,nPassBins+1))/(totalHist->Integral(0,nTotalBins+1));
}

// user-defined virtual functions
void common::CLASSNAME::preEventLoop() {};
void common::CLASSNAME::perEventLoop() {};
void common::CLASSNAME::postEventLoop() {};
void common::CLASSNAME::setHistProperties(const std::string &name) {};
void common::CLASSNAME::setGraphProperties(const std::string &name) {};
