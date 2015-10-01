# ROOT Tree Analyzer Class and Make Script

Requires ROOT  
Requires `cmsenv` to have been run in the `/src` directory of a `CMSSW_X_Y_Z/` release

To use, navigate to the directory you want the package created and do
```
makeRootTreeAnalyzer.sh -f INPUTFILE -t TREENAME
```
Instead of `-f INPUTFILE` you can specify a `-d DIRECTORY` containing several `.root` files or a `-l LISTFILE` of `.root` files

Optionally, you can do `-n CLASSNAME` and `-p PACKAGENAME`; defaults are `RootTreeAnalyzer` and `Common/RootTreeAnalyzer`, respectively
