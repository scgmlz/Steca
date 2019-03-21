[    0ms main log] # Steca 2.1.2 started at 2019-03-20 17:56::07.163
[   14ms main ini] dfg.all off
[   16ms main ini] enableCorr on
[   11ms main ini] linkCuts on
[   74ms main ini] detDistance 1035
[    8ms main ini] detPixelSize 1
[    7ms main ini] beamOffsetI 0
[    8ms main ini] beamOffsetJ 0
[    8ms main ini] cutLeft 5
[    8ms main ini] cutTop 5
[    8ms main ini] cutBottom 5
[    8ms main ini] cutRight 5
[    8ms main ini] dropIncomplete off
[   10ms main ini] combineMeasurements 2
[    8ms main ini] numSlices 4
[   11ms main ini] degree 2
[   10ms main ini] reflTyp 1
[   11ms main ini] doInterpol off
[    8ms main ini] stepAlpha 5
[    9ms main ini] stepBeta 5
[    8ms main ini] idwRadius 10
[    9ms main ini] avgAlphaMax 5
[    8ms main ini] avgRadius 5
[    9ms main ini] threshold 100
[   12ms main ini] idxMeas 1
[    8ms main ini] idxTheta 1
[    9ms main ini] idxSlice 1
[   21ms main ini] diagramCoordX 17
[    7ms main ini] diagramCoordY 6
[    8ms main ini] gridPts off
[    8ms main ini] normTyp 4
[    7ms main ini] intenAvg on
[    8ms main ini] intenScale 1
[   88ms main gui] # running script '/G/Steca/build/ftest/1.cmd'
[   41ms main scr] addFiles
[        fdia gui] fdia select @CMAKE_CURRENT_SOURCE_DIR@/in/m280580.dat
[        fdia gui] @accept fdia
[   17ms main gui] ## Long time task began:  "addFiles"
[  615ms main gui] ## Long time task ended:  "addFiles"
[        main gui] setupTabs 2 # Peakfits
[        main gui] dfgram add 40.2 41.45
[        main gui] mainTabs 4 # Polefig
[   25ms main gui] ## Long time task began:  "peak fitting"
[  645ms main gui] ## Long time task ended:  "peak fitting"
[        main gui] exportPolefig
[        ePol gui] saveMode 0
[   19ms ePol scr] selectDir
[        fdia gui] fdia select @CMAKE_CURRENT_BINARY_DIR@/out/1
[        fdia gui] @accept fdia
[        ePol gui] file polefig
[        ePol gui] save
[        ePol gui] @reject ePol
[        main gui] quit
[   15ms main gui] # done with script '/G/Steca/build/ftest/1.cmd'
[   21ms main log] # Steca session ended
[    0ms main log] # duration: 2334ms
[    0ms main log] # computing time: 1853ms
