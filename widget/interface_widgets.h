#pragma once
#include "interface_intern.h"

#include "IFrameApi.h"
#include "GPU_immediate.h"
class INWidget
{
public:
	INWidget();
	~INWidget();
	virtual void draw(const rctf* rect) = 0;
	void SetOff(float offx, float offy);
protected:
	GPUImmediate* m_pImmediate;
	
	//ÆÁÄ»×ø±ê
	float m_offx;
	float m_offy;
};