#include "laplace_model.hpp"

#include <cmath>
#include <cstdint>
#include <algorithm>

namespace model {
static double kVarianceTable[] = {0.015625, 0.03125, 0.0625, 0.125, 0.25, 0.5,   1.0,   2.0,
                                  4.0,      8.0,     16.0,   32.0,  64.0, 128.0, 256.0, 512.0};

static uint16_t cdf_0[] = {
    0,     1,     2,     3,     4,     5,     6,     7,     8,     9,     10,    11,    12,
    13,    14,    15,    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    40,    41,    42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
    52,    53,    54,    55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
    65,    66,    67,    68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
    78,    79,    80,    81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
    91,    92,    93,    94,    95,    96,    97,    98,    99,    100,   101,   102,   103,
    104,   105,   106,   107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
    117,   118,   119,   120,   121,   122,   123,   124,   125,   126,   127,   184,   32583,
    32640, 32641, 32643, 32644, 32645, 32646, 32647, 32648, 32649, 32650, 32651, 32652, 32653,
    32654, 32655, 32656, 32657, 32658, 32659, 32660, 32661, 32662, 32663, 32664, 32665, 32666,
    32667, 32668, 32669, 32670, 32671, 32672, 32673, 32674, 32675, 32676, 32677, 32678, 32679,
    32680, 32681, 32682, 32683, 32684, 32685, 32686, 32687, 32688, 32689, 32690, 32691, 32692,
    32693, 32694, 32695, 32696, 32697, 32698, 32699, 32700, 32701, 32702, 32703, 32704, 32705,
    32706, 32707, 32708, 32709, 32710, 32711, 32712, 32713, 32714, 32715, 32716, 32717, 32718,
    32719, 32720, 32721, 32722, 32723, 32724, 32725, 32726, 32727, 32728, 32729, 32730, 32731,
    32732, 32733, 32734, 32735, 32736, 32737, 32738, 32739, 32740, 32741, 32742, 32743, 32744,
    32745, 32746, 32747, 32748, 32749, 32750, 32751, 32752, 32753, 32754, 32755, 32756, 32757,
    32758, 32759, 32760, 32761, 32762, 32763, 32764, 32765, 32766, 32767, 32768,
};
static uint16_t cdf_1[] = {
    0,     1,     2,     3,     4,     5,     6,     7,     8,     9,     10,    11,    12,
    13,    14,    15,    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    40,    41,    42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
    52,    53,    54,    55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
    65,    66,    67,    68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
    78,    79,    80,    81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
    91,    92,    93,    94,    95,    96,    97,    98,    99,    100,   101,   102,   103,
    104,   105,   106,   107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
    117,   118,   119,   120,   121,   122,   123,   124,   125,   126,   127,   425,   32342,
    32640, 32641, 32642, 32643, 32645, 32646, 32647, 32648, 32649, 32650, 32651, 32652, 32653,
    32654, 32655, 32656, 32657, 32658, 32659, 32660, 32661, 32662, 32663, 32664, 32665, 32666,
    32667, 32668, 32669, 32670, 32671, 32672, 32673, 32674, 32675, 32676, 32677, 32678, 32679,
    32680, 32681, 32682, 32683, 32684, 32685, 32686, 32687, 32688, 32689, 32690, 32691, 32692,
    32693, 32694, 32695, 32696, 32697, 32698, 32699, 32700, 32701, 32702, 32703, 32704, 32705,
    32706, 32707, 32708, 32709, 32710, 32711, 32712, 32713, 32714, 32715, 32716, 32717, 32718,
    32719, 32720, 32721, 32722, 32723, 32724, 32725, 32726, 32727, 32728, 32729, 32730, 32731,
    32732, 32733, 32734, 32735, 32736, 32737, 32738, 32739, 32740, 32741, 32742, 32743, 32744,
    32745, 32746, 32747, 32748, 32749, 32750, 32751, 32752, 32753, 32754, 32755, 32756, 32757,
    32758, 32759, 32760, 32761, 32762, 32763, 32764, 32765, 32766, 32767, 32768,
};
static uint16_t cdf_2[] = {
    0,     1,     2,     3,     4,     5,     6,     7,     8,     9,     10,    11,    12,
    13,    14,    15,    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    40,    41,    42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
    52,    53,    54,    55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
    65,    66,    67,    68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
    78,    79,    80,    81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
    91,    92,    93,    94,    95,    96,    97,    98,    99,    100,   101,   102,   103,
    104,   105,   106,   107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
    117,   118,   119,   120,   121,   122,   123,   124,   125,   126,   130,   1088,  31679,
    32637, 32641, 32642, 32643, 32644, 32646, 32647, 32648, 32649, 32650, 32651, 32652, 32653,
    32654, 32655, 32656, 32657, 32658, 32659, 32660, 32661, 32662, 32663, 32664, 32665, 32666,
    32667, 32668, 32669, 32670, 32671, 32672, 32673, 32674, 32675, 32676, 32677, 32678, 32679,
    32680, 32681, 32682, 32683, 32684, 32685, 32686, 32687, 32688, 32689, 32690, 32691, 32692,
    32693, 32694, 32695, 32696, 32697, 32698, 32699, 32700, 32701, 32702, 32703, 32704, 32705,
    32706, 32707, 32708, 32709, 32710, 32711, 32712, 32713, 32714, 32715, 32716, 32717, 32718,
    32719, 32720, 32721, 32722, 32723, 32724, 32725, 32726, 32727, 32728, 32729, 32730, 32731,
    32732, 32733, 32734, 32735, 32736, 32737, 32738, 32739, 32740, 32741, 32742, 32743, 32744,
    32745, 32746, 32747, 32748, 32749, 32750, 32751, 32752, 32753, 32754, 32755, 32756, 32757,
    32758, 32759, 32760, 32761, 32762, 32763, 32764, 32765, 32766, 32767, 32768,
};
static uint16_t cdf_3[] = {
    0,     1,     2,     3,     4,     5,     6,     7,     8,     9,     10,    11,    12,
    13,    14,    15,    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    40,    41,    42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
    52,    53,    54,    55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
    65,    66,    67,    68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
    78,    79,    80,    81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
    91,    92,    93,    94,    95,    96,    97,    98,    99,    100,   101,   102,   103,
    104,   105,   106,   107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
    117,   118,   119,   120,   121,   122,   123,   124,   125,   126,   167,   2327,  30440,
    32600, 32641, 32642, 32643, 32644, 32645, 32646, 32647, 32649, 32650, 32651, 32652, 32653,
    32654, 32655, 32656, 32657, 32658, 32659, 32660, 32661, 32662, 32663, 32664, 32665, 32666,
    32667, 32668, 32669, 32670, 32671, 32672, 32673, 32674, 32675, 32676, 32677, 32678, 32679,
    32680, 32681, 32682, 32683, 32684, 32685, 32686, 32687, 32688, 32689, 32690, 32691, 32692,
    32693, 32694, 32695, 32696, 32697, 32698, 32699, 32700, 32701, 32702, 32703, 32704, 32705,
    32706, 32707, 32708, 32709, 32710, 32711, 32712, 32713, 32714, 32715, 32716, 32717, 32718,
    32719, 32720, 32721, 32722, 32723, 32724, 32725, 32726, 32727, 32728, 32729, 32730, 32731,
    32732, 32733, 32734, 32735, 32736, 32737, 32738, 32739, 32740, 32741, 32742, 32743, 32744,
    32745, 32746, 32747, 32748, 32749, 32750, 32751, 32752, 32753, 32754, 32755, 32756, 32757,
    32758, 32759, 32760, 32761, 32762, 32763, 32764, 32765, 32766, 32767, 32768,
};
static uint16_t cdf_4[] = {
    0,     1,     2,     3,     4,     5,     6,     7,     8,     9,     10,    11,    12,
    13,    14,    15,    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    40,    41,    42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
    52,    53,    54,    55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
    65,    66,    67,    68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
    78,    79,    80,    81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
    91,    92,    93,    94,    95,    96,    97,    98,    99,    100,   101,   102,   103,
    104,   105,   106,   107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
    117,   118,   119,   120,   121,   122,   123,   124,   125,   139,   360,   4079,  28688,
    32407, 32628, 32642, 32643, 32644, 32645, 32646, 32647, 32648, 32649, 32650, 32651, 32653,
    32654, 32655, 32656, 32657, 32658, 32659, 32660, 32661, 32662, 32663, 32664, 32665, 32666,
    32667, 32668, 32669, 32670, 32671, 32672, 32673, 32674, 32675, 32676, 32677, 32678, 32679,
    32680, 32681, 32682, 32683, 32684, 32685, 32686, 32687, 32688, 32689, 32690, 32691, 32692,
    32693, 32694, 32695, 32696, 32697, 32698, 32699, 32700, 32701, 32702, 32703, 32704, 32705,
    32706, 32707, 32708, 32709, 32710, 32711, 32712, 32713, 32714, 32715, 32716, 32717, 32718,
    32719, 32720, 32721, 32722, 32723, 32724, 32725, 32726, 32727, 32728, 32729, 32730, 32731,
    32732, 32733, 32734, 32735, 32736, 32737, 32738, 32739, 32740, 32741, 32742, 32743, 32744,
    32745, 32746, 32747, 32748, 32749, 32750, 32751, 32752, 32753, 32754, 32755, 32756, 32757,
    32758, 32759, 32760, 32761, 32762, 32763, 32764, 32765, 32766, 32767, 32768,
};
static uint16_t cdf_5[] = {
    0,     1,     2,     3,     4,     5,     6,     7,     8,     9,     10,    11,    12,
    13,    14,    15,    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    40,    41,    42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
    52,    53,    54,    55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
    65,    66,    67,    68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
    78,    79,    80,    81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
    91,    92,    93,    94,    95,    96,    97,    98,    99,    100,   101,   102,   103,
    104,   105,   106,   107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
    117,   118,   119,   120,   121,   122,   123,   126,   139,   235,   936,   6108,  26659,
    31831, 32532, 32628, 32641, 32644, 32645, 32646, 32647, 32648, 32649, 32650, 32651, 32652,
    32653, 32654, 32655, 32656, 32658, 32659, 32660, 32661, 32662, 32663, 32664, 32665, 32666,
    32667, 32668, 32669, 32670, 32671, 32672, 32673, 32674, 32675, 32676, 32677, 32678, 32679,
    32680, 32681, 32682, 32683, 32684, 32685, 32686, 32687, 32688, 32689, 32690, 32691, 32692,
    32693, 32694, 32695, 32696, 32697, 32698, 32699, 32700, 32701, 32702, 32703, 32704, 32705,
    32706, 32707, 32708, 32709, 32710, 32711, 32712, 32713, 32714, 32715, 32716, 32717, 32718,
    32719, 32720, 32721, 32722, 32723, 32724, 32725, 32726, 32727, 32728, 32729, 32730, 32731,
    32732, 32733, 32734, 32735, 32736, 32737, 32738, 32739, 32740, 32741, 32742, 32743, 32744,
    32745, 32746, 32747, 32748, 32749, 32750, 32751, 32752, 32753, 32754, 32755, 32756, 32757,
    32758, 32759, 32760, 32761, 32762, 32763, 32764, 32765, 32766, 32767, 32768,
};
static uint16_t cdf_6[] = {
    0,     1,     2,     3,     4,     5,     6,     7,     8,     9,     10,    11,    12,
    13,    14,    15,    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    40,    41,    42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
    52,    53,    54,    55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
    65,    66,    67,    68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
    78,    79,    80,    81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
    91,    92,    93,    94,    95,    96,    97,    98,    99,    100,   101,   102,   103,
    104,   105,   106,   107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
    117,   118,   119,   120,   121,   123,   129,   152,   240,   599,   2075,  8143,  24624,
    30692, 32168, 32527, 32615, 32638, 32644, 32646, 32647, 32648, 32649, 32650, 32651, 32652,
    32653, 32654, 32655, 32656, 32657, 32658, 32659, 32660, 32661, 32662, 32663, 32664, 32666,
    32667, 32668, 32669, 32670, 32671, 32672, 32673, 32674, 32675, 32676, 32677, 32678, 32679,
    32680, 32681, 32682, 32683, 32684, 32685, 32686, 32687, 32688, 32689, 32690, 32691, 32692,
    32693, 32694, 32695, 32696, 32697, 32698, 32699, 32700, 32701, 32702, 32703, 32704, 32705,
    32706, 32707, 32708, 32709, 32710, 32711, 32712, 32713, 32714, 32715, 32716, 32717, 32718,
    32719, 32720, 32721, 32722, 32723, 32724, 32725, 32726, 32727, 32728, 32729, 32730, 32731,
    32732, 32733, 32734, 32735, 32736, 32737, 32738, 32739, 32740, 32741, 32742, 32743, 32744,
    32745, 32746, 32747, 32748, 32749, 32750, 32751, 32752, 32753, 32754, 32755, 32756, 32757,
    32758, 32759, 32760, 32761, 32762, 32763, 32764, 32765, 32766, 32767, 32768,
};
static uint16_t cdf_7[] = {
    0,     1,     2,     3,     4,     5,     6,     7,     8,     9,     10,    11,    12,
    13,    14,    15,    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    40,    41,    42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
    52,    53,    54,    55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
    65,    66,    67,    68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
    78,    79,    80,    81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
    91,    92,    93,    94,    95,    96,    97,    98,    99,    100,   101,   102,   103,
    104,   105,   106,   107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
    117,   118,   120,   123,   129,   146,   189,   304,   615,   1460,  3754,  9987,  22780,
    29013, 31307, 32152, 32463, 32578, 32621, 32638, 32644, 32647, 32649, 32650, 32651, 32652,
    32653, 32654, 32655, 32656, 32657, 32658, 32659, 32660, 32661, 32662, 32663, 32664, 32665,
    32666, 32667, 32668, 32669, 32670, 32671, 32672, 32673, 32674, 32675, 32677, 32678, 32679,
    32680, 32681, 32682, 32683, 32684, 32685, 32686, 32687, 32688, 32689, 32690, 32691, 32692,
    32693, 32694, 32695, 32696, 32697, 32698, 32699, 32700, 32701, 32702, 32703, 32704, 32705,
    32706, 32707, 32708, 32709, 32710, 32711, 32712, 32713, 32714, 32715, 32716, 32717, 32718,
    32719, 32720, 32721, 32722, 32723, 32724, 32725, 32726, 32727, 32728, 32729, 32730, 32731,
    32732, 32733, 32734, 32735, 32736, 32737, 32738, 32739, 32740, 32741, 32742, 32743, 32744,
    32745, 32746, 32747, 32748, 32749, 32750, 32751, 32752, 32753, 32754, 32755, 32756, 32757,
    32758, 32759, 32760, 32761, 32762, 32763, 32764, 32765, 32766, 32767, 32768,
};
static uint16_t cdf_8[] = {
    0,     1,     2,     3,     4,     5,     6,     7,     8,     9,     10,    11,    12,
    13,    14,    15,    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    40,    41,    42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
    52,    53,    54,    55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
    65,    66,    67,    68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
    78,    79,    80,    81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
    91,    92,    93,    94,    95,    96,    97,    98,    99,    100,   101,   102,   103,
    104,   105,   106,   107,   108,   109,   110,   111,   112,   113,   114,   116,   118,
    121,   127,   138,   159,   201,   286,   455,   798,   1493,  2901,  5755,  11542, 21225,
    27012, 29866, 31274, 31969, 32312, 32481, 32566, 32608, 32629, 32640, 32646, 32649, 32651,
    32653, 32654, 32655, 32656, 32657, 32658, 32659, 32660, 32661, 32662, 32663, 32664, 32665,
    32666, 32667, 32668, 32669, 32670, 32671, 32672, 32673, 32674, 32675, 32676, 32677, 32678,
    32679, 32680, 32681, 32682, 32683, 32684, 32685, 32686, 32687, 32688, 32689, 32690, 32692,
    32693, 32694, 32695, 32696, 32697, 32698, 32699, 32700, 32701, 32702, 32703, 32704, 32705,
    32706, 32707, 32708, 32709, 32710, 32711, 32712, 32713, 32714, 32715, 32716, 32717, 32718,
    32719, 32720, 32721, 32722, 32723, 32724, 32725, 32726, 32727, 32728, 32729, 32730, 32731,
    32732, 32733, 32734, 32735, 32736, 32737, 32738, 32739, 32740, 32741, 32742, 32743, 32744,
    32745, 32746, 32747, 32748, 32749, 32750, 32751, 32752, 32753, 32754, 32755, 32756, 32757,
    32758, 32759, 32760, 32761, 32762, 32763, 32764, 32765, 32766, 32767, 32768,
};
static uint16_t cdf_9[] = {
    0,     1,     2,     3,     4,     5,     6,     7,     8,     9,     10,    11,    12,
    13,    14,    15,    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    40,    41,    42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
    52,    53,    54,    55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
    65,    66,    67,    68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
    78,    79,    80,    81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
    91,    92,    93,    94,    95,    96,    97,    98,    99,    100,   101,   102,   103,
    104,   105,   106,   107,   108,   109,   111,   113,   116,   120,   125,   134,   147,
    168,   203,   259,   351,   503,   752,   1162,  1837,  2949,  4783,  7805,  12787, 19980,
    24962, 27984, 29818, 30930, 31605, 32015, 32264, 32416, 32508, 32564, 32599, 32620, 32633,
    32642, 32647, 32651, 32654, 32656, 32658, 32659, 32660, 32661, 32662, 32663, 32664, 32665,
    32666, 32667, 32668, 32669, 32670, 32671, 32672, 32673, 32674, 32675, 32676, 32677, 32678,
    32679, 32680, 32681, 32682, 32683, 32684, 32685, 32686, 32687, 32688, 32689, 32690, 32691,
    32692, 32693, 32694, 32695, 32696, 32697, 32698, 32699, 32700, 32701, 32702, 32703, 32704,
    32705, 32706, 32707, 32708, 32709, 32710, 32711, 32713, 32714, 32715, 32716, 32717, 32718,
    32719, 32720, 32721, 32722, 32723, 32724, 32725, 32726, 32727, 32728, 32729, 32730, 32731,
    32732, 32733, 32734, 32735, 32736, 32737, 32738, 32739, 32740, 32741, 32742, 32743, 32744,
    32745, 32746, 32747, 32748, 32749, 32750, 32751, 32752, 32753, 32754, 32755, 32756, 32757,
    32758, 32759, 32760, 32761, 32762, 32763, 32764, 32765, 32766, 32767, 32768,
};
static uint16_t cdf_10[] = {
    0,     1,     2,     3,     4,     5,     6,     7,     8,     9,     10,    11,    12,
    13,    14,    15,    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    40,    41,    42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
    52,    53,    54,    55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
    65,    66,    67,    68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
    78,    79,    80,    81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
    91,    92,    93,    94,    95,    96,    97,    98,    99,    100,   101,   103,   104,
    106,   109,   111,   115,   119,   125,   133,   144,   159,   180,   210,   252,   311,
    395,   514,   684,   925,   1267,  1754,  2448,  3435,  4841,  6842,  9691,  13749, 19018,
    23076, 25925, 27926, 29332, 30319, 31013, 31500, 31842, 32083, 32253, 32372, 32456, 32515,
    32557, 32587, 32608, 32623, 32634, 32642, 32648, 32652, 32656, 32658, 32661, 32663, 32664,
    32666, 32667, 32668, 32669, 32670, 32671, 32672, 32673, 32674, 32675, 32676, 32677, 32678,
    32679, 32680, 32681, 32682, 32683, 32684, 32685, 32686, 32687, 32688, 32689, 32690, 32691,
    32692, 32693, 32694, 32695, 32696, 32697, 32698, 32699, 32700, 32701, 32702, 32703, 32704,
    32705, 32706, 32707, 32708, 32709, 32710, 32711, 32712, 32713, 32714, 32715, 32716, 32717,
    32718, 32719, 32720, 32721, 32722, 32723, 32724, 32725, 32726, 32727, 32728, 32729, 32730,
    32731, 32732, 32733, 32734, 32735, 32736, 32737, 32738, 32739, 32740, 32741, 32742, 32744,
    32745, 32746, 32747, 32748, 32749, 32750, 32751, 32752, 32753, 32754, 32755, 32756, 32757,
    32758, 32759, 32760, 32761, 32762, 32763, 32764, 32765, 32766, 32767, 32768,
};
static uint16_t cdf_11[] = {
    0,     1,     2,     3,     4,     5,     6,     7,     8,     9,     10,    11,    12,
    13,    14,    15,    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    40,    41,    42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
    52,    53,    54,    55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
    65,    66,    67,    68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
    78,    79,    80,    81,    82,    83,    84,    85,    86,    87,    88,    89,    91,
    92,    93,    95,    96,    98,    100,   103,   105,   109,   113,   117,   123,   130,
    139,   150,   164,   182,   204,   233,   269,   315,   374,   450,   547,   671,   830,
    1034,  1295,  1630,  2061,  2613,  3322,  4233,  5401,  6901,  8826,  11299, 14473, 18294,
    21468, 23941, 25866, 27366, 28534, 29445, 30154, 30706, 31137, 31472, 31733, 31937, 32096,
    32220, 32317, 32393, 32452, 32498, 32534, 32563, 32585, 32603, 32617, 32628, 32637, 32644,
    32650, 32654, 32658, 32662, 32664, 32667, 32669, 32671, 32672, 32674, 32675, 32676, 32678,
    32679, 32680, 32681, 32682, 32683, 32684, 32685, 32686, 32687, 32688, 32689, 32690, 32691,
    32692, 32693, 32694, 32695, 32696, 32697, 32698, 32699, 32700, 32701, 32702, 32703, 32704,
    32705, 32706, 32707, 32708, 32709, 32710, 32711, 32712, 32713, 32714, 32715, 32716, 32717,
    32718, 32719, 32720, 32721, 32722, 32723, 32724, 32725, 32726, 32727, 32728, 32729, 32730,
    32731, 32732, 32733, 32734, 32735, 32736, 32737, 32738, 32739, 32740, 32741, 32742, 32743,
    32744, 32745, 32746, 32747, 32748, 32749, 32750, 32751, 32752, 32753, 32754, 32755, 32756,
    32757, 32758, 32759, 32760, 32761, 32762, 32763, 32764, 32765, 32766, 32768,
};
static uint16_t cdf_12[] = {
    0,     1,     2,     3,     4,     5,     6,     7,     8,     9,     10,    11,    12,
    13,    14,    15,    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    40,    41,    42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
    52,    53,    54,    55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
    65,    66,    67,    68,    69,    70,    71,    72,    73,    75,    76,    77,    78,
    80,    81,    83,    84,    86,    88,    90,    92,    94,    97,    100,   104,   107,
    112,   117,   123,   130,   138,   147,   159,   172,   187,   205,   226,   252,   282,
    317,   360,   410,   470,   541,   626,   727,   848,   991,   1162,  1366,  1609,  1899,
    2245,  2658,  3150,  3737,  4438,  5273,  6271,  7461,  8880,  10574, 12596, 15008, 17759,
    20171, 22193, 23887, 25306, 26496, 27494, 28329, 29030, 29617, 30109, 30522, 30868, 31158,
    31401, 31605, 31776, 31919, 32040, 32141, 32226, 32297, 32357, 32407, 32450, 32485, 32515,
    32541, 32562, 32580, 32595, 32608, 32620, 32629, 32637, 32644, 32650, 32655, 32660, 32663,
    32667, 32670, 32673, 32675, 32677, 32679, 32681, 32683, 32684, 32686, 32687, 32689, 32690,
    32691, 32692, 32694, 32695, 32696, 32697, 32698, 32699, 32700, 32701, 32702, 32703, 32704,
    32705, 32706, 32707, 32708, 32709, 32710, 32711, 32712, 32713, 32714, 32715, 32716, 32717,
    32718, 32719, 32720, 32721, 32722, 32723, 32724, 32725, 32726, 32727, 32728, 32729, 32730,
    32731, 32732, 32733, 32734, 32735, 32736, 32737, 32738, 32739, 32740, 32741, 32742, 32743,
    32744, 32745, 32746, 32747, 32748, 32749, 32750, 32751, 32752, 32753, 32754, 32755, 32756,
    32757, 32758, 32759, 32760, 32761, 32762, 32763, 32764, 32765, 32766, 32768,
};
static uint16_t cdf_13[] = {
    0,     1,     2,     3,     4,     5,     6,     7,     8,     9,     10,    11,    12,
    13,    14,    15,    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    40,    41,    42,    43,    44,    45,    46,    47,    48,    49,    50,    52,
    53,    54,    55,    56,    57,    59,    60,    61,    63,    64,    65,    67,    69,
    70,    72,    74,    76,    78,    80,    83,    85,    88,    91,    95,    98,    103,
    107,   112,   117,   123,   130,   138,   146,   155,   166,   177,   190,   205,   222,
    240,   261,   285,   311,   341,   375,   413,   457,   505,   561,   623,   694,   773,
    864,   966,   1082,  1213,  1361,  1529,  1719,  1934,  2178,  2454,  2767,  3121,  3523,
    3978,  4493,  5076,  5737,  6486,  7335,  8296,  9386,  10620, 12018, 13603, 15398, 17369,
    19164, 20749, 22147, 23381, 24471, 25432, 26281, 27030, 27691, 28274, 28789, 29244, 29646,
    30000, 30313, 30589, 30833, 31048, 31238, 31406, 31554, 31685, 31801, 31903, 31994, 32073,
    32144, 32206, 32262, 32310, 32354, 32392, 32426, 32456, 32482, 32506, 32527, 32545, 32562,
    32577, 32590, 32601, 32612, 32621, 32629, 32637, 32644, 32650, 32655, 32660, 32664, 32669,
    32672, 32676, 32679, 32682, 32684, 32687, 32689, 32691, 32693, 32695, 32697, 32698, 32700,
    32702, 32703, 32704, 32706, 32707, 32708, 32710, 32711, 32712, 32713, 32714, 32715, 32717,
    32718, 32719, 32720, 32721, 32722, 32723, 32724, 32725, 32726, 32727, 32728, 32729, 32730,
    32731, 32732, 32733, 32734, 32735, 32736, 32737, 32738, 32739, 32740, 32741, 32742, 32743,
    32744, 32745, 32746, 32747, 32748, 32749, 32750, 32751, 32752, 32753, 32754, 32755, 32756,
    32757, 32758, 32759, 32760, 32761, 32762, 32763, 32764, 32765, 32766, 32768,
};
static uint16_t cdf_14[] = {
    0,     1,     2,     3,     4,     5,     6,     7,     8,     9,     10,    11,    12,
    13,    14,    15,    16,    17,    18,    20,    21,    22,    23,    24,    25,    26,
    27,    29,    30,    31,    32,    33,    35,    36,    37,    39,    40,    41,    43,
    44,    46,    48,    49,    51,    53,    55,    57,    59,    61,    63,    65,    68,
    70,    73,    76,    79,    82,    86,    89,    93,    98,    102,   107,   112,   118,
    124,   130,   137,   145,   153,   162,   171,   182,   193,   205,   218,   232,   248,
    265,   283,   303,   325,   348,   374,   402,   432,   465,   501,   541,   584,   630,
    681,   737,   798,   864,   936,   1015,  1101,  1195,  1297,  1409,  1531,  1664,  1809,
    1968,  2141,  2330,  2537,  2763,  3009,  3278,  3572,  3893,  4243,  4626,  5044,  5500,
    5999,  6544,  7138,  7788,  8498,  9273,  10120, 11044, 12055, 13158, 14364, 15680, 17087,
    18403, 19609, 20712, 21723, 22647, 23494, 24269, 24979, 25629, 26223, 26768, 27267, 27723,
    28141, 28524, 28874, 29195, 29489, 29758, 30004, 30230, 30437, 30626, 30799, 30958, 31103,
    31236, 31358, 31470, 31572, 31666, 31752, 31831, 31903, 31969, 32030, 32086, 32137, 32183,
    32226, 32266, 32302, 32335, 32365, 32393, 32419, 32442, 32464, 32484, 32502, 32519, 32535,
    32549, 32562, 32574, 32585, 32596, 32605, 32614, 32622, 32630, 32637, 32643, 32649, 32655,
    32660, 32665, 32669, 32674, 32678, 32681, 32685, 32688, 32691, 32694, 32697, 32699, 32702,
    32704, 32706, 32708, 32710, 32712, 32714, 32716, 32718, 32719, 32721, 32723, 32724, 32726,
    32727, 32728, 32730, 32731, 32732, 32734, 32735, 32736, 32737, 32738, 32740, 32741, 32742,
    32743, 32744, 32745, 32746, 32747, 32749, 32750, 32751, 32752, 32753, 32754, 32755, 32756,
    32757, 32758, 32759, 32760, 32761, 32762, 32763, 32764, 32765, 32766, 32768,
};
static uint16_t cdf_15[] = {
    0,     6,     7,     9,     10,    12,    13,    15,    16,    18,    19,    21,    23,
    24,    26,    28,    30,    32,    34,    36,    38,    40,    42,    45,    47,    50,
    52,    55,    58,    61,    64,    67,    71,    74,    78,    82,    86,    90,    94,
    99,    104,   109,   114,   120,   126,   133,   139,   146,   154,   162,   170,   179,
    188,   198,   208,   219,   231,   243,   256,   270,   284,   300,   316,   334,   352,
    372,   392,   415,   438,   463,   489,   517,   547,   579,   613,   648,   686,   727,
    770,   815,   864,   916,   970,   1029,  1091,  1157,  1227,  1301,  1381,  1465,  1555,
    1650,  1752,  1860,  1975,  2098,  2228,  2366,  2514,  2670,  2837,  3015,  3204,  3405,
    3619,  3847,  4089,  4347,  4621,  4914,  5225,  5555,  5908,  6282,  6681,  7106,  7558,
    8039,  8551,  9096,  9676,  10293, 10950, 11649, 12394, 13186, 14030, 14927, 15883, 16884,
    17840, 18737, 19581, 20373, 21118, 21817, 22474, 23091, 23671, 24216, 24728, 25209, 25661,
    26086, 26485, 26859, 27212, 27542, 27853, 28146, 28420, 28678, 28920, 29148, 29362, 29563,
    29752, 29930, 30097, 30253, 30401, 30539, 30669, 30792, 30907, 31015, 31117, 31212, 31302,
    31386, 31466, 31540, 31610, 31676, 31738, 31797, 31851, 31903, 31952, 31997, 32040, 32081,
    32119, 32154, 32188, 32220, 32250, 32278, 32304, 32329, 32352, 32375, 32395, 32415, 32433,
    32451, 32467, 32483, 32497, 32511, 32524, 32536, 32548, 32559, 32569, 32579, 32588, 32597,
    32605, 32613, 32621, 32628, 32634, 32641, 32647, 32653, 32658, 32663, 32668, 32673, 32677,
    32681, 32685, 32689, 32693, 32696, 32700, 32703, 32706, 32709, 32712, 32715, 32717, 32720,
    32722, 32725, 32727, 32729, 32731, 32733, 32735, 32737, 32739, 32741, 32743, 32744, 32746,
    32748, 32749, 32751, 32752, 32754, 32755, 32757, 32758, 32760, 32761, 32768,
};

static uint16_t* cdf_ptrs[] = {
    cdf_0, cdf_1, cdf_2,  cdf_3,  cdf_4,  cdf_5,  cdf_6,  cdf_7,
    cdf_8, cdf_9, cdf_10, cdf_11, cdf_12, cdf_13, cdf_14, cdf_15,
};

int cdf(int x, int i_var) {
    if (x <= -128) return 0;
    return cdf_ptrs[i_var][x + 128];
}

int icdf(int y, int i_var) {
    int l{-129}, r{129};
    while (l + 1 != r) {
        int mid = (l + r) / 2;
        int mid_cdf = cdf(mid, i_var);
        if (mid_cdf <= y && cdf(mid + 1, i_var) > y) {
            return mid;
        }
        if (mid_cdf <= y) {
            l = mid;
        } else {
            r = mid;
        }
    }
    return r;
}

int freq(int x, int i_var) { return cdf(x + 1, i_var) - cdf(x, i_var); }

uint8_t var_to_ivar(double var) {
    return std::max(std::min(int(log2(std::min(std::max(var, 0.001), 1024.0))) + 6, 15), 0);
}

double ivar_to_var(int v) { return kVarianceTable[std::min(std::max(0, v), 15)]; }

int scale() {
    return 15;
}
}  // namespace model