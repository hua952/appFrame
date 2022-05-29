#ifndef _msgPmpID_h__
#define _msgPmpID_h__

enum msgPmpID
{	msgPmpID_test,
	msgPmpID_login,
	msgPmpID_sysfun,
	msgPmpID_CChess,
};

#define CChess2FullMsg(p) ((uword)((msgPmpID_CChess * 100)+p))
#define login2FullMsg(p) ((uword)((msgPmpID_login * 100)+p))
#define sysfun2FullMsg(p) ((uword)((msgPmpID_sysfun * 100)+p))
#define test2FullMsg(p) ((uword)((msgPmpID_test * 100)+p))

#endif
