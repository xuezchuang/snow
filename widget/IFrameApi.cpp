#include "IFrameApi.h"
#include "GPU_immediate.h"
#define _SAFE_DELETE(x) { if (x) { delete (x); (x)=NULL; } }

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
	_SAFE_DELETE(m_GPUImmediate);
}


GPUImmediate* IFrameAPI::Immediate()
{
	return m_GPUImmediate;
}