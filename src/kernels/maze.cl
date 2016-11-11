constant sampler_t inS = 
    CLK_NORMALIZED_COORDS_FALSE
  | CLK_ADDRESS_CLAMP_TO_EDGE
  | CLK_FILTER_NEAREST;

static float3 reflect(float3 v, float3 n){
  return v - 2.0f*dot(v, n) * n;
}

static float2 fract2_z(float2 n) {
  return n - floor(n);
}

static float fract_z(float n) {
  return n - floor(n);
}

static float hash(float n) {
  return fmod((sin(n)*43578.0f), 1.0f);
}

static float hash2(float2 n) {
  return hash(dot(n, (float2)(12.232f, 39.343f)));
}

static float3 key(float ti) {
  return (float3)(0.0f, 0.9f*sin(ti), 0.0f);
}

static float de(float3 p) {
  float2 t = floor(p.xz);

  p.xz = fract2_z(p.xz) - 0.5f;
  p.x *= 2.0f*floor(fract_z(hash2(t))*1.8f) - 1.0f;

  float d = fabs(1.0f - 2.0f*fabs(dot(p.xz, float2(1.0f))))/(2.0f*sqrt(5.0f));

  d = max(d - 0.5f/4.0f, p.y + 0.5f);
  return min(d, p.y + 1.0f);
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

static float3 normal(float3 p) {
  float2 h = (float2)(0.001f, 0.0f);
  float3 n = (float3)(
    de(p + h.xyy) - de(p - h.xyy),
    de(p + h.yxy) - de(p - h.yxy),
    de(p + h.yyx) - de(p - h.yyx)
  );

  return normalize(n);
}

static float3 cone(float s) {
  float a = 3.14159f*hash(s + 12.23f);
  float b = 6.28138f*hash(s + 29.23f);

  return (float3)(sin(a)*sin(b), sin(a)*cos(b), cos(a));
}

static float3 render(float3 ro, float3 rd, float sa, float ti) {
  float3 col = float3(0.0f);

  for(float b = 0.0f; b < 3.0f; b++) {
    rd = normalize(rd);

    float t = trace(ro, rd, 24.0f, 0.0001f, 200, 1.0f);
    if(t < 0.0f) return col;

    float se = sa + 12.23f*b;

    float3 k = key(ti);

    float3 pos = ro + rd*t;
    float3 nor = normal(pos);

    float3 lig = normalize(k - pos);
    float dis = length(k - pos) - 0.06f;

    col += clamp(dot(lig, nor), 0.0f, 1.0f)
      *step(0.0f, -trace(pos + nor*0.001f, lig, dis, 0.0001f, 200, 1.0f));

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

  float2 uv = (-res + 2.0f*(tf + of))/res.y;

  float3 ro = (float3)(3.0*sin(0.0), 1, -3.0*cos(0.0));
  float3 ww = normalize((float3)(0.0f, 0.5f, 0.0f)-ro);
  float3 uu = normalize(cross((float3)(0.0f, 1.0f, 0.0f), ww));
  float3 vv = normalize(cross(ww, uu));
  float3 rd = normalize(uv.x*uu + uv.y*vv + 0.7f*ww);

  float3 color = 0.0f;
  if(frame > 0.0f) color = read_imagef(in, inS, t).xyz;
  color += render(ro, rd, sa, time);

  write_imagef(image, t, (float4)(color, 1.0f));
}
