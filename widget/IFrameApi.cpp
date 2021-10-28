#include "IFrameApi.h"
#include "GPU_immediate.h"
IFrameAPI* IFrameAPI::_instance = nullptr;
IFrameAPI* IFrameAPI::Instance ()
{
  if (_instance == nullptr)
  {
    _instance = new IFrameAPI;
  }
  return _instance;
}

void IFrameAPI::Clear()
{
  if (_instance)
  {
    delete _instance;
    _instance = nullptr;
  }
}
 bool IFrameAPI::isNull()
 {
	 return _instance == nullptr;
 }

 IFrameAPI::IFrameAPI()
{
	 m_GPUImmediate = new GPUImmediate();
}

IFrameAPI::~IFrameAPI()
{
	//if(m_pCmdMgr)
	//	m_pCmdMgr->exitCmd();
	//if(m_pPlugin)
	//	m_pPlugin->clearAll();
	//if(m_pPluginSlot)
	//	m_pPluginSlot->unLoad();
	//if(m_pMainPlugin)
	//	m_pMainPlugin->clearAll();
	////
	//_SAFE_DELETE(m_pCmdBar);
	//_SAFE_DELETE(m_pCmdMgr);
	//_SAFE_DELETE(m_pStatusBar);
	//_SAFE_DELETE(m_pProjectMgr);
	//_SAFE_DELETE(m_pProductMgr);
	//_SAFE_DELETE(m_pMenuMgr   );
	//_SAFE_DELETE(m_pCmdResMgr );
	//_SAFE_DELETE(m_pResMgr	);
	//_SAFE_DELETE(m_pDockBar);
	//_SAFE_DELETE(m_pViewMgr);
	//_SAFE_DELETE(m_pUnitCtrl);
	//_SAFE_DELETE(m_pDBMgr);
	//
	//_SAFE_DELETE(m_pFigMakerMgr);
 //   _SAFE_DELETE(m_pGripEditorMgr);
	//_SAFE_DELETE(m_pLanguage);
	//// 	
	//_SAFE_DELETE(m_pPlugin    );
	//_SAFE_DELETE(m_pPluginSlot);
	//_SAFE_DELETE(m_pMainPlugin);
	//_SAFE_DELETE(m_pToolbar);
	////
 //   _SAFE_DELETE(m_pEventMgr);
 //   _SAFE_DELETE(m_pServiceMgr);
	//_SAFE_DELETE(m_pFrmUtilMgr);
	//_SAFE_DELETE(m_pLinkMgr);
	//_SAFE_DELETE(m_pLockMgr);
	//_SAFE_DELETE(m_pUserPrivileges);
}


GPUImmediate* IFrameAPI::Immediate()
{
	return m_GPUImmediate;
}