// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/output/output_polefigures.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef OUTPUT_POLEFIGURES_H
#define OUTPUT_POLEFIGURES_H

#include "output_dialogs.h"
#include "typ/angles.h"
#include "typ/str.h"

namespace gui {
namespace output {

class TabGraph : public Tab {
private:
    using super = Tab;
public:
    using deg = typ::deg;
    using rad = typ::rad;

    TabGraph(TheHub&, Params&);
    void set(calc::ReflectionInfos);

protected:
    void update();

    calc::ReflectionInfos rs_;
    void paintEvent(QPaintEvent*);

    QPointF p(deg alpha, deg beta) const;
    deg alpha(QPointF const&) const;
    deg beta(QPointF const&) const;

    void circle(QPointF c, qreal r);

    void paintGrid();
    void paintPoints();

    // valid during paintEvent
    QPainter* p_;
    QPointF c_;
    qreal r_;

    bool flat_;
    qreal alphaMax_, avgAlphaMax_;

    QCheckBox* cbFlat_;
};

class TabPoleFiguresSave : public TabSave {
private:
    using super = TabSave;
public:
    TabPoleFiguresSave(TheHub& hub, Params& params);

    bool onlySelectedRefl() const;
    bool outputInten() const;
    bool outputTth() const;
    bool outputFWHM() const;

    void rawReflSettings(bool on);

protected:
    QRadioButton *rbSelectedRefl_, *rbAllRefls_;
    QCheckBox *outputInten_, *outputTth_, *outputFWHM_;
};

class PoleFiguresFrame : public Frame {
private:
    using super = Frame;
public:
    PoleFiguresFrame(TheHub&, rcstr title, QWidget*);

protected:
    TabGraph* tabGraph_;
    TabPoleFiguresSave* tabSave_;

    void displayReflection(uint reflIndex, bool interpolated);

    bool savePoleFigureOutput();
    bool writePoleFigureOutputFiles(rcstr filePath, uint index);
    void writePoleFile(rcstr filePath, calc::ReflectionInfos, qreal_vec const&);
    void writeListFile(rcstr filePath, calc::ReflectionInfos, qreal_vec const&);
    void writeErrorMask(rcstr filePath, calc::ReflectionInfos, qreal_vec const&);
};
}
}
#endif // OUTPUT_POLEFIGURES_H
