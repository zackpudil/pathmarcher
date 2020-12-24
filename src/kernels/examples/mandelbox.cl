static float tmax() { return 10.0f; }
static float fudge() { return 0.25f; }
static int iterations() { return 200; }

constant float3 light = (float3)(0.8f, 0.7f, -1.0f);

static float de(float3 p, float t) {
  float4 q = (float4)(p, 1.0f);
  float4 c = q;

  for(int i = 0; i < 10; i++) {
    q.xyz = 2.0f*clamp(q.xyz, -1.0f, 1.0f) - q.xyz;
    q *= clamp(1.0f/dot(q.xyz, q.xyz), 1.0f, 1.0f/0.45F);
    q = 3.0f*q - c;
  }

  return min((length(q.xz) - 0.3f)/q.w, p.y + 1.0f);
}

static float3 material(float3 p, float time) {
  return (float3)(1.0f);
}

static float3 shade(float3 pos, float3 nor,
                    float3 ref, float3 rd,
                    float td, float se, float ti) {

  float3 col = (float3)(0.0f);

  float3 key = normalize(light);
  float3 dom = normalize((float3)(0.0f, 1.0f, 0.0f) + cone(se + 12.23f));

  col += clamp(dot(key, nor), 0.0f, 1.0f)
    *(float3)(1.64f, 1.27f, 0.99f)
    *step(0.0f, -trace(pos + nor*0.00f, key, 10.0f, ti));

  col += clamp(dot(dom, nor), 0.0f, 1.0f)
    *(float3)(0.16f, 0.20f, 0.30f)
    *step(0.0f, -trace(pos + nor*0.00f, dom, 10.0f, ti));

  col *= 0.4f;

  return col;
}

static void camera(float3 *ro, float3 *rd, float2 uv, float time) {
  float atime = 0.4f*time;
  *ro = (float3)(3.5f*sin(atime), 1.7f - fabs(sin(atime)), -3.5f*cos(atime));
  float3 lat = (float3)(0.0f, 0.0f, 0.0f);

  float3 ww = normalize(lat-*ro);
  float3 uu = normalize(cross((float3)(0.0f, 1.0f, 0.0f), ww));
  float3 vv = normalize(cross(ww, uu));

  *rd = normalize(uv.x*uu + uv.y*vv + 0.97f*ww);
}
