static float tmax() { return 10.0f; }
static float fudge() { return 1.0f; }
static int iterations() { return 200; }

static float2 de(float3 p, float time) {
  float4 q = (float4)(p, 1.0f);
  q.xyz -= (float3)(1.0f, 1.8f, 1.0f);

  for(int i = 0; i < 5; i++) {
    q.xyz = fabs(q.xyz + 1.0f) - 1.0f;
    q /= clamp(dot(q.xyz, q.xyz), 0.25f, 1.0f);
    q *= 1.1f;
  }

  float f = (length(q.xyz) - 1.5f)/q.w;

  f = fmin(f, p.y + 1.0f);

  return (float2)(f, 0.0f);
}

static float3 shade(float3 pos, float3 nor, float3 ref, float3 rd, float2 t, float se, float time) {
  float3 col = (float3)(0.0f);

  float3 key = normalize((float3)(0.0f, 1.0f, 0.0f) + cone(se + 2.23f));

  float edg = edge(pos, 0.55f, time);

  col += 0.5f*clamp(dot(key, nor), 0.0f, 1.0f)
    *step(0.0f, -trace(pos + nor*0.001f, key, 10.0f, time).x);

  col += 0.5f*clamp(1.0f + dot(rd, nor), 0.0f, 1.0f)
    *step(0.0f, -trace(pos + nor*0.001f, ref, 10.0f, time).x);

  col += (float3)(10.0f, 0.2f, 0.2f)*edg;

  return col;
}

static void camera(float3* ro, float3* rd, float2 uv, float time) {
  *ro = (float3)(3.5f*sin(time), 2.0f, -3.5f*cos(time));

  float3 ww = normalize((float3)(0.0f, 2.0f + sin(time), 0.0f)-*ro);
  float3 uu = normalize(cross((float3)(0.0f, 1.0f, 0.0f), ww));
  float3 vv = normalize(cross(ww, uu));

  *rd = normalize(uv.x*uu + uv.y*vv + 0.97f*ww);
}
