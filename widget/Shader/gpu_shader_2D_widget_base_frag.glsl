#version 330

in vec4 vs_fs_color;

layout (location = 0) out vec4 color;
flat in vec2 outRectSize;
uniform vec4 vcolor;
noperspective in vec2 uvInterp;
/**
* 长方形 box：  1. 原点位于长方形的中心点，形状是轴对称的
* 				2. b表示长方形右上角顶点的坐标
*/
float sdBox( in vec2 p, in vec2 b)
{
    // abs(p)是常用技巧，由于该图形四个象限都是相同的，因此都映射到第一象限即可
    // 现在的d表示长方体右上角顶点直线p点的向量
    vec2 d = abs(p)-b;
    // p点在外部：length(max(d,0.0)), 在内部则是min(max(d.x,d.y),0.0), 这两项总至少有一项为0
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
       color = vcolor;//vec4(1.0,0.0,0.0,1.0);
    }
}
