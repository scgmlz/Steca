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
#include "gui/base/layout.h"
#include "gui/mainwin.h"
#include "gui/actions/triggers.h"
#include <QSpacerItem>

// ************************************************************************** //
//  class ExperimentControls
// ************************************************************************** //

ExperimentControls::ExperimentControls() {

    auto layout = newQ::HBoxLayout();
    setLayout(layout);

    // 'combine' control
    layout->addWidget(new QLabel("combine:"));
    layout->addWidget(&combineMeasurements_);
    connect(&combineMeasurements_, _SLOT_(QSpinBox, valueChanged, int),
            [this](int num) { gSession->dataset().setBinning(num); });

    // 'if incomplete' control
    layout->addWidget(&dropIncompleteButton_);
    connect(&dropIncompleteAction_, &QAction::toggled,
            [this](bool on) { gSession->dataset().setDropIncomplete(on); });
    layout->addStretch(1);

    // back connection, to change controls from saved session
    connect(gSession, &Session::sigClusters, [=]() {
            combineMeasurements_.setValue(gSession->dataset().binning());
            dropIncompleteAction_.setEnabled(gSession->dataset().hasIncomplete()); });
    dropIncompleteAction_.setEnabled(false);
}


// ************************************************************************** //
//  class CutControls
// ************************************************************************** //

CutControls::CutControls()
{
    connect(gSession, &Session::sigDetector, this, &CutControls::fromSession);

    setFrameShape(QFrame::Box);
    QGridLayout* layout = newQ::GridLayout();
    setLayout(layout);

    layout->addWidget(new QLabel("cut"), 1, 0);
    layout->setColumnStretch(1, 1);
    layout->addWidget(&cutLeft_, 1, 2);
    layout->addWidget(new XIconButton(&gGui->toggles->linkCuts), 1, 3, Qt::AlignHCenter);
    layout->addWidget(&cutTop_, 0, 3);
    layout->addWidget(&cutBottom_, 2, 3);
    layout->addWidget(&cutRight_, 1, 4);
    layout->setColumnStretch(5, 1);
//    layout->addItem(new QSpacerItem(1,1,QSizePolicy::Ignored), 1, 4);

    connect(&cutLeft_,   _SLOT_(QSpinBox, valueChanged, int), [this](int value) {
            onChangedValue(true, value); });
    connect(&cutTop_,    _SLOT_(QSpinBox, valueChanged, int), [this](int value) {
            onChangedValue(true, value); });
    connect(&cutRight_,  _SLOT_(QSpinBox, valueChanged, int), [this](int value) {
            onChangedValue(false, value); });
    connect(&cutBottom_, _SLOT_(QSpinBox, valueChanged, int), [this](int value) {
            onChangedValue(false, value); });
}

void CutControls::onChangedValue(bool isTopOrLeft, int value) {
    ASSERT(value >= 0);
    if (gGui->toggles->linkCuts.isChecked())
        gSession->setImageCut(isTopOrLeft, true, ImageCut(value, value, value, value));
    else
        gSession->setImageCut(isTopOrLeft, false,
                              ImageCut(cutLeft_.value(), cutTop_.value(),
                                       cutRight_.value(), cutBottom_.value()));
};

void CutControls::fromSession()
{
    const ImageCut& cut = gSession->imageCut();

    cutLeft_.setValue(cut.left);
    cutTop_.setValue(cut.top);
    cutRight_.setValue(cut.right);
    cutBottom_.setValue(cut.bottom);
}


// ************************************************************************** //
//  class ControlsDetector
// ************************************************************************** //

ControlsDetector::ControlsDetector()
{
    connect(gSession, &Session::sigDetector, this, &ControlsDetector::fromSession);

    QVBoxLayout* vbox = newQ::VBoxLayout();
    setLayout(vbox);

    // widgets

    detDistance_.setValue(Geometry::DEF_DETECTOR_DISTANCE);
    connect(&detDistance_, _SLOT_(QDoubleSpinBox, valueChanged, double), [this]() { toSession(); });

    detPixelSize_.setDecimals(3);
    detPixelSize_.setValue(Geometry::DEF_DETECTOR_PIXEL_SIZE);
    connect(&detPixelSize_, _SLOT_(QDoubleSpinBox, valueChanged, double), [this]() {
            toSession(); });

    // layout

    QGridLayout* grid = newQ::GridLayout();
    int row = 0;

    auto _add = [&grid, &row](const QVector<QWidget*>& ws, int left = 1) {
        int i = 0, cnt = ws.count();
        QBoxLayout* hbox = newQ::HBoxLayout();
        hbox->addStretch(1);
        while (i < left)
            hbox->addWidget(ws.at(i++));
        grid->addLayout(hbox, row, 0);
        hbox = newQ::HBoxLayout();
        while (i < cnt)
            hbox->addWidget(ws.at(i++));
        grid->addLayout(hbox, row, 1);
        hbox->addStretch(1);
        row++;
    };

    _add({ new QLabel("det. distance"), &detDistance_, new QLabel("mm") });
    _add({ new QLabel("pixel size"), &detPixelSize_, new QLabel("mm") });
    _add({ new QLabel("beam offset X"), &beamOffsetI_, new QLabel("pix") });
    _add({ new QLabel("Y"), &beamOffsetJ_, new QLabel("pix") });
    _add({ new QLabel("image rotate"),
                new XIconButton(&gGui->triggers->rotateImage),
                new QLabel("mirror"),
                new XIconButton(&gGui->toggles->mirrorImage) });

    grid->setColumnStretch(grid->columnCount(), 1);

    vbox->addLayout(grid);
    vbox->addWidget(&cutControls);
    vbox->addWidget(&experimentControls);
    vbox->addStretch();
}

void ControlsDetector::toSession() {
    gSession->setGeometry(
        qMax(qreal(Geometry::MIN_DETECTOR_DISTANCE), detDistance_.value()),
        qMax(qreal(Geometry::MIN_DETECTOR_PIXEL_SIZE), detPixelSize_.value()),
        IJ(beamOffsetI_.value(), beamOffsetJ_.value()));
}

void ControlsDetector::fromSession() {
    const Geometry& g = gSession->geometry();

    detDistance_.setValue(g.detectorDistance);
    detPixelSize_.setValue(g.pixSize);

    beamOffsetI_.setValue(g.midPixOffset.i);
    beamOffsetJ_.setValue(g.midPixOffset.j);
}
