// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/subframe_image.cpp
//! @brief     Implements class SubframeImage
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "subframe_image.h"
#include "core/data/measurement.h"
#include "core/session.h"
#include "gui/cfg/colors.h"
#include "gui/thehub.h"
#include "gui/widgets/new_q.h"
#include "gui/widgets/various_widgets.h"
#include <qmath.h>
#include <QPainter>

// ************************************************************************** //
//  local class ImageWidget
// ************************************************************************** //

class ImageWidget final : public QWidget {
public:
    ImageWidget();

    void setPixmap(QPixmap const&);
    void setScale();

private:
    void resizeEvent(QResizeEvent*);
    void paintEvent(QPaintEvent*);

    qreal scale_;
    QPixmap original_, scaled_;
};


ImageWidget::ImageWidget() : scale_(0) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(gHub->toggle_showOverlay, &QAction::toggled, [this]() { update(); });
    connect(gHub->toggle_stepScale, &QAction::toggled, [this]() { setScale(); });
}

void ImageWidget::setPixmap(QPixmap const& pixmap) {
    original_ = pixmap;
    setScale();
}

void ImageWidget::setScale() {
    if (original_.isNull()) {
        scale_ = 0;
    } else {
        const QSize& sz = size();
        const QSize& os = original_.size();
        scale_ = qMin(qreal(sz.width() - 2) / os.width(), qreal(sz.height() - 2) / os.height());
    }

    if (gHub->toggle_stepScale->isChecked() && scale_ > 0)
        scale_ = (scale_ >= 1) ? qFloor(scale_) : 1.0 / qCeil(1.0 / scale_);

    if (original_.isNull() || !(scale_ > 0))
        scaled_ = QPixmap();
    else
        scaled_ = original_.scaled(original_.size() * scale_);

    update();
}

void ImageWidget::resizeEvent(QResizeEvent* e) {
    QWidget::resizeEvent(e);
    setScale();
}

void ImageWidget::paintEvent(QPaintEvent*) {
    // paint centered
    const QSize margin = (size() - scaled_.size()) / 2;
    const QRect rect(QPoint(margin.width(), margin.height()), scaled_.size());

    QPainter p(this);

    // image
    p.drawPixmap(rect.left(), rect.top(), scaled_);

    // overlay
    if (gHub->toggle_showOverlay->isChecked()) {
        p.setPen(Qt::lightGray);

        // cut
        const ImageCut& cut = gSession->imageCut();
        const QRect r = rect.adjusted(-1, -1, 0, 0)
                      .adjusted(
                          qRound(scale_ * cut.left), qRound(scale_ * cut.top),
                          -qRound(scale_ * cut.right), -qRound(scale_ * cut.bottom));
        p.drawRect(r);

        const QPoint rc = r.center();
        const int rcx = rc.x(), rcy = rc.y();

        int rl, rt, rr, rb;
        r.getCoords(&rl, &rt, &rr, &rb);
        const int rw = rr - rl;

        // cross
        const IJ& off = gSession->geometry().midPixOffset;
        const int x = qRound(rcx + scale_ * off.i);
        const int y = qRound(rcy + scale_ * off.j);
        p.drawLine(x, rt, x, rb);
        p.drawLine(rl, y, rr, y);

        // text
        const QFontMetrics& fm = fontMetrics();
        QPoint pos(rr - rw / 5, rcy);
        p.setPen(Qt::cyan);
        p.drawText(pos, "γ=0");
    }

    // frame
    p.setPen(Qt::black);
    p.drawRect(rect.adjusted(-1, -1, 0, 0));
}


// ************************************************************************** //
//  class SubframeImage
// ************************************************************************** //

SubframeImage::SubframeImage() {
    setTabPosition(QTabWidget::North);

    {
        QBoxLayout& box = newQ::Tab(this, "Image")->box();

        QBoxLayout* hb = newQ::HBoxLayout();
        box.addLayout(hb);
        box.setAlignment(hb, Qt::AlignTop);

        hb->addWidget(newQ::IconButton(gHub->toggle_fixedIntenImage));
        hb->addWidget(newQ::IconButton(gHub->toggle_stepScale));
        hb->addWidget(newQ::IconButton(gHub->toggle_showOverlay));
        hb->addWidget((spinN_ = newQ::SpinBox(4, false, 1)));

        hb->addStretch(1);

        hb->addWidget(newQ::IconButton(gHub->toggle_showBins));
        hb->addWidget(newQ::Label("γ count"));
        hb->addWidget((numSlices_ = newQ::SpinBox(4, false, 0)));
        hb->addWidget(newQ::Label("#"));
        hb->addWidget((numSlice_ = newQ::SpinBox(4, false, 1)));

        hb->addWidget(newQ::Label("min"));
        hb->addWidget((minGamma_ = newQ::DoubleSpinBox(4, 2)));
        hb->addWidget(newQ::Label("max"));
        hb->addWidget((maxGamma_ = newQ::DoubleSpinBox(4, 2)));

        minGamma_->setReadOnly(true);
        maxGamma_->setReadOnly(true);

        hb->addWidget(newQ::Label("bin#"));
        hb->addWidget((numBin_ = newQ::SpinBox(4, false, 1)));

        box.addWidget((dataImageWidget_ = new ImageWidget()));

        connect(spinN_, _SLOT_(QSpinBox, valueChanged, int), [this]() { render(); });
        connect(numSlices_, _SLOT_(QSpinBox, valueChanged, int), [this]() { render(); });
        connect(numSlice_, _SLOT_(QSpinBox, valueChanged, int), [this]() { render(); });
        connect(numBin_, _SLOT_(QSpinBox, valueChanged, int), [this]() { render(); });
    }

    {
        BoxWidget& tab = *newQ::Tab(this, "Correction");

        connect(gHub, &TheHub::sigCorrFile,
                [&tab](shp_Datafile file) { tab.setEnabled(!file.isNull()); });

        QBoxLayout& box = tab.box();

        QBoxLayout* hb = newQ::HBoxLayout();
        box.addLayout(hb);
        box.setAlignment(hb, Qt::AlignTop);

        hb->addWidget(newQ::IconButton(gHub->toggle_fixedIntenImage));
        hb->addWidget(newQ::IconButton(gHub->toggle_stepScale));
        hb->addWidget(newQ::IconButton(gHub->toggle_showOverlay));
        hb->addStretch(1);

        box.addWidget((corrImageWidget_ = new ImageWidget()));
    }

    connect(gHub->toggle_enableCorr, &QAction::toggled, [this](bool) { render(); });
    connect(gHub->toggle_showBins, &QAction::toggled, [this]() { render(); });

    connect(gHub, &TheHub::sigDisplayChanged, [this](){ render(); });
    connect(gHub, &TheHub::sigGeometryChanged, [this](){ render(); });
    connect(gHub, &TheHub::sigNormChanged, [this](){ render(); });
    connect(gHub, &TheHub::sigSuiteSelected, [this](shp_Suite dataseq){ setSuite(dataseq); });

    render();
}

QPixmap SubframeImage::makeBlankPixmap() {
    const size2d size = gSession->imageSize();

    QPixmap pixmap(to_i(size.w), to_i(size.h));
    pixmap.fill(QColor(0, 0, 0, 0));

    return pixmap;
}

QImage SubframeImage::makeImage(shp_Image image, bool curvedScale) {
    QImage im;
    if (!image)
        return im;

    shp_ImageLens imageLens = gSession->imageLens(*image, true, false);
    const size2d size = imageLens->size();
    if (size.isEmpty())
        return im;

    im = QImage(QSize(to_i(size.w), to_i(size.h)), QImage::Format_RGB32);

    const Range rgeInten = imageLens->rgeInten(gHub->isFixedIntenImageScale());
    inten_t maxInten = inten_t(rgeInten.max);

    for_ij (size.w, size.h)
        im.setPixel(to_i(i), to_i(j),
                    colormap::intenImage(imageLens->imageInten(i, j), maxInten, curvedScale));
    return im;
}

QPixmap SubframeImage::makePixmap(shp_Image image) {
    return QPixmap::fromImage(makeImage(image, !gHub->isFixedIntenImageScale()));
}

QPixmap SubframeImage::makePixmap(
    Measurement const& dataseq, const Range& rgeGma, const Range& rgeTth) {
    QImage im = makeImage(dataseq.image(), !gHub->isFixedIntenImageScale());
    shp_AngleMap angleMap = gSession->angleMap(dataseq);

    const QSize& size = im.size();
    for_ij (size.width(), size.height()) {
        ScatterDirection const& a = angleMap->at(to_u(i), to_u(j));
        QColor color = im.pixel(i, j);
        if (rgeGma.contains(a.gma)) {
            if (rgeTth.contains(a.tth)) {
                color = Qt::yellow;
            } else {
                color.setGreen(qFloor(color.green() * .3 + 255 * .7));
            }
        } else if (rgeTth.contains(a.tth)) {
            color.setGreen(qFloor(color.green() * .3 + 255 * .7));
        }
        im.setPixel(i, j, color.rgb());
    }

    return QPixmap::fromImage(im);
}

void SubframeImage::setSuite(shp_Suite dataseq) {
    dataseq_ = dataseq;
    render();
}

void SubframeImage::render() {
    {
        QPixmap pixMap;

        const uint nSlices = to_u(numSlices_->value());
        numSlice_->setMaximum(qMax(1, to_i(nSlices)));
        numSlice_->setEnabled(nSlices > 0);

        if (dataseq_) {
            // 1 - based
            const uint by = qBound(1u, uint(gHub->suiteGroupedBy()), dataseq_->count());
            const uint n = qBound(1u, to_u(spinN_->value()), by);

            spinN_->setValue(to_i(n));
            spinN_->setEnabled(by > 1);

            lens_ = gSession->defaultDatasetLens(*dataseq_);

            Range rge;
            if (nSlices > 0) {
                uint nSlice = qMax(1u, to_u(numSlice_->value()));
                uint iSlice = nSlice - 1;

                const Range rgeGma = lens_->rgeGma();
                const qreal min = rgeGma.min;
                const qreal wn = rgeGma.width() / nSlices;

                rge = Range(min + iSlice * wn, min + (iSlice + 1) * wn);

                minGamma_->setValue(rge.min);
                maxGamma_->setValue(rge.max);
            } else {
                rge = Range::infinite();
                minGamma_->clear();
                maxGamma_->clear();
            }

            gHub->setGammaRange(rge);

            shp_Measurement measurement = dataseq_->at(n - 1);

            numBin_->setEnabled(true);
            if (gHub->toggle_showBins->isChecked()) {
                Range rgeTth = lens_->rgeTth();
                int count = to_i(lens_->makeCurve().count());
                numBin_->setMaximum(count - 1);
                int min = rgeTth.min, wdt = rgeTth.width();
                qreal num = qreal(numBin_->value());
                pixMap = makePixmap(
                    *measurement, rge,
                    Range(min + wdt * (num / count), min + wdt * ((num + 1) / count)));
            } else {
                pixMap = makePixmap(measurement->image());
            }
        } else {
            spinN_->setEnabled(false);
            numBin_->setMaximum(0);
            numBin_->setEnabled(false);

            pixMap = makeBlankPixmap();
        }

        dataImageWidget_->setPixmap(pixMap);
    }

    {
        QPixmap pixMap = makePixmap(gSession->corrImage());
        corrImageWidget_->setPixmap(pixMap);
    }
}
