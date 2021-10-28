#include "interface_widgets.h"

INWidget::INWidget()
{
	m_pImmediate = IFrameAPI::Instance()->Immediate();
}

INWidget::~INWidget()
{
	
}

void INWidget::SetOff(float offx, float offy)
{
	m_offx = offx;
	m_offy = offy;
}

