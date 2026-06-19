LIBRARIES:

AndCommon            : FP function library (plots,, TGraph, string, some statistical analysis)
lecroyparser         : library to read trc files from waverunner
pshape_functions     : functions acting on the pshapes saved point by point


PROGRAMS C++:
makeAllrawTree                  : loop over files in folder that compiles makeRawTree
makeRawTree                     : create one rawtree for each file.trc read 
makeTree                        : unifies rawtrees in one single rawtree and make a high level quantities tree 
