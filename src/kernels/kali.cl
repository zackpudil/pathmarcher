constant sampler_t inS = 
    CLK_NORMALIZED_COORDS_FALSE
  | CLK_ADDRESS_CLAMP_TO_EDGE
  | CLK_FILTER_NEAREST;

static float3 reflect(float3 v, float3 n){
  return v - 2.0f*dot(v, n) * n;
}

static float fract_z(float n) {
  return n - floor(n);
}

static float hash(float n) {
  return fmod(sin(n)*43578.5453f, 1.0f);
}

static float de(float3 p) {
  //return length(fmax(fabs(p) - 1.0f, 0.0f));
  float4 q = (float4)(p, 1.0f);
  q.xyz -= (float3)(1.0f, 1.8f, 1.0f);

  for(int i = 0; i < 6; i++) {
    q.xyz = fabs(q.xyz + 1.0f) - 1.0f;
    q /= clamp(dot(q.xyz, q.xyz), 0.25f, 1.0f);
    q *= 1.2f;
  }

  float f = (length(q.xz) - 1.5f)/q.w;

  f = fmin(f, p.y + 1.0f);

  return f;
}

static float trace(float3 ro, float3 rd, float mx, float eps, int it, float fu) {
  float t = 0.001f;

  for(int i = 0; i < it; i++) {
    float d = de(ro + rd*t);
    if(fabs(d) < eps*t || t >= mx) break;
    t += d*fu;
  }

  return t < mx ? t : -1.0f;
}

static float3 normal(float3 p, float* e) {
  float2 h = (float2)(0.001f, 0.0f);

  float3 n1 = (float3)(de(p + h.xyy), de(p + h.yxy), de(p + h.yyx));
  float3 n2 = (float3)(de(p - h.xyy), de(p - h.yxy), de(p - h.yyx));

  float d = de(p);
  float3 v = fabs(d - 0.5f*(n1 + n2));

  *e = fmin(1.0f, pow(v.x+v.y+v.z, 0.65f)*10.0f);

  return normalize(n1 - n2);
}

static float3 cone(float s) {
  float a = 3.14159f*hash(s + 12.23f);
  float b = 6.28138f*hash(s + 29.23f);

  return (float3)(sin(a)*sin(b), sin(a)*cos(b), cos(a));
}

static float3 render(float3 ro, float3 rd, float sa) {
  float3 col = float3(0.0f);

  for(float b = 0.0f; b < 3.0f; b++) {
    rd = normalize(rd);

    float t = trace(ro, rd, 10.0f, 0.0001f, 200, 0.75f);
    if(t < 0.0f) return col;

    float se = sa + 12.23f*b;

    float edg;
    float3 pos = ro + rd*t;
    float3 nor = normal(pos, &edg);
    float3 ref = reflect(rd, nor);

    float3 key = normalize((float3)(0.0f, 1.0f, 0.0f) + cone(se + 2.23f));

    col += clamp(1.0f + dot(rd, nor), 0.0f, 1.0f)
      *step(0.0f, -trace(pos + nor*0.001f, ref, 10.0f, 0.0001f, 200, 1.0f));

    col += (float3)(10.0f, 0.2f, 0.2f)*edg;

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

  float2 res = float2((float)width, (float)height);
  float2 tf = convert_float2(t);

  float sa = hash(dot(tf, (float2)(12.23f, 83.34f)) + 1833.4f*frame);
  float2 of = -0.5f + (float2)(hash(sa + 12.23f), hash(sa + 93.34f));

  float2 uv = (-res + 2.0f*(tf + of))/res;

  float3 ro = (float3)(3.5f*sin(time), 2.0f, -3.5f*cos(time));
  float3 ww = normalize((float3)(0.0f, 2.0f + sin(time), 0.0f)-ro);
  float3 uu = normalize(cross((float3)(0.0f, 1.0f, 0.0f), ww));
  float3 vv = normalize(cross(ww, uu));
  float3 rd = normalize(uv.x*uu + uv.y*vv + 0.97f*ww);

  float3 color = 0.0f;
  if(frame > 0.0f) color = read_imagef(in, inS, t).xyz;
  color += render(ro, rd, sa);

  write_imagef(image, t, (float4)(color, 1.0f));
}
