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
#include "core/session.h"
#include "gui/cfg/colors.h"
#include "gui/thehub.h"
#include "gui/base/various_widgets.h"
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

    connect(gHub->toggle_showOverlay, &QAction::toggled, [this](bool /*unused*/) { update(); });
    connect(gHub->toggle_stepScale, &QAction::toggled, [this](bool /*unused*/) { setScale(); });
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
        hb->addWidget((minGamma_ = newQ::DoubleSpinBox(6, true)));
        hb->addWidget(newQ::Label("max"));
        hb->addWidget((maxGamma_ = newQ::DoubleSpinBox(6, true)));

        minGamma_->setReadOnly(true);
        maxGamma_->setReadOnly(true);

        hb->addWidget(newQ::Label("bin#"));
        hb->addWidget((numBin_ = newQ::SpinBox(4, false, 1)));

        box.addWidget((dataImageWidget_ = new ImageWidget()));

        connect(spinN_, _SLOT_(QSpinBox, valueChanged, int),
                [this](int /*unused*/) { render(); });
        connect(numSlices_, _SLOT_(QSpinBox, valueChanged, int),
                [this](int /*unused*/) { render(); });
        connect(numSlice_, _SLOT_(QSpinBox, valueChanged, int),
                [this](int /*unused*/) { render(); });
        connect(numBin_, _SLOT_(QSpinBox, valueChanged, int),
                [this](int /*unused*/) { render(); });
    }

    {
        BoxWidget& tab = *newQ::Tab(this, "Correction");

        connect(gHub, &TheHub::sigCorrFile, [&tab](const Rawfile* file) { tab.setEnabled(file); });

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

    connect(gHub->toggle_enableCorr, &QAction::toggled, [this](bool /*unused*/) { render(); });
    connect(gHub->toggle_showBins, &QAction::toggled, [this](bool /*unused*/) { render(); });

    connect(gHub, &TheHub::sigDisplayChanged, this, &SubframeImage::render);
    connect(gHub, &TheHub::sigGeometryChanged, this, &SubframeImage::render);
    connect(gHub, &TheHub::sigNormChanged, this, &SubframeImage::render);
    connect(gHub, &TheHub::sigClusterHighlight, this, &SubframeImage::setCluster);

    render();
}

QPixmap SubframeImage::makeBlankPixmap() {
    const size2d size = gSession->imageSize();
    QPixmap pixmap(size.w, size.h);
    pixmap.fill(QColor(0, 0, 0, 0));
    return pixmap;
}

QImage SubframeImage::makeImage(shp_Image image, bool curvedScale) {
    if (!image)
        return {};

    shp_ImageLens imageLens = gSession->imageLens(*image, true, false);
    const size2d size = imageLens->size();
    if (size.isEmpty())
        return {};

    QImage ret(QSize(size.w, size.h), QImage::Format_RGB32);

    const Range rgeInten = imageLens->rgeInten(gHub->isFixedIntenImageScale());
    inten_t maxInten = inten_t(rgeInten.max);

    for_ij (size.w, size.h)
        ret.setPixel(i, j,
                    colormap::intenImage(imageLens->imageInten(i, j), maxInten, curvedScale));
    return ret;
}

QPixmap SubframeImage::makePixmap(shp_Image image) {
    return QPixmap::fromImage(makeImage(image, !gHub->isFixedIntenImageScale()));
}

QPixmap SubframeImage::makePixmap(
    Measurement const& cluster, const Range& rgeGma, const Range& rgeTth) {
    QImage im = makeImage(cluster.image(), !gHub->isFixedIntenImageScale());
    shp_AngleMap angleMap = gSession->angleMap(cluster);

    const QSize& size = im.size();
    for_ij (size.width(), size.height()) {
        ScatterDirection const& a = angleMap->at(i, j);
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

void SubframeImage::setCluster(const Cluster* cluster) {
    cluster_ = cluster;
    render();
}

void SubframeImage::render() {
    {
        QPixmap pixMap;

        const int nSlices = numSlices_->value();
        numSlice_->setMaximum(qMax(1, nSlices));
        numSlice_->setEnabled(nSlices > 0);

        if (cluster_) {
            // 1 - based
            const int by = qBound(1, int(gHub->clusterGroupedBy()), cluster_->count());
            const int n = qBound(1, spinN_->value(), by);

            spinN_->setValue(n);
            spinN_->setEnabled(by > 1);

            lens_ = gSession->defaultClusterLens(*cluster_);

            Range rge;
            if (nSlices > 0) {
                int nSlice = qMax(1, numSlice_->value());
                int iSlice = nSlice - 1;

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

            shp_Measurement measurement = cluster_->at(n - 1);

            numBin_->setEnabled(true);
            if (gHub->toggle_showBins->isChecked()) {
                Range rgeTth = lens_->rgeTth();
                int count = lens_->makeCurve().count();
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
