//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/pars/image_transform.h
//! @brief     Defines class ImageTransform
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef IMAGE_TRANSFORM_H
#define IMAGE_TRANSFORM_H

//! Encodes chosen rotation and mirror operations

class ImageTransform {
public:
    ImageTransform() {}
    void doRotate() { rotation = (rotation+1)%4; }
    bool isTransposed() const { return rotation & 1; }

    bool mirror {false};
    int rotation {0};
};

#endif // IMAGE_TRANSFORM_H
