//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/tab_image.cpp
//! @brief     Implements classes DataImageTab, CorrImageTab, and their dependences
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "tab_image.h"
#include "core/data/lens.h"
#include "core/raw/angle_map.h"
#include "core/session.h"
#include "gui/actions/toggles.h"
#include "gui/mainwin.h"
#include "qcr/base/debug.h"
#include <qmath.h>

namespace {

//! Color map for raw diffraction image: black-red-gold.
QRgb intenImage(float inten, float maxInten, bool curved) {
    if (qIsNaN(inten))
        return qRgb(0x00, 0xff, 0xff);
    if (qIsInf(inten))
        return qRgb(0xff, 0xff, 0xff);

    if (qIsNaN(maxInten) || maxInten <= 0)
        return qRgb(0x00, 0x00, 0x00);

    inten /= maxInten;

    if (curved && inten > 0)
        inten = qPow(inten, .6f);

    float const low = .25f, mid = .5f, high = .75f;
    if (inten < low)
        return qRgb(int(0xff * inten * 4), 0, 0);
    if (inten < mid)
        return qRgb(0xff, int(0xff * (inten - low) * 4), 0);
    if (inten < high)
        return qRgb(int(0xff - (0xff * (inten - mid) * 4)), 0xff, int(0xff * (inten - mid) * 4));
    return qRgb(int(0xff * (inten - high) * 4), 0xff, 0xff);
}

} // namespace

//  ***********************************************************************************************
//! @class ImageView

ImageView::ImageView()
    : scale_(0)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(&gGui->toggles->crosshair, &QAction::toggled, [this](bool /*unused*/) { update(); });
}

void ImageView::setPixmap(const QPixmap& pixmap)
{
    original_ = pixmap;
    setScale();
}

void ImageView::setScale()
{
    if (original_.isNull()) {
        scale_ = 0;
    } else {
        const QSize& sz = size();
        const QSize& os = original_.size();
        scale_ = qMin(double(sz.width() - 2) / os.width(), double(sz.height() - 2) / os.height());
    }

    if (scale_ <= 0)
        scaled_ = QPixmap();
    else
        scaled_ = original_.scaled(original_.size() * scale_);

    update();
}

void ImageView::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);
    setScale();
}

void ImageView::paintEvent(QPaintEvent*)
{
    // paint centered
    const QSize margin = (size() - scaled_.size()) / 2;
    const QRect rect(QPoint(margin.width(), margin.height()), scaled_.size());

    QPainter p(this);

    // image
    p.drawPixmap(rect.left(), rect.top(), scaled_);

    // crosshair overlay
    if (gGui->toggles->crosshair.getValue()) {
        p.setPen(Qt::lightGray);

        // cut
        const ImageCut& cut = gSession->params.imageCut;
        const QRect r = rect.adjusted(-1, -1, 0, 0)
                      .adjusted(
                          qRound(scale_ * cut.left.val()), qRound(scale_ * cut.top.val()),
                          -qRound(scale_ * cut.right.val()), -qRound(scale_ * cut.bottom.val()));
        p.drawRect(r);

        const QPoint rc = r.center();
        const int rcx = rc.x(), rcy = rc.y();

        int rl, rt, rr, rb;
        r.getCoords(&rl, &rt, &rr, &rb);
        const int rw = rr - rl;

        // cross
        const int x = qRound(rcx + scale_ * gSession->params.detector.pixOffset[0].val());
        const int y = qRound(rcy + scale_ * gSession->params.detector.pixOffset[1].val());
        p.drawLine(x, rt, x, rb);
        p.drawLine(rl, y, rr, y);

        // text
        QPoint pos(rr - rw / 5, rcy);
        p.setPen(Qt::cyan);
        p.drawText(pos, "γ=0");
    }

    // frame
    p.setPen(Qt::black);
    p.drawRect(rect.adjusted(-1, -1, 0, 0));
}

//  ***********************************************************************************************
//  base class ImageTab
//  ***********************************************************************************************

ImageTab::ImageTab()
    : btnScale_ {&gGui->toggles->fixedIntenImage}
    , btnOverlay_ {&gGui->toggles->crosshair}
{
    // layout
    box1_.addWidget(&btnScale_, Qt::AlignLeft);
    box1_.addWidget(&btnOverlay_, Qt::AlignLeft);
    controls_.addLayout(&box1_);

    box_.addLayout(&controls_);
    box_.addWidget(&imageView_);
    setLayout(&box_);

    setRemake([this]() {render();});
}

void ImageTab::render()
{
    gSession->corrset.clearIntens(); // trigger redisplay // TODO move this to more appriate place
    imageView_.setPixmap(pixmap());
}

QPixmap ImageTab::makePixmap(const Image& image)
{
    QImage im = makeImage(image);
    return QPixmap::fromImage(im);
}

QPixmap ImageTab::makeOverlayPixmap(const Measurement* m)
{
    gSession->gammaSelection.onData();
    gSession->thetaSelection.onData();
    QImage im = makeImage(m->image());
    const AngleMap& angleMap = m->angleMap();
    const Range& rgeGma = gSession->gammaSelection.range();
    const Range& rgeTth = gSession->thetaSelection.range();
    for (int j=0; j<im.size().height(); ++j) {
        for (int i=0; i<im.size().width(); ++i) {
            const ScatterDirection& a = angleMap.dirAt2(i, j);
            QColor color = im.pixel(i, j);
            if (rgeGma.contains(a.gma)) {
                if (rgeTth.contains(a.tth))
                    color = Qt::yellow;
                else
                    color.setGreen(qFloor(color.green() * .3 + 255 * .7));
            } else if (rgeTth.contains(a.tth)) {
                color.setGreen(qFloor(color.green() * .3 + 255 * .7));
            }
            im.setPixel(i, j, color.rgb());
        }
    }
    return QPixmap::fromImage(im);
}

QPixmap ImageTab::makeBlankPixmap()
{
    const size2d size = gSession->imageSize();
    QPixmap pixmap(size.w, size.h);
    pixmap.fill(QColor(0, 0, 0, 0));
    return pixmap;
}

QImage ImageTab::makeImage(const Image& image)
{
    ImageLens imageLens(image, true, false);
    const size2d sz = imageLens.imgSize();
    if (sz.isEmpty())
        return {};

    QImage ret(QSize(sz.w, sz.h), QImage::Format_RGB32);

    bool fixedScale = gGui->toggles->fixedIntenImage.getValue();
    const Range rgeInten = imageLens.rgeInten(fixedScale);
    float maxInten = float(rgeInten.max);

    for (int j=0; j<sz.h; ++j)
        for (int i=0; i<sz.w; ++i)
            ret.setPixel(i, j, intenImage(imageLens.imageInten(i, j), maxInten, !fixedScale));
    return ret;
}

//  ***********************************************************************************************
//! @class DataImageTab

DataImageTab::DataImageTab()
    : btnShowBins_ {&gGui->toggles->showBins}
{
    auto* idxMeas  = new QcrSpinBox{
        "idxMeas", &iMeas, 4, false, 1, INT_MAX,
        "Index of measurement within the current group of measurements"};
    auto* idxTheta = new QcrSpinBox{
        "idxTheta", &gSession->thetaSelection.currArc,
        4, false, 1, INT_MAX, "Index of 2θ bin to be shown" };
    auto* idxSlice = new QcrSpinBox{
        "idxSlice", &gSession->gammaSelection.currSlice,
        4, false, 1, INT_MAX, "Index of γ slice to be shown" };
    setRemake( [=]() {
            gSession->gammaSelection.onData();
            gSession->thetaSelection.onData();

            const Cluster* cluster = gSession->currentCluster();
            int n = cluster ? cluster->count() : 1;
            idxMeas->setMaximum(n);
            if (n>1) {
                idxMeas->setMinimum(1);
                idxMeas->setEnabled(true);
                idxMeas->setToolTip(
                    "Index of measurement within the current group of " +
                    QString::number(n) + " measurements");
            } else {
                idxMeas->setEnabled(false);
                idxMeas->setToolTip(
                    "Index of measurement within the current group of measurements");
            }

            int nGamma = gSession->gammaSelection.numSlices.val();
            idxSlice->setMaximum(nGamma);
            idxSlice->setEnabled(nGamma>1);

            gammaRangeTotal_.setText(cluster ? cluster->rgeGmaFull().to_s("deg") : "");
            gammaRangeSlice_.setText(gSession->gammaSelection.range().to_s("deg"));

            thetaRangeTotal_.setText(cluster ? cluster->rgeTth().to_s("deg") : "");
            thetaRangeBin_.setText(gSession->thetaSelection.range().to_s("deg"));

            render();
        } );

    // layout
    box1_.addWidget(&btnShowBins_, Qt::AlignLeft);

    boxIdx_.addWidget(new QLabel("idx (image)"), 0, 0, Qt::AlignLeft);
    boxIdx_.addWidget(idxMeas, 0, 1, Qt::AlignLeft);
    boxIdx_.addWidget(new QLabel("idx (ϑ)"), 1, 0, Qt::AlignLeft);
    boxIdx_.addWidget(idxTheta, 1, 1, Qt::AlignLeft);
    boxIdx_.addWidget(new QLabel("idx (γ)"), 2, 0, Qt::AlignLeft);
    boxIdx_.addWidget(idxSlice, 2, 1, Qt::AlignLeft);
    controls_.addStretch(100);
    controls_.addLayout(&boxIdx_);

    controls_.addStretch(1000);
    boxRanges_.addWidget(new QLabel("γ total:"), 0, 0, Qt::AlignLeft);
    boxRanges_.addWidget(new QLabel("γ slice:"), 1, 0, Qt::AlignLeft);
    boxRanges_.addWidget(new QLabel("ϑ total:" ), 2, 0, Qt::AlignLeft);
    boxRanges_.addWidget(new QLabel("ϑ bin:"   ), 3, 0, Qt::AlignLeft);
    boxRanges_.addWidget(&gammaRangeTotal_, 0, 1, Qt::AlignLeft);
    boxRanges_.addWidget(&gammaRangeSlice_, 1, 1, Qt::AlignLeft);
    boxRanges_.addWidget(&thetaRangeTotal_, 2, 1, Qt::AlignLeft);
    boxRanges_.addWidget(&thetaRangeBin_,   3, 1, Qt::AlignLeft);
    controls_.addLayout(&boxRanges_, Qt::AlignLeft|Qt::AlignBottom);
}

DataImageTab::~DataImageTab()
{
    box1_.removeWidget(&btnShowBins_);
    controls_.removeItem(&boxIdx_);
    controls_.removeItem(&boxRanges_);
}

QPixmap DataImageTab::pixmap()
{
    const Measurement* m = measurement();
    if (!m)
        return makeBlankPixmap();
    if (gGui->toggles->showBins.getValue())
        return makeOverlayPixmap(m);
    return makePixmap(m->image());
}

const Measurement* DataImageTab::measurement()
{
    const Cluster* cluster = gSession->currentCluster();
    return cluster ? cluster->at(iMeas.val()-1) : nullptr;
}



//  ***********************************************************************************************
//! @class CorrImageTab

CorrImageTab::CorrImageTab()
{
    controls_.addStretch(1);
}

QPixmap CorrImageTab::pixmap()
{
    if (!gSession->corrset.hasFile())
        return makeBlankPixmap();
    return makePixmap(gSession->corrset.image());
}
