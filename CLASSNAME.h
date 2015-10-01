#ifndef UCLASSNAME_H
#define UCLASSNAME_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <string>
#include "TFile.h"
#include "TChain.h"
#include "TH1.h"
#include "TGraphAsymmErrors.h"

namespace common
{
	class CLASSNAME
	{
		private:
			// external members
			TFile *fileout;
			TChain *tree;

			// internal data
			unsigned long nEntries;
			std::vector<std::string> dirnames;

			// BRANCH DECLARATIONS HERE
			
			// containers for plots
			std::map<std::string,TH1F*> hists;
			std::map<std::string,TGraphAsymmErrors*> graphs;
	
			// internal functions
			void setCurrDir(const std::string &dirname);

			// user-defined virtual functions
			virtual void preEventLoop();
			virtual void perEventLoop();
			virtual void postEventLoop();
			virtual void setHistProperties(const std::string &name);
			virtual void setGraphProperties(const std::string &name);
		
		public:
			// default constructor and virtual destructor
			CLASSNAME() {};
			virtual ~CLASSNAME() {};

			// constructor
			CLASSNAME(TChain* chain, std::string outpath);

			// get functions
			TH1F* getHist(std::string name) { return hists[name]; }
			TGraphAsymmErrors* getGraph(std::string name) { return graphs[name]; }
			TChain* getTree() { return tree; }
			TFile* getOutputFile() { return fileout; }

			// loop through tree entries and do something user-defined
			void eventLoop();
			void printProgress(unsigned first, unsigned current, unsigned last);

			// public make and fill functions
			void makeHist(std::string name, std::string dirname);
			void makeHist(std::vector<std::string> names, std::string dirname);
			void makeHist(std::map<std::string,std::string> namesdirs);
			void makeGraph(std::string name, std::string dirname);
			void makeGraph(std::vector<std::string> names, std::string dirname);
			void makeGraph(std::map<std::string,std::string> namesdirs);
			void histFill(std::string name, float expr, float weight=1);
			void histFillAndCut(std::string name, float expr, bool cutCondition, float weight=1, std::string cutName="Cut");
			void histFillAndCut(std::string name, float expr, std::map<std::string,bool> cutNamesConditions, float weight=1);

			float intEfficiency(std::string pass, std::string total);
	
	};
}

#endif
