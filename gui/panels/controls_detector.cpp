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
#include "qcr/engine/debug.h"
#include <QThread> // for sleep for debugging

//  ***********************************************************************************************
//! @class GeometryControls (local scope)

//! Control widgets that govern the detector geometry.

class GeometryControls : public QWidget {
public:
    GeometryControls();
private:
    void fromCore();

    QVBoxLayout vbox_;
    QGridLayout mmGrid_;
    QHBoxLayout trafoLayout_;
    QHBoxLayout offsetLayout_;

    QcrDoubleSpinBox* detDistance_;
    QcrDoubleSpinBox* detPixelSize_;
    QcrSpinBox* beamOffsetI_;
    QcrSpinBox* beamOffsetJ_;
};

GeometryControls::GeometryControls()
{
    // initialization
    detDistance_  = new QcrDoubleSpinBox {"detDistance", 6};
    detPixelSize_ = new QcrDoubleSpinBox {"detPixelSize", 3};
    beamOffsetI_  = new QcrSpinBox       {"beamOffsetI", 3, true};
    beamOffsetJ_  = new QcrSpinBox       {"beamOffsetJ", 3, true};
    fromCore();

    // inbound connection
    connect(gSession, &Session::sigDetector, this, &GeometryControls::fromCore);

    // outbound connections and control widget setup
    connect(detDistance_, &QcrDoubleSpinBox::valueReleased, [](double val) {
            gSession->geometry().setDetectorDistance(val); });
    connect(detPixelSize_, &QcrDoubleSpinBox::valueReleased, [](double val) {
            gSession->geometry().setPixSize(val); });
    connect(beamOffsetI_, &QcrSpinBox::valueReleased, [](int val) {
            gSession->geometry().midPixOffset().i = val;
            EMITS("GeometryControls", gSession->sigDetector()); });
    connect(beamOffsetJ_, &QcrSpinBox::valueReleased, [](int val) {
            gSession->geometry().midPixOffset().j = val;
            EMITS("GeometryControls", gSession->sigDetector()); });

    // layout
    mmGrid_.addWidget(new QLabel("det. distance"), 0, 0);
    mmGrid_.addWidget(detDistance_, 0, 1);
    mmGrid_.addWidget(new QLabel("mm"), 0, 2);
    mmGrid_.addWidget(new QLabel("pixel size"), 1, 0);
    mmGrid_.addWidget(detPixelSize_, 1, 1);
    mmGrid_.addWidget(new QLabel("mm"), 1, 2);

    trafoLayout_.addWidget(new QLabel("image rotate"));
    trafoLayout_.addWidget(new QcrIconButton(&gGui->imageTrafoActions->rotateImage));
    trafoLayout_.addWidget(new QLabel("mirror"));
    trafoLayout_.addWidget(new QcrIconButton(&gGui->imageTrafoActions->mirrorImage));
    trafoLayout_.addStretch(1);

    offsetLayout_.addWidget(new QLabel("offset X"));
    offsetLayout_.addWidget(beamOffsetI_);
    offsetLayout_.addWidget(new QLabel(" Y"));
    offsetLayout_.addWidget(beamOffsetJ_);
    offsetLayout_.addWidget(new QLabel("pix"));
    offsetLayout_.addStretch(1);

    vbox_.addLayout(&mmGrid_);
    vbox_.addLayout(&trafoLayout_);
    vbox_.addLayout(&offsetLayout_);
    setLayout(&vbox_);
}

void GeometryControls::fromCore()
{
    const Geometry& g = gSession->geometry();
    detDistance_ ->programaticallySetValue(g.detectorDistance());
    detPixelSize_->programaticallySetValue(g.pixSize());
    beamOffsetI_ ->programaticallySetValue(g.midPixOffset().i);
    beamOffsetJ_ ->programaticallySetValue(g.midPixOffset().j);
}

//  ***********************************************************************************************
//! @class CutControls (local scope)

//! Control widgets that govern the detector cuts.

class CutControls : public QFrame {
public:
    CutControls();
private:
    void fromCore();

    QGridLayout layout_;
    QcrIconButton link_ {&gGui->toggles->linkCuts};
    QcrSpinBox cutLeft_ {"cutLeft", 3, false, 0};
    QcrSpinBox cutTop_ {"cutTop", 3, false, 0};
    QcrSpinBox cutRight_ {"cutRight", 3, false, 0};
    QcrSpinBox cutBottom_ {"cutBottom", 3, false, 0};
};

CutControls::CutControls()
{
    // inbound connection
    connect(gSession, &Session::sigDetector, this, &CutControls::fromCore);

    // outbound connections
    connect(&cutLeft_, &QcrSpinBox::valueReleased, [](int value) {
            gSession->imageCut().setLeft(value); });
    connect(&cutRight_,  &QcrSpinBox::valueReleased, [](int value) {
            gSession->imageCut().setRight(value); });
    connect(&cutTop_,    &QcrSpinBox::valueReleased, [](int value) {
            gSession->imageCut().setTop(value); });
    connect(&cutBottom_, &QcrSpinBox::valueReleased, [](int value) {
            gSession->imageCut().setBottom(value); });
    connect(&gGui->toggles->linkCuts, &QAction::toggled, [](bool value) {
            gSession->imageCut().setLinked(value); });

    // layout
    layout_.addWidget(new QLabel("cut"), 1, 0);
    layout_.addWidget(&cutLeft_, 1, 2);
    layout_.addWidget(&link_, 1, 3, Qt::AlignHCenter);
    layout_.addWidget(&cutTop_, 0, 3);
    layout_.addWidget(&cutBottom_, 2, 3);
    layout_.addWidget(&cutRight_, 1, 4);
    layout_.setColumnStretch(5, 1);
    setLayout(&layout_);

    // initialization
    fromCore();
}

void CutControls::fromCore()
{
    const ImageCut& cut = gSession->imageCut();
    gGui->toggles->linkCuts.programaticallySetValue(cut.linked());
    cutLeft_.programaticallySetValue(cut.left());
    cutTop_.programaticallySetValue(cut.top());
    cutRight_.programaticallySetValue(cut.right());
    cutBottom_.programaticallySetValue(cut.bottom());
}

//  ***********************************************************************************************
//! @class ActiveClustersControls (local scope)

//! Control widgets that govern the combination of Measurement|s into Cluster|s.

class ActiveClustersControls : public QWidget {
public:
    ActiveClustersControls();
private:
    void fromCore();

    QHBoxLayout layout_;
    QcrSpinBox combineMeasurements_ {"combineMeasurements", 3, false, 1, 999,
            "Combine this number of measurements into one group"};
    QcrToggle dropIncompleteAction_ {"dropIncomplete",
            "Drop measurement groups that do not have the full number of members",
            false, ":/icon/dropIncomplete" };
    QcrIconButton dropIncompleteButton_ { &dropIncompleteAction_ };
};

ActiveClustersControls::ActiveClustersControls()
{
    // inbound connection
    connect(gSession, &Session::sigClusters, this, &ActiveClustersControls::fromCore);

    // outbound connections
    connect(&combineMeasurements_, &QcrSpinBox::valueReleased,
            [](int num) { gSession->dataset().setBinning(num); });
    connect(&dropIncompleteAction_, &QAction::toggled,
            [](bool on) { gSession->dataset().setDropIncomplete(on); });

    // layout
    layout_.addWidget(new QLabel("combine"));
    layout_.addWidget(&combineMeasurements_);
    layout_.addWidget(new QLabel("measurements"));
    layout_.addWidget(&dropIncompleteButton_);
    layout_.addStretch(1);
    setLayout(&layout_);

    //initialization
    dropIncompleteAction_.setEnabled(false);
    fromCore();
}

void ActiveClustersControls::fromCore()
{
    combineMeasurements_.programaticallySetValue(gSession->dataset().binning());
    dropIncompleteAction_.setEnabled(gSession->dataset().hasIncomplete());
}

//  ***********************************************************************************************
//! @class GammaControls (local scope)

//! Control widgets that govern the gamma slicing.

class GammaControls : public QWidget {
public:
    GammaControls();
private:
    void fromCore();

    QHBoxLayout layout_;
    QcrSpinBox numSlices_{"numSlices", 2, false, 0, INT_MAX,
            "Number of γ slices (0: no slicing, take entire image)" };
};

GammaControls::GammaControls()
{
    // inbound connection
    connect(gSession, &Session::sigClusters, this, &GammaControls::fromCore);

    // outbound connections
    connect(&numSlices_, &QcrSpinBox::valueReleased, [](int val) {
            gSession->gammaSelection().setNumSlices(val); });

    // layout
    layout_.addWidget(new QLabel("number of γ slices"));
    layout_.addWidget(&numSlices_);
    layout_.addStretch(1);
    setLayout(&layout_);

    //initialization
    fromCore();
}

void GammaControls::fromCore()
{
    numSlices_.programaticallySetValue(gSession->gammaSelection().numSlices());
    EMITS("GammaControls", gSession->sigImage()); // TODO redundant with emission from idxSlice
}

//  ***********************************************************************************************
//! @class ControlsDetector

ControlsDetector::ControlsDetector()
{
    auto* vbox = new QVBoxLayout;
    vbox->addWidget(new GeometryControls);
    vbox->addWidget(new CutControls);
    vbox->addWidget(new ActiveClustersControls);
    vbox->addWidget(new GammaControls);
    vbox->addStretch();
    setLayout(vbox);
}
