/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

caressglobals.h

Global definitions like command_ids, module_ids, types, etc.

file:           caressglobals.h
author:         K.H. Degenhardt

Some symbolic constants are obsolete. Do not delete these constants until
all program modules using these constants have deleted it.

update history:
        [12/07/2004] dfm. insert device LS_340
        [...]
        [20/Nov/2008] dcy: new command DETINFO (channel count, pixel width of detectors)

------------------------------------------------------------------------*/

#ifndef CAR_GLOBALS_H
#define CAR_GLOBALS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "car_datatypes.h"

#if defined(_MSC_VER) && !defined(WINVER)
// Visual Studio no longer defines WINVER
#define WINVER
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define ARRAY_SIZE(x) ((int32)(sizeof(x) / sizeof((x)[0])))

#ifdef TRUE
#undef TRUE
#endif
#ifdef FALSE
#undef FALSE
#endif

#define FALSE 0
#define TRUE 1

/*> actions ids */
#define READACTION 1    /* read info from hardware and print it */
#define LOADACTION 2    /* load hardware component with info */
#define ENABLELAM 3     /* obsolete */
#define CLEARLAM 4      /* obsolete */
#define GENERATION 5    /* CARESS internal command generation */
#define HARDWAREINIT 6  /* hardware initialization */
#define SETACTION 7     /* drive to a target position */
#define COMPARE 8       /* compare target with requested position */
#define LASTCOMPARE 9   /* obsolete */
#define READSTORE 10    /* read info from hardware, store into file */
#define KEEPACTION 11   /* Y66 motor should keep current position */
#define STOPACTION 12   /* stop a hardware activity */
#define LOADANDSTART 13 /* obsolete */
#define LOADMASTER 14   /* load preset counter with preset value */
#define LOADSLAVE 15    /* prepare a counter, histogram for counting */
#define RESETMODULE 16  /* clear a counter, histogram */
#define LOADMASTER1 LOADMASTER
#define LOADMASTER2 17    /* obsolete */
#define SPECIALLOAD 18    /* load special info into hardware */
#define LOAD_PERM_INFO 19 /* load permanent moving info */
#define READACTION2 20    /* read info from hardware and print it */

/*> command ids (cf. command description) */
#define LASTCOMMAND 0           /* internal, no string: program termination */
#define USN 1                   /* user name */
#define DAT 3                   /* obsolete */
#define DEV 4                   /* obsolete */
#define DISP 5                  /* plot refresh time */
#define BLS 6                   /* moving range */
#define SOF 7                   /* position offset */
#define TOL 8                   /* positioning tolerance */
#define WAV 9                   /* wave length */
#define WIND 10                 /* 1-dim. detector windows (software) */
#define UBM 11                  /* UB matrix */
#define TMO 12                  /* move action timeout */
#define MSH 13                  /* HKL conversion methode */
#define CALIB 14                /* multi detector channel correction */
#define CONV 15                 /* obsolete */
#define OFL 16                  /* set offline */
#define PASS 17                 /* do not move "calculated" component */
#define PRO 18                  /* protocol components */
#define MM1 19                  /* preset counter and value */
#define MM2 20                  /* obsolete */
#define SL1 21                  /* "slave" components */
#define SL2 22                  /* obsolete */
#define COM 23                  /* comment */
#define RANGE 24                /* obsolete */
#define PRT 25                  /* protocol ON/OFF, ... */
#define PRW 26                  /* windows for protocol */
#define PEAK 27                 /* peak search definition */
#define RELA 28                 /* multi detector parameters */
#define DS 29                   /* D space */
#define SS 30                   /* scattering sense */
#define FX 31                   /* fix incidet/final wave vector */
#define EU 32                   /* energy unit */
#define CHECK 33                /* check commands for error */
#define LAT 34                  /* lattice parameters */
#define SCH 35                  /* obsolete */
#define SPEC 36                 /* desired plot axes */
#define EXPTYPE 37              /* instrument selection info */
#define WINDA 38                /* area detector windows (software) */
#define PRWA 39                 /* area detector windows for protocol */
#define WAIT1 40                /* wait time befor start of scan */
#define WAIT2 41                /* wait time after a positioning */
#define DEFON 42                /* "ON" position definition */
#define ACV 43                  /* obsolete */
#define DEFOFF 44               /* "OFF" position definition */
#define DEFTURN 45              /* 3D polarization definition */
#define HIGHESTSTATICCOMMAND 45 /* obsolete */

#define CALCHKL 55   /* calculate HKL from angles */
#define DETPART 57   /* partional detector positioning cycle */
#define DETSTEP 58   /* detector positioning cycle */
#define RESOPART 59  /* partional resolution cycle */
#define DEFSAM 60    /* list of sample info for sample changer */
#define CHGLPOS 61   /* change a list position info */
#define PERMOVE 62   /* define permanent movement during data acq. */
#define ACQPMOVE 62  /* define permanent movement during data acq. */
#define MESR 63      /* internal,no string: resolution acquisition */
#define RESOSTEP 64  /* resolution cycle */
#define DEFLPOS 65   /* define list position */
#define PASSW 66     /* password */
#define SAM 67       /* sample name [,further info] */
#define DEFINE 68    /* definition of special positions */
#define CONFIG 69    /* DAU configuration */
#define DEFPOS 70    /* definition of sample changer positions */
#define TITLE 71     /* title */
#define SCANV 72     /* definition of suppl. scan components */
#define WAITC 73     /* string 'WAIT', unconditional wait */
#define CHOPPER 74   /* chopper definition */
#define TOF 76       /* DAU TOF definition */
#define SELDET 77    /* selection of single channel of a multi det.*/
#define MANUAL 78    /* set Y66 motor to manual mode */
#define FILEC 79     /* internal,string 'FILE' */
#define ONL 80       /* set online */
#define DATEC 85     /* internal,string 'DATE' */
#define TIMEC 86     /* internal,string 'TIME' */
#define POLC 87      /* internal, no string: polarization command */
#define POL 88       /* set polarization OFF?ON */
#define POLB 89      /* start of polarization cycle */
#define POLE 90      /* end of polarization cycle */
#define WAITTRIG 91  /* wait for trigger (V4), formerly MESSV */
#define POLV 92      /* internal, no string: polarization positions*/
#define SETV 93      /* internal, no string: scan positions */
#define SHOW 94      /* print a command string */
#define KILC 95      /* kill (abort) internal command generation */
#define KILD 96      /* kill (abort) data storage */
#define KILS 97      /* kill (abort) current action */
#define CONC 98      /* string 'CON',continitue interrupted action */
#define HLT 99       /* internal, no string */
#define SETC 100     /* string 'SET', move components to target */
#define READC 102    /* string 'READ', read and print current pos. */
#define MES 103      /* do a data acquisition */
#define SGEN 104     /* scan around a central position */
#define SCANB 107    /* start of a "free" scan */
#define SCANE 108    /* end of a "free" scan */
#define LOWL 109     /* obsolete */
#define CALC 110     /* calculate angles from HKLE... */
#define MES1 111     /* internal, no string */
#define REF 112      /* drive to reference position */
#define REFSET 113   /* drive to reference position and set new position */
#define CEN2 114     /* obsolete */
#define CENB 115     /* obsolete */
#define CENE 116     /* obsolete */
#define SETCON 117   /* drive but do not wait for "done" */
#define CENPAR 118   /* obsolete */
#define SGEN1 119    /* scan from a start to an end position */
#define EXTC 123     /* string 'EXTERN',switch to external process */
#define INTC 124     /* string 'INTERN',return from extern process */
#define CALLC 125    /* string 'CALL', get info from CARESS */
#define DCL 126      /* obsolete */
#define NUMOR 127    /* internal */
#define DEFNAME 128  /* list of names and ids of user defined comp.*/
#define BYPASS 129   /* force data acquisition */
#define SET_READ 130 /* internal, set read positions (data file */
#define RESO_POS 131 /* internal, resolution position (data file) */
#define LET 132      /* define a numerical variable */
#define IFC 133      /* string 'IF', if statement in PARF file */
#define PARF 134     /* definition of "parameter" file */
#define REPEATC 135  /* string 'REPEAT', start of a repeat loop */
#define ENDR 136     /* end of a repeat loop */
#define PROC 137     /* start of a procedure definition */
#define ENDP 138     /* end of a procedure definition */
#define LIST 139     /* print list of defined numerical variables */
#define DOC 140      /* string 'DO', call a procedure */
#define KILP 141     /* kill (abort) procedure execution */
#define EXITC 142    /* string 'EXIT', terminate CARESS */
#define LOADTEXT 143 /* load string for special device, e.g. ADET */
#define INITH 200    /* initialize hardware */
#define INITS 201    /* initialize software */
#define RESETC 202   /* string 'RESET', reset a command */
#define LOGDEV 203
#define LOGDAT 204
#define PMOVE 205     /* start permanent movement */
#define READGPIB 206  /* read a string from gpib-device */
#define WRITEGPIB 207 /* write a string to gpib-device */

#define CURPOS 254  /* internal, no string: all current positions */
#define CURRENT 255 /* obsolete */

/*> command ids used internally, only  */
#define COMPOUNDEMPTY 300        /* command generation (from scan) terminated */
#define COMPOUND 301             /* command generation active */
#define TRIGGER 302              /* trigger to get next generated command */
#define MANUALCOMPUTERSWITCH 303 /* obsolete */
#define MESACQUISITION 304       /* start data acquisition*/
#define MESSTORAGE 305           /* store measured data */
#define MESTAIL 306              /* do "last" action of a data acquisition */
#define SIMPLEMES 307            /* MES command is not part of a scan */
#define DESIREOFMESTAIL 308      /* measur desire MESTAIL as next command */
#define DESIREOFMES 309          /* measur desires MES as next command */
#define NEXTCOMMAND 310          /* requests next command */
#define MESSTORAGE_SAVE 311      /* data storage for save (security) purpose */
#define KERROR 320               /* error info */
#define RESULT 321               /* action result info */
#define OPENF 330                /* open data file for storage */
#define OPENC OPENF
#define STORE 332 /* store from data20_xx.dat into data file */
#define STORE1 STORE
#define STORE2 333 /*   "   */
#define STORE3 334 /*   "   */
#define UPD 333    /*   "   */
#define LIV 334    /* obsolete */
#define OPENTF 335 /* open scratch data file */
#define OPENT OPENTF
#define CLOSEF 336          /* close data file */
#define TASKMESSAGE 400     /* a general message of a CARESS process */
#define NONSENSECOMMAND 999 /* obsolete */
#define SHOWCODE 10000      /* obsolete */

/*> module ids */
#define OMGS 1       /* omega sample */
#define TTHS 2       /* 2theta sample */
#define PHIS 3       /* phi sample (eulerian cradle) */
#define CHIS 4       /* chi sample (eulerian cradle) */
#define NYS 5        /* vertical detetor position */
#define OMGM 6       /* omega monochromator */
#define TTHM 7       /* 2theta monochromator */
#define OMGX 8       /* special omega auxiliary sample (E1) */
#define OMGA 9       /* omega analyzator */
#define TTHA 10      /* 2theat analyzator */
#define ANA1 11      /* special motor to position OMGA (E2) */
#define ANA2 12      /* special motor to position OMGA (E2) */
#define ANA3 13      /* special motor to position OMGA (E2) */
#define ANA4 14      /* special motor to position OMGA (E2) */
#define TEMP 15      /* temperature */
#define MAGF 16      /* magnetic field */
#define PRES 17      /* pressure */
#define MON 18       /* monitor counter */
#define TIM1 19      /* timer counter */
#define TIM2 20      /* time counter */
#define LDET 21      /* 1-dim multi detector */
#define SDET 22      /* single detector */
#define H 23         /* reciprocal space coordinate (compound) */
#define K 24         /* reciprocal space coordinate (compound) */
#define L 25         /* reciprocal space coordinate (compound) */
#define E 26         /* energy loss or gain (compound) */
#define YSM 27       /* distance sample <-> monochromator */
#define KI 28        /* incident wave vector (compound) */
#define EI 29        /* incident energy (compound) */
#define KF 30        /* final wave vector (compound) */
#define EF 31        /* final energy (compund) */
#define RATE 32      /* obsolete */
#define MB1 33       /* monochromator bending */
#define MB2 34       /* monochromator bending */
#define COLL 36      /* collimator */
#define GF 37        /* obsolete */
#define X 38         /* translation */
#define Y 39         /* translation */
#define Z 40         /* translation */
#define VF 41        /* vertical field (compound) */
#define HF 42        /* horizontal field (compound) */
#define FL1 43       /* flipper (compound) */
#define FL2 44       /* flipper (compound) */
#define WI 45        /* incident wave length (compound) */
#define WF 46        /* final wave length (compound) */
#define PSI 47       /* psi */
#define MOT 48       /* a motor */
#define TTH0 49      /* obsolete */
#define COU 50       /* a counter */
#define POS 51       /* sample changer position (compound) */
#define ADET 52      /* area detector */
#define DOMGS 53     /* "delta" OMGS */
#define DTTHS 54     /* "delta" TTHS */
#define DPHIS 55     /* "delta" PHIS */
#define DCHIS 56     /* "delta" CHIS */
#define DNYS 57      /* "delta" NYS */
#define DOMGM 58     /* "delta" OMGM */
#define DTTHM 59     /* "delta" TTHM */
#define DOMGX 60     /* "delta" OMGX */
#define DOMGA 61     /* "delta" OMGA */
#define DTTHA 62     /* "delta" TTHA */
#define DANA1 63     /* "delta" ANA1 */
#define DANA2 64     /* "delta" ANA2 */
#define DANA3 65     /* "delta" ANA3 */
#define DANA4 66     /* "delta" ANA4 */
#define DTEMP 67     /* "delta" TEMP */
#define DMAGF 68     /* "delta" MAGF */
#define DPRES 69     /* "delta" PRES */
#define DH 70        /* "delta" H */
#define DK 71        /* "delta" K */
#define DL 72        /* "delta" L */
#define DE 73        /* "delta" E */
#define DYSM 74      /* "delta" YSM */
#define DMB1 75      /* "delta" MB1 */
#define DMB2 76      /* "delta" MB2 */
#define DX 77        /* "delta" X */
#define DY 78        /* "delta" Y */
#define DZ 79        /* "delta" Z */
#define DKI 80       /* "delta" KI */
#define DEI 81       /* "delta" EI */
#define DKF 82       /* "delta" KF */
#define DEF 83       /* "delta" EF */
#define DWI 84       /* "delta" WI */
#define DWF 85       /* "delta" WF */
#define DPSI 86      /* "delta" PSI */
#define DMOT 87      /* "delta" MOT */
#define DTTH0 88     /* obsolete */
#define STEP 89      /* scan step */
#define W1 90        /* histogram window */
#define W2 91        /* histogram window */
#define W3 92        /* histogram window */
#define W4 93        /* histogram window */
#define GPIB 94      /* GPIB controller */
#define MUX 95       /* motor multiplex */
#define INTR 96      /* obsolete */
#define INREG 97     /* digital IN register */
#define OUTREG 98    /* digitat OUT register */
#define PHI 99       /* phi */
#define DPHI 100     /* "delta" PHI */
#define XT 101       /* translation */
#define YT 102       /* translation*/
#define ZT 103       /* translation */
#define DXT 104      /* "delta" XT */
#define DYT 105      /* "delta" YT */
#define DZT 106      /* "delta" ZT */
#define ERRMOD 107   /* "error" module */
#define DECL 108     /* area detector declination (compound)*/
#define YSD 109      /* distance sample <-> detector */
#define HGHT 110     /* vertical position of detector */
#define HFI1 111     /* horizontal field power supply */
#define HFI2 112     /* horizontal field power supply */
#define HFI3 113     /* horizontal field power supply */
#define VFI1 114     /* vertical field power supply */
#define FL1I1 115    /* flipper1 power supply */
#define FL1I2 116    /* flipper1 power supply */
#define FL2I1 117    /* flipper2 power supply */
#define FL2I2 118    /* flipper2 power supply */
#define PSIS 119     /* psis (E1) */
#define DPSIS 120    /* "delta" PSIS */
#define DHFI1 121    /* "delta" HFI1 */
#define DHFI2 122    /* "delta" HFI2 */
#define DHFI3 123    /* "delta" HFI3 */
#define DVFI1 124    /* "delta" VFI1 */
#define DFL1I1 125   /* "delta" FL1I1 */
#define DFL1I2 126   /* "delta" FL1I2 */
#define DFL2I1 127   /* "delta" FL2I1 */
#define DFL2I2 128   /* "delta" FL2I2 */
#define SI 129       /* dau single detector info */
#define MU 130       /* dau multi detector info */
#define TB 131       /* DAU TOF: time base */
#define CN 132       /* DAU TOF: number of time channels */
#define CW 133       /* DAU TOF: time width of a channel */
#define D1 134       /* selected channel [region] of a multi detector */
#define D2 135       /* selected channel [region] of a multi detector */
#define D3 136       /* selected channel [region] of a multi detector */
#define D4 137       /* selected channel [region] of a multi detector */
#define DT 138       /* DAU TOF: delay time */
#define T_SET 139    /* set (target) temperature */
#define T_REG 140    /* regulation temperature */
#define T_SAM 141    /* sample temperature */
#define T_DELTA 142  /* temperature deviation from mean temperature */
#define T_MEAN 143   /* mean temperature */
#define CHOP 144     /* chopper (V5) */
#define DMM 145      /* digital multimeter */
#define STATEC 147   /* chopper: state (V5) */
#define SPEEDC 148   /* copper: speed (V5) */
#define PHASEC 149   /* chopper: phase (V5) */
#define RATIOC 150   /* chopper: velocity ratio (V5) */
#define LAMBDA 151   /* lambda */
#define DLAMBDA 152  /* "delta" LAMBDA */
#define ATT 153      /* attenuator (V4) */
#define BC 154       /* beamstop changer (compound) (V4) */
#define BSX 155      /* beamstop x translation (V4) */
#define BSY 156      /* beamstop y translation (V4) */
#define SD 157       /* distance sample <-> detector */
#define SY 158       /* sample?? vertical position */
#define POSX 159     /* sample changer: x position */
#define POSY 160     /* sample changer: y position */
#define MOT2 161     /* a motor */
#define DMOT2 162    /* "delta" MOT2 */
#define SWITCH1 163  /* a switch */
#define SWITCH2 164  /* a switch */
#define VR 165       /* velocity selector: rpm */
#define VT 166       /* velocity selector tilt (V4) */
#define H0 167       /* magnetic field (V4) */
#define MON1 168     /* monitor counter */
#define MON2 169     /* monitor counter */
#define MON3 170     /* monitor counter */
#define DH0 171      /* "delta" H0 */
#define MOT1 172     /* a motor */
#define DMOT1 173    /* "delta" MOT1 */
#define DVR 174      /* "delta" VR */
#define DVT 175      /* "delta" VT */
#define BS 176       /* beamstop position (V4) */
#define LOCK 177     /* lock ?? */
#define IOREG 178    /* digital IO register */
#define TEMPR 179    /* temperature: read sample temperature, only */
#define POLA 180     /* polarizator (V4) */
#define MONOC 181    /* monochromator position */
#define MONOT 182    /* monochromator position */
#define COLL15T 183  /* collimator position (E2) */
#define COLL30T 184  /* collimator position (E2) */
#define DMONOT 185   /* "delta" MONOT */
#define DCOLL15T 186 /* "delta" COLL15T */
#define DCOLL30T 187 /* "delta" COLL30T */
#define B4 188       /* power supply (V5) */
#define B5 189       /* power supply (V5) */
#define DB4 190      /* "delta B4 */
#define DB5 191      /* "delta" B5" */
#define YIN 192      /* power supply (E1) */
#define ZIN 193      /* power supply (E1) */
#define YOUT 194     /* power supply (E1) */
#define ZOUT 195     /* power supply (E1) */
#define DYIN 196     /* "delta" YIN */
#define DZIN 197     /* "delta" ZIN */
#define DYOUT 198    /* "delta" YOUT */
#define DZOUT 199    /* "delta" ZOUT */
#define INCOIL 200   /* gradient coil IN (compound) (E1) */
#define OUTCOIL 201  /* gradient coil OUT (compound) (E1) */
#define SHUTTER 202  /* shutter */
#define PMATRIX 203  /* polarization matrix (compound) (E1) */
#define ANA5 204     /* special motor to position OMGA (E2) */
#define DANA5 205    /* "delta" ANA5 3D (E1) */
#define DMONOC 206   /* "delta" MONOC */
#define LPOS 207     /* list position (compound) */
#define FL 208       /* special flipper (V4) */
#define CHIM 209     /* chi monochromator */
#define DCHIM 210    /* "delta" CHIM */
#define XM 211       /* translation */
#define DXM 212      /* "delta" XM */
#define YM 213       /* translation */
#define DYM 214      /* "delta" Ym */
#define VBM 215      /* vertical bending monochromator */
#define DVBM 216     /* "delta" VBM */
#define DCOLL 217    /* "delta" COLL */
#define BEAM 218     /* beam (compound) */
#define ALPHA_B 219  /* ?? */
#define DALPHA_B 220 /* "delta" ALPHA_B */
#define DAU_SI 221   /* DAU: sum of single detectore counts */
#define DAU_MU 222   /* DAU: sum of multi detector counts */
#define T_SAM2 223   /* sample temperature (scan option) */
#define T_SAM3 224   /* sample temperature (scan option) */
#define T_SAM4 225   /* sample temperature (scan option) */
#define T_SAM5 226   /* sample temperature (scan option) */
#define DHGHT 227    /* "delta" HGHT */
#define WID1_B 228   /* slit width (compound) */
#define WID2_B 229   /* slit width (compound) */
#define HIGH1_B 230  /* slit height (compound) */
#define HIGH2_B 231  /* slit height (compound) */
#define DWID1_B 232  /* "delta" WID1_B */
#define DWID2_B 233  /* "delta" WID2_B */
#define DHIGH1_B 234 /* "delta" HIGH1_B */
#define DHIGH2_B 235 /* "delta" HIGH2_B */
#define DDECL 236    /* "delta" DECL */
#define SHUT_S 237   /* shutter */
#define PERIOD 238
/* H0_A and H0_T are only internally used; they are not available for CARESS
 * input (neither GUI nor cmdint) !!! */
#define H0_A 239 /* BRUKER power supply B-EC1 (electric current) */
#define H0_T 240 /* BRUKER power supply B-EC1 (magnetic field) */
#define MDET                                                                                       \
    241 /* multi detector, dummy(?) 'frame' device to encapsulate a number of 'classical' detector \
           devices like ADET, LDET or SDET */
#define ADET1 242    /* indexed ADET as part of a MDET */
#define ADET2 243    /* indexed ADET as part of a MDET */
#define ADET3 244    /* indexed ADET as part of a MDET */
#define ADET4 245    /* indexed ADET as part of a MDET */
#define TTH1 246     /* indexed 2theta sample as part of a MDET */
#define DTTH1 247    /* indexed "delta" 2theta sample as part of a MDET */
#define TTH2 248     /* indexed 2theta sample as part of a MDET */
#define DTTH2 249    /* indexed "delta" 2theta sample as part of a MDET */
#define TTH3 250     /* indexed 2theta sample as part of a MDET */
#define DTTH3 251    /* indexed "delta" 2theta sample as part of a MDET */
#define TTH4 252     /* indexed 2theta sample as part of a MDET */
#define DTTH4 253    /* indexed "delta" 2theta sample as part of a MDET */
#define COLL15 254   /* */
#define DCOLL15 255  /* */
#define COLL30 256   /* */
#define DCOLL30 257  /* */
#define MONTR 258    /* */
#define DMONTR 259   /* */
#define MONTI 260    /* */
#define DMONTI 261   /* */
#define HTRANS 262   /* horizontal translation of the detektor frame */
#define DHTRANS 263  /* delta of -"- */
#define MUTILT 264   /* tilting of */
#define DMUTILT 265  /* delta of -"- */
#define OMGR 266     /* */
#define DOMGR 267    /* */
#define IEEE1 268    /* 'generic' IEEE device to be used by the READGPIB */
#define IEEE2 269    /* 'generic' IEEE device to be used by the READGPIB */
#define IEEE3 270    /* 'generic' IEEE device to be used by the READGPIB */
#define IEEE4 271    /* 'generic' IEEE device to be used by the READGPIB */
#define IEEE5 272    /* 'generic' IEEE device to be used by the READGPIB */
#define IEEE6 273    /* 'generic' IEEE device to be used by the READGPIB */
#define IEEE7 274    /* 'generic' IEEE device to be used by the READGPIB */
#define IEEE8 275    /* 'generic' IEEE device to be used by the READGPIB */
#define FKS 276      /* fan ("faecher") collimator? near sample */
#define DFKS 277     /* "delta" FKS */
#define FKM 278      /* fan ("faecher") collimator? near monitor */
#define DFKM 279     /* "delta" FKM */
#define UP3_DI 280   /* slit (upper blade) */
#define DUP3_DI 281  /* "delta" UP3_DI */
#define DO3_DI 282   /* slit (lower blade) */
#define DDO3_DI 283  /* "delta" DO3_DI */
#define LE3_DI 284   /* slit (left blade) */
#define DLE3_DI 285  /* "delta" LE3_DI */
#define RI3_DI 286   /* slit (right blade) */
#define DRI3_DI 287  /* "delta" RI3_DI */
#define HIGH3_B 288  /* slit height (compound) */
#define DHIGH3_B 289 /* "delta" HIGH3_B */
#define WID3_B 290   /* slit width (compound) */
#define DWID3_B 291  /* "delta" WID3_B */
#define CHP1SPE 292  /* chopper 1 speed */
#define DCHP1SPE 293 /* "delta" CHP1SPE */
#define CHP2SPE 294  /* chopper 2 speed */
#define DCHP2SPE 295 /* "delta" CHP2SPE */
#define CHP1PHS 296  /* chopper 1 phase */
#define DCHP1PHS 297 /* "delta" CHP1PHS */
#define CHP2PHS 298  /* chopper 2 phase */
#define DCHP2PHS 299 /* "delta" CHP2PHS */
#define DCHP_PHS 300 /* difference of chopper phase */
#define CHP1DIR 301  /* chopper 1 direction */
#define CHP2DIR 302  /* chopper 2 direction */
#define ADET5 303    /* indexed ADET as part of a MDET */
#define ADET6 304    /* indexed ADET as part of a MDET */
#define ADET7 305    /* indexed ADET as part of a MDET */
#define ADET8 306    /* indexed ADET as part of a MDET */
#define ADET9 307    /* indexed ADET as part of a MDET */
#define TTH5 308     /* indexed 2theta sample as part of a MDET */
#define DTTH5 309    /* indexed "delta" 2theta sample as part of a MDET */
#define TTH6 310     /* indexed 2theta sample as part of a MDET */
#define DTTH6 311    /* indexed "delta" 2theta sample as part of a MDET */
#define TTH7 312     /* indexed 2theta sample as part of a MDET */
#define DTTH7 313    /* indexed "delta" 2theta sample as part of a MDET */
#define TTH8 314     /* indexed 2theta sample as part of a MDET */
#define DTTH8 315    /* indexed "delta" 2theta sample as part of a MDET */
#define TTH9 316     /* indexed 2theta sample as part of a MDET */
#define DTTH9 317    /* indexed "delta" 2theta sample as part of a MDET */
#define DYSD 318     /* indexed "delta" distance sample <-> detector */
#define YSD1 319     /* indexed distance sample <-> detector as part of MDET */
#define DYSD1 320    /* indexed "delta" distance sample <-> detector */
#define YSD2 321     /* indexed distance sample <-> detector as part of MDET */
#define DYSD2 322    /* indexed "delta" distance sample <-> detector */
#define YSD3 323     /* indexed distance sample <-> detector as part of MDET */
#define DYSD3 324    /* indexed "delta" distance sample <-> detector */
#define YSD4 325     /* indexed distance sample <-> detector as part of MDET */
#define DYSD4 326    /* indexed "delta" distance sample <-> detector */
#define YSD5 327     /* indexed distance sample <-> detector as part of MDET */
#define DYSD5 328    /* indexed "delta" distance sample <-> detector */
#define YSD6 329     /* indexed distance sample <-> detector as part of MDET */
#define DYSD6 330    /* indexed "delta" distance sample <-> detector */
#define YSD7 331     /* indexed distance sample <-> detector as part of MDET */
#define DYSD7 332    /* indexed "delta" distance sample <-> detector */
#define YSD8 333     /* indexed distance sample <-> detector as part of MDET */
#define DYSD8 334    /* indexed "delta" distance sample <-> detector */
#define YSD9 335     /* indexed distance sample <-> detector as part of MDET */
#define DYSD9 336    /* indexed "delta" distance sample <-> detector */

#define DUMMOD 337

/*> E1 module alias */
#define FILT SHUT_S /* filter */
#define RA1H TTH0   /* motor, curvature of the horiz. monochromator axis */
#define DRA1H DTTH0

/*> E2 module alias */
#define CHI1 XM /* chi */
#define CHI2 YM /* chi */
#define DCHI1 DXM
#define DCHI2 DYM
#define M1_FOC1 B4 /* first motor at first monochromator */
#define M1_FOC2 B5 /* second motor at first monochromator */
#define DM1_FOC1 DB4
#define DM1_FOC2 DB5

/*> E3 and E7 and M2 module alias */
#define PSR ANA1 /* ??? */
#define PST ANA2 /* ??? */
#define SST ANA3 /* ??? */
#define DPSR DANA1
#define DPST DANA2
#define DSST DANA3
#define XE X /* translation */
#define YE Y /* translation */
#define ZE Z /* translation */
#define DXE DX
#define DYE DY
#define DZE DZ

/*> E4 and V1 module alias */
#define FOCUSM MB1 /* focus monochromator */
#define DFOCUSM DMB1

/*> E7 and M2 module alias */
#define M3_TR TTH0     /* translation monochromator */
#define M3_CHI COLL15T /* chi monochromator */
#define M3_FOC COLL30T /* focus monochromator */
#define DM3_TR DTTH0
#define DM3_CHI DCOLL15T
#define DM3_FOC DCOLL30T

/*> E10 module alias */
#define MONO_OM OMGM /* omega monochromator */
#define DMONO_OM DOMGM
#define MONO_Z VBM /* ??? */
#define DMONO_Z DVBM
#define COL_Z TTH0 /* collimator ???*/
#define DCOL_Z DTTH0
#define C1_OM COLL15T /* ??? */
#define DC1_OM DCOLL15T
#define C2_OM COLL30T /* ??? */
#define DC2_OM DCOLL30T

/*> E7 and M2 and E10 alias */
#define M1_TR MB1 /* translation monochromator */
#define DM1_TR DMB1
#define M2_TR MB2 /* translation monochromator */
#define DM2_TR DMB2
#define M1_CHI XM /* chi monochromator */
#define DM1_CHI DXM
#define M2_CHI YM /* chi monichromator */
#define DM2_CHI DYM
#define M1_FOC B4 /* focus monochromator */
#define DM1_FOC DB4
#define M2_FOC B5 /* focus monichromator */
#define DM2_FOC DB5

/*> E7 and M2 and V1 alias */
#define TRANSM YSM /* translation monochromator */
#define DTRANSM DYSM

/*> V1 module alias */
#define INCLM MB2 /* inclination monochromator */
#define DINCLM DMB2

/*> V5 module alias */
#define A1 ANA1   /* motor */
#define A2 ANA2   /* motor */
#define A3 ANA3   /* motor */
#define A4 ANA4   /* motor */
#define AS TTHS   /* motor */
#define OM OMGS   /* motor */
#define VM CHIS   /* motor */
#define DAU LDET  /* HMI data acquisition unit */
#define B1 OMGA   /* power supply */
#define B2 TTHA   /* power supply */
#define B3 OMGX   /* power supply */
#define B6 XM     /* power supply */
#define B7 YM     /* power supply */
#define H1 OMGM   /* power supply */
#define H2 TTHM   /* power supply */
#define H3 X      /* power supply */
#define H4 Y      /* power supply */
#define H5 Z      /* power supply */
#define H6 XT     /* power supply */
#define H7 YT     /* power supply */
#define H8 ZT     /* power supply */
#define H9 MB1    /* power supply */
#define H10 MB2   /* power supply */
#define H11 YSM   /* power supply */
#define H12 MOT   /* power supply */
#define H13 HFI1  /* power supply */
#define H14 HFI2  /* power supply */
#define H15 HFI3  /* power supply */
#define H16 VFI1  /* power supply */
#define H17 FL1I1 /* power supply */
#define H18 FL1I2 /* power supply */
#define H19 FL2I1 /* power supply */
#define H20 FL2I2 /* power supply */
#define H21 YIN   /* power supply */
#define H22 ZIN   /* power supply */
#define H23 YOUT  /* power supply */
#define H24 ZOUT  /* power supply */
#define DA1 DANA1
#define DA2 DANA2
#define DA3 DANA3
#define DA4 DANA4
#define DAS DTTHS
#define DOM DOMGS
#define DVM DCHIS
#define DB1 DOMGA
#define DB2 DTTHA
#define DB3 DOMGX
#define DB6 DXM
#define DB7 DYM
#define DH1 DOMGM
#define DH2 DTTHM
#define DH3 DX
#define DH4 DY
#define DH5 DZ
#define DH6 DXT
#define DH7 DYT
#define DH8 DZT
#define DH9 DMB1
#define DH10 DMB2
#define DH11 DYSM
#define DH12 DMOT
#define DH13 DHFI1
#define DH14 DHFI2
#define DH15 DHFI3
#define DH16 DVFI1
#define DH17 DFL1I1
#define DH18 DFL1I2
#define DH19 DFL2I1
#define DH20 DFL2I2
#define DH21 DYIN
#define DH22 DZIN
#define DH23 DYOUT
#define DH24 DZOUT

/*> V6 module alias */
#define OMEGA_S OMGS  /* omega sample */
#define CHI_S CHIS    /* chi sample */
#define HIGH_S HGHT   /* ??? */
#define DELTA_D TTHS  /* ??? */
#define FLIP1_F FL1I1 /* power supply */
#define FLIP2_F FL1I2 /* power supply */
#define COMP1_F FL2I1 /* power supply */
#define COMP2_F FL2I2 /* power supply */
#define OMEGA_M OMGM  /* omega monochromator */
#define PHI_M CHIM    /* phi monochromator */
#define FOCUS_M MB1   /* focus monochromator */
#define LIN_M MB2     /* ??? */
#define DISDS_B YSD   /* distance sample <-> detector */
#define DO1_DI X      /* slit */
#define UP1_DI XT     /* slit */
#define LE1_DI Y      /* slit */
#define RI1_DI YT     /* slit */
#define DO2_DI Z      /* slit */
#define UP2_DI ZT     /* slit */
#define LE2_DI XM     /* slit */
#define RI2_DI YM     /* slit */
#define OMEGA_V ANA1  /* omega sample vertical geometry */
#define TRANS_V ANA2  /* translation vertical geometry */
#define DOMEGA_S DOMGS
#define DCHI_S DCHIS
#define DHIGH_S DHGHT
#define DDELTA_D DTTHS
#define DFLIP1_F DFL1I1
#define DFLIP2_F DFL1I2
#define DCOMP1_F DFL2I1
#define DCOMP2_F DFL2I2
#define DOMEGA_M DOMGM
#define DPHI_M DCHIM
#define DFOCUS_M DMB1
#define DLIN_M DMB2
#define DDO1_DI DX
#define DUP1_DI DXT
#define DLE1_DI DY
#define DRI1_DI DYT
#define DDO2_DI DZ
#define DUP2_DI DZT
#define DLE2_DI DXM
#define DRI2_DI DYM
#define DOMEGA_V DANA1
#define DTRANS_V DANA2

/*> M1 and M2 alias */
#define SLITM_H ANA4  /* slit */
#define SLITM_W ANA5  /* slit */
#define SLITS_U OMGX  /* slit */
#define SLITS_D OMGA  /* slit */
#define SLITS_L TTHA  /* slit */
#define SLITS_R MONOT /* slit */
#define SLITS_H VR    /* slit (compound) */
#define SLITS_W VT    /* slit (compound) */
#define DSLITM_H DANA4
#define DSLITM_W DANA5
#define DSLITS_U DOMGX
#define DSLITS_D DOMGA
#define DSLITS_L DTTHA
#define DSLITS_R DMONOT
#define DSLITS_H DVR
#define DSLITS_W DVT

/*> M1 module alias */
#define ZM MONOC        /* translation monochromator */
#define SLITM_X YT      /* slit */
#define SLITM_Y ZT      /* slit */
#define SLITM_U VBM     /* slit */
#define SLITM_D TTH0    /* slit */
#define SLITM_L COLL15T /* slit */
#define SLITM_R COLL30T /* slit */
#define T1 ANA1         /* translation detector (SANS) */
#define T2 ANA2         /* translation detector (SANS) */
#define T3 ANA3         /* translation detector (SANS) */
#define DZM DMONOC
#define DSLITM_X DYT
#define DSLITM_Y DZT
#define DSLITM_U DVBM
#define DSLITM_D DTTH0
#define DSLITM_L DCOLL15T
#define DSLITM_R DCOLL30T
#define DT1 DANA1
#define DT2 DANA2
#define DT3 DANA3
#define MPOS LPOS      /* list position */
#define SHUT_P SHUTTER /* shutter */

/*> M2 module alias */
#define SLITE VBM     /* slit */
#define SLITS_X MONOC /* slit */
#define SLITS_Y CHIM  /* slit */
#define DSLITE DVBM
#define DSLITS_X DMONOC
#define DSLITS_Y DCHIM

/*> module categories */
#define MOTOR 1          /* motor */
#define ENVIRONMENT 2    /* temperature, magnetic field, ... */
#define POWER_SUPPLY 3   /* power supply */
#define MULTIMETER 4     /* multi meter */
#define META_MOTOR 5     /* compound motor: e.g. H, K, L, E, slit, ... */
#define META_PS 6        /* compound power supply: e.g. field, flipper, ... */
#define COUNTER 7        /* counter */
#define SINGLE_DET 8     /* single detector */
#define MULTI_DET 9      /* multi detector */
#define STEP_COUNTER 10  /* scan step info */
#define LIST_POS 11      /* list position info */
#define HIST_WINDOW 12   /* histogram window info */
#define DET_CHANNEL 13   /* selected channel (of multi detector) */
#define VIRTUAL_DRIVE 14 /* component realized in software, only */

/*> module ids for reply message */
#define RESPONSE1 200
#define RESPONSE2 201

/*> error codes, converted into error text by mesout */
#define SCANALREADYACTIVE 1                /* nested scans not allowed */
#define NOACTIVESCAN 2                     /* obsolete */
#define NOTEMERGENCYCOMMAND 3              /* command not allowed during interruption*/
#define SCANISACTIVE 4                     /* command not allowed during scan */
#define AUTOMATICHALT 5                    /* fatal error interruption */
#define TASKNOTINSTALLED 6                 /* obsolete */
#define LAMCONNECTIONFAILED 101            /* obsolete */
#define OFFLINE 102                        /* component is offline */
#define NOTSPECIFIED 103                   /* a parameter is not defined */
#define SETREADDIFFERENT 104               /* obsolete */
#define NOTINCOMPUTERMODE 105              /* obsolete */
#define WRONGSEQUENCEOFOPERATIONS 106      /* inconsistence compound > basic mod */
#define MODULECANTBEREAD 107               /* read failed */
#define WRONGREADREQUEST 108               /* obsolete */
#define MODULECANTBELOAD 109               /* obsolete */
#define WRONGABMPRESET 110                 /* obsolete */
#define WRONGPRESETFORCLOCKORDIVIDER 111   /* obsolete */
#define MODULECANTBESET 112                /* drive failed */
#define SETVALUEOUTOFSOFTWARELIMITS 113    /* target not in range */
#define MODULENOTALLOWEDFORCOMPARE 114     /* obsolete */
#define ACTUALVALUEOUTOFTOLERANCELIMIT 115 /* tolerance error */
#define NOLAMOFMODULETOCLEAR 116           /* obsolete */
#define SOFTWARELIMITSNOTSPECIFIED 117     /* range not defined */
#define TOLERANCELIMITNOTSPECIFIED 118     /* obsolete */
#define OFFSETNOTSPECIFIED 119             /* obsolete */
#define WAVELENGTHNOTSPECIFIED 120         /* wave length not defined */
#define TWOTHETAGREATER90DEGREE 121        /* error during HKL conversion */
#define WRONGORIENTATION 122               /*              "              */
#define MODENOTIMPLEMENTED 123             /* unknown conversion mode */
#define NYGREATER90DEGREE 124              /* error during HKL conversion */
#define MSHNOTSPECIFIED 125                /* conversion mode not defined */
#define UBMNOTSPECIFIED 126                /* UB matrix not defined */
#define RENDEZVOUSFAILED 127               /* CORBA communication failed */
#define NOCENTRALVALUE 128                 /* not central value (scan ) */
#define KIORKFNOTDEFINED 129               /* KI/ KF not defined */

/*> error codes for DATOUT */
#define ERRINCONVERSIONSTRING (-1)
#define ERRCANTOPENSHDFILE (-2)
#define ERRCANTOPENAFILE (-3)
#define ERRCOMMENTOVERFLOW (-4)
#define ERRSTORE (-5)
#define ERRCANTOPENRUNFILE (-6)
#define ERRFATAL (-7)

/*> error codes for protoc */
#define ERRCANTOPENPROTFILE 201
#define ERRWINDOWS 202
#define ERRPEAKPOINTS 203
#define ERRCANTOPENCAFILE 204

/*> caress system errors, cannot continue */
#define ERRSYSTEM10 (-10)
#define ERRSYSTEM11 (-11)
#define ERRSYSTEM12 (-12)
#define ERRSYSTEM13 (-13)
#define ERRSYSTEM14 (-14)
#define ERRSYSTEM15 (-15)
#define ERRSYSTEM16 (-16)
#define ERRSYSTEM17 (-17)
#define ERRSYSTEM18 (-18)

/*> kind of hardware components */
#define HOST_ID 1000         /* kind used for hardware server host */
#define GENERIC_CORBADEV 500 /* generic CORBA device driver (see 'corbadevice.idl') */
#define SOFT_MODULE 0        /* module realized in software, only */
#define FMC_SCALER 1         /* FMC 1301 scaler, max. preset < 10**6 */
#define NIDOS_SCALER 2       /* Nidos scaler, max. preset <10**7 */
#define HARWELL_DIGITIZER 3  /* Harwell motor control */
#define KSC_GPIB 4           /* Kinetic Systems 3388 CAMAC <-> GPIB */
#define NE_HISTOGRAM 5       /* NE 9082 histogram */
#define SOLARTRON_DMM 8      /* Solartron multimeter */
#define MUX1 10              /* stepper motor mux.(Stephan) */
#define MUX1_MOTOR 11        /* stepper motor via mux1 */
#define MUX2 12              /* stepper motor mux. Harm */
#define MUX2_MOTOR 13        /* stepper motor via mux2 */
#define Y66_ABS 14           /* Y66 motor drive absolute coder */
#define Y66_INCR 15          /* Y66 motor drive incremental coder */
#define ILR235 16            /* Hytec 235 interrupt reg */
#define BORER_motor 17       /* BORER stepper motor controller 1162 */
#define Y70_SCALER 19        /* HMI preset scaler, max. preset 10**8 */
#define LECROY_HISTO 20      /* LeCroy histogramm modules 3587/3588 */
#define IN_MODULE 21         /* KSC 3472 input register*/
#define OUT_MODULE 22        /* KSC 3075 output register */
#define ILL_TCP 23           /* ILL temperature control, RS232 link */
#define POWER_SUPPLY_1                                                                             \
    24                         /* power supply SM7020 DELTA ELEKTRONIKA                            \
                                  controlled by DAC 3195 KSC */
#define LS_TCP 25              /* Lake Shore temperature controller DRC 93CA */
#define BLIND1 26              /* blind control using BORER 1032 */
#define DMM199 27              /* KEITHLEY DMM 199 */
#define JULABO_IF1 28          /* LULABO IF1 temperature control */
#define V4_COLLIMATOR 29       /* V4 collimator */
#define V4_ATTENUATOR 30       /* V4 attenuator */
#define V4_MAGNET 31           /* DRUSCH 10 E-5 magnet */
#define LAUDA 32               /* LAUDA R61 temperature controller */
#define V4_VELOSELEC 33        /* v4 velocity selector */
#define BORER1032 34           /* BORER 1032 I/O module */
#define V4_BEAMSTOP_CHANGER 35 /* V4 beamstop changer */
#define V4_VELOSELEC_LOCK                                                                          \
    36 /* lock/unlock velocity selector,                                                           \
          using BORER 1032 */
#define LS_TCP_TEMPR                                                                               \
    37                /* LakeShore DRC 93CA controller, read up to 5                               \
                         temp's, only */
#define MUX3 38       /* HMI ST180, RS232 link  */
#define MUX3_MOTOR 39 /* motor driven by MUX3 */
#define E2_COLLI_CHANGER                                                                           \
    40 /* E2 collimator changer driven by                                                          \
          two MUX3 motors */
#define E1_HEINZINGER_PS                                                                           \
    41                   /* HEINZINGER power supplies as used at E1:                               \
                            immediate set assumed */
#define SHUTTER_CLOSE 42 /* set shutter to close by KSC 3075 bit (E1) */
#define V4_FLIPPER                                                                                 \
    43 /* flipper at V4 using HP 33120A Frequence                                                  \
          Generator and ROHDE&SCHWARZ URE2 Voltmeter*/
#define IPS_120                                                                                    \
    44 /* superconducting magnet OXFORD INSTRUMENTS                                                \
          IPS 120 (via GPIB Bus) */
#define ITC_503                                                                                    \
    45                          /* temperature controller ITC 503, OXFORD                          \
                                   INSTRUMENTS (via GPIB Bus) */
#define TCPIP_SOCKET 46         /* "info source" linked via TCPIP socket */
#define TCPIP_TEMPR 47          /* read temperatures and pressure via TCPIP */
#define HP34401A 48             /* HP multimeter 34401A (simple read) */
#define PSC44M 49               /* DELTA ELEKTRONIKA power supply via PSC44M */
#define BURSTER_PS 50           /* BURSTER DIGISTANT 6706 power supply */
#define EUROTHERM 51            /* EUROTHERM temp controller 2404, 2408i */
#define LS_450 52               /* LakeShore gaussmeter model 450 */
#define V5_VELO 53              /* DORNIER velocity selector for v5 */
#define V6_MAGNET 54            /* combo of PSC44M and LS-450 for V6 */
#define HEINZINGER_PTN 55       /* power supply HEINZINGER/KNUERR PTN */
#define FUG_NTN 56              /* power supply FUG NTN */
#define LAWSON_201 57           /* magnetometer LAWSON 201 */
#define MESYTEC_PSD_HISTO 58    /* histogram module MESYTEC PSD */
#define MESYTEC_PSD_COUNTER 59  /* counter MESYTEC PSD */
#define LECROY_E3 60            /* 128x128 histo to 102x102 histo,very special! */
#define DMM2010 61              /* KEITHLEY DMM 2010 */
#define HAAKE_THERM 62          /* HAAKE thermostate F6 or N6 */
#define NI_HISTO 63             /* NI histogram */
#define MESYTEC_PSD_V4_HISTO 64 /* MESYTEC histo for V4 */
#define DIO 65                  /* DIO via GREENSPRING IO24 */
#define ENET_GPIB 66            /* NI ethernet<->GPIB interface GPIB-ENET/100 */
#define RS_VMURE2 67            /* ROHDE&SCHWARZ voltmeter URE2 */
#define BRUKER_BEC1 68          /* BRUKER power supply B-EC1 */
#define SWITCH 69               /* general digital I/O switch */
#define LS_340 71               /* Lake Shore tempetature controller 340 */
#define JULABO_F25 72        /* Julabo F25 - HE temperature controller (Kälte-Umwälzthermostat) */
#define HUBER_SMC_MOTOR 73   /* HUBER SMC 9x motor controller */
#define HUBER_SMC_COUNTER 74 /* HUBER SMC 9x integrated counter */
#define GPIB_DEVICE 75       /* a Device at a GPIB-bus */
#define ISEG_HV_NHQ                                                                                \
    76 /* iseg high voltage controller over RS232                                                  \
          (iseg Spezialelektronik, NHQ x0xx/x2xx) */

#define X13 100               /* VME motor control with absolute encoder */
#define STR721A2 101          /* STRUCK STR 721 VME counter */
#define DAU_HISTO 102         /* HMI data acquisition unit */
#define X13_INCR 103          /* X13 with incremental encoder */
#define ICS4895_gpib 104      /* ICS 4895 interface RS232 <-> GPIB  */
#define HEINZINGER_PS 105     /* HEINZINGER TN power supply */
#define BRUKER_PS_1 106       /* BRUKER power supply type MN 40/60 */
#define BRUKER_PS_2 107       /* BRUKER power supply ty MN 29/380 */
#define VELO_SEL 108          /* velocity selector MDR 14500390 */
#define ONE_DETECTOR_CELL 109 /* one detector cell of a multi detector */
#define DORNIER_CHOPPER 110   /* DORNIER NCS chopper control */
#define X13_TWICE                                                                                  \
    111                 /* X13 abs for which positioning should                                    \
                           be tried twice if necessary */
#define BRUKER_PS_3 112 /* BRUKER power supply MC1 2x80/80 */
#define JOERGER_VSC_HISTO                                                                          \
    113                    /* histogram consisting of several JOERGER VSC                          \
                              VME counters */
#define EKF_44520_ABS 114  /* EKF 44520 motor control, abs. encoder, VME */
#define EKF_44520_INCR 115 /* EKF 44520 motor control, incr. encoder, VME */
#define JOERGER_VSC 116    /* JOERGER VSC 8/16 VME counter */
#define STR721A2_HISTO 117 /* histogram consisting of STRUCK STR 721 */
#define VICBUS 118         /* VIC Bus */
#define IBF_GPIB 119       /* VME GPIB with IP module */
#define IO24 120           /* GREENSPRING I/O register */
#define X18 121            /* VME histogram module HMI X18 */
#define JULIOS 122         /* Juelich detector system JULIOS */

/*> bus ids */
#define SOFTBUS 0 /* no hardware bus */
#define CAMAC 1   /* CAMAC bus */
#define IEC 2     /* GPIB bus */
#define GPIBBUS IEC
#define MUXBUS 3  /* motor multiplex */
#define V24 4     /* RS232 link */
#define TCPIP 5   /* special link to LABVIEW program */
#define COMBO 6   /* device is a combination of other devices */
#define MESYTEC 7 /* MESYTEC detector system */
#define DIOBUS 8  /* general digital IO (for switch) */
#define VME 11    /* VME bus */

/*> process ids */
#define CONTRL 1
#define MEASUR 2
#define CMDINT 3
#define MESOUT 4
#define DATOUT 5
#define PROTOC 10
#define EXTPRO 12
#define CARINP 13

#define NUMBEROFTASKS 13

//#endif  /* !WIN32 */

/*> data tupel ids (intermediate data storage in file data20_xx.dat) */
enum {
    MODULELIST = 0,          /* data structure for scan modules */
    SETVALUES_OVERWRITE = 1, /* data structure to overwrite parts of SETVALUES */
    PROTOCOLVALUES = 2,      /* data structure for protocol modules */
    SETVALUES = 3,           /* data structure for set (drive) modules */
    MASTER2VALUES = 4,       /* obsolete */
    MASTER1VALUES = 5,       /* data structure for MM1 and SL1 modules */
    POLVALUES = 6,           /* data structure for polarization modules */
    MASTER1VALUES_SAVE = 7,  /* as MASTER1VALUES but stored for saved data */
    MASTER2VALUES_SAVE = 8,  /* obsolete */
    CMD_INFO = 10,           /* data structure for scan relevant commands */
    LOGVALUES = 11,          /* data structure for module logging */
    NODATATYPE = 0           /* no data structure, end of info */
};

/*> value type ids */
enum {
    INTEGERTYPE = 1, /* short int (16 bit) */
    LONGINTEGER = 2, /* int (32 bit) */
    REALTYPE = 5,    /* float (32 bit) */
    DOUBLETYPE = 11, /* double (64 bit) */
    INT64TYPE = 13,  /* long long (64 bit) */
    CHARTYPE = 16,   /* string (not '\0' terminated) */
    STRINGTYPE = 17, /* string ('\0' terminated) */
    ADDRTYPE = 18,   /* address info */
    MODIDTYPE = 19,  /* short int (module ID) */
    /* cmdint - definitions */
    ONOFFTYPE = 21,       /* OT: strings "ON" or "OFF" (e.g. command CHECK) */
    PMATRIXTYPE = 22,     /* PT: strings "XX", "qpair",..., "ZZ" (cmd PMATRIX) */
    DAUTYPE = 23,         /* CT: strings "SI" or "MU" */
    ENUMERATIONTYPE = 24, /* EN: indicating enumeration in xx_ranges.dat */
    DAUDATATYPE = 25      /* IT: strings "I2" or "I4" in CONFIG command */
};

/*> status info */
enum {
    FATAL = -1, /* fatal error */
    ABORT = -1, /* abort eg. in case of arithmetic error */
    NOT_OK = 1,
    OK = 0,
    NOTHING = 0,
#if !defined(_MSC_VER) || !defined(WINVER)
    NOERROR = 0,
#endif /* !_MSC_VER || !WINVER */
    NOT_USED = 0,
    USED = 1,    /* component can detect "done" */
    DELAYED = 1, /* drive is delayed (e.g. multiplex) */
};

enum {
    OFF_LINE = 0,
    ON_LINE = 1,
    MANUAL_MODE = 2,   /* motor is in manual module */
    NOT_DEFINED = 3,   /* module was not defined */
    LIMIT_CONTACT = 4, /* motor is at a limit contact */

    NOT_ACTIVE = 1,
    ACTIVE = 2, /* module is moving */
    DONE = 3,   /* module reached target or aborted move */
    LOADED = 4,
    ACTIVE1 = 5,
    COMBO_ACTIVE = 6, /* multiplexer is active */
    MODULE_ERROR = 7, /* module error text available */
    MODULE_RESET = 8, /* fatal error, module has to be reset */
};

/*> time in s between consecutive calls of hwb_read_allmodules */
#define DELTA_TIME 1.0       /* normal */
#define SMALL_DELTA_TIME 0.1 /* short, during first 0.5 s of an action */

/*> miscelaneus */
enum {
    TRIGGERCOMPOUND = 1,  /* trigger for internal command generation */
    FREESCANCOMMAND = 2,  /* next "free" scan command expected */
    COMMANDAFTERHALT = 4, /* obsolete */
    NEWDATA = 0,          /* data have to be stored */
    NEWCHANDATA = 2,      /* obsolete */
    NODATA = 1            /* no storage data */
};

/*> DAU modes */
enum {
    CONFIG_MODE = 1, /* configuration info */
    TOF_MODE = 2,    /* currently in TOF mode */
    NSE_MODE = 3     /* currently in NSE mode */
};

/*> CARESS command message offsets: */
/*> normal message */
enum {
    LENGTH_OFFSET = 0,         /* length in short int */
    ID_OFFSET = 1,             /* command id */
    MODULE_NUMBER_OFFSET = 2,  /* number of modules */
    FIRST_MODULE_OFFSET = 3,   /* first module id */
    FIRST_GROUP_OFFSET = 4,    /* number of value groups of 1. mod */
    FIRST_VAL_NUM_OFFSET = 5,  /* number of values of 1. group */
    FIRST_VAL_TYPE_OFFSET = 6, /* type of values of 1. group */
    FIRST_VAL_OFFSET = 7,      /* 1. value of first group */
                               /*> response message */
    RESP_COMMAND_OFFSET = 7,   /* FIRST_VAL_OFFSET:id of processed command */
    RESP_REPLY_OFFSET = 8,     /* reply info */
    RESP_DESIRE_OFFSET = 9,    /* desire info */
    RESP_STORAGE_OFFSET = 10   /* storage info */
};

/*> hardwarebag data offsets */
enum {
    HWB_MODULE_ID = 0, /* module id */
    HWB_STATUS = 1,    /* special: device status, other data, dummy */
    HWB_VAL_NUM = 2,   /* number of values */
    HWB_VAL_TYPE = 3,  /* data type of values */
    HWB_FIRST_DATA = 4 /* first data offset */
};

/*> protocol file info types */
enum {
    HEADER_REC = 0,  /* header info */
    COMMAND_REC = 1, /* command string */
    ERROR_REC = 2,   /* error string */
    DATA_REC = 3,    /* data info */
    INFO_REC = 4     /* other (not further specified) info */
};

/*> acquisition status */
enum { ACQ_STARTED = 1, ACQ_STOPPED = 2 };

/*> resource name symbols for locking mechanism for file access */
#define SYS_DATA_RSC "sys_data"         /* data20_xx.dat */
#define LIVE_DATA_RSC "live_data"       /* live_data.dat */
#define RAW_DATA_RSC "raw_data"         /* current data storage file */
#define DAU_INFO_RSC "dau_info"         /* dau_info.dat */
#define DEVICE_LOG_RSC "device_log"     /* device_log.dat */
#define LIVE_PRT_RSC "live_prt"         /* live_prt.dat */
#define DATA_BASE_RSC "data_base"       /* data_base.dat */
#define DEFS_RSC "defs"                 /* defs.dat */
#define HISTO_SERVER_RSC "histo_server" /* histo_server.dat */
#define SCANEVENT_RSC "scan_event"      /* scan_event.sh */

/*> "normal" length of disk block in byte, short and int */
#define BLOCK_LEN_B 512
#define BLOCK_LEN_S 256
#define BLOCK_LEN_I 128

/*> "double" length of disk block in byte, short and int */
#define D_BLOCK_LEN_B 1024
#define D_BLOCK_LEN_S 512
#define D_BLOCK_LEN_I 256

/*> some max. values */
#define MAX_FILE_NAME_LEN 256
#define MAX_STR_LEN 4096
#define MAXMESLENGTH 1500                /* in short int */
#define BMAXMESLENGTH (2 * MAXMESLENGTH) /* in bytes */
#define MAXTASKNAMELENGTH 32
#define MAX_EXP_NAME_LENGTH 9
#define MAXNAMELENGTH 32          /* max module name length (not incl. 'D'+'\0') */
#define MAXCOM 68                 /* max length of comment */
#define MAXIDLENGTH 20            /* max length of id name */
#define MAX_MODULES 16384         /* max number of modules */
#define MAXNUMBEROFWINDOWS 4      /* max number of detector windows */
#define MAXNROFMODULES 128        /* max number of modules in a data tupel */
#define MAX_REC_LEN 4096          /* max length of record for live_prt */
#define MAXNUMBEROFCHANNELS 16384 /* max number of channels of multi-detector*/
#define MAX_COUPLED_GROUPS 8      /* max coupled groups (CSM command) */

#define MAXBLOCKS                                                                                  \
    (MAXNUMBEROFCHANNELS / 128 + 2) /* max. number of                                              \
blocks for data in data20 during a store action; 3 active stores possible*/

/*> some typedef */
/* CARESS message */
typedef int16 tmessage[MAXMESLENGTH];
/* " */
typedef tmessage commandtype;
/* process name */
typedef char ttaskname[MAXTASKNAMELENGTH + 1];
/* command string*/
typedef char command_str_t[MAX_STR_LEN + 1];
/* " */
typedef char str_t[MAX_STR_LEN];
/* file pointer as used by functions of c_fio.c  */
typedef int file_ptr_t[2];
/* identifer string */
typedef char
    tidentifier[((MAXIDLENGTH) >= (MAXNAMELENGTH + 2)) ? ((MAXIDLENGTH)+1) : (MAXNAMELENGTH + 2)];
/* a string */
typedef char str64_t[65];
/* info of many module  */
typedef int32 module_info_block_t[512];
/* info string as stored into live_prt.dat */
typedef char rec_line_t[MAX_REC_LEN];

/* protocol file info structure */
typedef struct rec_t {
    int32 kind;
    int32 length;
    rec_line_t str;
} rec_t;

/* type dependend value structure */
typedef struct typed_value_t {
    int32 value_type;
    union {
        int16 i2_value;
        int32 i4_value;
        int64 i8_value;
        float r4_value;
        double r8_value;
        char str_value[256];
    } UU;
} typed_value_t;

/*	module info element may be int or float	*/
union module_info_t {
    int32 i4;
    float r4;
};

#define c_dwFlag_Online 0x00000001       /* module is ONLINE */
#define c_dwFlag_ErrorSupport 0x00000002 /* error text support */
/* handle counter for offline detection */
#define c_dwFlag_OfflineInc 0x00001000  /* increment */
#define c_dwFlag_OfflineShift 12        /* bits to shift to left */
#define c_dwFlag_OfflineMask 0x000ff000 /* bit mask */

/* hardware component info structure */
typedef struct hardware_module_t {
    int32 id;       /* CARESS internal module id */
    int32 kind;     /* kind of module */
    int32 addr1;    /* bus id, e.g. CAMAC, GPIB, ... */
    int32 addr2;    /* supplementary addr info,e.g. crate addr, GPIB address,... */
    int32 addr3;    /* supplementary address info */
    float val1;     /* supplementary info */
    float val2;     /* supplementary info */
    float val3;     /* supplementary info */
    float val4;     /* supplementary info */
    float val5;     /* supplementary info */
    float val6;     /* supplementary info */
    float val7;     /* supplementary info */
    float val8;     /* supplementary info */
    float val9;     /* supplementary info */
    float val10;    /* supplementary info */
    float val11;    /* supplementary info */
    float val12;    /* supplementary info */
    float val13;    /* supplementary info */
    float val14;    /* supplementary info */
    float val15;    /* supplementary info */
    float val16;    /* supplementary info */
    uint32 dwFlags; /* status flags of module */
} hardware_module_t;

/* hardware server internal module list */
struct list_element_t {
    struct hardware_module_t hardware_module;
    union {
        int32 i4;
        float r4;
    } actual_value;
    union {
        int32 i4;
        float r4;
    } target_value;
    union {
        int32 i4;
        float r4;
    } delta_value;
    int32 event_number;
    int32 module_status;
    int32 lookup;
    int32 next_action;
    /* idx: 0/i4=id; 1=integral; 2/i4=data size (from idx 4); 3/i4=data type; 4=data */
    union module_info_t* pSnapshot;
    int32 timeout;
    time_t start_time;
    int32 retry;
    int32 max_readblock_size;
    char szErrorDesc[65]; /* last error text */
};

/* info for module to be driven permanently during data acquisition */
struct perm_module_t {
    int32 id;
    float val1;
    float val2;
    int32 val_index;
    int32 active;
};

#ifdef __cplusplus
}
#endif

#endif // CAR_GLOBALS_H
