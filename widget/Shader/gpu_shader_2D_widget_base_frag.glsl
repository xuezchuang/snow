#version 330

in vec4 vs_fs_color;

layout (location = 0) out vec4 color;
flat in vec2 outRectSize;
noperspective in vec2 uvInterp;
/**
* ������ box��  1. ԭ��λ�ڳ����ε����ĵ㣬��״����ԳƵ�
* 				2. b��ʾ���������ϽǶ��������
*/
float sdBox( in vec2 p, in vec2 b)
{
    // abs(p)�ǳ��ü��ɣ����ڸ�ͼ���ĸ����޶�����ͬ�ģ���˶�ӳ�䵽��һ���޼���
    // ���ڵ�d��ʾ���������ϽǶ���ֱ��p�������
    vec2 d = abs(p)-b;
    // p�����ⲿ��length(max(d,0.0)), ���ڲ�����min(max(d.x,d.y),0.0), ��������������һ��Ϊ0
    return length(max(d,0.0)) + min(max(d.x,d.y),0.0) ;
}


float sdBox_f( in vec2 p, in vec2 b,float r)
{
    vec2 d = abs(p)-b;

    vec2 tempb = b - r;
    vec2 tempd = abs(p) - tempb;
    if(tempd.x > 0 && tempd.y > 0)
        return length(tempd)-r;

    return length(max(d,0.0)) + min(max(d.x,d.y),0.0) ;
}


void main(void)
{
    color = vec4(1,1.0,1.0,1.0);
    float sdf = sdBox_f(uvInterp,outRectSize,0.5);
    if(sdf < 0)
    {
       color = vec4(1.0,0.0,0.0,1.0);
    }
}
