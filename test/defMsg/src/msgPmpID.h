#ifndef _msgPmpID_h__
#define _msgPmpID_h__

enum msgPmpID
{	msgPmpID_CChess,
};

#define CChess2FullMsg(p) ((uword)((msgPmpID_CChess * 100)+p))

#endif