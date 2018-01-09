// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/data/image_transform.h
//! @brief     Defines class ImageTransform
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef IMAGE_TRANSFORM_H
#define IMAGE_TRANSFORM_H

#include "core/def/numbers.h" // for uint

//! Encodes chosen rotation and mirror operations

class ImageTransform {
public:
    enum eTransform {
        ROTATE_0 = 0, // no transform
        ROTATE_1 = 1, // one quarter
        ROTATE_2 = 2, // two quarters
        ROTATE_3 = 3, // three quarters
        MIRROR = 4,
        MIRROR_ROTATE_0 = MIRROR | ROTATE_0,
        MIRROR_ROTATE_1 = MIRROR | ROTATE_1,
        MIRROR_ROTATE_2 = MIRROR | ROTATE_2,
        MIRROR_ROTATE_3 = MIRROR | ROTATE_3,
    } val;

    // clamps val appropriately
    ImageTransform(uint val = ROTATE_0);

    // adds/removes the mirror flag
    ImageTransform mirror(bool on) const;

    // rotates only; keeps the mirror flag
    ImageTransform rotateTo(ImageTransform const&) const;

    // rotates by one quarter-turn
    ImageTransform nextRotate() const;

    bool isTransposed() const { return 0 != (val & 1); }

    bool operator==(ImageTransform const& that) const { return val == that.val; }
};

#endif // IMAGE_TRANSFORM_H
