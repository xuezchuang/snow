in vec2 pass_UV;

out vec4 out_Scene;


layout (std140) uniform settings {
  vec4 sphere[4];
};

uniform bool bnew;

vec3 col[4] = vec3[](
	vec3(1,1,1),
	vec3(0.5,0,1),
	vec3(1,1,0),
	vec3(1,0,1)
);

vec4 AxisRadian(vec3 axis,float radian)
{
	float theta = radian * 0.5f;
	float sinTheta = sin(theta);
	float cosTheta = cos(theta);
	return vec4(axis.x * sinTheta,axis.y * sinTheta,axis.z * sinTheta,cosTheta);
}

//---------------------------Quaternion-------------------------------------//
vec4 mult(vec4 v11,vec4 v22)
{
    vec3 v1 = vec3(v11.xyz);
    vec3 v2 = vec3(v22.xyz);

	vec3 vcross = cross(v1,v2);                   // v x v'
    float dot = dot(v1,v2);                         // v . v'
    vec3 v3 = vcross + (v11.w * v2) + (v22.w * v1);   // v x v' + sv' + s'v

    return vec4(v3.xyz,v11.w*v22.w-dot);
}

vec4 Conjugate(vec4 q)
{
	return vec4(-q.x, -q.y, -q.z, q.w);
}
vec3 RotateVector(vec4 quat, vec3 pos)
{
	vec4 quatPos = vec4(pos,0);
	vec4 quatRotated = mult(mult(quat,quatPos), Conjugate(quat));
	return vec3(quatRotated.xyz);
}

mat3 FromTo_mat3(vec3 from, vec3 to)
{
	const float kEpsilon = 0.00001f;

	vec3 vec1 = normalize(from);
	vec3 vec2 = normalize(to);
	float cosTheta = dot(vec1, vec2);

	// When "from" almost equals to "to"
	if (cosTheta > 1.0f - kEpsilon)
	{
		return mat3(1.0);
	}

	// When "from" almost equals to negative "to"
	if (cosTheta < -1.0f + kEpsilon)
	{
		vec3 vecRef = vec3(1.0f, 0.0f, 0.0f);
		if (dot(vec1, vecRef) > 1.0f - kEpsilon)
			vecRef = vec3(0.0f, 0.0f, 1.0f);

		vec3 vecUp = normalize(cross(vec1, vecRef));
		vec3 vecRight = normalize(cross(vecUp, vec1));
		vec3 vecForward = vec1;
		mat3 matrixCoordinateFrom = mat3(vecRight, vecUp, vecForward);
		mat3 matrixCoordinateTo = mat3(-vecRight, vecUp, -vecForward);
		mat3 matrixCoordinateTransform = transpose(matrixCoordinateFrom) * matrixCoordinateTo;
		return matrixCoordinateTransform;
	}

	// When "from" not parallax to "to"
	vec3 v = cross(vec1, vec2);
	float c = dot(vec1, vec2);
	float k = 1.0f / (c + 1.0f);
	return mat3(v.x * v.x * k + c, v.x * v.y * k + v.z, v.x * v.z * k - v.y,
				v.y * v.x * k - v.z, v.y * v.y * k + c, v.y * v.z * k + v.x,
				v.z * v.x * k + v.y, v.z * v.y * k - v.x, v.z * v.z * k + c
				);
}

vec4 FromTo(vec3 from, vec3 to)
{
	mat3 m = FromTo_mat3(from, to);
	
	float w = sqrt(1.0f + m[0][0] + m[1][1] + m[2][2]) * 0.5f;
	float scaler = 1.0f / (w * 4.0f);
	float x = (m[2][1] - m[1][2]) * scaler;
	float y = (m[0][2] - m[2][0]) * scaler;
	float z = (m[1][0] - m[0][1]) * scaler;
	return vec4(x, y, z, w);
}


//----------------------------------------------------------------//

vec4 Identity = vec4(0.0f, 0.0f, 1.0f, 0.0f);

float PI = 3.141592653f;

float   kLightGI = 0.3f;
vec3	kLightColor = vec3(1.0,1.0,1.0);
vec3	kLightPosition = vec3(20.0f, 20.0f, -5.0f);
vec3	light_m_Direction  = normalize(kLightPosition);


vec3	kCameraPosition = vec3(0f, 0.1f, -6.0f);
vec4	kCameraRotation = FromTo(vec3(0.0f, 0.0f, 1.0f),normalize(-kCameraPosition));

float	kSphereRadius = 1.0f;
vec3	kSpherePosition = vec3(0.0f, 0.0f, 0.0f);
vec4	kSphereRotation = Identity;
vec3	kBoxPosition = vec3(0.0f, 0.0f, 0.0f);
vec3	kBoxSize = vec3(0.8f, 0.8f, 0.8f);
vec4	kBoxRotation = Identity;
vec4	kBoxSphereRotation = AxisRadian(vec3(0,1,0),PI*0.15f);

vec3	kPlanePosition = vec3(0.0f, -0.8f, 0.0f);
vec3	kPlaneNormal = vec3(0.0f, 1.0f, 0.0f);
vec4	kPlaneRotation = Identity;

int     kRaymarchStepMax = 100;
float   kRaymarchDistanceMax = 100.0f;
float   kRaymarchDistanceMin = 0.0001f;
vec3	kRaymarchSurfaceColor = vec3(0.5f, 0.8f, 1.0f);




float BoxDistance(vec3 position)
{
	vec3 transformedPosition = RotateVector(kBoxRotation, position - kBoxPosition);
	vec3 q = abs(transformedPosition) - kBoxSize;
	return length(max(q,vec3(0.0))) + min(max(max(q.x,q.y),q.z),0.0);
}

float SphereDistance(vec3 position)
{
	vec3 transformedSpherePosition = position - kSpherePosition;
	return length(transformedSpherePosition) - kSphereRadius;
}


float ObjectDistance(vec3 position)
{
	vec3 transformedPosition = RotateVector(kBoxSphereRotation, position - kBoxPosition);
	float distanceToObjectA = BoxDistance(transformedPosition);
	float distanceToObjectB = SphereDistance(transformedPosition);
	return max(distanceToObjectA, -distanceToObjectB);
}

float GetMinDistance(vec3 position)
{
	float distanceToNearestObject = (position - kPlanePosition).y;//∞Âæ‡¿Î
	float distanceToObject = ObjectDistance(position);
	distanceToNearestObject = min(distanceToNearestObject, distanceToObject);
	return distanceToNearestObject;
}

float Raymarching(vec3 m_Origin,vec3 m_Direction)
{
	float raymarchingDistance = 0.0f;
	for (int i = 0; i < kRaymarchStepMax; ++i)
	{
		vec3 raymarchingPosition = m_Origin + m_Direction * raymarchingDistance;
		float distanceToNearestSurface = GetMinDistance(raymarchingPosition);
		if (distanceToNearestSurface < kRaymarchDistanceMin)
			break;

		raymarchingDistance += distanceToNearestSurface;
		if (raymarchingDistance > kRaymarchDistanceMax)
			break;
	}

	return raymarchingDistance;
}

//----------------------------------------------------------------//


vec3 GetSurfaceNormal(vec3 surfacePosition)
{
	float distanceToNearestSurface = GetMinDistance(surfacePosition);
	vec3 normalDirection = vec3
	(
		distanceToNearestSurface - GetMinDistance(surfacePosition - vec3(0.01f, 0.00f, 0.00f)),
		distanceToNearestSurface - GetMinDistance(surfacePosition - vec3(0.00f, 0.01f, 0.00f)),
		distanceToNearestSurface - GetMinDistance(surfacePosition - vec3(0.00f, 0.00f, 0.01f))
	);

	return normalize(normalDirection);
}

float Shadowing(vec3 position)
{
	// Ray marching for shadow
	float shadowDistanceToNearestSurface = Raymarching(position,light_m_Direction);
	if (shadowDistanceToNearestSurface < kRaymarchDistanceMax)
		return 0.0f;

	return 1.0f;
}

vec3 Lighting(vec3 position, vec3 normalDirection)
{
	// Lambert
	float NdotL = dot(light_m_Direction, normalDirection);
	float lambert = clamp(NdotL,0,1);

	// Shadow
	float shadow = Shadowing(position + normalDirection * kRaymarchDistanceMin * 2.0f);

	// Output
	float lightingIntensity = mix(kLightGI, 1.0f, lambert * shadow);
	vec3 outputColor = kRaymarchSurfaceColor * kLightColor * lightingIntensity;
	return outputColor;
}

//----------------------------------------------------------------//

vec4 plane[1] = vec4[](vec4(0.71,0,-0.71,-40));

vec3 light = vec3(-3,1,-10);

float Distance(vec3 position,int i)
{
	vec3 transformedSpherePosition = position - vec3(sphere[i]);
	return length(transformedSpherePosition) - sphere[i].w;
}

vec3 getsdfPoint(vec3 origin, vec3 ray, int dontcare, inout int iidx)
{
	float d_less=40;
	vec3 dir = normalize(RotateVector(vec4(0,0,0,1),ray));
	float raymarchingDistance = 0.0f;
	int low=-1;
	for(int i = 0;i < 100;i++)
	{
		vec3 objray = origin + dir * raymarchingDistance;
		float d1 = objray.z - plane[0].z;
		//int templow = -1;
		for(int j=0;j<4;j++)
		{
			float d2 = Distance(objray,j);
			if(d1 > d2)
			{
				d1 = d2;
				low = j;
			}
		}
		if (d1 < 0.01)
		{
			iidx = low;
			break;
		}
		raymarchingDistance += d1;
		if (raymarchingDistance > 99)
		{
			iidx = -1;//low;
			break;
		}
	}
	d_less = raymarchingDistance;
	return origin + ray*d_less;
}

vec3 getSpherePoint(vec3 origin, vec3 ray, int dontcare, inout int iidx)
{
	int low=-1;
	float d_less=40;
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
	//vec3 p = getSpherePoint(origin, ray, -1, low);
	vec3 p = getsdfPoint(origin, ray, -1, low);
	if(bnew)
	{
		vec3 m_Direction = RotateVector(kCameraRotation,vec3(pass_UV.xy,1));
		m_Direction = normalize(m_Direction);
		float raymarchingDistance = Raymarching(kCameraPosition,m_Direction);
		out_Scene = vec4(0,0,0,1);
		if (raymarchingDistance < kRaymarchDistanceMax)
		{
			vec3 surfacePosition = kCameraPosition + m_Direction * raymarchingDistance;
			vec3 surfaceNormal = GetSurfaceNormal(surfacePosition);
			out_Scene = vec4(Lighting(surfacePosition, surfaceNormal),1);
		}
		return;
	}
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
	
	
	out_Scene = vec4(outs, 1);
	

}
