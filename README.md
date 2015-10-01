# makeRootTreeAnalyzer
## ROOT Tree Analyzer and Make Script

Requires ROOT  
Requires `cmsenv` to have been run in the `/src` directory of a `CMSSW_X_Y_Z/` release

To use, navigate to the directory you want the package created and do
```
makeRootTreeAnalyzer.sh -f INPUTFILE -t TREENAME
```
You must do exactly one of
  * `-f INPUTFILE` where `INPUTFILE` is a `.root` file
  * `-d DIRECTORY` where `DIRECTORY` is a directory containing several `.root` files
  * `-l LISTFILE` where `LISTFILE` is a text file containing paths to multiple root files

Optionally, you can do
  * `-n CLASSNAME` where `CLASSNAME` is the name of the class; defaults to `RootTreeAnalyzer`
  * `-p PACKAGENAME` where `PACKAGENAME` is the name of the package; defaults to `Common/RootTreeAnalyzer`
