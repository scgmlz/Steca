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

#include "gui/panels/tab_image.h"
#include "core/data/lens.h"
#include "core/session.h"
#include "gui/mainwin.h"
#include "gui/view/plot_image.h"
#include "gui/view/toggles.h"
//#include "qcr/base/debug.h"
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

void addOverlay(QImage& img, double midTth)
{
    gSession->gammaSelection.onData();
    gSession->thetaSelection.onData();
    const AngleMap& angleMap = gSession->angleMap.get(midTth);
    const Range& rgeGma = gSession->gammaSelection.range();
    const Range& rgeTth = gSession->thetaSelection.range();
    for (int j=0; j<img.size().height(); ++j) {
        for (int i=0; i<img.size().width(); ++i) {
            const ScatterDirection& a = angleMap.dirAt2(i, j);
            QColor color = img.pixel(i, j);
            if (rgeGma.contains(a.gma)) {
                if (rgeTth.contains(a.tth))
                    color = Qt::yellow;
                else
                    color.setGreen(qFloor(color.green() * .3 + 255 * .7));
            } else if (rgeTth.contains(a.tth)) {
                color.setGreen(qFloor(color.green() * .3 + 255 * .7));
            }
            img.setPixel(i, j, color.rgb());
        }
    }
}

} // namespace

//  ***********************************************************************************************
//! @class ImageTab

ImageTab::ImageTab()
{
    box1_ = new QHBoxLayout;
    box1_->addWidget(new QcrIconToggleButton{&gGui->toggles->fixedIntenImage}, Qt::AlignLeft);
    box1_->addWidget(new QcrIconToggleButton{&gGui->toggles->crosshair}, Qt::AlignLeft);

    controls_ = new QVBoxLayout;
    controls_->addLayout(box1_);

    imageView_ = new ImageView;

    auto* box = new QHBoxLayout;
    box->addLayout(controls_);
    box->addWidget(imageView_);
    setLayout(box);

    setRemake([this](){ render(); });
}

void ImageTab::render()
{
    imageView_->setPixmap(pixmap());
}

QPixmap ImageTab::blankPixmap()
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

    bool fixedScale = gSession->params.showAvgeDfgram.val();
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
    auto* gammaRangeTotal = new QLabel{"gammaRangeTotal"};
    auto* gammaRangeSlice = new QLabel{"gammaRangeSlice"};
    auto* thetaRangeTotal = new QLabel{"thetaRangeTotal"};
    auto* thetaRangeBin   = new QLabel{"thetaRangeBin"};
    setRemake( [=]() {
            gSession->gammaSelection.onData();
            gSession->thetaSelection.onData();

            const Cluster* cluster = gSession->currentCluster();
            int n = cluster ? cluster->size() : 1;
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

            gammaRangeTotal->setText(cluster ? cluster->rgeGmaFull().to_s("deg") : "");
            gammaRangeSlice->setText(gSession->gammaSelection.range().to_s("deg"));

            thetaRangeTotal->setText(cluster ? cluster->rgeTth().to_s("deg") : "");
            thetaRangeBin->setText(gSession->thetaSelection.range().to_s("deg"));

            render(); });

    box1_->addWidget(new QcrIconToggleButton{&gGui->toggles->showBins}, Qt::AlignLeft);

    auto* boxIdx = new QGridLayout;
    boxIdx->addWidget(new QLabel("idx (image)"), 0, 0, Qt::AlignLeft);
    boxIdx->addWidget(idxMeas, 0, 1, Qt::AlignLeft);
    boxIdx->addWidget(new QLabel("idx (ϑ)"), 1, 0, Qt::AlignLeft);
    boxIdx->addWidget(idxTheta, 1, 1, Qt::AlignLeft);
    boxIdx->addWidget(new QLabel("idx (γ)"), 2, 0, Qt::AlignLeft);
    boxIdx->addWidget(idxSlice, 2, 1, Qt::AlignLeft);
    controls_->addStretch(100);
    controls_->addLayout(boxIdx);

    controls_->addStretch(1000);
    auto* boxRanges = new QGridLayout;
    boxRanges->addWidget(new QLabel("γ total:"), 0, 0, Qt::AlignLeft);
    boxRanges->addWidget(new QLabel("γ slice:"), 1, 0, Qt::AlignLeft);
    boxRanges->addWidget(new QLabel("ϑ total:" ), 2, 0, Qt::AlignLeft);
    boxRanges->addWidget(new QLabel("ϑ bin:"   ), 3, 0, Qt::AlignLeft);
    boxRanges->addWidget(gammaRangeTotal, 0, 1, Qt::AlignLeft);
    boxRanges->addWidget(gammaRangeSlice, 1, 1, Qt::AlignLeft);
    boxRanges->addWidget(thetaRangeTotal, 2, 1, Qt::AlignLeft);
    boxRanges->addWidget(thetaRangeBin,   3, 1, Qt::AlignLeft);
    controls_->addLayout(boxRanges, Qt::AlignLeft|Qt::AlignBottom);
}

QPixmap DataImageTab::pixmap()
{
    const Measurement* m = measurement();
    if (!m)
        return blankPixmap();
    QImage img = makeImage(m->image());
    if (gGui->toggles->showBins.getValue())
        addOverlay(img, m->midTth());
    return QPixmap::fromImage(img);
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
    controls_->addStretch(1);
}

QPixmap CorrImageTab::pixmap()
{
    if (!gSession->corrset.hasFile())
        return blankPixmap();
    return QPixmap::fromImage(makeImage(gSession->corrset.image()));
}
