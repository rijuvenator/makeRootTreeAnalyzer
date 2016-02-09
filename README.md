# makeRootTreeAnalyzer
## ROOT Tree Analyzer and Make Script

Requires ROOT  
Requires `cmsenv` to have been run in the `/src` directory of a `CMSSW_X_Y_Z/` release

To use, navigate to the directory you want the package created (probably `/src`) and do
```
makeRootTreeAnalyzer.sh -f INPUTFILE -t TREENAME
```
You must do exactly one of
  * `-f INPUTFILE` where `INPUTFILE` is a `.root` file
  * `-d DIRECTORY` where `DIRECTORY` is a directory containing several `.root` files
  * `-l LISTFILE` where `LISTFILE` is a text file containing paths to multiple root files

Optionally, you can do
  * `-c CLASSNAME` where `CLASSNAME` defaults to `RootTreeAnalyzer`
  * `-p PACKAGENAME` where `PACKAGENAME` defaults to `Common/RootTreeAnalyzer`

This creates a `PACKAGENAME` folder containing
  * `BuildFile.xml`
  * `interface/CLASSNAME.h`
  * `src/CLASSNAME.cc`
  * `src/classes.h`
  * `src/classes_def.xml`

Edit the following functions:
  * `setGraphProperties`: with a naming system, set TGraph properties
  * `setHistProperties`: with a naming system, set TH1F properties
  * `preEventLoop`: runs immediately before tree loop
  * `perEventLoop`: runs once per loop event after filling tree branches
  * `postEventLoop`: runs immediately after tree loop

Compile it with `scram b`.

## ROOT Macros and Plotter

This also creates a `RootMacros` folder containing
  * `AnalyzerMacro.C`, a macro for reading and analyzing the `.root` files
    * run with `root -l -q AnalyzerMacro.C`; creates `output.root`
  * `Plotter.C`, classes for creating styled plots
    * requires `setStyle` from `.rootlogon.C` from [settings](https://github.com/rijuvenator/settings)
  * `plotterMacro.C`, a macro for configuring the abovementioned Plot and Canvas classes
    * configure and run with `root -l plotterMacro.C`; creates a styled canvas
    * suitable for batch and `canvas.c->SaveAs("plot.pdf")` with parameters
