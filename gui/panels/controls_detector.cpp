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

#include "controls_detector.h"
#include "core/session.h"
#include "gui/actions/image_trafo_actions.h"
#include "gui/actions/toggles.h"
#include "gui/mainwin.h"
#include "qcr/base/debug.h"
#include <QThread> // for sleep for debugging

//  ***********************************************************************************************
//! @class DetectorControls (local scope)

//! Control widgets that govern the detector detector.

class DetectorControls : public QcrWidget {
public:
    DetectorControls();
private:
    QVBoxLayout vbox_;
    QGridLayout mmGrid_;
    QHBoxLayout trafoLayout_;
    QHBoxLayout offsetLayout_;

    QcrDoubleSpinBox detDistance_;
    QcrDoubleSpinBox detPixelSize_;
    QcrSpinBox beamOffsetI_;
    QcrSpinBox beamOffsetJ_;
};

DetectorControls::DetectorControls()
    : detDistance_  {"detDistance", &gSession->params.detector.detectorDistance, 6, 1, 1e1, 1e5,
        "Distance from sample to detector, in mm"}
    , detPixelSize_ {"detPixelSize", &gSession->params.detector.pixSize, 3, 2, 1e-1, 1e2,
              "Side length of detector pixel square, in mm"}
    , beamOffsetI_  {"beamOffsetI", &gSession->params.detector.pixOffset[0], 3, true,
              INT_MIN, INT_MAX, "Horizontal detector offset, in pixel units"}
    , beamOffsetJ_  {"beamOffsetJ", &gSession->params.detector.pixOffset[1], 3, true,
              INT_MIN, INT_MAX, "Vertical detector offset, in pixel units"}
{
    // layout
    mmGrid_.addWidget(new QLabel("det. distance"), 0, 0);
    mmGrid_.addWidget(&detDistance_, 0, 1);
    mmGrid_.addWidget(new QLabel("mm"), 0, 2);
    mmGrid_.addWidget(new QLabel("pixel size"), 1, 0);
    mmGrid_.addWidget(&detPixelSize_, 1, 1);
    mmGrid_.addWidget(new QLabel("mm"), 1, 2);

    trafoLayout_.addWidget(new QLabel("image rotate"));
    trafoLayout_.addWidget(new QcrIconTriggerButton(&gGui->imageTrafoActions->rotateImage));
    trafoLayout_.addWidget(new QLabel("mirror"));
    trafoLayout_.addWidget(new QcrIconToggleButton(&gGui->imageTrafoActions->mirrorImage));
    trafoLayout_.addStretch(1);

    offsetLayout_.addWidget(new QLabel("offset X"));
    offsetLayout_.addWidget(&beamOffsetI_);
    offsetLayout_.addWidget(new QLabel(" Y"));
    offsetLayout_.addWidget(&beamOffsetJ_);
    offsetLayout_.addWidget(new QLabel("pix"));
    offsetLayout_.addStretch(1);

    vbox_.addLayout(&mmGrid_);
    vbox_.addLayout(&trafoLayout_);
    vbox_.addLayout(&offsetLayout_);
    setLayout(&vbox_);
}

//  ***********************************************************************************************
//! @class CutControls (local scope)

//! Control widgets that govern the detector cuts.

class CutControls : public QcrFrame {
public:
    CutControls();
private:
    QGridLayout layout_;
    QcrIconToggleButton link_;
    QcrSpinBox cutLeft_;
    QcrSpinBox cutTop_;
    QcrSpinBox cutRight_;
    QcrSpinBox cutBottom_;
};

CutControls::CutControls()
    : link_      {&gGui->toggles->linkCuts}
    , cutLeft_   {"cutLeft",   &gSession->params.imageCut.left,   3, false, 0, INT_MAX,
              "Number of pixels to be cut at the left"}
    , cutTop_    {"cutTop",    &gSession->params.imageCut.top,    3, false, 0, INT_MAX,
              "Number of pixels to be cut at the top"}
    , cutRight_  {"cutRight",  &gSession->params.imageCut.right,  3, false, 0, INT_MAX,
              "Number of pixels to be cut at the right"}
    , cutBottom_ {"cutBottom", &gSession->params.imageCut.bottom, 3, false, 0, INT_MAX,
              "Number of pixels to be cut at the bottom"}
{
    layout_.addWidget(new QLabel("cut"), 1, 0);
    layout_.addWidget(&cutLeft_, 1, 2);
    layout_.addWidget(&link_, 1, 3, Qt::AlignHCenter);
    layout_.addWidget(&cutTop_, 0, 3);
    layout_.addWidget(&cutBottom_, 2, 3);
    layout_.addWidget(&cutRight_, 1, 4);
    layout_.setColumnStretch(5, 1);
    setLayout(&layout_);
}

//  ***********************************************************************************************
//! @class ActiveClustersControls (local scope)

//! Control widgets that govern the combination of Measurement|s into Cluster|s.

class ActiveClustersControls : public QcrWidget {
public:
    ActiveClustersControls();
private:
    QHBoxLayout layout_;
    QcrSpinBox    combineMeasurements_;
    QcrToggle     dropIncompleteAction_;
    QcrIconToggleButton dropIncompleteButton_;
};

ActiveClustersControls::ActiveClustersControls()
    : combineMeasurements_ {
        "combineMeasurements", &gSession->dataset.binning, 3, false, 1, 999,
        "Combine this number of measurements into one group"}
    , dropIncompleteAction_ {
        "dropIncomplete", &gSession->dataset.dropIncomplete,
        "Drop measurement groups that do not have the full number of members",
        ":/icon/dropIncomplete" }
    , dropIncompleteButton_ { &dropIncompleteAction_ }
{
    // layout
    layout_.addWidget(new QLabel("combine"));
    layout_.addWidget(&combineMeasurements_);
    layout_.addWidget(new QLabel("measurements"));
    layout_.addWidget(&dropIncompleteButton_);
    layout_.addStretch(1);
    setLayout(&layout_);

    setRemake([this](){dropIncompleteAction_.setEnabled(gSession->dataset.hasIncomplete());});
}

//  ***********************************************************************************************
//! @class GammaControls (local scope)

//! Control widgets that govern the gamma slicing.

class GammaControls : public QcrWidget {
public:
    GammaControls();
private:
    QHBoxLayout layout_;
    QcrSpinBox* numSlices_;
};

GammaControls::GammaControls()
{
    numSlices_ = new QcrSpinBox {
        "numSlices", &gSession->gammaSelection.numSlices, 2, false, 0, INT_MAX,
        "Number of γ slices (0: no slicing, take entire image)" };

    // layout
    layout_.addWidget(new QLabel("number of γ slices"));
    layout_.addWidget(numSlices_);
    layout_.addStretch(1);
    setLayout(&layout_);
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
