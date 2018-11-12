constant sampler_t inS =
CLK_NORMALIZED_COORDS_FALSE
| CLK_ADDRESS_CLAMP_TO_EDGE
| CLK_FILTER_NEAREST;

static float3 reflect(float3 v, float3 n){
  return v - 2.0f*dot(v, n) * n;
}

static float hash(float n) {
  return fmod(sin(n)*43578.5453f, 1.0f);
}

static float2 dmin(float2 a, float2 b) {
    return a.x < b.x ? a : b;
}

static float smoothmin(float a, float b, float k) {
    float h = clamp( 0.5f+0.5f*(b-a)/k, 0.0f, 1.0f);
    return mix( b, a, h ) - k*h*(1.0f-h);
}

static float2 rot(float2 p, float a) {
    float s = sin(a);
    float c = cos(a);
    
    return (float2)(c*p.x + s*p.y, -s*p.x + c*p.y);
}

static float cylinder(float3 p, float2 h) {
    float2 d = fabs((float2)(length(p.xz), p.y)) - h;
    return fmin(fmax(d.x, d.y), 0.0f) + length(fmax(d, 0.0f));
}

static float segment(float3 p, float3 a, float3 b, float r) {
    float3 pa = p - a; 
    float3 ba = b - a;
    
    float h = clamp(dot(pa, ba)/dot(ba, ba), 0.0f, 1.0f);
    
    return length(pa - ba*h) - r;
}

static float tmax();
static float fudge();
static int iterations();
static float2 de(float3, float);
static float3 shade(float3 pos, float3 nor, float3 ref, float3 rd, float2 t, float se, float time);
static void camera(float3* ro, float3* rd, float2 uv, float time);

static float2 trace(float3 ro, float3 rd, float mx, float time) {
  float t = 0.001f;
  float m = -1.0f;

  float fu = fudge();
  int it = iterations();

  for(int i = 0; i < it; i++) {
    float2 d = de(ro + rd*t, time);
    if(fabs(d.x) < 0.0001f || t >= mx) break;
    t += d.x*fu;
    m = d.y;
  }

  return (float2)(t < mx ? t : -1.0f, m);
}

static float3 normal(float3 p, float time) {
  float2 h = (float2)(0.001f, 0.0f);

  float3 n = (float3)(
    de(p + h.xyy, time).x - de(p - h.xyy, time).x,
    de(p + h.yxy, time).x - de(p - h.yxy, time).x,
    de(p + h.yyx, time).x - de(p - h.yyx, time).x
  );

  return normalize(n);
}

static float edge(float3 p, float s, float time) {
  float2 h = (float2)(0.001f, 0.0f);

  float3 n1 = (float3)(
    de(p + h.xyy, time).x,
    de(p + h.yxy, time).x,
    de(p + h.yyx, time).x);

  float3 n2 = (float3)(
    de(p - h.xyy, time).x,
    de(p - h.yxy, time).x,
    de(p - h.yyx, time).x);

  float d = de(p, time).x;
  float3 v = fabs(d - 0.5f*(n1 + n2));

  float e = fmin(1.0f, pow(v.x+v.y+v.z, s)*10.0f);

  return e;
}

static float3 cone(float s) {
  float a = 3.14159f*hash(s + 12.23f);
  float b = 6.28138f*hash(s + 29.23f);

  return (float3)(sin(a)*sin(b), sin(a)*cos(b), cos(a));
}


static float3 render(float3 ro, float3 rd, float sa, float time) {
  float3 col = (float3)(0.0f);

  for(float b = 0.0f; b < 3.0f; b += 1.0f) {
    rd = normalize(rd);
    float2 t = trace(ro, rd, tmax(), time);

    if(t.x < 0.0f) return col;

    float3 pos = ro + rd*t.x;
    float3 nor = normal(pos, time);
    float3 ref = reflect(rd, nor);

    float se = sa + 12.23f*b;

    col += shade(pos, nor, ref, rd, t, se, time);

    ro = pos;
    rd = nor + cone(se);
  }
  
  return col;
}


void kernel pixel(global int* w, global int* h,
                  global float* f, global float* ti,
                  read_only image2d_t in, write_only image2d_t image) {

  int width = *w;
  int height = *h;
  float frame = *f;
  float time = *ti;

  int2 t = (int2)(get_global_id(0), get_global_id(1));

  float2 res = (float2)((float)width, (float)height);
  float2 tf = convert_float2(t);

  float sa = hash(dot(tf, (float2)(12.23f, 83.34f)) + 1833.4f*frame);
  float2 of = (float2)(-0.5f, -0.5f) + (float2)(hash(sa + 12.23f), hash(sa + 93.34f));

  float2 uv = (-res + 2.0f*(tf + of))/res;
  float3 ro, rd;

  camera(&ro, &rd, uv, time);
  float3 renderColor = render(ro, rd, sa, time);
  renderColor = (float3)(1.0f) - exp(-0.7f*renderColor);
  renderColor = pow(fabs(renderColor), (float3)(1.0f/2.2f));

  float3 color = 0.0f;
  if(frame > 0.0f) color = read_imagef(in, inS, t).xyz;
  color += renderColor;

  write_imagef(image, t, (float4)(color, 1.0f));
}
