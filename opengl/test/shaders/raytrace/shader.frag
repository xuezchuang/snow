in vec2 pass_UV;

out vec4 out_Scene;


layout (std140) uniform settings {
  vec4 sphere[4];
};

vec3 col[4] = vec3[](
	vec3(1,1,1),
	vec3(0.5,0,1),
	vec3(1,1,0),
	vec3(1,0,1)
);

vec4 plane[1] = vec4[](vec4(0.71,0,-0.71,-40));

vec3 light = vec3(-3,1,-10);


vec3 getSpherePoint(vec3 origin, vec3 ray, int dontcare, inout int iidx)
{
	int low=-1;
	float d_less=999;
	for(int i=0;i<4;i++)
	{
		if (i==dontcare)continue;
		if(dot(sphere[i].xyz-origin, ray)<0)continue;
	
		float ldotc = dot(ray, origin-sphere[i].xyz);
		float cdotc = dot(origin-sphere[i].xyz, origin-sphere[i].xyz);
		float rsq = sphere[i].a*sphere[i].a;
		
		float d = ldotc*ldotc - cdotc + rsq;
		
		if(d<0)continue;
		
		float dist = -ldotc - sqrt(d);
		
		if(dist<d_less)
		{
			d_less = dist;
			low=i;
		}
	
	}
	iidx = low;
	
	return origin + ray*d_less;
}

vec3 getPlanePoint(vec3 origin, vec3 ray, int dontcare, inout int iidx)
{
	int low=-1;
	iidx = -1;
	float d_less=999;
	//for(int i=0;i<1;i++)
	{
		if (0==dontcare)return vec3(0,0,0);
		
		float denom = dot(ray, plane[0].xyz);
		if (denom == 0)return vec3(0,0,0);
		
		float disttoback = (plane[0].w - dot(origin, plane[0].xyz))/denom;

		if(disttoback<0)return vec3(0,0,0);
		
		//if(disttoback<d_less)
		//{
			d_less = disttoback;
			low=0;
		//}
	
	}
	iidx = low;
	
	return origin + ray*d_less;
}

//Given an origin point and ray direction, does the ray cross a sphere? (1 if no, 0 if yes)
int getSphereBlock(vec3 origin, vec3 ray, int dontcare)
{
	for(int i=0;i<4;i++)
	{
		if(i==dontcare)continue;
		
		//Ignore all spheres behind the ray
		if(dot(sphere[i].xyz-origin, ray)<0)continue;
		
		float ldotc = dot(ray, origin-sphere[i].xyz);
		float cdotc = dot(origin-sphere[i].xyz, origin-sphere[i].xyz);
		float rsq = sphere[i].a*sphere[i].a;
		
		float d = ldotc*ldotc - cdotc + rsq;
		
		if(d<0)continue;
		
		
		return 0;		
	}
	return 1;
}

void main(void)
{	

	
	vec3 outs = vec3(0,0,0);
			
	vec3 ray = vec3(pass_UV.x, pass_UV.y,2);
	ray = normalize(ray);
	
	int dontcare = -1;
	vec3 origin = vec3(0,0,0);
	

	int low;
	vec3 p = getSpherePoint(origin, ray, -1, low);
	
	
	
	
	if(low!=-1)
	{
				
		vec3 norm = normalize(p - sphere[low].xyz);
		
		vec3 ray_light = normalize(light-p);
		
		vec3 ray_reflect = reflect(ray,norm);
		
		
		int lgt = getSphereBlock(p, ray_light, low);
		
		//calc reflection
		vec3 intensity=vec3(0,0,0);
		float z=0;
		
		int refl;
		vec3 refpt = getSpherePoint(p, ray_reflect, low, refl);
		
		if(refl!=-1)
		{
			vec3 refnorm = normalize(refpt - sphere[refl].xyz);
			vec3 reflightray = normalize(light-refpt);
			
			int b = getSphereBlock(refpt, reflightray, -1);
			
			
			float fresnel = 1-dot(ray_reflect, norm);
			
			intensity = fresnel*max(0,dot(refnorm, normalize(light-refpt)))*col[refl]*b;
		}
		
		else
		{
			int rrf;
			refpt = getPlanePoint(p, ray_reflect, -1, rrf);
			if(rrf!=-1)
			{		
				intensity = max(0,dot(plane[rrf].xyz, normalize(light-refpt)))*vec3(0.1);
			}
			else
			{
				//float denom = dot(-ray, plane[0].xyz);
		
				//float disttoback = (plane[0].w - dot(p, plane[0].xyz))/denom;
				//intensity = vec3(disttoback/10);
			}
		}
		/*
		for(int i=0;i<4;i++)
		{
			if(i==low)continue;
			if(dot(p-sphere[i].xyz, ray_reflect)<0)continue;
			
			float ldotc = dot(ray_reflect, p-sphere[i].xyz);
			float cdotc = dot(p-sphere[i].xyz, p-sphere[i].xyz);
			float rsq = sphere[i].a*sphere[i].a;
			
			float d = ldotc*ldotc - cdotc + rsq;
			
			if(d<0)continue;
			
			float dist2reflect = abs(-ldotc + sqrt(d));
			
			vec3 refpt = p + ray_reflect*dist2reflect;
			vec3 refnorm = normalize(sphere[i].xyz - refpt);
			
			intensity = max(0,dot(refnorm, normalize(refpt-light)))*col[i];
			z=1;
			break;		
		}
		*/
		
		
		vec3 l = max(0,dot(norm, normalize(light-p)))*lgt*0.5*col[low];
		
		//vec3 it = vec3(1);
		//if(intensity.r<1) it = vec3(0);
		vec3 it = intensity*0.5;
		
		outs += vec3(l.r+it.r,l.g+it.g,l.b+it.b);

		
		
		//outs += vec3(z,z,z);
		
	}
	
	else
	{
			int bk;
			vec3 backpt = getPlanePoint(origin, ray, -1, bk);
			
			//float disttoback = (plane[0].w - dot(vec3(0,0,0), plane[0].xyz))/dot(ray, plane[0].xyz);
			if(bk!=-1)
			{	
				int lgt = getSphereBlock(backpt, normalize(light-backpt), -1);
				float ints = max(0,dot(plane[0].xyz, normalize(light-backpt)))*0.1*lgt;
				//ints = backpt;
				outs+=vec3(ints);
			}
	}
	
	//}
	
	out_Scene = vec4(outs, 1);
	

}
