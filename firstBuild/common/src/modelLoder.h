#ifndef _modelLoder_h__
#define _modelLoder_h__

void* loadDll (const char* szFile);
void* getFun (void* handle, const char* szFunName);
int   unloadDll (void* handle);

#endif
