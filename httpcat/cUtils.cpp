#include  "cUtils.h"

const unsigned char* memscan(const unsigned char* pBuf, UINT BufLen,
							 const unsigned char* pPat, UINT PatLen)
{
	if ( PatLen > BufLen )
	{
		return 0;
	}

	UINT Scans = BufLen - PatLen + 1;
	for (UINT i=0; i<Scans; i++)
	{
		const unsigned char* pb = pBuf + i;
		const unsigned char* pp = pPat;
		UINT j = 0;
		for (j=0; j<PatLen; j++)
		{
			if ( *pb != *pp )
			{
				break;
			}
			pb++;
			pp++;
		}
		if ( PatLen == j )
		{
			return ( pBuf + i );
		}
	}
	return 0;
}