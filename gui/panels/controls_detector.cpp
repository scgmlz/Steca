// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/controls_detector.cpp
//! @brief     Implements class SubframeSetup, and local classes
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "controls_detector.h"
#include "core/session.h"
#include "gui/base/displays.h"
#include "gui/actions/triggers.h"
#include <QSpacerItem>

// ************************************************************************** //
//  class CutControls
// ************************************************************************** //

CutControls::CutControls()
{
    connect(gSession, &Session::sigDetector, this, &CutControls::fromCore);

    setLayout(&layout_);
    layout_.setSpacing(2);
    layout_.setContentsMargins(1,3,3,3);

    layout_.addWidget(new QLabel("cut"), 1, 0);
    layout_.setColumnStretch(1, 1);
    layout_.addWidget(&cutLeft_, 1, 2);
    layout_.addWidget(&link_, 1, 3, Qt::AlignHCenter);
    layout_.addWidget(&cutTop_, 0, 3);
    layout_.addWidget(&cutBottom_, 2, 3);
    layout_.addWidget(&cutRight_, 1, 4);
    layout_.setColumnStretch(5, 1);

    connect(&cutLeft_,   _SLOT_(QSpinBox, valueChanged, int), [this](int value) {
            gSession->imageCut().setLeft(value); });
    connect(&cutRight_,  _SLOT_(QSpinBox, valueChanged, int), [this](int value) {
            gSession->imageCut().setRight(value); });
    connect(&cutTop_,    _SLOT_(QSpinBox, valueChanged, int), [this](int value) {
            gSession->imageCut().setTop(value); });
    connect(&cutBottom_, _SLOT_(QSpinBox, valueChanged, int), [this](int value) {
            gSession->imageCut().setBottom(value); });
    connect(&gGui->toggles->linkCuts, &QAction::toggled, [this](bool value) {
            gSession->imageCut().setLinked(value); });
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
//  class ExperimentControls
// ************************************************************************** //

ExperimentControls::ExperimentControls() {

    auto layout = new QHBoxLayout();
    setLayout(layout);

    layout->addWidget(new QLabel("combine"));
    layout->addWidget(&combineMeasurements_);
    layout->addWidget(new QLabel("measurements"));
    layout->addWidget(&dropIncompleteButton_);
    layout->addStretch(1);

    // back connection, to change controls from saved session
    connect(gSession, &Session::sigClusters, [=]() {
            combineMeasurements_.setValue(gSession->dataset().binning());
            dropIncompleteAction_.setEnabled(gSession->dataset().hasIncomplete()); });
    dropIncompleteAction_.setEnabled(false);

    // forward connections
    connect(&combineMeasurements_, _SLOT_(QSpinBox, valueChanged, int),
            [this](int num) { gSession->dataset().setBinning(num); });
    connect(&dropIncompleteAction_, &QAction::toggled,
            [this](bool on) { gSession->dataset().setDropIncomplete(on); });
}


// ************************************************************************** //
//  class ControlsDetector
// ************************************************************************** //

ControlsDetector::ControlsDetector()
{
    connect(gSession, &Session::sigDetector, this, &ControlsDetector::fromCore);

    setLayout(&vbox_);

    detDistance_.setValue(Geometry::DEF_DETECTOR_DISTANCE);
    connect(&detDistance_, _SLOT_(QDoubleSpinBox, valueChanged, double), [this]() { toCore(); });

    detPixelSize_.setDecimals(3);
    detPixelSize_.setValue(Geometry::DEF_DETECTOR_PIXEL_SIZE);
    connect(&detPixelSize_, _SLOT_(QDoubleSpinBox, valueChanged, double), [this]() { toCore(); });

    mmGrid_.addWidget(new QLabel("det. distance"), 0, 0);
    mmGrid_.addWidget(&detDistance_, 0, 1);
    mmGrid_.addWidget(new QLabel("mm"), 0, 2);
    mmGrid_.addWidget(new QLabel("pixel size"), 1, 0);
    mmGrid_.addWidget(&detPixelSize_, 1, 1);
    mmGrid_.addWidget(new QLabel("mm"), 1, 2);

    trafoLayout_.addWidget(new QLabel("image rotate"));
    trafoLayout_.addWidget(new XIconButton(&gGui->triggers->rotateImage));
    trafoLayout_.addWidget(new QLabel("mirror"));
    trafoLayout_.addWidget(new XIconButton(&gGui->toggles->mirrorImage));
    trafoLayout_.addStretch(1);

    offsetLayout_.addWidget(new QLabel("offset X"));
    offsetLayout_.addWidget(&beamOffsetI_);
    offsetLayout_.addWidget(new QLabel(" Y"));
    offsetLayout_.addWidget(&beamOffsetJ_);
    offsetLayout_.addWidget(new QLabel("pix"));
    offsetLayout_.addStretch(1);
    connect(&beamOffsetI_, _SLOT_(QSpinBox, valueChanged, int), [this]() { toCore(); });
    connect(&beamOffsetJ_, _SLOT_(QSpinBox, valueChanged, int), [this]() { toCore(); });

    vbox_.addLayout(&mmGrid_);
    vbox_.addLayout(&trafoLayout_);
    vbox_.addLayout(&offsetLayout_);
    vbox_.addWidget(&cutControls);
    vbox_.addWidget(&experimentControls);
    vbox_.addStretch();
}

void ControlsDetector::toCore() {
    gSession->setGeometry(
        qMax(qreal(Geometry::MIN_DETECTOR_DISTANCE), detDistance_.value()),
        qMax(qreal(Geometry::MIN_DETECTOR_PIXEL_SIZE), detPixelSize_.value()),
        IJ(beamOffsetI_.value(), beamOffsetJ_.value()));
}

void ControlsDetector::fromCore() {
    const Geometry& g = gSession->geometry();

    detDistance_.setValue(g.detectorDistance);
    detPixelSize_.setValue(g.pixSize);

    beamOffsetI_.setValue(g.midPixOffset.i);
    beamOffsetJ_.setValue(g.midPixOffset.j);
}
