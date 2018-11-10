static float tmax() { return 25.0f; }
static float fudge() { return 1.0f; }
static int iterations() { return 200; }

static float2 fract2_z(float2 n) {
  return n - floor(n);
}

static float fract_z(float n) {
  return n - floor(n);
}

static float hash1(float n) {
  return fmod(sin(n), 1.0f);
}

static float hash2(float2 n) {
  return hash1(dot(n, (float2)(12.232f, 39.343f)));
}

static float3 key(float ti) {
  return (float3)(0.0f, 0.9f*sin(ti), 0.0f);
}

static float de(float3 p, float time) {
  float2 t = floor(p.xz);

  p.xz = fract2_z(p.xz) - 0.5f;
  p.x *= 2.0f*floor(fract_z(hash2(t))*1.8f) - 1.0f;

  float d = fabs(1.0f - 2.0f*fabs(dot(p.xz, (float2)(1.0f))))/(2.0f*sqrt(5.0f));

  d = max(d - 0.5f/4.0f, p.y + 0.5f);
  return min(d, p.y + 1.0f);
}

static float3 shade(float3 pos, float3 nor, float3 ref, float3 rd, float t, float sa, float time) {
  float3 col = (float3)(0.0f);

  float3 k = key(time);

  float3 lig = normalize(k - pos);
  float dis = length(k - pos) - 0.06f;

  col += clamp(dot(lig, nor), 0.0f, 1.0f)
    *step(0.0f, -trace(pos + nor*0.001f, lig, dis, time));

  return col;
}

static void camera(float3* ro, float3* rd, float2 uv, float time) {
  *ro = (float3)(-3.0f, 1.0f, -3.0f);
  float3 ww = normalize((float3)(0.0f, 0.5f, 0.0f)-*ro);
  float3 uu = normalize(cross((float3)(0.0f, 1.0f, 0.0f), ww));
  float3 vv = normalize(cross(ww, uu));
  *rd = normalize(uv.x*uu + uv.y*vv + 0.7f*ww);
}
