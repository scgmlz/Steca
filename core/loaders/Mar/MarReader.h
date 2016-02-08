/*
 *	MS Visual C++ definition for DLL's
 */
#if defined(_MSC_VER)
#include <windows.h>
#if defined(MARREADER_LIB)
#define MARREADER_DLL __declspec(dllexport)
#else
#define MARREADER_DLL __declspec(dllimport)
#endif
#else
#define MARREADER_DLL
#endif

#include "mar300_header.h"
#include "mar345_header.h"

#if defined(c_plusplus) || defined(__cplusplus)
extern "C" {
#endif

extern MARREADER_DLL void get_pck(FILE*, short int*);
extern MARREADER_DLL void swapint64(unsigned char*, int);
extern MARREADER_DLL void swapint32(unsigned char*, int);
extern MARREADER_DLL void swapint16(unsigned char*, int);
extern MARREADER_DLL void swapfloat(float*, int);
extern MARREADER_DLL int InputType(char*);
extern MARREADER_DLL void WrongType(int, char*, char*);
extern MARREADER_DLL float GetResol(float, float, float);
extern MARREADER_DLL float Resol2twotheta(float, float);
extern MARREADER_DLL float Resol2stol(float, float);
extern MARREADER_DLL void RemoveBlanks(char*);
extern MARREADER_DLL int SplitmarName(char*, char*, char*, int*);
extern MARREADER_DLL MAR345_HEADER Getmar345Header(FILE*);
extern MARREADER_DLL MAR300_HEADER Getmar300Header(FILE*);

#if defined(c_plusplus) || defined(__cplusplus)
}
#endif