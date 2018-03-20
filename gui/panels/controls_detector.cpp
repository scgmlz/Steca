// ************************************************************************** //
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
// ************************************************************************** //

#include "controls_detector.h"
#include "core/data/geometry.h"
#include "core/session.h"
#include "gui/actions/image_trafo_actions.h"
#include "gui/actions/toggles.h"
#include "gui/actions/triggers.h"
#include "gui/base/controls.h"
#include "gui/base/displays.h"
#include "gui/mainwin.h"
#include <QThread> // for sleep for debugging

// ************************************************************************** //
//  local class GeometryControls
// ************************************************************************** //

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

    CDoubleSpinBox detDistance_ {"detDistance", 6};
    CDoubleSpinBox detPixelSize_ {"detPixelSize", 6};
    CSpinBox beamOffsetI_ {"beamOffsetI", 3, true};
    CSpinBox beamOffsetJ_ {"beamOffsetJ", 3, true};
};

GeometryControls::GeometryControls()
{
    // initialization
    fromCore();
    detPixelSize_.setDecimals(3);

    // inbound connection
    connect(gSession, &Session::sigDetector, this, &GeometryControls::fromCore);

    // outbound connections and control widget setup
    connect(&detDistance_, _SLOT_(CDoubleSpinBox, valueReleased, double), [](double val) {
            gSession->geometry().setDetectorDistance(val); });
    connect(&detPixelSize_, _SLOT_(CDoubleSpinBox, valueReleased, double), [](double val) {
            gSession->geometry().setPixSize(val); });
    connect(&beamOffsetI_, _SLOT_(CSpinBox, valueReleased, int), [](int val) {
            gSession->geometry().midPixOffset().i = val; emit gSession->sigDetector(); });
    connect(&beamOffsetJ_, _SLOT_(CSpinBox, valueReleased, int), [](int val) {
            gSession->geometry().midPixOffset().j = val; emit gSession->sigDetector(); });

    // layout
    mmGrid_.addWidget(new QLabel("det. distance"), 0, 0);
    mmGrid_.addWidget(&detDistance_, 0, 1);
    mmGrid_.addWidget(new QLabel("mm"), 0, 2);
    mmGrid_.addWidget(new QLabel("pixel size"), 1, 0);
    mmGrid_.addWidget(&detPixelSize_, 1, 1);
    mmGrid_.addWidget(new QLabel("mm"), 1, 2);

    trafoLayout_.addWidget(new QLabel("image rotate"));
    trafoLayout_.addWidget(new XIconButton(&gGui->imageTrafoActions->rotateImage));
    trafoLayout_.addWidget(new QLabel("mirror"));
    trafoLayout_.addWidget(new XIconButton(&gGui->imageTrafoActions->mirrorImage));
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

void GeometryControls::fromCore()
{
    const Geometry& g = gSession->geometry();
    detDistance_.setValue(g.detectorDistance());
    detPixelSize_.setValue(g.pixSize());
    beamOffsetI_.setValue(g.midPixOffset().i);
    beamOffsetJ_.setValue(g.midPixOffset().j);
}

// ************************************************************************** //
//  local class CutControls
// ************************************************************************** //

//! Control widgets that govern the detector cuts.

class CutControls : public QFrame {
public:
    CutControls();
private:
    void fromCore();

    QGridLayout layout_;
    XIconButton link_ {&gGui->toggles->linkCuts};
    CSpinBox cutLeft_ {"cutLeft", 3, false, 0};
    CSpinBox cutTop_ {"cutTop", 3, false, 0};
    CSpinBox cutRight_ {"cutRight", 3, false, 0};
    CSpinBox cutBottom_ {"cutBottom", 3, false, 0};
};

CutControls::CutControls()
{
    // inbound connection
    connect(gSession, &Session::sigDetector, this, &CutControls::fromCore);

    // outbound connections
    connect(&cutLeft_, _SLOT_(CSpinBox, valueReleased, int), [](int value) {
            gSession->imageCut().setLeft(value); });
    connect(&cutRight_,  _SLOT_(CSpinBox, valueReleased, int), [](int value) {
            gSession->imageCut().setRight(value); });
    connect(&cutTop_,    _SLOT_(CSpinBox, valueReleased, int), [](int value) {
            gSession->imageCut().setTop(value); });
    connect(&cutBottom_, _SLOT_(CSpinBox, valueReleased, int), [](int value) {
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
    gGui->toggles->linkCuts.setChecked(cut.linked());
    cutLeft_.setValue(cut.left());
    cutTop_.setValue(cut.top());
    cutRight_.setValue(cut.right());
    cutBottom_.setValue(cut.bottom());
}

// ************************************************************************** //
//  local class ExperimentControls
// ************************************************************************** //

//! Control widgets that govern the combination of Measurement|s into Cluster|s.

class ExperimentControls : public QWidget {
public:
    ExperimentControls();
private:
    void fromCore();

    QHBoxLayout layout_;
    CSpinBox combineMeasurements_ {"combineMeasurements", 3, false, 1, 999,
            "Combine this number of measurements into one group"};
    CToggle dropIncompleteAction_ {"dropIncomplete",
            "Drop measurement groups that do not have the full number of members",
            false, ":/icon/dropIncomplete" };
    XIconButton dropIncompleteButton_ { &dropIncompleteAction_ };
};

ExperimentControls::ExperimentControls()
{
    // inbound connection
    connect(gSession, &Session::sigClusters, this, &ExperimentControls::fromCore);

    // outbound connections
    connect(&combineMeasurements_, _SLOT_(CSpinBox, valueReleased, int),
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

void ExperimentControls::fromCore()
{
    combineMeasurements_.setValue(gSession->dataset().binning());
    dropIncompleteAction_.setEnabled(gSession->dataset().hasIncomplete());
}

// ************************************************************************** //
//  local class GammaControls
// ************************************************************************** //

//! Control widgets that govern the gamma slicing.

class GammaControls : public QWidget {
public:
    GammaControls();
private:
    void fromCore();

    QHBoxLayout layout_;
    CSpinBox numSlices_{"numSlices", 2, false, 0, INT_MAX,
            "Number of γ slices (0: no slicing, take entire image)" };
};

GammaControls::GammaControls()
{
    // inbound connection
    connect(gSession, &Session::sigClusters, this, &GammaControls::fromCore);

    // outbound connections
    connect(&numSlices_, _SLOT_(CSpinBox, valueReleased, int), [](int val) {
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
    numSlices_.setValue(gSession->gammaSelection().numSlices());
    emit gSession->sigImage(); // TODO redundant with emission from idxSlice
}

// ************************************************************************** //
//  class ControlsDetector
// ************************************************************************** //

ControlsDetector::ControlsDetector()
{
    auto* vbox = new QVBoxLayout;
    vbox->addWidget(new GeometryControls);
    vbox->addWidget(new CutControls);
    vbox->addWidget(new ExperimentControls);
    vbox->addWidget(new GammaControls);

    //DEBUG
    auto* test = new CSpinBox {"test", 5, false};
    connect(test, &CSpinBox::valueReleased, test, [test](int val) {
            qDebug() << "TEST CHANGED " << test->value() << " = " << val;
            QThread::msleep(600);
            qDebug() << "WOKE UP "; },
        Qt::QueuedConnection);
    vbox->addWidget(test);

    vbox->addStretch();
    setLayout(vbox);
}
