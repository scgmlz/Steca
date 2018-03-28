// ************************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/theta_selection.h
//! @brief     Defines ThetaSelection
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************************************

#ifndef THETA_SELECTION_H
#define THETA_SELECTION_H


//! Select a theta bin for overlay in scattering image.

class ThetaSelection : public QObject {
public:
    ThetaSelection();

    void fromJson(const JsonObj& obj);
    void onData();
    void recomputeCache();

    void selectSlice(int);

    const Range& range() const { return range_; }
    int iSlice() const { return iSlice_; }
    bool isActive() const { return numSlices_; }
    QJsonObject toJson() const;

private:
    Range fullRange_;
    Range range_;
    int numSlices_;
    int iSlice_ {0};
};

#endif // THETA_SELECTION_H
