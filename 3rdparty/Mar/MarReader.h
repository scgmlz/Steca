/*
 *	MS Visual C++ definition for DLL's
 */
//#if defined(_MSC_VER)
//#include <windows.h>
//#if defined(MARREADER_LIB)
//#define MARREADER_DLL __declspec(dllexport)
//#else
//#define MARREADER_DLL __declspec(dllimport)
//#endif
//#else
//#define MARREADER_DLL
//#endif

#include "mar300_header.h"
#include "mar345_header.h"

//#if defined(c_plusplus) || defined(__cplusplus)
//extern "C" {
//#endif

void get_pck(FILE*, short int*);
void swapint64(unsigned char*, int);
void swapint32(unsigned char*, int);
void swapint16(unsigned char*, int);
void swapfloat(float*, int);
int InputType(char*);
void WrongType(int, char*, char*);
float GetResol(float, float, float);
float Resol2twotheta(float, float);
float Resol2stol(float, float);
void RemoveBlanks(char*);
int SplitmarName(char*, char*, char*, int*);
MAR345_HEADER Getmar345Header(FILE*);
MAR300_HEADER Getmar300Header(FILE*);

//#if defined(c_plusplus) || defined(__cplusplus)
//}
//#endif
