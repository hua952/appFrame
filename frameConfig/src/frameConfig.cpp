#include "frameConfig.h"
#include <memory>
bool  frameConfig::clearTag ()
{
    return m_clearTag;
}

void  frameConfig::setclearTag (bool v)
{
	m_clearTag = v;
const char*  frameConfig::depInclude ()
{
    return m_depInclude.get();
}

void  frameConfig::setdepInclude (const char* v)
{
strCpy(v, m_depInclude);
word  frameConfig::sendCount ()
{
    return m_sendCount;
}

void  frameConfig::setsendCount (word v)
{
	m_sendCount = v;
ubyte  frameConfig::sendMember ()
{
    return m_sendMember;
}

void  frameConfig::setsendMember (ubyte v)
{
	m_sendMember = v;
int  frameConfig::sendNum ()
{
    return m_sendNum;
}

void  frameConfig::setsendNum (int v)
{
	m_sendNum = v;
udword  frameConfig::sendSetp ()
{
    return m_sendSetp;
}

void  frameConfig::setsendSetp (udword v)
{
	m_sendSetp = v;

