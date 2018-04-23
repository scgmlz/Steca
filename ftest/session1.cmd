[        main]showBackground on
[        main]enableCorr on
[        main]addFiles
[        fdia]fdia select /home/jwu/AA15/m280578.dat;/home/jwu/AA15/m280579.dat
[        fdia]@close
[        main]corrFile
[        fdia]fdia select /home/jwu/AA15/V.dat
[        fdia]@close
[        main]linkCuts on
[        main]cutTop 1
[        main]cutTop 2
[        main]cutTop 3
[        main]cutTop 4
[        main]cutTop 5
[        main]numSlices 2
[        main]numSlices 3
[        main]combineMeasurements 2
[        main]combineMeasurements 3
[        main]setupTab 1
[        main]degree 1
[        main]degree 2
[        main]setupTab 2
[        main]addPeak
[        main]peakRangeMin 40.0446
[        main]peakRangeMax 41.9566
[        main]addPeak
[        main]reflTyp 1
[        main]peakRangeMin 45.8897
[        main]peakRangeMax 47.2736
[        main]setupTab 3
[        main]doInterpol on
[        main]viewsTab 4
[        main]exportPolefig
rbOriginalGrid on
file hhh
save
