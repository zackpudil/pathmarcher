

static float straw(float3 p, float iTime) {
    p.x += 0.6f;
    p.y -= 1.2f;
    
    float s = cylinder(p, (float2)(0.12f + 0.02f*sin(100.0f*p.y)*step(0.65f, p.y), 0.8f));
    
    p.y -= 0.97f;
    p.x += 0.3f;
	p.xy = rot(p.xy, -1.0f);
    
    p.xy = rot(p.xy + (float2)(0.0f, 0.2f), 0.2f*sin(14.0f*iTime)) - (float2)(0.0f, 0.2f);
    float us = cylinder(p, (float2)(0.12f + 0.02f*sin(100.0f*p.y)*step(0.15f, -p.y), 0.4f));
    us = fmax(us, -cylinder(p, (float2)(0.09f, 0.5f)));
    s = fmin(s, us);
    
    return s;
}

static float head(float3 p) {
    p.y -= 1.0f;
    
    float c = length(p) - 1.0f;
    c = fmax(c, -(length(p) - 0.8f));
    c = fmax(c, p.y - 0.3f);
    
    p.x += 1.0f;
    p.y += 0.3f;
    
    float h = length((float2)(length(p.xy) - 0.5f, p.z)) - 0.1f;
    h = fmax(h, p.x - 0.4f);
    
    h = smoothmin(h, c, 0.1f);
    
    p.yz = rot(p.yz, sign(p.z)*0.3f);
    float s = length((p - (float3)(1.6f, -0.37f, 0.0f))*(float3)(1.6f, 1.3f, 0.7f)) - 0.3f;
    h = fmax(h, -s);
    
    return h;
}

static float eyes(float3 p) {
    float3 op = p;
    
    p.y -= 1.07f;
    p.x -= 0.65f;
    
    p.y *= 0.7f;
    p.z *= 0.7f;
    
	p.z = fabs(p.z) - 0.14f;
    
    float e = length(p) - 0.4f;
    e = fmax(e, p.y - 0.16f);
    
    return fmax(e, -(length(op) - 0.8f));
}

static float innereyes(float3 p, float iTime) {
    p.y -= 1.07f;
    p.x -= 0.65f;
    
    p.y *= 0.7f;
    p.z *= 0.7f;
    
    p.xy = rot(p.xy, 0.3f);

    p.y *= 0.7f;
    p -= (float3)(0.3f, -0.0f, -0.0f);
    
    p.z += 0.07f*cos(10.0f*iTime)*smoothstep(-0.2f, 1.0f, sin(iTime));
    p.z = fabs(p.z) - 0.25f;
    
    float e = length(p) - 0.15f;
    return e;
}

static float nose(float3 p) {
    p.xy -= (float2)(0.9f, 0.6f);
    return length(p*(float3)(0.8f, 1.2f, 0.8f)) - 0.1f;
}

static float torso(float3 p) {
    float3 q = p;
    
    p.xz *= 1.0f - 0.4f*smoothstep(-1.1f, 0.3f, -p.y);
    float s = cylinder(p, (float2)(0.45f, 0.7f));
    
    q.y += 0.4f;
    s = min(s, max(length(q) - 0.83f, q.y - 0.1f));
    return s;
}

static float legs(float3 p, float iTime) {
    p.z = -fabs(p.z) + 0.4f;
    
 
    float b = -0.1f*cos(14.0f*iTime);
    float l = segment(p + (float3)(0.0f, b, 0.0f), 
                      (float3)(0.0f), 
                      (float3)(0.1f, -0.5f, -0.3f), 
                      0.17f);
    
    l = min(l, segment(p + (float3)(0.0f, b, 0.0f), 
                       (float3)(0.1f, -0.5f, -0.3f), 
                       (float3)(-0.2f, -1.0f + 1.2f*b, 0.0f), 0.17f));
    
    p.yz = rot(p.yz, -0.4f);
    return min(l, cylinder(p - (float3)(0.0f, 0.5f - b, 0.0f), (float2)(0.25f, 0.5f)));
}

static float shoes(float3 p, float iTime) {
    p.y += 0.7f;

    float b = -0.1f*cos(14.0f*iTime);
    p.z = -fabs(p.z) + 0.4f;

    float3 heel = (float3)(-0.2f, 0.1f-b, 0.0f);
    float3 toes = (float3)(0.3f, 0.0f, -0.4f);
    
    
    float sh = segment(p*(float3)(0.8f, 1.0f, 0.8f), heel, toes, 
                       0.25f + 0.14f*smoothstep(0.0f, 0.7f, p.x));
    
    return sh;
}

static float arms(float3 p) {
    p.y -= 0.2f;
    p.z = -fabs(p.z) + 0.85f;
    
    p.z -= 0.75f;
    p.yz = rot(p.yz, -0.6f*smoothstep(-1.0f, 0.5f, p.y));
    p.z += 0.75f;
    float a = segment(p, (float3)(0.0f), (float3)(0.0f, -0.7f, -0.3f), 0.08f);
    
    return a;
}

static float hands(float3 p) {
    p *= 0.9f;
    p.y -= 0.4f;
    p.z = -fabs(p.z) + 1.07f;
    
    float s = length((p - (float3)(0.0f, 0.1f, 0.0f))*(float3)(1.2f, 1.4f, 1.2f)) - 0.35f;
    s = fmin(s, length((p - (float3)(0.0f, 0.4f, 0.0f))*(float3)(1.0f, 1.6f, 1.0f)) - 0.2f);
    
    s = smoothmin(s, segment(p - (float3)(0.0, 0.16, 0.17), 
                             (float3)(0), 
                             (float3)(-0.1, -0.2, 0.2), 0.08), 0.1);
    
    s = smoothmin(s, segment(p - (float3)(0.0f, 0.16f, 0.17f),
                             (float3)(-0.1f, -0.2f, 0.2f),
                             (float3)(-0.1f, -0.3f, 0.1f), 0.08f), 0.01f);
    
    p.z = -fabs(p.z) + 0.12f;
    p.z = -fabs(p.z) + 0.05f;
    
    p.x -= 0.1f;
    p.y += 0.05f;
    s = smoothmin(s, segment(p, (float3)(0.0f), (float3)(0.0f, -0.12f, 0.0f), 0.07f), 0.1f);
    s = smoothmin(s, segment(p, (float3)(0.0f, -0.12f, 0.0f), (float3)(-0.2f, -0.1f, 0.0f), 0.07f), 0.02f);
    return s;
}

#define SHOES 1.0
#define LEGS 2.0
#define HANDS 3.0
#define TORSO 4.0
#define HEAD  5.0
#define STRAW 6.0
#define EYES 7.0
#define INNEREYES 8.0
#define NOSE 9.0

static float tmax() { return 10.0f; }
static float fudge() { return 0.85f; }
static int iterations() { return 100; }


static float2 de(float3 p, float iTime) {
    float2 f = (float2)(fmin(p.y + 1.0f, p.x + 3.0f), 0.0f);
    float2 sh = (float2)(shoes(p, iTime), SHOES);
    
    p.y -= 0.2f*cos(14.0f*iTime);
	p.y -= 0.6f;
    float2 l = (float2)(legs(p, iTime), LEGS);
    float2 ds = (float2)(hands(p), HANDS);
    p.y -= 1.35f;
    
    float2 t = (float2)(smoothmin(torso(p), arms(p), 0.4f), TORSO);
    
    p.y -= 0.6f;
    float2 h = (float2)(head(p), HEAD);
    float2 s = (float2)(straw(p, iTime), STRAW);
    float2 e = (float2)(eyes(p), EYES);
    float2 ie = (float2)(innereyes(p, iTime), INNEREYES);
    float2 m = (float2)(nose(p), NOSE);
    
    float2 ret = dmin(f, sh);
    ret = dmin(ret, dmin(l, t));
    ret = dmin(ret, dmin(h, s));
    ret = dmin(ret, e);
    ret = dmin(ret, ie);
    ret = dmin(ret, ds);
    ret = dmin(ret, m);
    
    return ret;
}

static float3 shade(float3 pos, float3 nor, float3 ref, float3 rd, float2 t, float se, float time) {
  float3 col = (float3)(0.0f);

  float3 key = normalize((float3)(0.0f, 1.0f, 0.0f) + cone(se + 2.23f));

  col += 0.5f*clamp(dot(key, nor), 0.0f, 1.0f)
    *step(0.0f, -trace(pos + nor*0.001f, key, 10.0f, time).x);

  return col;
}


static void camera(float3* ro, float3* rd, float2 uv, float time) {
  *ro = (float3)(3.5f*sin(time), 2.0f, -3.5f*cos(time));
  *ro = (float3)(0.0f, 0.5f, -3.0f);

  float3 ww = normalize((float3)(0.0f)-*ro);
  float3 uu = normalize(cross((float3)(0.0f, 1.0f, 0.0f), ww));
  float3 vv = normalize(cross(ww, uu));

  *rd = normalize(uv.x*uu + uv.y*vv + 0.97f*ww);
}

/*
vec3 materail(vec3 p, float m) {
    p.y -= 0.2*cos(14.0*iTime);
    
    if(m == 0.0) {
        return vec3(1.5);
    } else if(m == STRAW) {
        p.xy = rot(p.xy, -1.0*step(4.4, p.y));
    	vec3 m = vec3(1.5, 0, 0);
        m += vec3(0.5, 2.0, 2.0)*smoothstep(0.3, 0.8, abs(cos(10.0*p.y)));
        
        return m;
    } else if(m == TORSO) return vec3(0.1) + vec3(1, 0, 0)*step(-1.63, -p.y);
    else if(m == HANDS) return vec3(2.0);
    else if(m == EYES) return vec3(2.0);
    else if(m == INNEREYES) return vec3(0.1);
    else if(m == NOSE) return vec3(1.5, 0.1, 0.1);
    else if(m == SHOES) return vec3(1.09, 0.54, 0.14);
    else if(m == LEGS) {
        p.z = -abs(p.z) + 0.4;
        p.yz = rot(p.yz, -0.4);
        
        p.y -= 0.1*cos(14.0*iTime);;
        
        if(p.y > 0.52) return vec3(1.1, 0.1, 0.1);
        else return vec3(1);
	} else return vec3(1);
}
*/