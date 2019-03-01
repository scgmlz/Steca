[    0ms main log] # Steca 2.1.2 started at 2019-02-27 13:36::03.556
[   12ms main ini] dfg.all off
[   30ms main ini] enableCorr on
[   36ms main ini] linkCuts on
[   76ms main ini] detDistance 1035
[   27ms main ini] detPixelSize 1
[   33ms main ini] beamOffsetI 0
[   38ms main ini] beamOffsetJ 0
[   32ms main ini] cutLeft 0
[   28ms main ini] cutTop 0
[   36ms main ini] cutBottom 0
[   28ms main ini] cutRight 0
[   37ms main ini] dropIncomplete off
[   34ms main ini] combineMeasurements 1
[   32ms main ini] numSlices 5
[   36ms main ini] degree 1
[   47ms main ini] reflTyp 1
[   39ms main ini] doInterpol off
[   42ms main ini] stepAlpha 5
[   40ms main ini] stepBeta 5
[   39ms main ini] idwRadius 10
[   35ms main ini] avgAlphaMax 5
[   49ms main ini] avgRadius 5
[   27ms main ini] threshold 100
[   38ms main ini] idxMeas 1
[   26ms main ini] idxTheta 1
[   33ms main ini] idxSlice 1
[   39ms main ini] diagramCoordX 17
[   33ms main ini] diagramCoordY 4
[   33ms main ini] gridPts off
[   39ms main ini] normTyp 0
[   35ms main ini] intenAvg on
[   35ms main ini] intenScale 1
[        main gui] addFiles
[        fdia gui] fdia select /home/jwu/AA15/m280578.dat;/home/jwu/AA15/m280579.dat;/home/jwu/AA15/m280580.dat;/home/jwu/AA15/m280581.dat;/home/jwu/AA15/m280582.dat;/home/jwu/AA15/m280583.dat
[        fdia gui] @close
[   65ms main gui] ## Long time task began:  "addFiles"
[ 2414ms main gui] ## Long time task ended:  "addFiles"
[        main gui] loadCorr
[        fdia gui] fdia select /home/jwu/AA15/V.dat
[        fdia gui] @close
[        main gui] cutTop 1
[        main gui] cutTop 4
[        main gui] cutTop 5
[        main gui] combineMeasurements 2
[        main gui] combineMeasurements 3
[        main gui] combineMeasurements 4
[        main gui] setupTabs 1 # Baseline
[        main gui] dfgram add 37.4 39.6
[        main gui] dfgram add 42.1 45.5
[        main gui] dfgram add 47.75 51
[        main gui] degree 2
[        main gui] setupTabs 2 # Peakfits
[        main gui] dfgram add 40.15 41.45
[        main gui] dfgram add 46 47.1
[        main gui] reflTyp 2 # Lorentzian
[        main gui] mainTabs 1 # Corr image
[        main gui] mainTabs 4 # Polefig
[   12ms main gui] ## Long time task began:  "peak fitting"
[ 4514ms main gui] ## Long time task ended:  "peak fitting"
[        main gui] setupTabs 3 # Interpol
[        main gui] doInterpol on
[    9ms main gui] ## Long time task began:  "interpolation"
[  193ms main gui] ## Long time task ended:  "interpolation"
[        main gui] gridPts on
[        main gui] gridPts off
[        main gui] mainTabs 3 # Diagram
[        main gui] diagramCoordX 2 # γ1
[        main gui] diagramCoordX 1 # β
[        main gui] mainTabs 2 # Table
[        main gui] colSel highlight 0
[        main gui] colSel activate 0
[        main gui] colSel highlight 1
[        main gui] colSel highlight 1
[        main gui] colSel activate 1
[        main gui] colSel highlight 4
[        main gui] colSel highlight 4
[        main gui] colSel activate 4
[        main gui] colSel highlight 5
[        main gui] colSel highlight 5
[        main gui] colSel activate 5
[        main gui] spawnTable
[        main gui] exportBigtable
[        dlog gui] file h
[        dlog gui] save
[        dlog gui] mainTabs 3 # Diagram
[        dlog gui] spawnDiagram
[        dlog gui] exportDiagram
[        dlog gui] file h
[        dlog gui] file hh
[        dlog gui] save
[        dlog gui] mainTabs 4 # Polefig
[        dlog gui] spawnPolefig
[        dlog gui] dfgram sel 41.1
[   17ms dlog gui] ## Long time task began:  "peak fitting"
[  273ms dlog gui] ## Long time task ended:  "peak fitting"
[    1ms dlog gui] ## Long time task began:  "interpolation"
[  177ms dlog gui] ## Long time task ended:  "interpolation"
[        dlog gui] exportPolefig
[        dlog gui] saveMode 2
[        dlog gui] file h
[        dlog gui] file hh
[        dlog gui] file hhh
[        dlog gui] save
[    8ms dlog gui] ## Long time task began:  "save diffractograms"
[    7ms dlog gui] ## Long time task ended:  "save diffractograms"
[        dlog gui] exportPolefig
[        dlog gui] saveMode 2
[        dlog gui] fmt.dat off
[        dlog gui] fmt.pol on
[        dlog gui] file h
[        dlog gui] file hh
[        dlog gui] file hhh
[        dlog gui] file hhhh
[        dlog gui] saveMode 1
[        dlog gui] save
[    8ms dlog gui] ## Long time task began:  "save pole figures"
[    3ms dlog gui] ## Long time task ended:  "save pole figures"
[        dlog gui] normTyp 3 # time
[   21ms dlog gui] ## Long time task began:  "peak fitting"
[ 4654ms dlog gui] ## Long time task ended:  "peak fitting"
[    0ms dlog gui] ## Long time task began:  "interpolation"
[  176ms dlog gui] ## Long time task ended:  "interpolation"
[        dlog gui] normTyp 4 # Δ time
[   25ms dlog gui] ## Long time task began:  "peak fitting"
[ 4669ms dlog gui] ## Long time task ended:  "peak fitting"
[    1ms dlog gui] ## Long time task began:  "interpolation"
[  180ms dlog gui] ## Long time task ended:  "interpolation"
[        dlog gui] normTyp 2 # Δ monitor
[   26ms dlog gui] ## Long time task began:  "peak fitting"
[ 4716ms dlog gui] ## Long time task ended:  "peak fitting"
[    0ms dlog gui] ## Long time task began:  "interpolation"
[  180ms dlog gui] ## Long time task ended:  "interpolation"
[        dlog gui] dfg.all on
[    3ms dlog gui] ## Long time task began:  "computeAvgCurve"
[ 3870ms dlog gui] ## Long time task ended:  "computeAvgCurve"
[        dlog gui] actZoom on
[        dlog gui] dfg.fixInt on
[        dlog gui] dfg.fixInt off
[        dlog gui] showBackground off
[        dlog gui] showBackground on
[        dlog gui] exportDfgram
[        dlog gui] saveMode 1
[        dlog gui] file h
[        dlog gui] file hd
[        dlog gui] fmt.dat off
[        dlog gui] fmt.csv on
[        dlog gui] save
[    7ms dlog gui] ## Long time task began:  "save diffractograms"
[  245ms dlog gui] ## Long time task ended:  "save diffractograms"
[        dlog gui] quit
[41039ms dlog gui] ##FATAL: Cannot deregister, there is no entry 'mirrorImage' in the widget registry 'dlog'
