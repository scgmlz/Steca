//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/view/toggles.h
//! @brief     Defines class Toggles
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef TOGGLES_H
#define TOGGLES_H

#include "qcr/widgets/controls.h"

//! Collection of toggle actions, for use as member of MainWin.

class Toggles {
public:
    Toggles();
    QcrToggle combinedDfgram;
    QcrToggle crosshair {"crosshair", "Show crosshair", false, ":/icon/crop"};
    QcrToggle enableCorr;
    QcrToggle fixedIntenDfgram {"dfg.fixInt", "Fixed intensity scale", false, ":/icon/scale"};
    QcrToggle fixedIntenImage {"img.fixInt", "Global intensity scale", false, ":/icon/scale"};
    QcrToggle linkCuts;
    QcrToggle showBackground {"showBackground", "Show fitted background", true,
            ":/icon/showBackground"};
    QcrToggle showBins {"showBins", "Show bins", false, ":/icon/angle"};
    QcrToggle viewClusters {"viewMeasurements", "Measurements", true, "", Qt::Key_F9};
    QcrToggle viewFiles {"viewFiles", "Files", true, "", Qt::Key_F8};
    QcrToggle viewMetadata {"viewMetadata", "Metadata", true, "", Qt::Key_F10};
    QcrToggle viewStatusbar {"viewStatusbar", "Statusbar", true, "", Qt::Key_F12};
#ifndef Q_OS_OSX
    QcrToggle fullScreen {"toggle_fullScreen", "FullScreen", false, "", Qt::Key_F11};
#endif
};

#endif // TOGGLES_H
