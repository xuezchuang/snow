#pragma once
class GPUImmediate;
class IFrameAPI
{
	friend class CAppBase;
public:
	static IFrameAPI* Instance();
	static void Clear();
	static bool isNull();
	GPUImmediate* Immediate();
private:
	GPUImmediate* m_GPUImmediate;

protected:
	IFrameAPI();
	virtual ~IFrameAPI();
private:
	static IFrameAPI* _instance;
};



