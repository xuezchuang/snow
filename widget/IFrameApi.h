#pragma once
class IFrameAPI
{
	friend class CAppBase;
public:
  static IFrameAPI* Instance();
  static void Clear();
  static bool isNull();

protected:
  IFrameAPI();
  virtual ~IFrameAPI();
private:
  static IFrameAPI *_instance;
};



