#pragma once
#include "interface_intern.h"

#include "IFrameApi.h"
#include "GPU_immediate.h"
class INWidget
{
public:
	INWidget();
	~INWidget();
	virtual void draw() = 0;
	//void SetOff(float offx, float offy);
	virtual void set_rect(const rctf* rect);
protected:
	GPUImmediate* m_pImmediate;
	
	//ÆÁÄ»×ø±ê
	//float m_offx;
	//float m_offy;
	rctf m_rect;
};