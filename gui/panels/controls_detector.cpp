//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/controls_detector.cpp
//! @brief     Implements class ControlsDetector, and local classes
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "gui/panels/controls_detector.h"
#include "core/session.h"
#include "gui/actions/image_trafo_actions.h"
#include "gui/view/toggles.h"
#include "gui/mainwin.h"
//#include "qcr/base/debug.h"
#include <QThread> // for sleep for debugging

//  ***********************************************************************************************
//! @class DetectorControls (local scope)

//! Control widgets that govern the detector detector.

class DetectorControls : public QcrWidget {
public:
    DetectorControls();
};

DetectorControls::DetectorControls()
{
    auto* mmGrid = new QGridLayout;
    mmGrid->addWidget(new QLabel("det. distance"), 0, 0);
    mmGrid->addWidget(new QcrDoubleSpinBox
                      {"detDistance", &gSession->params.detector.detectorDistance, 6, 1, 1e1, 1e5,
                              "Distance from sample to detector, in mm"},
                      0, 1);
    mmGrid->addWidget(new QLabel("mm"), 0, 2);
    mmGrid->addWidget(new QLabel("pixel size"), 1, 0);
    mmGrid->addWidget(new QcrDoubleSpinBox
                      {"detPixelSize", &gSession->params.detector.pixSize, 3, 2, 1e-1, 1e2,
                              "Side length of detector pixel square, in mm"},
                      1, 1);
    mmGrid->addWidget(new QLabel("mm"), 1, 2);

    auto* trafoLayout = new QHBoxLayout;
    trafoLayout->addWidget(new QLabel("image rotate"));
    trafoLayout->addWidget(new QcrIconTriggerButton(&gGui->imageTrafoActions->rotateImage));
    trafoLayout->addWidget(new QLabel("mirror"));
    trafoLayout->addWidget(new QcrIconToggleButton(&gGui->imageTrafoActions->mirrorImage));
    trafoLayout->addStretch(1);

    auto* offsetLayout = new QHBoxLayout;
    offsetLayout->addWidget(new QLabel("offset X"));
    offsetLayout->addWidget(new QcrSpinBox
                            {"beamOffsetI", &gSession->params.detector.pixOffset[0], 3, true,
                                    INT_MIN, INT_MAX,
                                    "Horizontal detector offset, in pixel units"});
    offsetLayout->addWidget(new QLabel(" Y"));
    offsetLayout->addWidget(new QcrSpinBox
                            {"beamOffsetJ", &gSession->params.detector.pixOffset[1], 3, true,
                                    INT_MIN, INT_MAX,
                                    "Vertical detector offset, in pixel units"});

    offsetLayout->addWidget(new QLabel("pix"));
    offsetLayout->addStretch(1);

    auto* vbox = new QVBoxLayout;
    vbox->addLayout(mmGrid);
    vbox->addLayout(trafoLayout);
    vbox->addLayout(offsetLayout);
    setLayout(vbox);
}

//  ***********************************************************************************************
//! @class CutControls (local scope)

//! Control widgets that govern the detector cuts.

class CutControls : public QcrFrame {
public:
    CutControls();
};

CutControls::CutControls()
{
    auto* layout = new QGridLayout;
    layout->addWidget(new QLabel("cut"), 1, 0);
    layout->addWidget(
        new QcrSpinBox{"cutLeft",   &gSession->params.imageCut.left,   3, false, 0, INT_MAX,
                "Number of pixels to be cut at the left"},
        1, 2);
    layout->addWidget(
        new QcrSpinBox{"cutTop",    &gSession->params.imageCut.top,    3, false, 0, INT_MAX,
                "Number of pixels to be cut at the top"},
        0, 3);
    layout->addWidget(
        new QcrSpinBox{"cutBottom", &gSession->params.imageCut.bottom, 3, false, 0, INT_MAX,
                "Number of pixels to be cut at the bottom"},
        2, 3);
    layout->addWidget(
        new QcrSpinBox{"cutRight",  &gSession->params.imageCut.right,  3, false, 0, INT_MAX,
                "Number of pixels to be cut at the right"},
        1, 4);
    layout->addWidget(
        new QcrIconToggleButton{&gGui->toggles->linkCuts},
        1, 3, Qt::AlignHCenter);
    layout->setColumnStretch(5, 1);
    setLayout(layout);
}

//  ***********************************************************************************************
//! @class ActiveClustersControls (local scope)

//! Control widgets that govern the combination of Measurement|s into Cluster|s.

class ActiveClustersControls : public QcrWidget {
public:
    ActiveClustersControls();
};

ActiveClustersControls::ActiveClustersControls()
{
    auto* dropIncompleteAction = new QcrToggle{
        "dropIncomplete", &gSession->dataset.dropIncomplete,
        "Drop measurement groups that do not have the full number of members",
        ":/icon/dropIncomplete"};

    auto* layout = new QHBoxLayout;
    layout->addWidget(new QLabel("combine"));
    layout->addWidget(new QcrSpinBox
                      {"combineMeasurements", &gSession->dataset.binning, 3, false, 1, 999,
                              "Combine this number of measurements into one group"});
    layout->addWidget(new QLabel("measurements"));
    layout->addWidget(new QcrIconToggleButton{dropIncompleteAction});
    layout->addStretch(1);
    setLayout(layout);

    setRemake([=](){dropIncompleteAction->setEnabled(gSession->dataset.hasIncomplete());});
}

//  ***********************************************************************************************
//! @class GammaControls (local scope)

//! Control widgets that govern the gamma slicing.

class GammaControls : public QcrWidget {
public:
    GammaControls();
};

GammaControls::GammaControls()
{
    auto layout = new QHBoxLayout;
    layout->addWidget(new QLabel("number of γ slices"));
    layout->addWidget(new QcrSpinBox
                      {"numSlices", &gSession->gammaSelection.numSlices, 2, false, 1, INT_MAX,
                              "Number of γ slices (0: no slicing, take entire image)" });
    layout->addStretch(1);
    setLayout(layout);
}

//  ***********************************************************************************************
//! @class ControlsDetector

ControlsDetector::ControlsDetector()
{
    auto* vbox = new QVBoxLayout;
    vbox->addWidget(new DetectorControls);
    vbox->addWidget(new CutControls);
    vbox->addWidget(new ActiveClustersControls);
    vbox->addWidget(new GammaControls);
    vbox->addStretch();
    setLayout(vbox);
}
