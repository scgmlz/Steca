//***************************************************************************************************
//
//  STeCa: the strain and texture calculator
//
//! @file      Core/ReadData.h
//! @brief     ..
//!
//! @homepage  @HOMEPAGE@
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright TU Clausthal-Zellerfeld 2012, Forschungszentrum Jülich GmbH 2015
//! @authors   Christian Randau, Joachim Wuttke
//! @authors   Scientific Computing Group at MLZ Garching
//
//***************************************************************************************************

#include <vector>
#include "TStandardTypedef.h"
#include "Setup.h"
class Dataset;
class Measurement;

namespace ReadData {
string readCommentCaress(string path);
string readDateCaress(string path);
string readTimeCaress(string path);
void readDatasetCaress(string path, Setup& setup, std::vector<Dataset*>& vectorDataset,
                       bool calcDiff, bool rebindTo256 = false);

string readCommentMDLL(string path);
string readDateMDLL(string path);
string readTimeMDLL(string path);
void readDatasetMDLL_StressSpec(string path, Setup& setup, std::vector<Dataset*>& vectorDataset,
                                bool rebindTo256 = false);

string readCommentXmlGkssTex2(string path);
string readDateXmlGkssTex2(string path);
string readTimeXmlGkssTex2(string path);

string readCommentMar(string path);
string readDateMar(string path);
string readTimeMar(string path);
void readDatasetMar(const Measurement& measurement, string path, Setup& setup,
                    std::vector<Dataset*>& vectorDataset);

string readCommentASCI(string path);
string readDateASCI(string path);
string readTimeASCI(string path);
void readDatasetASCI(string path, Setup& setup, std::vector<Dataset*>& vectorDataset,
                     bool calcDiff);

string readCommentASCII_Wang(string path);
void readDatasetASCII_Wang(string path, Setup& setup, std::vector<Dataset*>& vectorDataset,
                           bool calcDiff);

void readDatasetTif(const Measurement& measurement, string path, Setup& setup,
                    std::vector<Dataset*>& vectorDataset);

void readDatasetInspire(string path, Setup& setup, std::vector<Dataset*>& vectorDataset);
void readDatasetXmlGkssTex2(string path, Setup& setup, std::vector<Dataset*>& vectorDataset);
void readDatasetXmlMira(string path, Setup& setup, std::vector<Dataset*>& vectorDataset);
}
