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
#include "qcr/widgets/controls.h"
#include "core/session.h"
#include "gui/actions/image_trafo_actions.h"
#include "gui/view/toggles.h"
#include "gui/mainwin.h"
//#include "qcr/base/debug.h"
#include <QThread> // for sleep for debugging
#include <qmath.h> //round

//  ***********************************************************************************************
//! @class DetectorControls (local scope)

//! Control widgets that govern the detector detector.

class DetectorControls : public QcrWidget {
public:
    DetectorControls();
};

DetectorControls::DetectorControls()
    : QcrWidget{"DetectorControls"}
{
    auto* mmGrid = new QGridLayout;
    mmGrid->addWidget(new QLabel{"det. distance"}, 0, 0);
    mmGrid->addWidget(new QcrDoubleSpinBox
                      {"detDistance", &gSession->params.detector.detectorDistance, 6, 1, 1e1, 1e5,
                              "Distance from sample to detector, in mm"},
                      0, 1);
    mmGrid->addWidget(new QLabel{"mm"}, 0, 2);
    mmGrid->addWidget(new QLabel{"pixel size"}, 1, 0);
    mmGrid->addWidget(new QcrDoubleSpinBox
                      {"detPixelSize", &gSession->params.detector.pixSize, 3, 2, 1e-1, 1e2,
                              "Side length of detector pixel square, in mm"},
                      1, 1);
    mmGrid->addWidget(new QLabel{"mm"}, 1, 2);

    auto* trafoLayout = new QHBoxLayout;
    trafoLayout->addWidget(new QLabel{"image rotate"});
    trafoLayout->addWidget(new QcrIconTriggerButton{&gGui->imageTrafoActions->rotateImage});
    trafoLayout->addWidget(new QLabel{"mirror"});
    trafoLayout->addWidget(new QcrIconToggleButton{&gGui->imageTrafoActions->mirrorImage});
    trafoLayout->addStretch(1);

    auto* offsetLayout = new QHBoxLayout;
    offsetLayout->addWidget(new QLabel{"offset X"});
    offsetLayout->addWidget(new QcrSpinBox
                            {"beamOffsetI", &gSession->params.detector.pixOffset[0], 3, true,
                                    INT_MIN, INT_MAX,
                                    "Horizontal detector offset, in pixel units"});
    offsetLayout->addWidget(new QLabel{" Y"});
    offsetLayout->addWidget(new QcrSpinBox
                            {"beamOffsetJ", &gSession->params.detector.pixOffset[1], 3, true,
                                    INT_MIN, INT_MAX,
                                    "Vertical detector offset, in pixel units"});

    offsetLayout->addWidget(new QLabel{"pix"});
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
    : QcrFrame{"CutControls"}
{
    auto* layout = new QGridLayout;
    layout->addWidget(new QLabel{"cut"}, 1, 0);
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

//! Control widgets that govern the combination of `Measurement`s into `Cluster`s.

class ActiveClustersControls : public QcrWidget {
public:
    ActiveClustersControls();
};

ActiveClustersControls::ActiveClustersControls()
    : QcrWidget{"ActiveClustersControls"}
{
    auto* dropIncompleteBtn = new QcrIconToggleButton{
        "dropIncomplete", &gSession->dataset.dropIncomplete,
        "Drop measurement groups that do not have the full number of members",
        ":/icon/dropIncomplete"};

    auto* layout = new QHBoxLayout;
    layout->addWidget(new QLabel{"combine"});
    layout->addWidget(new QcrSpinBox
                      {"combineMeasurements", &gSession->dataset.binning, 3, false, 1, 999,
                              "Combine this number of measurements into one group"});
    layout->addWidget(new QLabel{"measurements"});
    layout->addWidget(dropIncompleteBtn);
    layout->addStretch(1);
    setLayout(layout);

    setRemake([=](){dropIncompleteBtn->setEnabled(gSession->dataset.hasIncomplete());});
}

//  ***********************************************************************************************
//! @class GammaControls (local scope)

//! Control widgets that govern the gamma slicing.

class GammaControls : public QcrWidget {
public:
    GammaControls();
};

GammaControls::GammaControls()
    : QcrWidget{"GammaControls"}
{
    auto layout = new QHBoxLayout;
    layout->addWidget(new QLabel{"number of γ slices"});
    layout->addWidget(new QcrSpinBox
                      {"numSlices", &gSession->gammaSelection.numSlices, 2, false, 1, INT_MAX,
                              "Number of γ slices (0: no slicing, take entire image)" });
    layout->addStretch(1);

    auto* cellmin = new QcrCell<double>{0.};
    auto* cellmax = new QcrCell<double>{0.};
    auto* limitRange = new QcrCell<bool>{false};
    cellmin->setCoerce([](const double val){return 0.01*std::round(val/0.01);});
    cellmax->setCoerce([](const double val){return 0.01*std::round(val/0.01);});
    cellmin->setHook([](const double val){
        gSession->gammaSelection.limitedGammaRange.setMin(val);
        gSession->onDetector();
    });
    cellmax->setHook([](const double val){
        gSession->gammaSelection.limitedGammaRange.setMax(val);
        gSession->onDetector();
    });
    auto* spinmin = new QcrDoubleSpinBox{"adhoc_min", cellmin, 4, 2, -180., 180.};
    auto* spinmax = new QcrDoubleSpinBox{"adhoc_max", cellmax, 4, 2, -180., 180.};
    auto* limitCheck = new QcrCheckBox{"adhoc_limit", "restrict γ range to", limitRange};
    spinmin->setSingleStep(0.01);
    spinmax->setSingleStep(0.01);
    limitRange->setHook([spinmax, spinmin](const bool val){
        if (val) {
            gSession->gammaSelection.limitedGammaRange = gSession->currentCluster()->rangeGma();
            spinmax->setMaximum(gSession->currentCluster()->rangeGma().max);
            spinmin->setMinimum(gSession->currentCluster()->rangeGma().min);
        }
        gSession->gammaSelection.limit = val;
        gSession->onDetector();
        spinmax->setEnabled(val);
        spinmin->setEnabled(val);
    });
    auto layer = new QHBoxLayout;
    layer->addWidget(limitCheck);
    layer->addWidget(spinmin);
    layer->addWidget(new QLabel{".."});
    layer->addWidget(spinmax);
    layer->addWidget(new QLabel{"deg"});
    layer->addStretch(1);

    auto lay = new QVBoxLayout;
    lay->addLayout(layout);
    lay->addLayout(layer);

    setLayout(lay);
    setRemake([cellmin, cellmax, spinmin, spinmax, limitCheck, limitRange](){
        const Cluster* c = gSession->currentCluster();
        limitCheck->setEnabled(c);
        spinmax->setEnabled(c && limitRange->val());
        spinmin->setEnabled(c && limitRange->val());
        if (!c)
            return;
        Range r = c->rangeGma();
        spinmin->setMaximum(r.max-0.01);
        spinmax->setMinimum(r.min+0.01);
        cellmin->pureSetVal(r.min);
        cellmax->pureSetVal(r.max);
    });
}

//  ***********************************************************************************************
//! @class ControlsDetector

ControlsDetector::ControlsDetector()
    : QcrWidget{"ControlsDetector"}
{
    auto* vbox = new QVBoxLayout;
    vbox->addWidget(new DetectorControls);
    vbox->addWidget(new CutControls);
    vbox->addWidget(new ActiveClustersControls);
    vbox->addWidget(new GammaControls);
    vbox->addStretch();
    setLayout(vbox);
}
