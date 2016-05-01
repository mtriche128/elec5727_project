/**************************************************************************//**
 * 
 * @file  features.h
 * @brief Header file for feature definitions.
 * 
 * CITATION:
 * This code is derived from Skyler Saleh's "Fiducial Based Visual Servoing"
 * software. 
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to 
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or 
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 ******************************************************************************/

#ifndef _SYMDET_FEATURES_H
#define _SYMDET_FEATURES_H

struct Pattern{ const char data[8];int id;};
Pattern petal_patts[]={
{.data={1,0,0,0,0,0,0,0,},.id=0},
{.data={1,1,0,0,0,0,0,0,},.id=1},
{.data={1,0,0,1,0,0,0,0,},.id=2},
{.data={1,1,0,1,0,0,0,0,},.id=3},
{.data={1,0,1,1,0,0,0,0,},.id=4},
{.data={1,1,1,1,0,0,0,0,},.id=5},
{.data={1,1,0,0,1,0,0,0,},.id=6},
{.data={1,1,1,0,1,0,0,0,},.id=7},
{.data={1,0,0,0,0,1,0,0,},.id=8},
{.data={1,1,0,0,0,1,0,0,},.id=9},
{.data={0,0,0,1,0,1,0,0,},.id=10},
{.data={1,0,0,1,0,1,0,0,},.id=11},
{.data={1,1,0,1,0,1,0,0,},.id=12},
{.data={1,0,1,1,0,1,0,0,},.id=13},
{.data={0,1,1,1,0,1,0,0,},.id=14},
{.data={1,1,1,1,0,1,0,0,},.id=15},
{.data={1,0,1,0,1,1,0,0,},.id=16},
{.data={1,1,1,0,1,1,0,0,},.id=17},
{.data={0,0,0,1,1,1,0,0,},.id=18},
{.data={1,0,0,1,1,1,0,0,},.id=19},
{.data={1,1,0,1,1,1,0,0,},.id=20},
{.data={1,0,1,1,1,1,0,0,},.id=21},
{.data={1,1,1,1,1,1,0,0,},.id=22},
{.data={1,0,1,0,0,0,1,0,},.id=23},
{.data={1,0,0,1,1,0,1,0,},.id=24},
{.data={1,0,0,0,0,1,1,0,},.id=25},
{.data={1,0,0,1,0,1,1,0,},.id=26},
{.data={0,0,1,1,0,1,1,0,},.id=27},
{.data={1,0,1,1,0,1,1,0,},.id=28},
{.data={1,1,1,1,0,1,1,0,},.id=29},
{.data={1,0,0,1,1,1,1,0,},.id=30},
{.data={0,0,1,1,1,1,1,0,},.id=31},
{.data={1,0,1,1,1,1,1,0,},.id=32},
{.data={1,0,0,0,0,0,0,1,},.id=33},
{.data={1,0,0,1,0,0,0,1,},.id=34},
{.data={1,0,0,0,1,0,0,1,},.id=35},
{.data={1,0,0,0,0,1,0,1,},.id=36},
{.data={1,0,0,1,0,1,0,1,},.id=37},
{.data={1,1,0,1,0,1,0,1,},.id=38},
{.data={1,0,0,1,1,1,0,1,},.id=39},
{.data={1,0,0,0,1,0,1,1,},.id=40},
{.data={1,0,0,1,1,0,1,1,},.id=41},
{.data={1,0,0,0,0,1,1,1,},.id=42},
{.data={0,0,0,1,0,1,1,1,},.id=43},
{.data={1,0,0,1,0,1,1,1,},.id=44},
{.data={1,0,1,1,0,1,1,1,},.id=45},
{.data={1,1,1,1,0,1,1,1,},.id=46},
{.data={1,0,0,1,1,1,1,1,},.id=47},
};

#endif