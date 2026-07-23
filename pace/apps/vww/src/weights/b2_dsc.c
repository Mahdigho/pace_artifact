#include <libfixedAbstract/fixed.h>
#include <libmatAbstract/mat.h>
#include <libmsp/nv.h>

__ro_hinv fixed b2_dscw[4][1][3][3] = {
	{
		{
			{ -1352, 88, -1845},
			{ -600, 1104, 1473},
			{ -1991, -175, 8403}
		}
	},
	{
		{
			{ -256, 574, 676},
			{ -761, -2336, -524},
			{ -180, -2999, -3104}
		}
	},
	{
		{
			{ -1145, -267, 1458},
			{ -950, 1605, 3927},
			{ -492, 4485, 2516}
		}
	},
	{
		{
			{ 1417, -4701, 5842},
			{ 5155, -1970, -8170},
			{ 9514, 1565, -30034}
		}
	}
};

__ro_hinv fixed b2_dscs_1[1] = { -32667};

__ro_hinv fixed b2_dscb_1[1] = { 6064};

__ro_hinv fixed b2_dscs_2[2] = { -32627, 32612};

__ro_hinv fixed b2_dscb_2[2] = { 6063, 6265};

__ro_hinv fixed b2_dscs_3[3] = { -32768, 32588, 32767};

__ro_hinv fixed b2_dscb_3[3] = { 6065, 6265, 508};

__ro_hinv fixed b2_dscs_4[4] = { -32626, 32767, 32687, 32767};

__ro_hinv fixed b2_dscb_4[4] = { 6065, 6280, 505, 5693};

