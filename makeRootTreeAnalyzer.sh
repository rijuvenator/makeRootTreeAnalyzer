#!/bin/bash

	# print help dialog if no arguments, or if first argument is ? or -h, then exit
if [ $# -eq 0 -o "$1" == "?" -o "$1" == "-h" ]; then 
	echo -e "\e[1mmakeRootTreeAnalyzer.sh -f INPUTFILE -t TREENAME\e[m"
	echo -e "Options must include \e[1m-t TREENAME\e[m and exactly one of"
	echo -e "    \e[1m-f INPUTFILE\e[m   (.root file)"
	echo -e "    \e[1m-d DIRECTORY\e[m   (directory containing .root files)"
	echo -e "    \e[1m-l LISTFILE\e[m    (list of .root files)"
	echo -e "and optionally"
	echo -e "    \e[1m-c CLASSNAME\e[m   (defaults to RootTreeAnalyzer)"
	echo -e "    \e[1m-p PACKAGENAME\e[m (defaults to Common/RootTreeAnalyzer)"
	exit
fi

	# get script directory
SCRIPTDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

	# fill argument variables
while [ $# -gt 0 ];
do
	case "$1" in
		-f)
			INFILENAME="$2"; shift ;;
		-d)
			INDIRNAME="$2"; shift ;;
		-l)
			INFILELIST="$2"; shift ;;
		-t)
			TREENAME="$2"; shift ;;
		-c)
			CLASSNAME="$2"; shift ;;
		-p)
			PACKAGENAME="$2"; shift ;;
	esac
	shift
done

INPUTS=0
if [ ! -z "$INFILENAME" ]; then ((INPUTS+=1)); fi
if [ ! -z "$INDIRNAME" ]; then ((INPUTS+=1)); fi
if [ ! -z "$INFILELIST" ]; then ((INPUTS+=1)); fi

	# tree name not filled
if [ -z "$TREENAME" ];		then echo "Specify a tree path name."; exit; fi
	# default class name
if [ -z "$CLASSNAME" ];		then CLASSNAME="RootTreeAnalyzer"; fi
	# default package name
if [ -z "$PACKAGENAME" ];	then PACKAGENAME="Common/RootTreeAnalyzer"; fi
	# more or less than 1 input specified
if [ $INPUTS -ne 1 ];		then echo "Specify exactly one input"; exit; fi

	# upper case version of class name
UCLASSNAME=`echo $CLASSNAME | tr '[:lower:]' '[:upper:]'`

	# make package file structure if it does not exist
mkdir -p $PACKAGENAME/interface $PACKAGENAME/src

	# if classes.h does not exist, create it
if [ ! -f "$PACKAGENAME/src/classes.h" ]; then
	cp $SCRIPTDIR/classes.h $PACKAGENAME/src/
	# if it does, and the current class files do not exist, insert lines
elif [ ! -f "$PACKAGENAME/src/${CLASSNAME}.cc" ]; then
	# find PLACEHOLDER, insert #include statement
	sed -e "/PLACEHOLDER/i #include \"${PACKAGENAME}/interface/${CLASSNAME}.h\"" -i $PACKAGENAME/src/classes.h
	# count the number of dummy declarations, find end-of-struct declaraction, insert <tab><tab> dummy declaration
	COUNT=`grep -c dummy ${PACKAGENAME}/src/classes.h`
	sed -e "/\};/i \\\t\tcommon::${CLASSNAME} dummy$((COUNT+1));" -i $PACKAGENAME/src/classes.h
fi
	# if classes_def.xml does not exist, create it
if [ ! -f "$PACKAGENAME/src/classes_def.xml" ]; then
	cp $SCRIPTDIR/classes_def.xml $PACKAGENAME/src/
	# if it does, and the current class files do not exist, insert lines
elif [ ! -f "$PACKAGENAME/src/${CLASSNAME}.cc" ]; then
	# find /lcgdict, insert <tab> class name declaration
	sed -e "/\/lcgdict/i \\\t<class name=\"common::${CLASSNAME}\"/>" -i $PACKAGENAME/src/classes_def.xml
fi
	# if BuildFile.xml does not exist, create it
if [ ! -f "$PACKAGENAME/BuildFile.xml" ]; then
	cp $SCRIPTDIR/BuildFile.xml $PACKAGENAME/
fi

	# remove existing class files
rm -f $PACKAGENAME/interface/${CLASSNAME}.h 2> /dev/null
rm -f $PACKAGENAME/src/${CLASSNAME}.cc 2> /dev/null
	# create empty class files
touch $PACKAGENAME/interface/${CLASSNAME}.h
touch $PACKAGENAME/src/${CLASSNAME}.cc

	# copy CLASSNAME.h; replace CLASSNAME and UCLASSNAME in the whole file
sed -e "s/UCLASSNAME/${UCLASSNAME}/g" -e "s/CLASSNAME/${CLASSNAME}/g" $SCRIPTDIR/CLASSNAME.h >> $PACKAGENAME/interface/${CLASSNAME}.h
	# copy CLASSNAME.cc; replace PACKAGENAME (note special separator) and CLASSNAME in the whole file
sed -e "s|PACKAGENAME|${PACKAGENAME}|g" -e "s/CLASSNAME/${CLASSNAME}/g" $SCRIPTDIR/CLASSNAME.cc >> $PACKAGENAME/src/${CLASSNAME}.cc
	# in classes.h, replace PACKAGENAME (note special separator) and CLASSNAME in the whole file
sed -e "s|PACKAGENAME|${PACKAGENAME}|g" -e "s/CLASSNAME/${CLASSNAME}/g" -i $PACKAGENAME/src/classes.h 
	# in classes_def.xml, replace CLASSNAME in the whole file
sed -e "s/CLASSNAME/${CLASSNAME}/g" -i $PACKAGENAME/src/classes_def.xml 

	# get filenames
if [ ! -z "$INFILENAME" ]; then
	FILES="$INFILENAME"
elif [ ! -z "$INDIRNAME" ]; then
	FILES=`ls ${INDIRNAME}/*.root`
elif [ ! -z "$INFILELIST" ]; then
	FILES=`cat ${INFILELIST}`
fi

	# make class
cat > MakeClassMacro.C <<EOF
{
  TChain c("$TREENAME");
EOF

for FILE in $FILES
do
  echo "  c.Add(\"${FILE}\"); " >> MakeClassMacro.C
done

cat >> MakeClassMacro.C <<EOF
  c.MakeClass("TempClass");
}
EOF
root -l -q MakeClassMacro.C
rm MakeClassMacro.C

	# get branch variable declarations (between two known lines)
sed -n '/Declaration of leaf types/,/List of branches/p' TempClass.h >> decfile

	# delete blank lines, leading spaces, from semicolon to end of line; delete last line, first line
sed -e '/^$/d' -e "s/^[ \t]*//" -e 's/;.*/;/' -i decfile
head -n -1 decfile > tmp; mv tmp decfile
tail -n +2 decfile > tmp; mv tmp decfile
sed -e 's/vector/std::vector/g' -i decfile

	# copy file; delete semicolons, asterisks; grab variable name (last field)
cp decfile varfile
sed -e 's/;//' -e 's/\*//' -i varfile
awk '{ print $NF }' varfile > tmp; mv tmp varfile
	# copy file; insert null initializations and a tab
cp varfile intfile
sed -e 's/$/ = 0;/' -e "s/^/\t/" -i intfile
	# for each variable in varfile, write to brfile the SetBranchAddress statement
touch brfile
NUM=`wc -l < varfile`
for ((i=1;i<=NUM;i++));
do
	TMP=`sed -n "$((i))p" varfile`
	echo -e "\ttree->SetBranchAddress(\"${TMP}\",&${TMP});" >> brfile
done
sed -e "s/^/\t\t\t/" -i decfile

sed -e "/BRANCH DECLARATIONS HERE/ r decfile" -i $PACKAGENAME/interface/${CLASSNAME}.h
sed -e "/BRANCH INITIALIZATIONS HERE/ r intfile" -i $PACKAGENAME/src/${CLASSNAME}.cc
sed -e "/SET BRANCH ADDRESSES HERE/ r brfile" -i $PACKAGENAME/src/${CLASSNAME}.cc

rm *file
rm TempClass.*

mkdir -p RootMacros
rm -f RootMacros/AnalyzerMacro.C 2> /dev/null

cat > RootMacros/AnalyzerMacro.C <<EOF
#include "$PACKAGENAME/interface/${CLASSNAME}.h"

void AnalyzerMacro()
{
	TChain *c = new TChain("$TREENAME");
EOF

for FILE in $FILES
do
  echo -e "\tc->Add(\"${FILE}\");" >> RootMacros/AnalyzerMacro.C
done

cat >> RootMacros/AnalyzerMacro.C <<EOF
	common::$CLASSNAME analyzer(c,"output.root");
	analyzer.eventLoop();
}
EOF

cp $SCRIPTDIR/Plotter.cc RootMacros/
cp $SCRIPTDIR/plotterMacro.C RootMacros/
