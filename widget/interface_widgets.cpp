#include "interface_widgets.h"

float BLI_rcti_cent_x_fl(const struct rctf* rct)
{
	return (float)(rct->xmin + rct->xmax) / 2.0f;
}
float BLI_rcti_cent_y_fl(const struct rctf* rct)
{
	return (float)(rct->ymin + rct->ymax) / 2.0f;

}
INWidget::INWidget()
{
	m_pImmediate = IFrameAPI::Instance()->Immediate();
}

INWidget::~INWidget()
{
	
}
//
//void INWidget::SetOff(float offx, float offy)
//{
//	m_offx = offx;
//	m_offy = offy;
//}
void INWidget::set_rect(const rctf* rect)
{
	m_rect = *rect;
}

