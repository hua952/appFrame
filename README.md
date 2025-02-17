# appFrame

cmake .. -A x64 -Dprotobuf_BUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release -Dprotobuf_BUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=C:/myProgram/protobufInstallMd
cmake .. -A x64 -Dprotobuf_BUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Debug -Dprotobuf_BUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=C:/myProgram/protobufInstallMdDebug

cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_BUILD_TYPE=Debug  ..
cmake cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake -DCMAKE_BUILD_TYPE=Release ..
level0=libcppLevel0L.so addLogic=libtestLogic.so procId=1


level0=C:\work\appFrame\bin\Debug\cppLevel0L.dll addLogic=C:\work\appFrame\bin\Debug\libtestLogic.dll procId=1
level0=cppLevel0L.dll addLogic=libtestLogic.dll procId=1
cppLevel0.exe level0=D:\pzh\work\appFrame\bin\Debug\cppLevel0L.dll addLogic=D:\pzh\work\appFrame\bin\Debug\testLogicB.dll#127.0.0.1:11000 addLogic=D:\pzh\work\appFrame\bin\Debug\testLogic.dll procId=1 netLib=D:\pzh\work\appFrame\bin\Debug\libeventSession.dll
gcc -E server.cpp -ID:\pzh\work\include -ID:\pzh\work\appFrame\common\src -ID:\pzh\work\appFrame\cLog\src> out.txr

genTool.exe defFile=C:\work\appFrame\test\openWorkDir.xml structBadyType=0
genTool.exe defFile=C:\study\vulkan\vulk1.xml structBadyType=0
genTool.exe defFile=C:\study\vulkan\Khronos\khr9.xml structBadyType=0

configFile.exe file=C:\work\appFrame\test\frameConfig.xml projectDir=c:/work/appFrame
./configFile file=~/work/appFrame/test/frameConfig.xml projectDir=~/work/appFrame


genTool.exe defFile=C:/work/appFrame/test/localTest.xml structBadyType=1
genTool.exe defFile=C:/work/appFrame/test/newSGCTest.xml structBadyType=1 projectDir=c:/work/appFrameProject/project
LD_LIBRARY_PATH=~/work/appFrameInstall/lib  ./genTool defFile=~/work/appFrame/test/newSGCTest.xml structBadyType=1 projectDir=~/work/appFrameProject
LD_LIBRARY_PATH=~/work/appFrameInstall/lib  ./genTool defFile=~/work/appFrame/test/main.xml  projectDir=~/study/msg1


genTool taskMgr.xml projectInstallDir=C:/work/appFrameInstall
static bool sendFun (void *pNum)
{
	static int sendNum = 0;
	auto pThA = (thA*)(pNum);
	threadAtoBAskMsg  ask;
	auto& pa = *(ask.pack());
	pa.m_reqid = 2;
	strcpy(pa.m_txt, "OKOK");
	pThA->sendMsg (ask);
	return sendNum < 5;
}
addTimer
	gInfo("rec ret from thB result = "<<rRet.m_result);	

	gInfo("rec ask from thA id = "<<rAsk.m_reqid<< "txt = "<< rAsk.m_txt);	
	rRet.m_result = 1;
