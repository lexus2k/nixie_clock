/*
    Copyright (C) 2016 Alexey Dynda

    This file is part of Nixie Library.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "nixie_melodies.h"

const SNixieTempoNote marioNotes[] = {
  { NOTE_E7, 12 },
  { NOTE_E7, 12 },
  { NOTE_SILENT, 12 },
  { NOTE_E7, 12 },
  { NOTE_SILENT, 12},
  { NOTE_C7, 12 },
  { NOTE_E7, 12 },
  { NOTE_SILENT, 12 },
  { NOTE_G7, 12 },
  { NOTE_SILENT, 12 },
  { NOTE_SILENT, 12 },
  { NOTE_SILENT, 12 },
  { NOTE_G6, 12 },
  { NOTE_SILENT, 12 },
  { NOTE_SILENT, 12 },
  { NOTE_SILENT, 12 },

  { NOTE_C7, 12 },
  { NOTE_SILENT, 12 },
  { NOTE_SILENT, 12 },
  { NOTE_G6, 12 },
  { NOTE_SILENT, 12 },
  { NOTE_SILENT, 12 },
  { NOTE_E6, 12 },
  { NOTE_SILENT, 12 },
  { NOTE_SILENT, 12 },
  { NOTE_A6, 12 },
  { NOTE_SILENT, 12 },
  { NOTE_B6, 12 },
  { NOTE_SILENT, 12 },
  { NOTE_AS6, 12 },
  { NOTE_A6, 12 },
  { NOTE_SILENT, 12 },

  { NOTE_G6, 9 },
  { NOTE_E7, 9},
  { NOTE_G7, 9},
  { NOTE_A7, 12 },
  { NOTE_SILENT, 12 },
  { NOTE_F7, 12 },
  { NOTE_G7, 12 },
  { NOTE_SILENT, 12 },
  { NOTE_E7, 12 },
  { NOTE_SILENT, 12 },
  { NOTE_C7, 12},
  { NOTE_D7, 12 },
  { NOTE_B6, 12 },
  { NOTE_SILENT, 12 },
  { NOTE_SILENT, 12 },

  { NOTE_C7, 12 },
  { NOTE_SILENT, 12 },
  { NOTE_SILENT, 12 },
  { NOTE_G6, 12 },
  { NOTE_SILENT, 12 },
  { NOTE_SILENT, 12 },
  { NOTE_E6, 12 },
  { NOTE_SILENT, 12 },
  { NOTE_SILENT, 12 },
  { NOTE_A6, 12 },
  { NOTE_SILENT, 12 },
  { NOTE_B6, 12 },
  { NOTE_SILENT, 12 },
  { NOTE_AS6, 12 },
  { NOTE_A6, 12 },
  { NOTE_SILENT, 12 },

  { NOTE_G6, 9 },
  { NOTE_E7, 9},
  { NOTE_G7, 9},
  { NOTE_A7, 12 },
  { NOTE_SILENT, 12 },
  { NOTE_F7, 12 },
  { NOTE_G7, 12 },
  { NOTE_SILENT, 12 },
  { NOTE_E7, 12 },
  { NOTE_SILENT, 12 },
  { NOTE_C7, 12},
  { NOTE_D7, 12 },
  { NOTE_B6, 12 },
  { NOTE_SILENT, 12 },
  { NOTE_SILENT, 12 },
  { NOTE_STOP, 0 }
};

const SNixieTempoNote marioNotes2[] =
{
    { NOTE_E5, 10},
    { NOTE_E5, 10},
    { NOTE_SILENT, 10},
    { NOTE_E5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_C5, 10}, 
    { NOTE_E5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_G5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_SILENT, 5}, 
    { NOTE_G4, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_SILENT, 5}, 
    { NOTE_C5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_G4, 10}, 
    { NOTE_SILENT, 5}, 
    { NOTE_E4, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_A4, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_B4, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_AS4, 10}, 
    { NOTE_A4, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_G4, 10}, 
    { NOTE_E5, 10}, 
    { NOTE_G5, 10}, 
    { NOTE_A5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_F5, 10}, 
    { NOTE_G5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_E5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_C5, 10}, 
    { NOTE_D5, 10}, 
    { NOTE_B4, 10}, 
    { NOTE_SILENT, 5}, 
    { NOTE_C5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_G4, 10}, 
    { NOTE_SILENT, 5}, 
    { NOTE_E4, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_A4, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_B4, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_AS4, 10}, 
    { NOTE_A4, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_G4, 10}, 
    { NOTE_E5, 10}, 
    { NOTE_G5, 10}, 
    { NOTE_A5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_F5, 10}, 
    { NOTE_G5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_E5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_C5, 10}, 
    { NOTE_D5, 10}, 
    { NOTE_B4, 10}, 
    { NOTE_SILENT, 5}, 
    { NOTE_SILENT, 5}, 
    { NOTE_G5, 10}, 
    { NOTE_FS5, 10}, 
    { NOTE_F5, 10}, 
    { 622, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_E5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_GS4, 10}, 
    { NOTE_A4, 10}, 
    { NOTE_C5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_A4, 10}, 
    { NOTE_C5, 10}, 
    { NOTE_D5, 10}, 
    { NOTE_SILENT, 5}, 
    { NOTE_G5, 10}, 
    { NOTE_FS5, 10}, 
    { NOTE_F5, 10}, 
    { 622, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_E5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_C6, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_C6, 10}, 
    { NOTE_C6, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_SILENT, 5}, 
    { NOTE_SILENT, 5}, 
    { NOTE_G5, 10}, 
    { NOTE_FS5, 10}, 
    { NOTE_F5, 10}, 
    { 622, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_E5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_GS4, 10}, 
    { NOTE_A4, 10}, 
    { NOTE_C5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_A4, 10}, 
    { NOTE_C5, 10}, 
    { NOTE_D5, 10}, 
    { NOTE_SILENT, 5}, 
    { 622, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_D5, 10}, 
    { NOTE_SILENT, 5}, 
    { NOTE_C5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_SILENT, 5}, 
    { NOTE_SILENT, 2}, 
    { NOTE_C5, 10}, 
    { NOTE_C5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_C5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_C5, 10}, 
    { NOTE_D5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_E5, 10}, 
    { NOTE_C5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_A4, 10}, 
    { NOTE_G4, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_SILENT, 5}, 
    { NOTE_C5, 10}, 
    { NOTE_C5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_C5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_C5, 10}, 
    { NOTE_D5, 10}, 
    { NOTE_E5, 10}, 
    { NOTE_SILENT, 2}, 
    { NOTE_SILENT, 2}, 
    { NOTE_C5, 10}, 
    { NOTE_C5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_C5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_C5, 10}, 
    { NOTE_D5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_E5, 10}, 
    { NOTE_C5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_A4, 10}, 
    { NOTE_G4, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_SILENT, 5}, 
    { NOTE_E5, 10}, 
    { NOTE_E5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_E5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_C5, 10}, 
    { NOTE_E5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_G5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_SILENT, 5}, 
    { NOTE_G4, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_SILENT, 5}, 
    { NOTE_C5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_G4, 10}, 
    { NOTE_SILENT, 5}, 
    { NOTE_E4, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_A4, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_B4, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_AS4, 10}, 
    { NOTE_A4, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_G4, 10}, 
    { NOTE_E5, 10}, 
    { NOTE_G5, 10}, 
    { NOTE_A5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_F5, 10}, 
    { NOTE_G5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_E5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_C5, 10}, 
    { NOTE_D5, 10}, 
    { NOTE_B4, 10}, 
    { NOTE_SILENT, 5}, 
    { NOTE_C5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_G4, 10}, 
    { NOTE_SILENT, 5}, 
    { NOTE_E4, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_A4, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_B4, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_AS4, 10}, 
    { NOTE_A4, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_G4, 10}, 
    { NOTE_E5, 10}, 
    { NOTE_G5, 10}, 
    { NOTE_A5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_F5, 10}, 
    { NOTE_G5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_E5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_C5, 10}, 
    { NOTE_D5, 10}, 
    { NOTE_B4, 10}, 
    { NOTE_SILENT, 5}, 
    { NOTE_E5, 10}, 
    { NOTE_C5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_G4, 10}, 
    { NOTE_SILENT, 5}, 
    { NOTE_GS4, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_A4, 10}, 
    { NOTE_F5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_F5, 10}, 
    { NOTE_A4, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_SILENT, 5}, 
    { NOTE_B4, 10}, 
    { NOTE_A5, 10}, 
    { NOTE_A5, 10}, 
    { NOTE_A5, 10}, 
    { NOTE_G5, 10}, 
    { NOTE_F5, 10}, 
    { NOTE_E5, 10}, 
    { NOTE_C5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_A4, 10}, 
    { NOTE_G4, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_SILENT, 5}, 
    { NOTE_E5, 10}, 
    { NOTE_C5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_G4, 10}, 
    { NOTE_SILENT, 5}, 
    { NOTE_GS4, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_A4, 10}, 
    { NOTE_F5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_F5, 10}, 
    { NOTE_A4, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_SILENT, 5}, 
    { NOTE_B4, 10}, 
    { NOTE_F5, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_F5, 10}, 
    { NOTE_F5, 10}, 
    { NOTE_E5, 10}, 
    { NOTE_D5, 10}, 
    { NOTE_C5, 10}, 
    { NOTE_E4, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_E4, 10}, 
    { NOTE_C4, 10}, 
    { NOTE_SILENT, 10}, 
    { NOTE_SILENT, 5},
    { NOTE_STOP, 0 },
};
 
const SNixieTempoNote underworld_notes[] = {
  { NOTE_C4, 8 },
  { NOTE_C5, 8},
  { NOTE_A3, 8 },
  { NOTE_A4, 8},
  { NOTE_AS3, 8 },
  { NOTE_AS4, 8},
  { NOTE_SILENT, 4 },
  { NOTE_SILENT, 2 },
  { NOTE_C4, 8 },
  { NOTE_C5, 8},
  { NOTE_A3, 8 },
  { NOTE_A4, 8},
  { NOTE_AS3, 8 },
  { NOTE_AS4, 8},
  { NOTE_SILENT, 4 },
  { NOTE_SILENT, 2 },
  { NOTE_F3, 8},
  { NOTE_F4, 8},
  { NOTE_D3, 8},
  { NOTE_D4, 8},
  { NOTE_DS3, 8},
  { NOTE_DS4, 8},
  { NOTE_SILENT, 4},
  { NOTE_SILENT, 2 },
  { NOTE_F3, 8},
  { NOTE_F4, 8},
  { NOTE_D3, 8},
  { NOTE_D4, 8},
  { NOTE_DS3, 8},
  { NOTE_DS4, 8},
  { NOTE_SILENT, 4},
  { NOTE_SILENT, 4},
  { NOTE_DS4, 12 },
  { NOTE_CS4, 12 },
  { NOTE_D4, 12 },
  { NOTE_CS4, 4},
  { NOTE_DS4, 4},
  { NOTE_DS4, 4},
  { NOTE_GS3, 4},
  { NOTE_G3, 4},
  { NOTE_CS4, 4},
  { NOTE_C4, 12},
  { NOTE_FS4, 12},
  { NOTE_F4, 12},
  { NOTE_E3, 12},
  { NOTE_AS4, 12},
  { NOTE_A4, 12},
  { NOTE_GS4, 7},
  { NOTE_DS4, 7},
  { NOTE_B3, 7},
  { NOTE_AS3, 7},
  { NOTE_A3, 7},
  { NOTE_GS3, 7},
  { NOTE_SILENT, 2},
  { NOTE_SILENT, 2},
  { NOTE_SILENT, 2},
  { NOTE_STOP, 0}
};

const SNixieTempoNote imperialMarch[] = {
    { NOTE_A3, 2},
    { NOTE_A3, 2},
    { NOTE_A3, 2},
    { NOTE_F3, 2},
    { NOTE_C4, 8},
    { NOTE_A3, 2},
    { NOTE_F3, 2},
    { NOTE_C4, 8},
    { NOTE_A3, 1},
    { NOTE_E4, 2},
    { NOTE_E4, 2},
    { NOTE_E4, 2},
    { NOTE_F4, 2},
    { NOTE_C4, 8},
    { NOTE_GS3, 2},
    { NOTE_F3, 2},
    { NOTE_C4, 8},
    { NOTE_A3, 1},
    { NOTE_A4, 2},
    { NOTE_A3, 2},
    { NOTE_A3, 8},
    { NOTE_A4, 2},
    { NOTE_GS4, 2},
    { NOTE_G4, 8},
    { NOTE_FS4, 8},
    { NOTE_E4, 8},
    { NOTE_F4, 4},
    { NOTE_AS3, 4},
    { NOTE_DS4, 2},
    { NOTE_D4, 2},
    { NOTE_CS4, 8},
    { NOTE_C4, 8},
    { NOTE_B3, 8},
    { NOTE_C4, 4},
    { NOTE_F3, 4},
    { NOTE_GS3, 2},
    { NOTE_F3, 2},
    { NOTE_A3, 8},
    { NOTE_C4, 2},
    { NOTE_A3, 2},
    { NOTE_C4, 8},
    { NOTE_E4, 1},
    { NOTE_A4, 2},
    { NOTE_A3, 2},
    { NOTE_A3, 8},
    { NOTE_A4, 2},
    { NOTE_GS4, 2},
    { NOTE_G4, 8},
    { NOTE_FS4, 8},
    { NOTE_E4, 8},
    { NOTE_F4, 4},
    { NOTE_AS3, 4},
    { NOTE_DS4, 2},
    { NOTE_D4, 2},
    { NOTE_CS4, 8},
    { NOTE_C4, 8},
    { NOTE_B3, 8},
    { NOTE_C4, 4},
    { NOTE_F3, 4},
    { NOTE_GS3, 2},
    { NOTE_F3, 2},
    { NOTE_C4, 8},
    { NOTE_A3, 2},
    { NOTE_F3, 2},
    { NOTE_C4, 8},
    { NOTE_A3, 1},
    { NOTE_STOP, 0 }
};

const SNixieTempoNote monkeyIsland[] =
{
{ NOTE_SILENT, 7},
{ NOTE_B3, 83},
{ NOTE_E4, 83},
{ NOTE_G4, 55},
{ NOTE_SILENT, 8},
{ NOTE_B3, 83},
{ NOTE_E4, 83},
{ NOTE_G4, 55},
{ NOTE_SILENT, 7},
{ NOTE_D3, 71},
{ NOTE_SILENT, 6},
{ NOTE_D4, 71},
{ NOTE_SILENT, 7},
{ NOTE_A3, 71},
{ NOTE_D4, 100},
{ NOTE_FS4, 52},
{ NOTE_D3, 24},
{ NOTE_SILENT, 12},
{ NOTE_A3, 71},
{ NOTE_D4, 100},
{ NOTE_FS4, 52},
{ NOTE_D3, 8},
{ NOTE_SILENT, 52},
{ NOTE_E3, 4},
{ NOTE_SILENT, 14},
{ NOTE_B3, 83},
{ NOTE_E4, 83},
{ NOTE_G4, 55},
{ NOTE_SILENT, 8},
{ NOTE_B3, 83},
{ NOTE_E4, 83},
{ NOTE_G4, 55},
{ NOTE_SILENT, 7},
{ NOTE_B4, 125},
{ NOTE_E5, 7},
{ NOTE_SILENT, 47},
{ NOTE_G4, 66},
{ NOTE_SILENT, 7},
{ NOTE_B3, 83},
{ NOTE_E4, 83},
{ NOTE_G4, 55},
{ NOTE_E5, 9},
{ NOTE_SILENT, 76},
{ NOTE_B3, 83},
{ NOTE_E4, 83},
{ NOTE_G4, 50},
{ NOTE_G5, 7},
{ NOTE_FS5, 5},
{ NOTE_SILENT, 250},
{ NOTE_E5, 7},
{ NOTE_A3, 71},
{ NOTE_D4, 71},
{ NOTE_FS4, 62},
{ NOTE_D5, 8},
{ NOTE_A3, 71},
{ NOTE_D4, 100},
{ NOTE_FS4, 62},
{ NOTE_SILENT, 7},
{ NOTE_SILENT, 125},
{ NOTE_SILENT, 200},
{ NOTE_E5, 3},
{ NOTE_G3, 100},
{ NOTE_C4, 66},
{ NOTE_E4, 66},
{ NOTE_C3, 11},
{ NOTE_SILENT, 25},
{ NOTE_G3, 100},
{ NOTE_C4, 66},
{ NOTE_E4, 55},
{ NOTE_C3, 6},
{ NOTE_G2, 3},
{ NOTE_SILENT, 50},
{ NOTE_G3, 100},
{ NOTE_B3, 62},
{ NOTE_D4, 58},
{ NOTE_D5, 8},
{ NOTE_G3, 66},
{ NOTE_B3, 83},
{ NOTE_D4, 71},
{ NOTE_SILENT, 7},
{ NOTE_G4, 125},
{ NOTE_D5, 6},
{ NOTE_C5, 6},
{ NOTE_G3, 66},
{ NOTE_B3, 62},
{ NOTE_D4, 71},
{ NOTE_B4, 8},
{ NOTE_G3, 66},
{ NOTE_B3, 62},
{ NOTE_D4, 58},
{ NOTE_D5, 7},
{ NOTE_C5, 6},
{ NOTE_SILENT, 111},
{ NOTE_C4, 66},
{ NOTE_SILENT, 7},
{ NOTE_A3, 71},
{ NOTE_C4, 90},
{ NOTE_E4, 55},
{ NOTE_C5, 8},
{ NOTE_A3, 71},
{ NOTE_C4, 90},
{ NOTE_E4, 66},
{ NOTE_SILENT, 8},
{ NOTE_B4, 4},
{ NOTE_SILENT, 11},
{ NOTE_G3, 100},
{ NOTE_B3, 62},
{ NOTE_E4, 66},
{ NOTE_E2, 27},
{ NOTE_SILENT, 12},
{ NOTE_SILENT, 100},
{ NOTE_SILENT, 200},
{ NOTE_B3, 62},
{ NOTE_E4, 66},
{ NOTE_E2, 41},
{ NOTE_SILENT, 125},
{ NOTE_B4, 166},
{ NOTE_E5, 58},
{ NOTE_E2, 27},
{ NOTE_SILENT, 166},
{ NOTE_G5, 90},
{ NOTE_SILENT, 83},
{ NOTE_SILENT, 142},
{ NOTE_B5, 100},
{ NOTE_E2, 41},
{ NOTE_SILENT, 7},
{ NOTE_E4, 66},
{ NOTE_SILENT, 7},
{ NOTE_E5, 111},
{ NOTE_B3, 83},
{ NOTE_E4, 83},
{ NOTE_G4, 55},
{ NOTE_E5, 13},
{ NOTE_SILENT, 20},
{ NOTE_B3, 83},
{ NOTE_E4, 83},
{ NOTE_G4, 55},
{ NOTE_SILENT, 7},
{ NOTE_B4, 125},
{ NOTE_E5, 3},
{ NOTE_B3, 83},
{ NOTE_E4, 83},
{ NOTE_G4, 55},
{ NOTE_SILENT, 8},
{ NOTE_B3, 83},
{ NOTE_E4, 111},
{ NOTE_G4, 55},
{ NOTE_G5, 6},
{ NOTE_FS5, 5},
{ NOTE_E5, 6},
{ NOTE_A3, 71},
{ NOTE_D4, 100},
{ NOTE_FS4, 52},
{ NOTE_D5, 8},
{ NOTE_A3, 71},
{ NOTE_D4, 71},
{ NOTE_FS4, 76},
{ NOTE_SILENT, 7},
{ NOTE_C3, 66},
{ NOTE_SILENT, 200},
{ NOTE_E5, 3},
{ NOTE_SILENT, 100},
{ NOTE_G3, 100},
{ NOTE_C4, 66},
{ NOTE_E4, 66},
{ NOTE_SILENT, 100},
{ NOTE_C3, 10},
{ NOTE_SILENT, 55},
{ NOTE_G3, 100},
{ NOTE_C4, 66},
{ NOTE_E4, 55},
{ NOTE_C3, 5},
{ NOTE_SILENT, 7},
{ NOTE_E4, 66},
{ NOTE_SILENT, 7},
{ NOTE_G3, 100},
{ NOTE_C4, 66},
{ NOTE_E4, 66},
{ NOTE_SILENT, 8},
{ NOTE_G3, 100},
{ NOTE_C4, 66},
{ NOTE_E4, 55},
{ NOTE_FS5, 8},
{ NOTE_G5, 6},
{ NOTE_SILENT, 45},
{ NOTE_B3, 62},
{ NOTE_SILENT, 7},
{ NOTE_G3, 100},
{ NOTE_B3, 83},
{ NOTE_D4, 58},
{ NOTE_G5, 35},
{ NOTE_SILENT, 255},
{ NOTE_G5, 10},
{ NOTE_G3, 66},
{ NOTE_B3, 83},
{ NOTE_D4, 71},
{ NOTE_SILENT, 7},
{ NOTE_A4, 200},
{ NOTE_A5, 3},
{ NOTE_A3, 55},
{ NOTE_A5, 142},
{ NOTE_C4, 66},
{ NOTE_SILENT, 8},
{ NOTE_A3, 55},
{ NOTE_C4, 43},
{ NOTE_SILENT, 6},
{ NOTE_FS5, 5},
{ NOTE_D3, 8},
{ NOTE_A3, 71},
{ NOTE_D4, 100},
{ NOTE_FS4, 52},
{ NOTE_D3, 50},
{ NOTE_SILENT, 9},
{ NOTE_A3, 71},
{ NOTE_D4, 142},
{ NOTE_FS4, 62},
{ NOTE_SILENT, 255},
{ NOTE_G5, 6},
{ NOTE_FS5, 6},
{ NOTE_SILENT, 47},
{ NOTE_E5, 6},
{ NOTE_A3, 111},
{ NOTE_D4, 58},
{ NOTE_FS4, 62},
{ NOTE_D5, 9},
{ NOTE_FS5, 125},
{ NOTE_A3, 71},
{ NOTE_D4, 100},
{ NOTE_FS4, 52},
{ NOTE_FS5, 8},
{ NOTE_G5, 255},
{ NOTE_SILENT, 255},
{ NOTE_G5, 6},
{ NOTE_SILENT, 55},
{ NOTE_G2, 7},
{ NOTE_SILENT, 166},
{ NOTE_G5, 76},
{ NOTE_B3, 83},
{ NOTE_D4, 71},
{ NOTE_G4, 55},
{ NOTE_G5, 8},
{ NOTE_B3, 83},
{ NOTE_D4, 71},
{ NOTE_G4, 66},
{ NOTE_SILENT, 7},
{ NOTE_FS5, 4},
{ NOTE_B2, 9},
{ NOTE_B3, 125},
{ NOTE_DS4, 76},
{ NOTE_FS4, 62},
{ NOTE_B2, 31},
{ NOTE_SILENT, 10},
{ NOTE_B3, 83},
{ NOTE_DS4, 76},
{ NOTE_FS4, 62},
{ NOTE_SILENT, 8},
{ NOTE_E5, 4},
{ NOTE_E3, 7},
{ NOTE_B3, 83},
{ NOTE_E4, 66},
{ NOTE_G4, 66},
{ NOTE_E3, 83},
{ NOTE_SILENT, 9},
{ NOTE_B3, 83},
{ NOTE_E4, 83},
{ NOTE_G4, 50},
{ NOTE_G5, 8},
{ NOTE_SILENT, 255},
{ NOTE_G5, 166},
{ NOTE_FS5, 5},
{ NOTE_E5, 6},
{ NOTE_A3, 71},
{ NOTE_D4, 71},
{ NOTE_FS4, 62},
{ NOTE_D5, 8},
{ NOTE_A3, 111},
{ NOTE_D4, 58},
{ NOTE_FS4, 62},
{ NOTE_FS5, 6},
{ NOTE_G5, 5},
{ NOTE_SILENT, 58},
{ NOTE_G2, 8},
{ NOTE_B3, 125},
{ NOTE_D4, 100},
{ NOTE_G4, 55},
{ NOTE_G5, 8},
{ NOTE_B3, 83},
{ NOTE_D4, 71},
{ NOTE_G4, 55},
{ NOTE_SILENT, 255},
{ NOTE_SILENT, 6},
{ NOTE_FS5, 5},
{ NOTE_B2, 7},
{ NOTE_B3, 83},
{ NOTE_DS4, 100},
{ NOTE_FS4, 62},
{ NOTE_SILENT, 8},
{ NOTE_B3, 83},
{ NOTE_DS4, 76},
{ NOTE_FS4, 62},
{ NOTE_SILENT, 6},
{ NOTE_E5, 5},
{ NOTE_E3, 7},
{ NOTE_B3, 83},
{ NOTE_E4, 83},
{ NOTE_G4, 66},
{ NOTE_SILENT, 8},
{ NOTE_B3, 83},
{ NOTE_E4, 83},
{ NOTE_G4, 50},
{ NOTE_G5, 7},
{ NOTE_FS5, 5},
{ NOTE_SILENT, 250},
{ NOTE_E5, 6},
{ NOTE_A3, 71},
{ NOTE_D4, 71},
{ NOTE_FS4, 62},
{ NOTE_D5, 8},
{ NOTE_A3, 71},
{ NOTE_D4, 71},
{ NOTE_FS4, 71},
{ NOTE_SILENT, 6},
{ NOTE_E5, 6},
{ NOTE_SILENT, 25},
{ NOTE_C3, 8},
{ NOTE_G3, 66},
{ NOTE_C4, 125},
{ NOTE_E4, 47},
{ NOTE_E5, 8},
{ NOTE_G3, 100},
{ NOTE_C4, 66},
{ NOTE_E4, 66},
{ NOTE_SILENT, 6},
{ NOTE_E5, 3},
{ NOTE_G3, 100},
{ NOTE_C4, 66},
{ NOTE_E4, 66},
{ NOTE_C3, 13},
{ NOTE_SILENT, 21},
{ NOTE_G3, 100},
{ NOTE_C4, 66},
{ NOTE_E4, 66},
{ NOTE_SILENT, 166},
{ NOTE_C3, 10},
{ NOTE_SILENT, 31},
{ NOTE_G4, 66},
{ NOTE_SILENT, 6},
{ NOTE_B3, 62},
{ NOTE_SILENT, 7},
{ NOTE_E5, 200},
{ NOTE_G3, 100},
{ NOTE_C4, 66},
{ NOTE_E4, 55},
{ NOTE_E5, 9},
{ NOTE_SILENT, 76},
{ NOTE_G3, 100},
{ NOTE_C4, 66},
{ NOTE_E4, 66},
{ NOTE_SILENT, 7},
{ NOTE_D5, 5},
{ NOTE_C5, 6},
{ NOTE_G3, 66},
{ NOTE_B3, 83},
{ NOTE_D4, 58},
{ NOTE_B4, 8},
{ NOTE_G3, 100},
{ NOTE_B3, 83},
{ NOTE_D4, 58},
{ NOTE_D5, 6},
{ NOTE_C5, 6},
{ NOTE_A2, 8},
{ NOTE_SILENT, 83},
{ NOTE_A3, 55},
{ NOTE_A4, 142},
{ NOTE_C4, 66},
{ NOTE_SILENT, 166},
{ NOTE_C5, 8},
{ NOTE_A3, 55},
{ NOTE_C5, 125},
{ NOTE_C4, 66},
{ NOTE_A2, 8},
{ NOTE_SILENT, 17},
{ NOTE_B4, 5},
{ NOTE_E2, 9},
{ NOTE_B3, 83},
{ NOTE_E4, 83},
{ NOTE_G4, 66},
{ NOTE_SILENT, 8},
{ NOTE_B3, 83},
{ NOTE_E4, 83},
{ NOTE_G4, 55},
{ NOTE_E2, 8},
{ NOTE_SILENT, 71},
{ NOTE_B4, 62},
{ NOTE_SILENT, 6},
{ NOTE_G4, 66},
{ NOTE_SILENT, 7},
{ NOTE_B3, 83},
{ NOTE_E4, 83},
{ NOTE_G4, 55},
{ NOTE_SILENT, 8},
{ NOTE_B3, 83},
{ NOTE_E4, 83},
{ NOTE_G4, 55},
{ NOTE_SILENT, 7},
{ NOTE_D3, 71},
{ NOTE_SILENT, 6},
{ NOTE_B2, 6},
{ NOTE_A3, 111},
{ NOTE_D4, 71},
{ NOTE_FS4, 52},
{ NOTE_D3, 9},
{ NOTE_SILENT, 52},
{ NOTE_A3, 71},
{ NOTE_D4, 100},
{ NOTE_FS4, 62},
{ NOTE_SILENT, 31},
{ NOTE_B4, 62},
{ NOTE_SILENT, 55},
{ NOTE_E5, 58},
{ NOTE_SILENT, 111},
{ NOTE_G5, 58},
{ NOTE_SILENT, 58},
{ NOTE_B5, 255},
{ NOTE_SILENT, 255},
{ NOTE_B5, 111},
{ NOTE_SILENT, 255},
{ NOTE_B5, 255},
{ NOTE_E3, 6},
{ NOTE_SILENT, 100},
{ NOTE_E4, 66},
{ NOTE_SILENT, 7},
{ NOTE_B3, 83},
{ NOTE_E4, 111},
{ NOTE_G4, 55},
{ NOTE_E3, 33},
{ NOTE_SILENT, 10},
{ NOTE_B3, 83},
{ NOTE_E4, 83},
{ NOTE_G4, 55},
{ NOTE_E3, 6},
{ NOTE_SILENT, 6},
{ NOTE_G4, 66},
{ NOTE_SILENT, 7},
{ NOTE_B3, 83},
{ NOTE_E4, 83},
{ NOTE_G4, 55},
{ NOTE_SILENT, 8},
{ NOTE_STOP, 0 },
};

const SNixieSamplingNote monkeyIslandP [] =
{
{ NOTE_SILENT, 137},
{ NOTE_B3, 12},
{ NOTE_E4, 12},
{ NOTE_G4, 18},
{ NOTE_SILENT, 123},
{ NOTE_B3, 12},
{ NOTE_E4, 12},
{ NOTE_G4, 18},
{ NOTE_SILENT, 135},
{ NOTE_D3, 14},
{ NOTE_SILENT, 155},
{ NOTE_D4, 14},
{ NOTE_SILENT, 139},
{ NOTE_A3, 14},
{ NOTE_D4, 10},
{ NOTE_FS4, 19},
{ NOTE_D3, 41},
{ NOTE_SILENT, 81},
{ NOTE_A3, 14},
{ NOTE_D4, 10},
{ NOTE_FS4, 19},
{ NOTE_D3, 116},
{ NOTE_SILENT, 19},
{ NOTE_E3, 249},
{ NOTE_SILENT, 68},
{ NOTE_B3, 12},
{ NOTE_E4, 12},
{ NOTE_G4, 18},
{ NOTE_SILENT, 123},
{ NOTE_B3, 12},
{ NOTE_E4, 12},
{ NOTE_G4, 18},
{ NOTE_SILENT, 135},
{ NOTE_B4, 8},
{ NOTE_E5, 135},
{ NOTE_SILENT, 21},
{ NOTE_G4, 15},
{ NOTE_SILENT, 137},
{ NOTE_B3, 12},
{ NOTE_E4, 12},
{ NOTE_G4, 18},
{ NOTE_E5, 109},
{ NOTE_SILENT, 13},
{ NOTE_B3, 12},
{ NOTE_E4, 12},
{ NOTE_G4, 20},
{ NOTE_G5, 141},
{ NOTE_FS5, 177},
{ NOTE_SILENT, 4},
{ NOTE_E5, 126},
{ NOTE_A3, 14},
{ NOTE_D4, 14},
{ NOTE_FS4, 16},
{ NOTE_D5, 123},
{ NOTE_A3, 14},
{ NOTE_D4, 10},
{ NOTE_FS4, 16},
{ NOTE_SILENT, 137},
{ NOTE_SILENT, 8},
{ NOTE_SILENT, 5},
{ NOTE_E5, 305},
{ NOTE_G3, 10},
{ NOTE_C4, 15},
{ NOTE_E4, 15},
{ NOTE_C3, 84},
{ NOTE_SILENT, 39},
{ NOTE_G3, 10},
{ NOTE_C4, 15},
{ NOTE_E4, 18},
{ NOTE_C3, 145},
{ NOTE_G2, 286},
{ NOTE_SILENT, 20},
{ NOTE_G3, 10},
{ NOTE_B3, 16},
{ NOTE_D4, 17},
{ NOTE_D5, 121},
{ NOTE_G3, 15},
{ NOTE_B3, 12},
{ NOTE_D4, 14},
{ NOTE_SILENT, 137},
{ NOTE_G4, 8},
{ NOTE_D5, 145},
{ NOTE_C5, 164},
{ NOTE_G3, 15},
{ NOTE_B3, 16},
{ NOTE_D4, 14},
{ NOTE_B4, 119},
{ NOTE_G3, 15},
{ NOTE_B3, 16},
{ NOTE_D4, 17},
{ NOTE_D5, 129},
{ NOTE_C5, 164},
{ NOTE_SILENT, 9},
{ NOTE_C4, 15},
{ NOTE_SILENT, 137},
{ NOTE_A3, 14},
{ NOTE_C4, 11},
{ NOTE_E4, 18},
{ NOTE_C5, 122},
{ NOTE_A3, 14},
{ NOTE_C4, 11},
{ NOTE_E4, 15},
{ NOTE_SILENT, 119},
{ NOTE_B4, 249},
{ NOTE_SILENT, 89},
{ NOTE_G3, 10},
{ NOTE_B3, 16},
{ NOTE_E4, 15},
{ NOTE_E2, 36},
{ NOTE_SILENT, 82},
{ NOTE_SILENT, 10},
{ NOTE_SILENT, 5},
{ NOTE_B3, 16},
{ NOTE_E4, 15},
{ NOTE_E2, 24},
{ NOTE_SILENT, 8},
{ NOTE_B4, 6},
{ NOTE_E5, 17},
{ NOTE_E2, 36},
{ NOTE_SILENT, 6},
{ NOTE_G5, 11},
{ NOTE_SILENT, 12},
{ NOTE_SILENT, 7},
{ NOTE_B5, 10},
{ NOTE_E2, 24},
{ NOTE_SILENT, 137},
{ NOTE_E4, 15},
{ NOTE_SILENT, 128},
{ NOTE_E5, 9},
{ NOTE_B3, 12},
{ NOTE_E4, 12},
{ NOTE_G4, 18},
{ NOTE_E5, 74},
{ NOTE_SILENT, 48},
{ NOTE_B3, 12},
{ NOTE_E4, 12},
{ NOTE_G4, 18},
{ NOTE_SILENT, 135},
{ NOTE_B4, 8},
{ NOTE_E5, 308},
{ NOTE_B3, 12},
{ NOTE_E4, 12},
{ NOTE_G4, 18},
{ NOTE_SILENT, 123},
{ NOTE_B3, 12},
{ NOTE_E4, 9},
{ NOTE_G4, 18},
{ NOTE_G5, 143},
{ NOTE_FS5, 169},
{ NOTE_E5, 144},
{ NOTE_A3, 14},
{ NOTE_D4, 10},
{ NOTE_FS4, 19},
{ NOTE_D5, 121},
{ NOTE_A3, 14},
{ NOTE_D4, 14},
{ NOTE_FS4, 13},
{ NOTE_SILENT, 137},
{ NOTE_C3, 15},
{ NOTE_SILENT, 5},
{ NOTE_E5, 287},
{ NOTE_SILENT, 10},
{ NOTE_G3, 10},
{ NOTE_C4, 15},
{ NOTE_E4, 15},
{ NOTE_SILENT, 10},
{ NOTE_C3, 92},
{ NOTE_SILENT, 18},
{ NOTE_G3, 10},
{ NOTE_C4, 15},
{ NOTE_E4, 18},
{ NOTE_C3, 176},
{ NOTE_SILENT, 127},
{ NOTE_E4, 15},
{ NOTE_SILENT, 137},
{ NOTE_G3, 10},
{ NOTE_C4, 15},
{ NOTE_E4, 15},
{ NOTE_SILENT, 124},
{ NOTE_G3, 10},
{ NOTE_C4, 15},
{ NOTE_E4, 18},
{ NOTE_FS5, 116},
{ NOTE_G5, 164},
{ NOTE_SILENT, 22},
{ NOTE_B3, 16},
{ NOTE_SILENT, 136},
{ NOTE_G3, 10},
{ NOTE_B3, 12},
{ NOTE_D4, 17},
{ NOTE_G5, 28},
{ NOTE_SILENT, 1},
{ NOTE_G5, 96},
{ NOTE_G3, 15},
{ NOTE_B3, 12},
{ NOTE_D4, 14},
{ NOTE_SILENT, 137},
{ NOTE_A4, 5},
{ NOTE_A5, 312},
{ NOTE_A3, 18},
{ NOTE_A5, 7},
{ NOTE_C4, 15},
{ NOTE_SILENT, 124},
{ NOTE_A3, 18},
{ NOTE_C4, 23},
{ NOTE_SILENT, 166},
{ NOTE_FS5, 173},
{ NOTE_D3, 123},
{ NOTE_A3, 14},
{ NOTE_D4, 10},
{ NOTE_FS4, 19},
{ NOTE_D3, 20},
{ NOTE_SILENT, 102},
{ NOTE_A3, 14},
{ NOTE_D4, 7},
{ NOTE_FS4, 16},
{ NOTE_SILENT, 3},
{ NOTE_G5, 143},
{ NOTE_FS5, 143},
{ NOTE_SILENT, 21},
{ NOTE_E5, 152},
{ NOTE_A3, 9},
{ NOTE_D4, 17},
{ NOTE_FS4, 16},
{ NOTE_D5, 111},
{ NOTE_FS5, 8},
{ NOTE_A3, 14},
{ NOTE_D4, 10},
{ NOTE_FS4, 19},
{ NOTE_FS5, 122},
{ NOTE_G5, 3},
{ NOTE_SILENT, 1},
{ NOTE_G5, 164},
{ NOTE_SILENT, 18},
{ NOTE_G2, 133},
{ NOTE_SILENT, 6},
{ NOTE_G5, 13},
{ NOTE_B3, 12},
{ NOTE_D4, 14},
{ NOTE_G4, 18},
{ NOTE_G5, 121},
{ NOTE_B3, 12},
{ NOTE_D4, 14},
{ NOTE_G4, 15},
{ NOTE_SILENT, 137},
{ NOTE_FS5, 220},
{ NOTE_B2, 105},
{ NOTE_B3, 8},
{ NOTE_DS4, 13},
{ NOTE_FS4, 16},
{ NOTE_B2, 32},
{ NOTE_SILENT, 91},
{ NOTE_B3, 12},
{ NOTE_DS4, 13},
{ NOTE_FS4, 16},
{ NOTE_SILENT, 124},
{ NOTE_E5, 204},
{ NOTE_E3, 134},
{ NOTE_B3, 12},
{ NOTE_E4, 15},
{ NOTE_G4, 15},
{ NOTE_E3, 12},
{ NOTE_SILENT, 111},
{ NOTE_B3, 12},
{ NOTE_E4, 12},
{ NOTE_G4, 20},
{ NOTE_G5, 125},
{ NOTE_SILENT, 1},
{ NOTE_G5, 6},
{ NOTE_FS5, 169},
{ NOTE_E5, 152},
{ NOTE_A3, 14},
{ NOTE_D4, 14},
{ NOTE_FS4, 16},
{ NOTE_D5, 123},
{ NOTE_A3, 9},
{ NOTE_D4, 17},
{ NOTE_FS4, 16},
{ NOTE_FS5, 143},
{ NOTE_G5, 181},
{ NOTE_SILENT, 17},
{ NOTE_G2, 122},
{ NOTE_B3, 8},
{ NOTE_D4, 10},
{ NOTE_G4, 18},
{ NOTE_G5, 125},
{ NOTE_B3, 12},
{ NOTE_D4, 14},
{ NOTE_G4, 18},
{ NOTE_SILENT, 1},
{ NOTE_SILENT, 157},
{ NOTE_FS5, 173},
{ NOTE_B2, 130},
{ NOTE_B3, 12},
{ NOTE_DS4, 10},
{ NOTE_FS4, 16},
{ NOTE_SILENT, 124},
{ NOTE_B3, 12},
{ NOTE_DS4, 13},
{ NOTE_FS4, 16},
{ NOTE_SILENT, 166},
{ NOTE_E5, 169},
{ NOTE_E3, 128},
{ NOTE_B3, 12},
{ NOTE_E4, 12},
{ NOTE_G4, 15},
{ NOTE_SILENT, 125},
{ NOTE_B3, 12},
{ NOTE_E4, 12},
{ NOTE_G4, 20},
{ NOTE_G5, 133},
{ NOTE_FS5, 169},
{ NOTE_SILENT, 4},
{ NOTE_E5, 152},
{ NOTE_A3, 14},
{ NOTE_D4, 14},
{ NOTE_FS4, 16},
{ NOTE_D5, 121},
{ NOTE_A3, 14},
{ NOTE_D4, 14},
{ NOTE_FS4, 14},
{ NOTE_SILENT, 146},
{ NOTE_E5, 155},
{ NOTE_SILENT, 39},
{ NOTE_C3, 115},
{ NOTE_G3, 15},
{ NOTE_C4, 8},
{ NOTE_E4, 21},
{ NOTE_E5, 125},
{ NOTE_G3, 10},
{ NOTE_C4, 15},
{ NOTE_E4, 15},
{ NOTE_SILENT, 145},
{ NOTE_E5, 317},
{ NOTE_G3, 10},
{ NOTE_C4, 15},
{ NOTE_E4, 15},
{ NOTE_C3, 76},
{ NOTE_SILENT, 47},
{ NOTE_G3, 10},
{ NOTE_C4, 15},
{ NOTE_E4, 15},
{ NOTE_SILENT, 6},
{ NOTE_C3, 99},
{ NOTE_SILENT, 32},
{ NOTE_G4, 15},
{ NOTE_SILENT, 154},
{ NOTE_B3, 16},
{ NOTE_SILENT, 132},
{ NOTE_E5, 5},
{ NOTE_G3, 10},
{ NOTE_C4, 15},
{ NOTE_E4, 18},
{ NOTE_E5, 108},
{ NOTE_SILENT, 13},
{ NOTE_G3, 10},
{ NOTE_C4, 15},
{ NOTE_E4, 15},
{ NOTE_SILENT, 137},
{ NOTE_D5, 177},
{ NOTE_C5, 143},
{ NOTE_G3, 15},
{ NOTE_B3, 12},
{ NOTE_D4, 17},
{ NOTE_B4, 121},
{ NOTE_G3, 10},
{ NOTE_B3, 12},
{ NOTE_D4, 17},
{ NOTE_D5, 163},
{ NOTE_C5, 160},
{ NOTE_A2, 118},
{ NOTE_SILENT, 12},
{ NOTE_A3, 18},
{ NOTE_A4, 7},
{ NOTE_C4, 15},
{ NOTE_SILENT, 6},
{ NOTE_C5, 118},
{ NOTE_A3, 18},
{ NOTE_C5, 8},
{ NOTE_C4, 15},
{ NOTE_A2, 118},
{ NOTE_SILENT, 56},
{ NOTE_B4, 178},
{ NOTE_E2, 109},
{ NOTE_B3, 12},
{ NOTE_E4, 12},
{ NOTE_G4, 15},
{ NOTE_SILENT, 125},
{ NOTE_B3, 12},
{ NOTE_E4, 12},
{ NOTE_G4, 18},
{ NOTE_E2, 122},
{ NOTE_SILENT, 14},
{ NOTE_B4, 16},
{ NOTE_SILENT, 153},
{ NOTE_G4, 15},
{ NOTE_SILENT, 137},
{ NOTE_B3, 12},
{ NOTE_E4, 12},
{ NOTE_G4, 18},
{ NOTE_SILENT, 123},
{ NOTE_B3, 12},
{ NOTE_E4, 12},
{ NOTE_G4, 18},
{ NOTE_SILENT, 135},
{ NOTE_D3, 14},
{ NOTE_SILENT, 155},
{ NOTE_B2, 154},
{ NOTE_A3, 9},
{ NOTE_D4, 14},
{ NOTE_FS4, 19},
{ NOTE_D3, 102},
{ NOTE_SILENT, 19},
{ NOTE_A3, 14},
{ NOTE_D4, 10},
{ NOTE_FS4, 16},
{ NOTE_SILENT, 32},
{ NOTE_B4, 16},
{ NOTE_SILENT, 18},
{ NOTE_E5, 17},
{ NOTE_SILENT, 9},
{ NOTE_G5, 17},
{ NOTE_SILENT, 17},
{ NOTE_B5, 3},
{ NOTE_SILENT, 1},
{ NOTE_B5, 9},
{ NOTE_SILENT, 1},
{ NOTE_B5, 3},
{ NOTE_E3, 146},
{ NOTE_SILENT, 10},
{ NOTE_E4, 15},
{ NOTE_SILENT, 137},
{ NOTE_B3, 12},
{ NOTE_E4, 9},
{ NOTE_G4, 18},
{ NOTE_E3, 30},
{ NOTE_SILENT, 95},
{ NOTE_B3, 12},
{ NOTE_E4, 12},
{ NOTE_G4, 18},
{ NOTE_E3, 152},
{ NOTE_SILENT, 152},
{ NOTE_G4, 15},
{ NOTE_SILENT, 137},
{ NOTE_B3, 12},
{ NOTE_E4, 12},
{ NOTE_G4, 18},
{ NOTE_SILENT, 123},
{ NOTE_STOP, 0 }
};

const NixieMelody  melodyMario = {
    .notes = (const uint8_t*)marioNotes,
    .type = MELODY_TYPE_PROGMEM_TEMPO,
    .pause = 30,
};

const NixieMelody  melodyMario2 = {
    .notes = (const uint8_t*)marioNotes2,
    .type = MELODY_TYPE_PROGMEM_TEMPO,
    .pause = 30,
};

const NixieMelody  melodyMarioUnderground = {
    .notes = (const uint8_t*)underworld_notes,
    .type = MELODY_TYPE_PROGMEM_TEMPO,
    .pause = 30,
};

const NixieMelody  melodyStarwars = {
    .notes = (const uint8_t*)imperialMarch,
    .type = MELODY_TYPE_PROGMEM_TEMPO,
    .pause = 30,
};

const NixieMelody  melodyMonkeyIsland = {
    .notes = (const uint8_t*)monkeyIsland,
    .type = MELODY_TYPE_PROGMEM_TEMPO,
    .pause = 10,
};

const NixieMelody  melodyMonkeyIslandP = {
    .notes = (const uint8_t*)monkeyIslandP,
    .type = MELODY_TYPE_PROGMEM_SAMPLING,
    .pause = -3, // negative value means that pause = note_duration * Value / 32
};


const SNixieTempoNote cancelNotes[] = {
  { 125, 24 }, { NOTE_SILENT, 16 }, { 125, 24 }, { NOTE_SILENT, 16 }, { 125, 8 },
  { NOTE_STOP, 0 },
};

const NixieMelody soundCancel = {
     .notes = (const uint8_t *)cancelNotes,
     .type = MELODY_TYPE_PROGMEM_TEMPO,
     .pause = 30,
};

const SNixieSamplingNote clickNotes[] = {
  { 4000, 20 },    { NOTE_SILENT, 10 }, { 2000, 20 }, // pick pick
  { NOTE_STOP, 0 },
};

const NixieMelody soundClick = {
     .notes = (const uint8_t *)clickNotes,
     .type = MELODY_TYPE_PROGMEM_SAMPLING,
     .pause = 30,
};

const SNixieTempoNote okNotes[] = {
  { 250, 32 }, { 500, 32 }, {750, 48}, {1000, 48}, {1250, 64}, // pick pick
  { NOTE_STOP, 0 },
};

const NixieMelody soundOk = {
     .notes = (const uint8_t *)okNotes,
     .type = MELODY_TYPE_PROGMEM_TEMPO,
     .pause = 30,
};


const SNixieTempoNote timeChange1[] = {
    { 1000, 16 }, { NOTE_SILENT, 16 }, {1000, 16},
    { NOTE_SILENT, 2},
    { 1000, 16 }, { NOTE_SILENT, 16 }, {1000, 16},
    { NOTE_STOP, 0 },
};

const SNixieSamplingNote doorBellNotes[] = {
    { 660, 700 }, { 550, 700 }, { 440, 700 },
    { NOTE_STOP, 0 },
};

const NixieMelody doorBell = {
     .notes = (const uint8_t *)doorBellNotes,
     .type = MELODY_TYPE_PROGMEM_SAMPLING,
     .pause = 5,
};

const NixieMelody soundClock = {
     .notes = (const uint8_t *)timeChange1,
     .type  = MELODY_TYPE_PROGMEM_TEMPO,
     .pause = 30,
};
