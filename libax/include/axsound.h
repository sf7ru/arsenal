// ***************************************************************************
// TITLE
//
// PROJECT
//
// ***************************************************************************
//
// FILE
//      $Id$
// HISTORY
//      $Log$
// ***************************************************************************


#ifndef __AXSOUND_H__
#define __AXSOUND_H__

#include <arsenal.h>


// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------


#define NOTE_C0             1635
#define NOTE_Db0            1732
#define NOTE_D0             1835
#define NOTE_Eb0            1945
#define NOTE_E0             2060
#define NOTE_F0             2183
#define NOTE_Gb0            2312
#define NOTE_G0             2450
#define NOTE_Ab0            2596
#define NOTE_A0             2750
#define NOTE_Bb0            2914
#define NOTE_B0             3087
#define NOTE_C1             3270
#define NOTE_Db1            3465
#define NOTE_D1             3671
#define NOTE_Eb1            3889
#define NOTE_E1             4120
#define NOTE_F1             4365
#define NOTE_Gb1            4625
#define NOTE_G1             4900
#define NOTE_Ab1            5191
#define NOTE_A1             5500
#define NOTE_Bb1            5827
#define NOTE_B1             6174
#define NOTE_C2             6541
#define NOTE_Db2            6930
#define NOTE_D2             7342
#define NOTE_Eb2            7778
#define NOTE_E2             8241
#define NOTE_F2             8731
#define NOTE_Gb2            9250
#define NOTE_G2             9800
#define NOTE_Ab2            10383
#define NOTE_A2             11000
#define NOTE_Bb2            11654
#define NOTE_B2             12347
#define NOTE_C3             13081
#define NOTE_Db3            13859
#define NOTE_D3             14683
#define NOTE_Eb3            15556
#define NOTE_E3             16481
#define NOTE_F3             17461
#define NOTE_Gb3            18500
#define NOTE_G3             19600
#define NOTE_Ab3            20765
#define NOTE_A3             22000
#define NOTE_Bb3            23308
#define NOTE_B3             24694
#define NOTE_C4             26163
#define NOTE_Db4            27718
#define NOTE_D4             29366
#define NOTE_Eb4            31113
#define NOTE_E4             32963
#define NOTE_F4             34923
#define NOTE_Gb4            36999
#define NOTE_G4             39200
#define NOTE_Ab4            41530
#define NOTE_A4             44000
#define NOTE_Bb4            46616
#define NOTE_B4             49388
#define NOTE_C5             52325
#define NOTE_Db5            55437
#define NOTE_D5             58733
#define NOTE_Eb5            62225
#define NOTE_E5             65926
#define NOTE_F5             69846
#define NOTE_Gb5            73999
#define NOTE_G5             78399
#define NOTE_Ab5            83061
#define NOTE_A5             88000
#define NOTE_Bb5            93233
#define NOTE_B5             98777
#define NOTE_C6             104650
#define NOTE_Db6            110873
#define NOTE_D6             117466
#define NOTE_Eb6            124451
#define NOTE_E6             131851
#define NOTE_F6             139691
#define NOTE_Gb6            147998
#define NOTE_G6             156798
#define NOTE_Ab6            166122
#define NOTE_A6             176000
#define NOTE_Bb6            186466
#define NOTE_B6             197553
#define NOTE_C7             209300
#define NOTE_Db7            221746
#define NOTE_D7             234932
#define NOTE_Eb7            248902
#define NOTE_E7             263702
#define NOTE_F7             279383
#define NOTE_Gb7            295996
#define NOTE_G7             313596
#define NOTE_Ab7            332244
#define NOTE_A7             352001
#define NOTE_Bb7            372931
#define NOTE_B7             395107
#define NOTE_C8             418601
#define NOTE_Db8            443492
#define NOTE_D8             469864
#define NOTE_Eb8            497803

#define BPM_QUARTER(bpm)    (60000/bpm)             //quarter 1/4
#define BPM_WHOLE(bpm)      (BPM_QUARTER(bpm) * 4)  // whole 4/4
#define BPM_HALF(bpm)       (BPM_QUARTER(bpm) * 2)  //half 2/4
#define BPM_EIGHTH(bpm)     (BPM_QUARTER(bpm) / 2)  //eighth 1/8
#define BPM_SIXTHEENTH(bpm) (BPM_QUARTER(bpm) / 4)  // sixteenth 1/16

#define DUR_QUARTER         (32/4)                  //quarter 1/4
#define DUR_WHOLE           (DUR_QUARTER * 4)  // whole 4/4
#define DUR_HALF            (DUR_QUARTER * 2)  //half 2/4
#define DUR_EIGHTH          (DUR_QUARTER / 2)  //eighth 1/8
#define DUR_SIXTHEENTH      (DUR_QUARTER / 4)  // sixteenth 1/16
#define DUR_THIRTYSECONDTH  (DUR_QUARTER / 8)  // thirty-secondth 1/32


// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef struct tag_SOUNDNOTE
{
    UINT                    tone;
    UINT                    duration;
} SOUNDNOTE, * PSOUNDNOTE;

typedef const SOUNDNOTE *    PCSOUNDNOTE;


#endif // #ifndef __AXSOUND_H__

