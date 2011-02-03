/*
    OpenDCP: Builds Digital Cinema Packages
    Copyright (c) 2010 Terrence Meiczinger, All Rights Reserved

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

/* Nominalized gamma lookup tables
 *   The purpose of these tables are to reduce use of the pow function during the rgb->xyz conversion.
 *   The pow function is quite slow and having to it 12 million times adds up. This will at least 
 *   speed up the ingress gamma conversion. These tables can be easily generated for different bit
 *   depths and reference levels.
 */


/* Bit Depth:       12
 * Reference White: SRGB (Gamma 2.6) 
 */

#ifndef _OPEN_DCP_IMAGE_H_
#define _OPEN_DCP_IMAGE_H_

enum LUT_GAMMA_ENUM {
	SRGB_GAMMA_SIMPLE = 0,
	SRGB_GAMMA_COMPLEX,
        MAX_GAMMA
};

static float *lut_gamma[MAX_GAMMA];
static float srgb_gamma_simple[4096] = {
    0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000001,0.000001,0.000001,0.000001,0.000002,0.000002,0.000003,0.000003,0.000004,
    0.000004,0.000005,0.000006,0.000007,0.000007,0.000008,0.000009,0.000010,0.000011,0.000012,0.000013,0.000015,0.000016,0.000017,0.000019,
    0.000020,0.000022,0.000023,0.000025,0.000026,0.000028,0.000030,0.000032,0.000034,0.000036,0.000038,0.000040,0.000042,0.000044,0.000047,
    0.000049,0.000051,0.000054,0.000056,0.000059,0.000062,0.000065,0.000067,0.000070,0.000073,0.000076,0.000079,0.000082,0.000086,0.000089,
    0.000092,0.000096,0.000099,0.000103,0.000106,0.000110,0.000114,0.000118,0.000121,0.000125,0.000129,0.000134,0.000138,0.000142,0.000146,
    0.000151,0.000155,0.000160,0.000164,0.000169,0.000174,0.000179,0.000183,0.000188,0.000193,0.000199,0.000204,0.000209,0.000214,0.000220,
    0.000225,0.000231,0.000236,0.000242,0.000248,0.000254,0.000259,0.000265,0.000271,0.000278,0.000284,0.000290,0.000296,0.000303,0.000309,
    0.000316,0.000323,0.000329,0.000336,0.000343,0.000350,0.000357,0.000364,0.000371,0.000379,0.000386,0.000393,0.000401,0.000408,0.000416,
    0.000424,0.000432,0.000440,0.000448,0.000456,0.000464,0.000472,0.000480,0.000489,0.000497,0.000505,0.000514,0.000523,0.000532,0.000540,
    0.000549,0.000558,0.000567,0.000576,0.000586,0.000595,0.000604,0.000614,0.000623,0.000633,0.000643,0.000653,0.000662,0.000672,0.000682,
    0.000693,0.000703,0.000713,0.000723,0.000734,0.000744,0.000755,0.000766,0.000776,0.000787,0.000798,0.000809,0.000820,0.000831,0.000843,
    0.000854,0.000866,0.000877,0.000889,0.000900,0.000912,0.000924,0.000936,0.000948,0.000960,0.000972,0.000984,0.000997,0.001009,0.001022,
    0.001034,0.001047,0.001060,0.001073,0.001086,0.001099,0.001112,0.001125,0.001138,0.001151,0.001165,0.001178,0.001192,0.001206,0.001220,
    0.001233,0.001247,0.001261,0.001276,0.001290,0.001304,0.001318,0.001333,0.001348,0.001362,0.001377,0.001392,0.001407,0.001422,0.001437,
    0.001452,0.001467,0.001482,0.001498,0.001513,0.001529,0.001545,0.001560,0.001576,0.001592,0.001608,0.001624,0.001641,0.001657,0.001673,
    0.001690,0.001706,0.001723,0.001740,0.001757,0.001774,0.001791,0.001808,0.001825,0.001842,0.001859,0.001877,0.001894,0.001912,0.001930,
    0.001948,0.001966,0.001984,0.002002,0.002020,0.002038,0.002056,0.002075,0.002093,0.002112,0.002131,0.002149,0.002168,0.002187,0.002206,
    0.002226,0.002245,0.002264,0.002284,0.002303,0.002323,0.002342,0.002362,0.002382,0.002402,0.002422,0.002442,0.002462,0.002483,0.002503,
    0.002524,0.002544,0.002565,0.002586,0.002607,0.002628,0.002649,0.002670,0.002691,0.002713,0.002734,0.002755,0.002777,0.002799,0.002821,
    0.002842,0.002864,0.002887,0.002909,0.002931,0.002953,0.002976,0.002998,0.003021,0.003044,0.003067,0.003089,0.003112,0.003136,0.003159,
    0.003182,0.003205,0.003229,0.003252,0.003276,0.003300,0.003324,0.003348,0.003372,0.003396,0.003420,0.003444,0.003469,0.003493,0.003518,
    0.003543,0.003567,0.003592,0.003617,0.003642,0.003668,0.003693,0.003718,0.003744,0.003769,0.003795,0.003820,0.003846,0.003872,0.003898,
    0.003924,0.003951,0.003977,0.004003,0.004030,0.004056,0.004083,0.004110,0.004137,0.004164,0.004191,0.004218,0.004245,0.004273,0.004300,
    0.004328,0.004355,0.004383,0.004411,0.004439,0.004467,0.004495,0.004523,0.004551,0.004580,0.004608,0.004637,0.004666,0.004694,0.004723,
    0.004752,0.004781,0.004811,0.004840,0.004869,0.004899,0.004928,0.004958,0.004988,0.005018,0.005048,0.005078,0.005108,0.005138,0.005168,
    0.005199,0.005229,0.005260,0.005291,0.005322,0.005353,0.005384,0.005415,0.005446,0.005477,0.005509,0.005540,0.005572,0.005604,0.005635,
    0.005667,0.005699,0.005732,0.005764,0.005796,0.005828,0.005861,0.005894,0.005926,0.005959,0.005992,0.006025,0.006058,0.006091,0.006125,
    0.006158,0.006192,0.006225,0.006259,0.006293,0.006327,0.006361,0.006395,0.006429,0.006463,0.006498,0.006532,0.006567,0.006601,0.006636,
    0.006671,0.006706,0.006741,0.006776,0.006812,0.006847,0.006882,0.006918,0.006954,0.006990,0.007025,0.007061,0.007098,0.007134,0.007170,
    0.007206,0.007243,0.007279,0.007316,0.007353,0.007390,0.007427,0.007464,0.007501,0.007538,0.007576,0.007613,0.007651,0.007689,0.007727,
    0.007764,0.007802,0.007841,0.007879,0.007917,0.007955,0.007994,0.008033,0.008071,0.008110,0.008149,0.008188,0.008227,0.008266,0.008306,
    0.008345,0.008385,0.008424,0.008464,0.008504,0.008544,0.008584,0.008624,0.008664,0.008705,0.008745,0.008786,0.008826,0.008867,0.008908,
    0.008949,0.008990,0.009031,0.009072,0.009114,0.009155,0.009197,0.009239,0.009280,0.009322,0.009364,0.009406,0.009449,0.009491,0.009533,
    0.009576,0.009618,0.009661,0.009704,0.009747,0.009790,0.009833,0.009876,0.009920,0.009963,0.010007,0.010050,0.010094,0.010138,0.010182,
    0.010226,0.010270,0.010314,0.010359,0.010403,0.010448,0.010492,0.010537,0.010582,0.010627,0.010672,0.010717,0.010763,0.010808,0.010854,
    0.010899,0.010945,0.010991,0.011037,0.011083,0.011129,0.011175,0.011221,0.011268,0.011314,0.011361,0.011408,0.011455,0.011502,0.011549,
    0.011596,0.011643,0.011691,0.011738,0.011786,0.011834,0.011881,0.011929,0.011977,0.012025,0.012074,0.012122,0.012170,0.012219,0.012268,
    0.012316,0.012365,0.012414,0.012463,0.012513,0.012562,0.012611,0.012661,0.012710,0.012760,0.012810,0.012860,0.012910,0.012960,0.013010,
    0.013061,0.013111,0.013162,0.013212,0.013263,0.013314,0.013365,0.013416,0.013467,0.013519,0.013570,0.013622,0.013673,0.013725,0.013777,
    0.013829,0.013881,0.013933,0.013985,0.014038,0.014090,0.014143,0.014195,0.014248,0.014301,0.014354,0.014407,0.014461,0.014514,0.014567,
    0.014621,0.014675,0.014728,0.014782,0.014836,0.014890,0.014944,0.014999,0.015053,0.015108,0.015162,0.015217,0.015272,0.015327,0.015382,
    0.015437,0.015492,0.015548,0.015603,0.015659,0.015715,0.015770,0.015826,0.015882,0.015939,0.015995,0.016051,0.016108,0.016164,0.016221,
    0.016278,0.016335,0.016392,0.016449,0.016506,0.016563,0.016621,0.016678,0.016736,0.016794,0.016851,0.016909,0.016968,0.017026,0.017084,
    0.017142,0.017201,0.017260,0.017318,0.017377,0.017436,0.017495,0.017554,0.017614,0.017673,0.017733,0.017792,0.017852,0.017912,0.017972,
    0.018032,0.018092,0.018152,0.018213,0.018273,0.018334,0.018394,0.018455,0.018516,0.018577,0.018638,0.018700,0.018761,0.018822,0.018884,
    0.018946,0.019007,0.019069,0.019131,0.019194,0.019256,0.019318,0.019381,0.019443,0.019506,0.019569,0.019632,0.019695,0.019758,0.019821,
    0.019884,0.019948,0.020011,0.020075,0.020139,0.020203,0.020267,0.020331,0.020395,0.020459,0.020524,0.020588,0.020653,0.020718,0.020783,
    0.020848,0.020913,0.020978,0.021043,0.021109,0.021174,0.021240,0.021306,0.021372,0.021438,0.021504,0.021570,0.021636,0.021703,0.021769,
    0.021836,0.021903,0.021970,0.022037,0.022104,0.022171,0.022238,0.022306,0.022373,0.022441,0.022509,0.022577,0.022645,0.022713,0.022781,
    0.022849,0.022918,0.022986,0.023055,0.023124,0.023193,0.023262,0.023331,0.023400,0.023469,0.023539,0.023608,0.023678,0.023748,0.023818,
    0.023888,0.023958,0.024028,0.024099,0.024169,0.024240,0.024310,0.024381,0.024452,0.024523,0.024594,0.024665,0.024737,0.024808,0.024880,
    0.024951,0.025023,0.025095,0.025167,0.025239,0.025312,0.025384,0.025457,0.025529,0.025602,0.025675,0.025748,0.025821,0.025894,0.025967,
    0.026041,0.026114,0.026188,0.026261,0.026335,0.026409,0.026483,0.026557,0.026632,0.026706,0.026781,0.026855,0.026930,0.027005,0.027080,
    0.027155,0.027230,0.027305,0.027381,0.027456,0.027532,0.027608,0.027684,0.027760,0.027836,0.027912,0.027988,0.028065,0.028141,0.028218,
    0.028295,0.028372,0.028449,0.028526,0.028603,0.028681,0.028758,0.028836,0.028913,0.028991,0.029069,0.029147,0.029225,0.029304,0.029382,
    0.029460,0.029539,0.029618,0.029697,0.029776,0.029855,0.029934,0.030013,0.030093,0.030172,0.030252,0.030332,0.030411,0.030491,0.030572,
    0.030652,0.030732,0.030813,0.030893,0.030974,0.031055,0.031136,0.031217,0.031298,0.031379,0.031460,0.031542,0.031623,0.031705,0.031787,
    0.031869,0.031951,0.032033,0.032115,0.032198,0.032280,0.032363,0.032446,0.032529,0.032612,0.032695,0.032778,0.032861,0.032945,0.033028,
    0.033112,0.033196,0.033280,0.033364,0.033448,0.033532,0.033616,0.033701,0.033785,0.033870,0.033955,0.034040,0.034125,0.034210,0.034295,
    0.034381,0.034466,0.034552,0.034638,0.034724,0.034810,0.034896,0.034982,0.035068,0.035155,0.035241,0.035328,0.035415,0.035502,0.035589,
    0.035676,0.035763,0.035851,0.035938,0.036026,0.036113,0.036201,0.036289,0.036377,0.036465,0.036554,0.036642,0.036731,0.036819,0.036908,
    0.036997,0.037086,0.037175,0.037265,0.037354,0.037443,0.037533,0.037623,0.037712,0.037802,0.037892,0.037983,0.038073,0.038163,0.038254,
    0.038345,0.038435,0.038526,0.038617,0.038708,0.038800,0.038891,0.038982,0.039074,0.039166,0.039258,0.039349,0.039441,0.039534,0.039626,
    0.039718,0.039811,0.039904,0.039996,0.040089,0.040182,0.040275,0.040369,0.040462,0.040555,0.040649,0.040743,0.040836,0.040930,0.041024,
    0.041119,0.041213,0.041307,0.041402,0.041496,0.041591,0.041686,0.041781,0.041876,0.041971,0.042067,0.042162,0.042258,0.042354,0.042449,
    0.042545,0.042641,0.042738,0.042834,0.042930,0.043027,0.043123,0.043220,0.043317,0.043414,0.043511,0.043608,0.043706,0.043803,0.043901,
    0.043999,0.044096,0.044194,0.044292,0.044391,0.044489,0.044587,0.044686,0.044785,0.044883,0.044982,0.045081,0.045180,0.045280,0.045379,
    0.045479,0.045578,0.045678,0.045778,0.045878,0.045978,0.046078,0.046178,0.046279,0.046379,0.046480,0.046581,0.046682,0.046783,0.046884,
    0.046985,0.047087,0.047188,0.047290,0.047392,0.047493,0.047595,0.047698,0.047800,0.047902,0.048005,0.048107,0.048210,0.048313,0.048416,
    0.048519,0.048622,0.048725,0.048829,0.048932,0.049036,0.049140,0.049244,0.049348,0.049452,0.049556,0.049661,0.049765,0.049870,0.049974,
    0.050079,0.050184,0.050289,0.050395,0.050500,0.050605,0.050711,0.050817,0.050923,0.051028,0.051135,0.051241,0.051347,0.051453,0.051560,
    0.051667,0.051773,0.051880,0.051987,0.052095,0.052202,0.052309,0.052417,0.052524,0.052632,0.052740,0.052848,0.052956,0.053064,0.053173,
    0.053281,0.053390,0.053498,0.053607,0.053716,0.053825,0.053935,0.054044,0.054153,0.054263,0.054373,0.054482,0.054592,0.054702,0.054812,
    0.054923,0.055033,0.055144,0.055254,0.055365,0.055476,0.055587,0.055698,0.055809,0.055921,0.056032,0.056144,0.056256,0.056368,0.056479,
    0.056592,0.056704,0.056816,0.056929,0.057041,0.057154,0.057267,0.057380,0.057493,0.057606,0.057719,0.057833,0.057946,0.058060,0.058174,
    0.058288,0.058402,0.058516,0.058630,0.058745,0.058859,0.058974,0.059089,0.059203,0.059318,0.059434,0.059549,0.059664,0.059780,0.059895,
    0.060011,0.060127,0.060243,0.060359,0.060475,0.060592,0.060708,0.060825,0.060942,0.061058,0.061175,0.061292,0.061410,0.061527,0.061644,
    0.061762,0.061880,0.061998,0.062116,0.062234,0.062352,0.062470,0.062589,0.062707,0.062826,0.062945,0.063064,0.063183,0.063302,0.063421,
    0.063540,0.063660,0.063780,0.063899,0.064019,0.064139,0.064260,0.064380,0.064500,0.064621,0.064741,0.064862,0.064983,0.065104,0.065225,
    0.065346,0.065468,0.065589,0.065711,0.065833,0.065955,0.066077,0.066199,0.066321,0.066443,0.066566,0.066688,0.066811,0.066934,0.067057,
    0.067180,0.067303,0.067427,0.067550,0.067674,0.067797,0.067921,0.068045,0.068169,0.068293,0.068418,0.068542,0.068667,0.068792,0.068916,
    0.069041,0.069166,0.069292,0.069417,0.069542,0.069668,0.069794,0.069919,0.070045,0.070171,0.070298,0.070424,0.070550,0.070677,0.070804,
    0.070930,0.071057,0.071184,0.071312,0.071439,0.071566,0.071694,0.071822,0.071949,0.072077,0.072205,0.072333,0.072462,0.072590,0.072719,
    0.072847,0.072976,0.073105,0.073234,0.073363,0.073493,0.073622,0.073752,0.073881,0.074011,0.074141,0.074271,0.074401,0.074531,0.074662,
    0.074792,0.074923,0.075054,0.075185,0.075316,0.075447,0.075578,0.075709,0.075841,0.075973,0.076104,0.076236,0.076368,0.076500,0.076633,
    0.076765,0.076898,0.077030,0.077163,0.077296,0.077429,0.077562,0.077695,0.077829,0.077962,0.078096,0.078230,0.078364,0.078498,0.078632,
    0.078766,0.078900,0.079035,0.079170,0.079304,0.079439,0.079574,0.079709,0.079845,0.079980,0.080116,0.080251,0.080387,0.080523,0.080659,
    0.080795,0.080931,0.081068,0.081204,0.081341,0.081478,0.081615,0.081752,0.081889,0.082026,0.082163,0.082301,0.082439,0.082576,0.082714,
    0.082852,0.082990,0.083129,0.083267,0.083406,0.083544,0.083683,0.083822,0.083961,0.084100,0.084239,0.084379,0.084518,0.084658,0.084798,
    0.084938,0.085078,0.085218,0.085358,0.085499,0.085639,0.085780,0.085921,0.086061,0.086202,0.086344,0.086485,0.086626,0.086768,0.086910,
    0.087051,0.087193,0.087335,0.087478,0.087620,0.087762,0.087905,0.088048,0.088190,0.088333,0.088476,0.088619,0.088763,0.088906,0.089050,
    0.089194,0.089337,0.089481,0.089625,0.089770,0.089914,0.090058,0.090203,0.090348,0.090492,0.090637,0.090782,0.090928,0.091073,0.091218,
    0.091364,0.091510,0.091656,0.091802,0.091948,0.092094,0.092240,0.092387,0.092533,0.092680,0.092827,0.092974,0.093121,0.093268,0.093416,
    0.093563,0.093711,0.093858,0.094006,0.094154,0.094302,0.094451,0.094599,0.094748,0.094896,0.095045,0.095194,0.095343,0.095492,0.095641,
    0.095791,0.095940,0.096090,0.096240,0.096390,0.096540,0.096690,0.096840,0.096990,0.097141,0.097292,0.097442,0.097593,0.097744,0.097896,
    0.098047,0.098198,0.098350,0.098502,0.098653,0.098805,0.098957,0.099110,0.099262,0.099414,0.099567,0.099720,0.099873,0.100026,0.100179,
    0.100332,0.100485,0.100639,0.100792,0.100946,0.101100,0.101254,0.101408,0.101562,0.101717,0.101871,0.102026,0.102180,0.102335,0.102490,
    0.102646,0.102801,0.102956,0.103112,0.103267,0.103423,0.103579,0.103735,0.103891,0.104048,0.104204,0.104360,0.104517,0.104674,0.104831,
    0.104988,0.105145,0.105302,0.105460,0.105617,0.105775,0.105933,0.106091,0.106249,0.106407,0.106566,0.106724,0.106883,0.107041,0.107200,
    0.107359,0.107518,0.107678,0.107837,0.107997,0.108156,0.108316,0.108476,0.108636,0.108796,0.108956,0.109117,0.109277,0.109438,0.109599,
    0.109760,0.109921,0.110082,0.110243,0.110404,0.110566,0.110728,0.110890,0.111051,0.111214,0.111376,0.111538,0.111701,0.111863,0.112026,
    0.112189,0.112352,0.112515,0.112678,0.112841,0.113005,0.113169,0.113332,0.113496,0.113660,0.113824,0.113989,0.114153,0.114318,0.114482,
    0.114647,0.114812,0.114977,0.115142,0.115307,0.115473,0.115638,0.115804,0.115970,0.116136,0.116302,0.116468,0.116634,0.116801,0.116968,
    0.117134,0.117301,0.117468,0.117635,0.117803,0.117970,0.118137,0.118305,0.118473,0.118641,0.118809,0.118977,0.119145,0.119314,0.119482,
    0.119651,0.119820,0.119989,0.120158,0.120327,0.120496,0.120666,0.120835,0.121005,0.121175,0.121345,0.121515,0.121685,0.121855,0.122026,
    0.122196,0.122367,0.122538,0.122709,0.122880,0.123052,0.123223,0.123394,0.123566,0.123738,0.123910,0.124082,0.124254,0.124426,0.124599,
    0.124771,0.124944,0.125117,0.125290,0.125463,0.125636,0.125810,0.125983,0.126157,0.126330,0.126504,0.126678,0.126852,0.127027,0.127201,
    0.127376,0.127550,0.127725,0.127900,0.128075,0.128250,0.128426,0.128601,0.128777,0.128952,0.129128,0.129304,0.129480,0.129656,0.129833,
    0.130009,0.130186,0.130363,0.130540,0.130717,0.130894,0.131071,0.131248,0.131426,0.131604,0.131781,0.131959,0.132137,0.132316,0.132494,
    0.132672,0.132851,0.133030,0.133209,0.133387,0.133567,0.133746,0.133925,0.134105,0.134284,0.134464,0.134644,0.134824,0.135004,0.135184,
    0.135365,0.135545,0.135726,0.135907,0.136088,0.136269,0.136450,0.136632,0.136813,0.136995,0.137176,0.137358,0.137540,0.137722,0.137905,
    0.138087,0.138270,0.138452,0.138635,0.138818,0.139001,0.139184,0.139367,0.139551,0.139734,0.139918,0.140102,0.140286,0.140470,0.140654,
    0.140839,0.141023,0.141208,0.141393,0.141577,0.141762,0.141948,0.142133,0.142318,0.142504,0.142690,0.142875,0.143061,0.143247,0.143434,
    0.143620,0.143806,0.143993,0.144180,0.144367,0.144554,0.144741,0.144928,0.145115,0.145303,0.145491,0.145678,0.145866,0.146054,0.146243,
    0.146431,0.146619,0.146808,0.146997,0.147186,0.147375,0.147564,0.147753,0.147942,0.148132,0.148321,0.148511,0.148701,0.148891,0.149081,
    0.149272,0.149462,0.149653,0.149843,0.150034,0.150225,0.150416,0.150608,0.150799,0.150990,0.151182,0.151374,0.151566,0.151758,0.151950,
    0.152142,0.152335,0.152527,0.152720,0.152913,0.153106,0.153299,0.153492,0.153685,0.153879,0.154072,0.154266,0.154460,0.154654,0.154848,
    0.155043,0.155237,0.155432,0.155626,0.155821,0.156016,0.156211,0.156406,0.156602,0.156797,0.156993,0.157189,0.157384,0.157580,0.157777,
    0.157973,0.158169,0.158366,0.158563,0.158759,0.158956,0.159153,0.159351,0.159548,0.159745,0.159943,0.160141,0.160339,0.160537,0.160735,
    0.160933,0.161132,0.161330,0.161529,0.161728,0.161927,0.162126,0.162325,0.162524,0.162724,0.162923,0.163123,0.163323,0.163523,0.163723,
    0.163923,0.164124,0.164324,0.164525,0.164726,0.164927,0.165128,0.165329,0.165530,0.165732,0.165934,0.166135,0.166337,0.166539,0.166741,
    0.166944,0.167146,0.167349,0.167551,0.167754,0.167957,0.168160,0.168363,0.168567,0.168770,0.168974,0.169178,0.169382,0.169586,0.169790,
    0.169994,0.170199,0.170403,0.170608,0.170813,0.171018,0.171223,0.171428,0.171633,0.171839,0.172044,0.172250,0.172456,0.172662,0.172868,
    0.173075,0.173281,0.173488,0.173694,0.173901,0.174108,0.174315,0.174523,0.174730,0.174937,0.175145,0.175353,0.175561,0.175769,0.175977,
    0.176185,0.176394,0.176602,0.176811,0.177020,0.177229,0.177438,0.177647,0.177857,0.178066,0.178276,0.178486,0.178696,0.178906,0.179116,
    0.179326,0.179537,0.179747,0.179958,0.180169,0.180380,0.180591,0.180803,0.181014,0.181226,0.181437,0.181649,0.181861,0.182073,0.182285,
    0.182498,0.182710,0.182923,0.183136,0.183348,0.183562,0.183775,0.183988,0.184201,0.184415,0.184629,0.184843,0.185057,0.185271,0.185485,
    0.185699,0.185914,0.186129,0.186343,0.186558,0.186773,0.186989,0.187204,0.187419,0.187635,0.187851,0.188067,0.188283,0.188499,0.188715,
    0.188931,0.189148,0.189365,0.189581,0.189798,0.190015,0.190233,0.190450,0.190668,0.190885,0.191103,0.191321,0.191539,0.191757,0.191975,
    0.192194,0.192412,0.192631,0.192850,0.193069,0.193288,0.193507,0.193727,0.193946,0.194166,0.194386,0.194606,0.194826,0.195046,0.195266,
    0.195487,0.195707,0.195928,0.196149,0.196370,0.196591,0.196813,0.197034,0.197256,0.197477,0.197699,0.197921,0.198143,0.198365,0.198588,
    0.198810,0.199033,0.199256,0.199479,0.199702,0.199925,0.200148,0.200372,0.200595,0.200819,0.201043,0.201267,0.201491,0.201715,0.201940,
    0.202164,0.202389,0.202614,0.202839,0.203064,0.203289,0.203515,0.203740,0.203966,0.204192,0.204418,0.204644,0.204870,0.205096,0.205323,
    0.205549,0.205776,0.206003,0.206230,0.206457,0.206684,0.206912,0.207139,0.207367,0.207595,0.207823,0.208051,0.208279,0.208507,0.208736,
    0.208965,0.209193,0.209422,0.209651,0.209881,0.210110,0.210339,0.210569,0.210799,0.211029,0.211259,0.211489,0.211719,0.211949,0.212180,
    0.212411,0.212642,0.212873,0.213104,0.213335,0.213566,0.213798,0.214029,0.214261,0.214493,0.214725,0.214957,0.215190,0.215422,0.215655,
    0.215888,0.216121,0.216354,0.216587,0.216820,0.217053,0.217287,0.217521,0.217755,0.217989,0.218223,0.218457,0.218691,0.218926,0.219161,
    0.219395,0.219630,0.219865,0.220101,0.220336,0.220571,0.220807,0.221043,0.221279,0.221515,0.221751,0.221987,0.222224,0.222460,0.222697,
    0.222934,0.223171,0.223408,0.223645,0.223883,0.224120,0.224358,0.224596,0.224834,0.225072,0.225310,0.225549,0.225787,0.226026,0.226264,
    0.226503,0.226742,0.226982,0.227221,0.227460,0.227700,0.227940,0.228180,0.228420,0.228660,0.228900,0.229141,0.229381,0.229622,0.229863,
    0.230104,0.230345,0.230586,0.230828,0.231069,0.231311,0.231553,0.231795,0.232037,0.232279,0.232521,0.232764,0.233006,0.233249,0.233492,
    0.233735,0.233978,0.234222,0.234465,0.234709,0.234953,0.235197,0.235441,0.235685,0.235929,0.236173,0.236418,0.236663,0.236908,0.237153,
    0.237398,0.237643,0.237888,0.238134,0.238380,0.238625,0.238871,0.239117,0.239364,0.239610,0.239857,0.240103,0.240350,0.240597,0.240844,
    0.241091,0.241339,0.241586,0.241834,0.242081,0.242329,0.242577,0.242826,0.243074,0.243322,0.243571,0.243820,0.244068,0.244317,0.244567,
    0.244816,0.245065,0.245315,0.245565,0.245814,0.246064,0.246315,0.246565,0.246815,0.247066,0.247316,0.247567,0.247818,0.248069,0.248320,
    0.248572,0.248823,0.249075,0.249327,0.249579,0.249831,0.250083,0.250335,0.250588,0.250840,0.251093,0.251346,0.251599,0.251852,0.252105,
    0.252359,0.252612,0.252866,0.253120,0.253374,0.253628,0.253882,0.254137,0.254391,0.254646,0.254901,0.255156,0.255411,0.255666,0.255922,
    0.256177,0.256433,0.256689,0.256945,0.257201,0.257457,0.257713,0.257970,0.258226,0.258483,0.258740,0.258997,0.259254,0.259512,0.259769,
    0.260027,0.260284,0.260542,0.260800,0.261059,0.261317,0.261575,0.261834,0.262093,0.262352,0.262611,0.262870,0.263129,0.263388,0.263648,
    0.263908,0.264167,0.264427,0.264688,0.264948,0.265208,0.265469,0.265729,0.265990,0.266251,0.266512,0.266774,0.267035,0.267297,0.267558,
    0.267820,0.268082,0.268344,0.268606,0.268869,0.269131,0.269394,0.269656,0.269919,0.270182,0.270446,0.270709,0.270972,0.271236,0.271500,
    0.271764,0.272028,0.272292,0.272556,0.272821,0.273085,0.273350,0.273615,0.273880,0.274145,0.274410,0.274676,0.274941,0.275207,0.275473,
    0.275739,0.276005,0.276271,0.276538,0.276804,0.277071,0.277338,0.277605,0.277872,0.278139,0.278407,0.278674,0.278942,0.279210,0.279478,
    0.279746,0.280014,0.280282,0.280551,0.280819,0.281088,0.281357,0.281626,0.281895,0.282165,0.282434,0.282704,0.282974,0.283244,0.283514,
    0.283784,0.284054,0.284325,0.284595,0.284866,0.285137,0.285408,0.285679,0.285951,0.286222,0.286494,0.286765,0.287037,0.287309,0.287581,
    0.287854,0.288126,0.288399,0.288672,0.288944,0.289217,0.289491,0.289764,0.290037,0.290311,0.290585,0.290858,0.291132,0.291407,0.291681,
    0.291955,0.292230,0.292505,0.292779,0.293054,0.293329,0.293605,0.293880,0.294156,0.294431,0.294707,0.294983,0.295259,0.295535,0.295812,
    0.296088,0.296365,0.296642,0.296919,0.297196,0.297473,0.297751,0.298028,0.298306,0.298584,0.298861,0.299140,0.299418,0.299696,0.299975,
    0.300253,0.300532,0.300811,0.301090,0.301369,0.301649,0.301928,0.302208,0.302488,0.302767,0.303048,0.303328,0.303608,0.303889,0.304169,
    0.304450,0.304731,0.305012,0.305293,0.305574,0.305856,0.306137,0.306419,0.306701,0.306983,0.307265,0.307548,0.307830,0.308113,0.308395,
    0.308678,0.308961,0.309245,0.309528,0.309811,0.310095,0.310379,0.310662,0.310946,0.311231,0.311515,0.311799,0.312084,0.312369,0.312654,
    0.312939,0.313224,0.313509,0.313794,0.314080,0.314366,0.314652,0.314938,0.315224,0.315510,0.315796,0.316083,0.316370,0.316657,0.316944,
    0.317231,0.317518,0.317805,0.318093,0.318381,0.318668,0.318956,0.319245,0.319533,0.319821,0.320110,0.320399,0.320687,0.320976,0.321265,
    0.321555,0.321844,0.322134,0.322423,0.322713,0.323003,0.323293,0.323584,0.323874,0.324164,0.324455,0.324746,0.325037,0.325328,0.325619,
    0.325911,0.326202,0.326494,0.326786,0.327078,0.327370,0.327662,0.327954,0.328247,0.328540,0.328832,0.329125,0.329419,0.329712,0.330005,
    0.330299,0.330592,0.330886,0.331180,0.331474,0.331768,0.332063,0.332357,0.332652,0.332947,0.333242,0.333537,0.333832,0.334128,0.334423,
    0.334719,0.335015,0.335310,0.335607,0.335903,0.336199,0.336496,0.336792,0.337089,0.337386,0.337683,0.337980,0.338278,0.338575,0.338873,
    0.339171,0.339469,0.339767,0.340065,0.340363,0.340662,0.340961,0.341259,0.341558,0.341857,0.342157,0.342456,0.342756,0.343055,0.343355,
    0.343655,0.343955,0.344255,0.344556,0.344856,0.345157,0.345458,0.345759,0.346060,0.346361,0.346662,0.346964,0.347265,0.347567,0.347869,
    0.348171,0.348473,0.348776,0.349078,0.349381,0.349684,0.349987,0.350290,0.350593,0.350897,0.351200,0.351504,0.351807,0.352111,0.352416,
    0.352720,0.353024,0.353329,0.353633,0.353938,0.354243,0.354548,0.354853,0.355159,0.355464,0.355770,0.356076,0.356382,0.356688,0.356994,
    0.357300,0.357607,0.357914,0.358221,0.358527,0.358835,0.359142,0.359449,0.359757,0.360064,0.360372,0.360680,0.360988,0.361297,0.361605,
    0.361913,0.362222,0.362531,0.362840,0.363149,0.363458,0.363768,0.364077,0.364387,0.364697,0.365007,0.365317,0.365627,0.365938,0.366248,
    0.366559,0.366870,0.367181,0.367492,0.367803,0.368114,0.368426,0.368738,0.369049,0.369361,0.369674,0.369986,0.370298,0.370611,0.370924,
    0.371236,0.371549,0.371863,0.372176,0.372489,0.372803,0.373116,0.373430,0.373744,0.374058,0.374373,0.374687,0.375002,0.375316,0.375631,
    0.375946,0.376262,0.376577,0.376892,0.377208,0.377524,0.377839,0.378155,0.378472,0.378788,0.379104,0.379421,0.379738,0.380055,0.380372,
    0.380689,0.381006,0.381324,0.381641,0.381959,0.382277,0.382595,0.382913,0.383231,0.383550,0.383868,0.384187,0.384506,0.384825,0.385144,
    0.385464,0.385783,0.386103,0.386422,0.386742,0.387062,0.387383,0.387703,0.388023,0.388344,0.388665,0.388986,0.389307,0.389628,0.389949,
    0.390271,0.390593,0.390914,0.391236,0.391558,0.391881,0.392203,0.392525,0.392848,0.393171,0.393494,0.393817,0.394140,0.394464,0.394787,
    0.395111,0.395435,0.395759,0.396083,0.396407,0.396731,0.397056,0.397380,0.397705,0.398030,0.398355,0.398681,0.399006,0.399332,0.399657,
    0.399983,0.400309,0.400635,0.400962,0.401288,0.401615,0.401941,0.402268,0.402595,0.402922,0.403250,0.403577,0.403905,0.404232,0.404560,
    0.404888,0.405216,0.405545,0.405873,0.406202,0.406530,0.406859,0.407188,0.407517,0.407847,0.408176,0.408506,0.408836,0.409166,0.409496,
    0.409826,0.410156,0.410487,0.410817,0.411148,0.411479,0.411810,0.412141,0.412472,0.412804,0.413136,0.413467,0.413799,0.414131,0.414464,
    0.414796,0.415129,0.415461,0.415794,0.416127,0.416460,0.416793,0.417127,0.417460,0.417794,0.418128,0.418462,0.418796,0.419130,0.419464,
    0.419799,0.420134,0.420469,0.420804,0.421139,0.421474,0.421809,0.422145,0.422481,0.422816,0.423152,0.423489,0.423825,0.424161,0.424498,
    0.424835,0.425172,0.425509,0.425846,0.426183,0.426521,0.426858,0.427196,0.427534,0.427872,0.428210,0.428548,0.428887,0.429225,0.429564,
    0.429903,0.430242,0.430581,0.430921,0.431260,0.431600,0.431940,0.432280,0.432620,0.432960,0.433300,0.433641,0.433982,0.434322,0.434663,
    0.435004,0.435346,0.435687,0.436029,0.436370,0.436712,0.437054,0.437396,0.437739,0.438081,0.438424,0.438766,0.439109,0.439452,0.439795,
    0.440139,0.440482,0.440826,0.441169,0.441513,0.441857,0.442201,0.442546,0.442890,0.443235,0.443580,0.443925,0.444270,0.444615,0.444960,
    0.445306,0.445651,0.445997,0.446343,0.446689,0.447035,0.447382,0.447728,0.448075,0.448422,0.448769,0.449116,0.449463,0.449810,0.450158,
    0.450506,0.450853,0.451201,0.451549,0.451898,0.452246,0.452595,0.452943,0.453292,0.453641,0.453990,0.454340,0.454689,0.455039,0.455388,
    0.455738,0.456088,0.456439,0.456789,0.457139,0.457490,0.457841,0.458192,0.458543,0.458894,0.459245,0.459597,0.459948,0.460300,0.460652,
    0.461004,0.461356,0.461709,0.462061,0.462414,0.462767,0.463120,0.463473,0.463826,0.464180,0.464533,0.464887,0.465241,0.465595,0.465949,
    0.466303,0.466657,0.467012,0.467367,0.467722,0.468077,0.468432,0.468787,0.469143,0.469498,0.469854,0.470210,0.470566,0.470922,0.471278,
    0.471635,0.471992,0.472348,0.472705,0.473062,0.473420,0.473777,0.474134,0.474492,0.474850,0.475208,0.475566,0.475924,0.476283,0.476641,
    0.477000,0.477359,0.477718,0.478077,0.478436,0.478796,0.479155,0.479515,0.479875,0.480235,0.480595,0.480955,0.481316,0.481676,0.482037,
    0.482398,0.482759,0.483120,0.483482,0.483843,0.484205,0.484567,0.484928,0.485291,0.485653,0.486015,0.486378,0.486740,0.487103,0.487466,
    0.487829,0.488193,0.488556,0.488920,0.489283,0.489647,0.490011,0.490375,0.490740,0.491104,0.491469,0.491833,0.492198,0.492563,0.492928,
    0.493294,0.493659,0.494025,0.494391,0.494757,0.495123,0.495489,0.495855,0.496222,0.496588,0.496955,0.497322,0.497689,0.498056,0.498424,
    0.498791,0.499159,0.499527,0.499895,0.500263,0.500631,0.501000,0.501368,0.501737,0.502106,0.502475,0.502844,0.503213,0.503583,0.503953,
    0.504322,0.504692,0.505062,0.505432,0.505803,0.506173,0.506544,0.506915,0.507286,0.507657,0.508028,0.508399,0.508771,0.509143,0.509515,
    0.509886,0.510259,0.510631,0.511003,0.511376,0.511749,0.512121,0.512495,0.512868,0.513241,0.513614,0.513988,0.514362,0.514736,0.515110,
    0.515484,0.515858,0.516233,0.516608,0.516982,0.517357,0.517732,0.518108,0.518483,0.518859,0.519234,0.519610,0.519986,0.520362,0.520738,
    0.521115,0.521491,0.521868,0.522245,0.522622,0.522999,0.523377,0.523754,0.524132,0.524509,0.524887,0.525265,0.525644,0.526022,0.526400,
    0.526779,0.527158,0.527537,0.527916,0.528295,0.528675,0.529054,0.529434,0.529814,0.530194,0.530574,0.530954,0.531335,0.531715,0.532096,
    0.532477,0.532858,0.533239,0.533620,0.534002,0.534383,0.534765,0.535147,0.535529,0.535911,0.536294,0.536676,0.537059,0.537442,0.537825,
    0.538208,0.538591,0.538975,0.539358,0.539742,0.540126,0.540510,0.540894,0.541278,0.541663,0.542047,0.542432,0.542817,0.543202,0.543587,
    0.543972,0.544358,0.544744,0.545129,0.545515,0.545901,0.546288,0.546674,0.547061,0.547447,0.547834,0.548221,0.548608,0.548996,0.549383,
    0.549771,0.550158,0.550546,0.550934,0.551322,0.551711,0.552099,0.552488,0.552877,0.553266,0.553655,0.554044,0.554433,0.554823,0.555212,
    0.555602,0.555992,0.556382,0.556773,0.557163,0.557554,0.557944,0.558335,0.558726,0.559117,0.559509,0.559900,0.560292,0.560684,0.561075,
    0.561468,0.561860,0.562252,0.562645,0.563037,0.563430,0.563823,0.564216,0.564609,0.565003,0.565396,0.565790,0.566184,0.566578,0.566972,
    0.567366,0.567761,0.568155,0.568550,0.568945,0.569340,0.569735,0.570131,0.570526,0.570922,0.571318,0.571714,0.572110,0.572506,0.572902,
    0.573299,0.573696,0.574092,0.574489,0.574887,0.575284,0.575681,0.576079,0.576477,0.576875,0.577273,0.577671,0.578069,0.578468,0.578866,
    0.579265,0.579664,0.580063,0.580462,0.580862,0.581261,0.581661,0.582061,0.582461,0.582861,0.583261,0.583662,0.584062,0.584463,0.584864,
    0.585265,0.585666,0.586067,0.586469,0.586871,0.587272,0.587674,0.588076,0.588479,0.588881,0.589284,0.589686,0.590089,0.590492,0.590895,
    0.591299,0.591702,0.592106,0.592509,0.592913,0.593317,0.593721,0.594126,0.594530,0.594935,0.595340,0.595745,0.596150,0.596555,0.596960,
    0.597366,0.597772,0.598177,0.598583,0.598990,0.599396,0.599802,0.600209,0.600616,0.601023,0.601430,0.601837,0.602244,0.602652,0.603059,
    0.603467,0.603875,0.604283,0.604691,0.605100,0.605508,0.605917,0.606326,0.606735,0.607144,0.607553,0.607963,0.608372,0.608782,0.609192,
    0.609602,0.610012,0.610423,0.610833,0.611244,0.611655,0.612066,0.612477,0.612888,0.613299,0.613711,0.614123,0.614535,0.614947,0.615359,
    0.615771,0.616183,0.616596,0.617009,0.617422,0.617835,0.618248,0.618661,0.619075,0.619489,0.619902,0.620316,0.620731,0.621145,0.621559,
    0.621974,0.622389,0.622803,0.623218,0.623634,0.624049,0.624464,0.624880,0.625296,0.625712,0.626128,0.626544,0.626960,0.627377,0.627794,
    0.628210,0.628627,0.629045,0.629462,0.629879,0.630297,0.630715,0.631133,0.631551,0.631969,0.632387,0.632806,0.633224,0.633643,0.634062,
    0.634481,0.634900,0.635320,0.635739,0.636159,0.636579,0.636999,0.637419,0.637839,0.638260,0.638680,0.639101,0.639522,0.639943,0.640364,
    0.640786,0.641207,0.641629,0.642051,0.642473,0.642895,0.643317,0.643740,0.644162,0.644585,0.645008,0.645431,0.645854,0.646277,0.646701,
    0.647124,0.647548,0.647972,0.648396,0.648821,0.649245,0.649669,0.650094,0.650519,0.650944,0.651369,0.651794,0.652220,0.652646,0.653071,
    0.653497,0.653923,0.654349,0.654776,0.655202,0.655629,0.656056,0.656483,0.656910,0.657337,0.657765,0.658192,0.658620,0.659048,0.659476,
    0.659904,0.660332,0.660761,0.661189,0.661618,0.662047,0.662476,0.662905,0.663335,0.663764,0.664194,0.664624,0.665054,0.665484,0.665914,
    0.666345,0.666775,0.667206,0.667637,0.668068,0.668499,0.668931,0.669362,0.669794,0.670226,0.670658,0.671090,0.671522,0.671955,0.672387,
    0.672820,0.673253,0.673686,0.674119,0.674552,0.674986,0.675420,0.675853,0.676287,0.676721,0.677156,0.677590,0.678025,0.678459,0.678894,
    0.679329,0.679764,0.680200,0.680635,0.681071,0.681506,0.681942,0.682378,0.682815,0.683251,0.683688,0.684124,0.684561,0.684998,0.685435,
    0.685873,0.686310,0.686748,0.687185,0.687623,0.688061,0.688499,0.688938,0.689376,0.689815,0.690254,0.690693,0.691132,0.691571,0.692011,
    0.692450,0.692890,0.693330,0.693770,0.694210,0.694650,0.695091,0.695531,0.695972,0.696413,0.696854,0.697296,0.697737,0.698179,0.698620,
    0.699062,0.699504,0.699946,0.700389,0.700831,0.701274,0.701716,0.702159,0.702602,0.703046,0.703489,0.703933,0.704376,0.704820,0.705264,
    0.705708,0.706153,0.706597,0.707042,0.707486,0.707931,0.708376,0.708822,0.709267,0.709712,0.710158,0.710604,0.711050,0.711496,0.711942,
    0.712389,0.712835,0.713282,0.713729,0.714176,0.714623,0.715071,0.715518,0.715966,0.716414,0.716862,0.717310,0.717758,0.718206,0.718655,
    0.719104,0.719552,0.720002,0.720451,0.720900,0.721350,0.721799,0.722249,0.722699,0.723149,0.723599,0.724050,0.724500,0.724951,0.725402,
    0.725853,0.726304,0.726755,0.727207,0.727658,0.728110,0.728562,0.729014,0.729466,0.729919,0.730371,0.730824,0.731277,0.731730,0.732183,
    0.732636,0.733090,0.733544,0.733997,0.734451,0.734905,0.735360,0.735814,0.736268,0.736723,0.737178,0.737633,0.738088,0.738543,0.738999,
    0.739455,0.739910,0.740366,0.740822,0.741279,0.741735,0.742191,0.742648,0.743105,0.743562,0.744019,0.744476,0.744934,0.745391,0.745849,
    0.746307,0.746765,0.747223,0.747682,0.748140,0.748599,0.749058,0.749517,0.749976,0.750435,0.750895,0.751354,0.751814,0.752274,0.752734,
    0.753194,0.753654,0.754115,0.754576,0.755036,0.755497,0.755959,0.756420,0.756881,0.757343,0.757805,0.758267,0.758729,0.759191,0.759653,
    0.760116,0.760578,0.761041,0.761504,0.761967,0.762431,0.762894,0.763358,0.763821,0.764285,0.764749,0.765213,0.765678,0.766142,0.766607,
    0.767072,0.767537,0.768002,0.768467,0.768933,0.769398,0.769864,0.770330,0.770796,0.771262,0.771728,0.772195,0.772662,0.773128,0.773595,
    0.774062,0.774530,0.774997,0.775465,0.775932,0.776400,0.776868,0.777337,0.777805,0.778273,0.778742,0.779211,0.779680,0.780149,0.780618,
    0.781088,0.781557,0.782027,0.782497,0.782967,0.783437,0.783908,0.784378,0.784849,0.785319,0.785790,0.786262,0.786733,0.787204,0.787676,
    0.788148,0.788620,0.789092,0.789564,0.790036,0.790509,0.790981,0.791454,0.791927,0.792400,0.792873,0.793347,0.793820,0.794294,0.794768,
    0.795242,0.795716,0.796191,0.796665,0.797140,0.797615,0.798090,0.798565,0.799040,0.799516,0.799991,0.800467,0.800943,0.801419,0.801895,
    0.802371,0.802848,0.803325,0.803801,0.804278,0.804756,0.805233,0.805710,0.806188,0.806666,0.807144,0.807622,0.808100,0.808578,0.809057,
    0.809535,0.810014,0.810493,0.810972,0.811452,0.811931,0.812411,0.812891,0.813370,0.813851,0.814331,0.814811,0.815292,0.815772,0.816253,
    0.816734,0.817215,0.817697,0.818178,0.818660,0.819142,0.819623,0.820106,0.820588,0.821070,0.821553,0.822035,0.822518,0.823001,0.823484,
    0.823968,0.824451,0.824935,0.825419,0.825903,0.826387,0.826871,0.827355,0.827840,0.828325,0.828809,0.829294,0.829780,0.830265,0.830750,
    0.831236,0.831722,0.832208,0.832694,0.833180,0.833667,0.834153,0.834640,0.835127,0.835614,0.836101,0.836588,0.837076,0.837563,0.838051,
    0.838539,0.839027,0.839516,0.840004,0.840493,0.840981,0.841470,0.841959,0.842448,0.842938,0.843427,0.843917,0.844407,0.844897,0.845387,
    0.845877,0.846368,0.846858,0.847349,0.847840,0.848331,0.848822,0.849314,0.849805,0.850297,0.850789,0.851281,0.851773,0.852265,0.852757,
    0.853250,0.853743,0.854236,0.854729,0.855222,0.855716,0.856209,0.856703,0.857197,0.857691,0.858185,0.858679,0.859174,0.859668,0.860163,
    0.860658,0.861153,0.861648,0.862144,0.862639,0.863135,0.863631,0.864127,0.864623,0.865119,0.865616,0.866113,0.866609,0.867106,0.867603,
    0.868101,0.868598,0.869096,0.869593,0.870091,0.870589,0.871088,0.871586,0.872084,0.872583,0.873082,0.873581,0.874080,0.874579,0.875079,
    0.875578,0.876078,0.876578,0.877078,0.877578,0.878079,0.878579,0.879080,0.879581,0.880082,0.880583,0.881084,0.881586,0.882087,0.882589,
    0.883091,0.883593,0.884095,0.884598,0.885100,0.885603,0.886106,0.886609,0.887112,0.887616,0.888119,0.888623,0.889127,0.889631,0.890135,
    0.890639,0.891143,0.891648,0.892153,0.892658,0.893163,0.893668,0.894173,0.894679,0.895184,0.895690,0.896196,0.896702,0.897209,0.897715,
    0.898222,0.898728,0.899235,0.899742,0.900250,0.900757,0.901265,0.901772,0.902280,0.902788,0.903296,0.903805,0.904313,0.904822,0.905331,
    0.905840,0.906349,0.906858,0.907367,0.907877,0.908387,0.908897,0.909407,0.909917,0.910427,0.910938,0.911448,0.911959,0.912470,0.912981,
    0.913493,0.914004,0.914516,0.915027,0.915539,0.916051,0.916564,0.917076,0.917589,0.918101,0.918614,0.919127,0.919640,0.920154,0.920667,
    0.921181,0.921694,0.922208,0.922723,0.923237,0.923751,0.924266,0.924781,0.925295,0.925810,0.926326,0.926841,0.927356,0.927872,0.928388,
    0.928904,0.929420,0.929936,0.930453,0.930969,0.931486,0.932003,0.932520,0.933037,0.933555,0.934072,0.934590,0.935108,0.935626,0.936144,
    0.936662,0.937181,0.937700,0.938218,0.938737,0.939256,0.939776,0.940295,0.940815,0.941334,0.941854,0.942374,0.942895,0.943415,0.943935,
    0.944456,0.944977,0.945498,0.946019,0.946540,0.947062,0.947583,0.948105,0.948627,0.949149,0.949671,0.950194,0.950716,0.951239,0.951762,
    0.952285,0.952808,0.953332,0.953855,0.954379,0.954902,0.955426,0.955951,0.956475,0.956999,0.957524,0.958049,0.958574,0.959099,0.959624,
    0.960149,0.960675,0.961200,0.961726,0.962252,0.962778,0.963305,0.963831,0.964358,0.964885,0.965412,0.965939,0.966466,0.966993,0.967521,
    0.968049,0.968577,0.969105,0.969633,0.970161,0.970690,0.971218,0.971747,0.972276,0.972805,0.973335,0.973864,0.974394,0.974923,0.975453,
    0.975983,0.976514,0.977044,0.977575,0.978105,0.978636,0.979167,0.979698,0.980230,0.980761,0.981293,0.981825,0.982357,0.982889,0.983421,
    0.983954,0.984486,0.985019,0.985552,0.986085,0.986618,0.987152,0.987685,0.988219,0.988753,0.989287,0.989821,0.990355,0.990890,0.991424,
    0.991959,0.992494,0.993029,0.993564,0.994100,0.994635,0.995171,0.995707,0.996243,0.996779,0.997316,0.997852,0.998389,0.998926,0.999463,
    1.000000};

#endif