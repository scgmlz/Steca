[main]# Steca started at 2018-04-13 13:10::23.830
[main]## "Trigger about"  emits  changed()
[main]## "Trigger addFiles"  emits  changed()
[main]## "Trigger addPeak"  emits  changed()
[main]## "Trigger checkUpdate"  emits  changed()
[main]## "Trigger clearBackground"  emits  changed()
[main]## "Trigger clearPeaks"  emits  changed()
[main]## "Trigger clearSession"  emits  changed()
[main]## "Trigger corrFile"  emits  changed()
[main]## "Trigger exportDfgram"  emits  changed()
[main]## "Trigger exportPolefig"  emits  changed()
[main]## "Trigger exportBigtable"  emits  changed()
[main]## "Trigger exportDiagram"  emits  changed()
[main]## "Trigger spawnTable"  emits  changed()
[main]## "Trigger spawnDiagram"  emits  changed()
[main]## "Trigger spawnPolefig"  emits  changed()
[main]## "Trigger loadSession"  emits  changed()
[main]## "Trigger online"  emits  changed()
[main]## "Trigger quit"  emits  changed()
[main]## "Trigger removeFile"  emits  changed()
[main]## "Trigger removePeak"  emits  changed()
[main]## "Trigger saveSession"  emits  changed()
[main]## "Trigger viewReset"  emits  changed()
[main]## "Toggle dfg.all"  emits  changed()
[main]## "Toggle crosshair"  emits  changed()
[main]## "Toggle enableCorr"  emits  changed()
[main]## "Toggle dfg.fixInt"  emits  changed()
[main]## "Toggle img.fixInt"  emits  changed()
[main]## "Toggle linkCuts"  emits  changed()
[main]## "Toggle showBackground"  emits  changed()
[main]## "Toggle showBins"  emits  changed()
[main]## "Toggle viewMeasurements"  emits  changed()
[main]## "Toggle viewFiles"  emits  changed()
[main]## "Toggle viewMetadata"  emits  changed()
[main]## "Toggle viewStatusbar"  emits  changed()
[main]## "Toggle toggle_fullScreen"  emits  changed()
[main]#: xAxis choose 0
[main]#: yAxis choose 0
[main]## "Trigger rotateImage"  emits  changed()
[main]## "Toggle mirrorImage"  emits  changed()
[main]#: detDistance set 1035
[main]#: detPixelSize set 1
[main]## "Toggle dropIncomplete"  emits  changed()
[main]#: numSlices set 1
[main]## GammaControls  emits  gSession->sigImage()
[main]#: setup choose 0
[main]#: stepAlpha set 5
[main]#: stepBeta set 6
[main]#: idwRadius set 10
[main]#: avgAlphaMax set 5
[main]#: avgRadius set 5
[main]#: mainframe choose 0
[main]#: rbAll switch on
[main]#: rbAll switch off
[main]#: rbHidden switch on
[main]#: rbHidden switch off
[main]#: rbInten switch on
[main]#: rbInten switch off
[main]#: rbTth switch on
[main]#: rbTth switch off
[main]#: rbFWHM switch on
[main]## ColumnSelector  0  emits  gSession->sigBigtableCols()
[main]#: rbFWHM switch off
[main]#: rbHidden switch on
[main]## ColumnSelector  1  emits  gSession->sigBigtableCols()
[main]## ColumnSelector  2  emits  gSession->sigBigtableCols()
[main]## ColumnSelector  3  emits  gSession->sigBigtableCols()
[main]## ColumnSelector  4  emits  gSession->sigBigtableCols()
[main]## ColumnSelector  5  emits  gSession->sigBigtableCols()
[main]## ColumnSelector  6  emits  gSession->sigBigtableCols()
[main]## ColumnSelector  7  emits  gSession->sigBigtableCols()
[main]## ColumnSelector  8  emits  gSession->sigBigtableCols()
[main]## ColumnSelector  9  emits  gSession->sigBigtableCols()
[main]## ColumnSelector  10  emits  gSession->sigBigtableCols()
[main]## ColumnSelector  11  emits  gSession->sigBigtableCols()
[main]## ColumnSelector  12  emits  gSession->sigBigtableCols()
[main]## ColumnSelector  13  emits  gSession->sigBigtableCols()
[main]## ColumnSelector  14  emits  gSession->sigBigtableCols()
[main]## ColumnSelector  15  emits  gSession->sigBigtableCols()
[main]## ColumnSelector  16  emits  gSession->sigBigtableCols()
[main]## ColumnSelector  17  emits  gSession->sigBigtableCols()
[main]## ColumnSelector  18  emits  gSession->sigBigtableCols()
[main]## ColumnSelector  19  emits  gSession->sigBigtableCols()
[main]## ColumnSelector  20  emits  gSession->sigBigtableCols()
[main]## ColumnSelector  21  emits  gSession->sigBigtableCols()
[main]## ColumnSelector  22  emits  gSession->sigBigtableCols()
[main]## ColumnSelector  23  emits  gSession->sigBigtableCols()
[main]## ColumnSelector  24  emits  gSession->sigBigtableCols()
[main]## ColumnSelector  25  emits  gSession->sigBigtableCols()
[main]## ColumnSelector  26  emits  gSession->sigBigtableCols()
[main]## ColumnSelector  27  emits  gSession->sigBigtableCols()
[main]## ColumnSelector  28  emits  gSession->sigBigtableCols()
[main]## ColumnSelector  29  emits  gSession->sigBigtableCols()
[main]## ColumnSelector  30  emits  gSession->sigBigtableCols()
[main]## ColumnSelector  31  emits  gSession->sigBigtableCols()
[main]#: rbHidden switch off
[main]#: rbAll switch on
[main]## ColumnSelector  32  emits  gSession->sigBigtableCols()
[main]## "Toggle actZoom"  emits  changed()
[main]showBackground switch on
[main]## Toggles::showBackground  emits  gSession->sigDfgram()
[main]## sigDfgram  emits  sigDoFits()
[main]## MainWin::runFits  emits  gSession->sigRawFits()
[main]#: intenAvg switch on
[main]#: intenScale set 0.001
[main]enableCorr switch on
[main]addFiles trigger
[main]#! @push fdia
[fdia]fdia select /home/jwu/AA15/m280578.dat;/home/jwu/AA15/m280579.dat
[fdia]@close
[fdia]#! @pop
[main]## Long time task began:  "addFiles"
[main]## Dataset::onFileChanged  emits  gSession->sigFiles()
[main]## Dataset::onFileChanged  emits  gSession->sigClusters()
[main]## GammaControls  emits  gSession->sigImage()
[main]## Dataset::onFileChanged  emits  gSession->sigActivated()
[main]## sigActivated  emits  sigDfgram()
[main]## sigDfgram  emits  sigDoFits()
[main]## MainWin::runFits  emits  gSession->sigRawFits()
[main]## CheckTableModel::onActivated  emits  dataChanged(createIndex(0,1),createIndex(rowCount()-1,1))
[main]## CheckTableModel::onActivated  emits  dataChanged(createIndex(0,1),createIndex(rowCount()-1,1))
[main]## Dataset::onFileChanged  emits  gSession->sigFiles()
[main]## Dataset::onFileChanged  emits  gSession->sigClusters()
[main]## GammaControls  emits  gSession->sigImage()
[main]## Dataset::onFileChanged  emits  gSession->sigActivated()
[main]## sigActivated  emits  sigDfgram()
[main]## sigDfgram  emits  sigDoFits()
[main]## MainWin::runFits  emits  gSession->sigRawFits()
[main]## CheckTableModel::onActivated  emits  dataChanged(createIndex(0,1),createIndex(rowCount()-1,1))
[main]## CheckTableModel::onActivated  emits  dataChanged(createIndex(0,1),createIndex(rowCount()-1,1))
[main]## HighlightedData::setCluster  emits  gSession->sigDataHighlight()
[main]## sigDataHighlight  emits  sigImage()
[main]## GammaSelection onData, highlighted cluster =  0x1bd43e0
[main]## recompute angle map
[main]## AngleMap: core computation
[main]## AngleMap: core computation done
[main]## AngleMap: compute ranges
[main]## AngleMap: found gamma range  "-10.10 .. 10.17"
[main]## AngleMap: found full range  "-11.45 .. 11.54"
[main]## AngleMap: found theta range  "36.95 .. 51.34"
[main]## AngleMap: compute indices
[main]## AngleMap: compute indices done
[main]## GammaSelection::recomputeCache  emits  gSession->sigGamma()
[main]## sigGamma  emits  sigDfgram()
[main]## sigDfgram  emits  sigDoFits()
[main]## MainWin::runFits  emits  gSession->sigRawFits()
[main]## DataImageTab  emits  gSession->sigImage()
[main]## ThetaSelection onData, highlighted cluster =  0x1bd43e0
[main]## ThetaSelection::recomputeCache  emits  gSession->sigTheta()
[main]## DataImageTab  emits  gSession->sigImage()
[main]## TableModel::refreshModel  emits  dataChanged(createIndex(0,0),createIndex(rowCount(),columnCount()-1))
[main]## TableModel::refreshModel  emits  dataChanged(createIndex(0,0),createIndex(rowCount(),columnCount()-1))
[main]## Long time task ended:  "addFiles"
[main]setup choose 2
[main]#: setup choose 2
[main]addPeak trigger
[main]## Peaks::add  emits  gSession->sigPeaks()
[main]## sigPeaks  emits  sigDfgram()
[main]## sigDfgram  emits  sigDoFits()
[main]## rawFits began
[main]## Task with progress bar [ "peak fitting" ] began
[main]## rawFits ended
[main]## Task with progress bar [ "peak fitting" ] ended
[main]## MainWin::runFits  emits  gSession->sigRawFits()
[main]reflTyp choose 1
[main]## ControlsPeakfits  emits  gSession->sigPeaks()
[main]## sigPeaks  emits  sigDfgram()
[main]## sigDfgram  emits  sigDoFits()
[main]## rawFits began
[main]## Task with progress bar [ "peak fitting" ] began
[main]## rawFits ended
[main]## Task with progress bar [ "peak fitting" ] ended
[main]## MainWin::runFits  emits  gSession->sigRawFits()
[main]## Peak::invalidateGuesses  emits  gSession->sigPeaks()
[main]## sigPeaks  emits  sigDfgram()
[main]## sigDfgram  emits  sigDoFits()
[main]## rawFits began
[main]## Task with progress bar [ "peak fitting" ] began
[main]## rawFits ended
[main]## Task with progress bar [ "peak fitting" ] ended
[main]## MainWin::runFits  emits  gSession->sigRawFits()
[main]#: peakRangeMin set 39.98
[main]#: peakRangeMax set 41.97
[main]peakRangeMin set 39.9832
[main]peakRangeMax set 41.974
[main]mainframe choose 4
[main]#: mainframe choose 4
