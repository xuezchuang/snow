#include "IFrameApi.h"
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
	 /*m_pCmdBar	  = nullptr;
	 m_pCmdMgr	  = nullptr;
	 m_pStatusBar  = nullptr;
	 m_pProjectMgr = nullptr;
	 m_pProductMgr = nullptr;
	 m_pMenuMgr    = nullptr;
	 m_pCmdResMgr  = nullptr;
	 m_pResMgr	  = nullptr;
	 m_pDockBar    = nullptr;
	 m_pViewMgr    = nullptr;
	 m_pUnitCtrl	  = nullptr;
	 m_pDBMgr	  = nullptr;
	 m_pPlugin	  = nullptr;
	 m_pMainPlugin = nullptr;
	 m_pEventMgr   = nullptr;
	 m_pLabelMgr   = nullptr;
	 m_pFigMakerMgr= nullptr;
	 m_pGripEditorMgr= nullptr;
	 m_pFigurePoolMgr = nullptr;
	 m_pPluginSlot = nullptr;
	 m_pLanguage	  = nullptr;
	 m_pToolbar    = nullptr;
	 m_pServiceMgr = nullptr;
	 m_pOptionMgr  = nullptr;
	 m_pFrmUtilMgr = nullptr;
	 m_pLinkMgr	  = nullptr;
	 m_pLockMgr    = nullptr;
	 m_pUserPrivileges = new IUserPrivileges();*/
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


