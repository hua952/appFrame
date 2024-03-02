#ifndef _xmlLoad_h__
#define _xmlLoad_h__
#include <memory>

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "rapidxml/rapidxml_print.hpp"

class xmlLoad
{
public:
    xmlLoad ();
    ~xmlLoad ();
	int load (const char* szFileName);
private:
};
#endif
