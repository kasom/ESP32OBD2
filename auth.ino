// taken from https://github.com/openvehicles/Open-Vehicle-Monitoring-System-3/blob/master/vehicle/OVMS.V3/components/vehicle_mgev/src/mg_auth.cpp
uint32_t umul_lsr45(uint32_t a, uint32_t b) {
    uint32_t i = a & 0xffffu;
    a >>= 16u;
    uint32_t j = b & 0xffffu;
    b >>= 16u;
    return (((((i * j) >> 16u) + (i * b + j * a)) >> 16u) + (a * b)) >> 13u;
}

uint32_t iterate(uint32_t seed, uint32_t count) {
    while (count--) {
        seed = (seed << 1u) | ((((((((seed >> 6u) ^ seed) >> 12u) ^ seed) >> 10u) ^ seed) >> 2u) & 1u);
    }
    return seed;
}

uint32_t GWMKey1(uint32_t seed) {
    uint32_t i = seed & 0xffffu;
    uint32_t i2 = 1u;
    uint32_t i3 = 0x12e5u;
    while (i3) {
        if (i3 & 1u)
        {
            uint32_t tmp = i2 * i;
            i2 = (tmp - (umul_lsr45(tmp, 0x82b87f05u) * 0x3eabu));
        }
        uint32_t tmp2 = i * i;
        i = (tmp2 - (umul_lsr45(tmp2, 0x82b87f05u) * 0x3eabu));
        i3 >>= 1u;
    }
    uint32_t i5 = ((i2 >> 8u) + i2) ^ 0x0fu;
    uint32_t i6 = (i2 ^ (i5 << 8u)) & 0xff00u;
    uint32_t i7 = ((i2 ^ i5) & 0xffu) | i6;
    return (i7 | i7 << 16u) ^ 0xad0779e2u;
}

uint32_t GWMKey2(uint32_t seed) {
    uint32_t count = 0x25u + (((seed >> 0x18u) & 0x1cu) ^ 0x08u);
    return iterate(seed, count) ^ 0xdc8fe1aeu;
}

uint32_t BCMKey(uint32_t seed) {
    uint32_t count = 0x2bu + (((seed >> 0x18u) & 0x17u) ^ 0x02u);
    return iterate(seed, count) ^ 0x594e348au;
}

duk_ret_t native_GWMKey1(duk_context *ctx) {
  uint32_t t=GWMKey1(duk_get_int(ctx, -1));

  duk_idx_t arr_idx=duk_push_array(ctx);
  duk_push_uint(ctx,(t>>24 & 0xff));
  duk_put_prop_index(ctx,arr_idx,0);
  duk_push_uint(ctx,(t>>16 & 0xff));
  duk_put_prop_index(ctx,arr_idx,1);
  duk_push_uint(ctx,(t>>8 & 0xff));
  duk_put_prop_index(ctx,arr_idx,2);
  duk_push_uint(ctx,(t & 0xff));
  duk_put_prop_index(ctx,arr_idx,3);

  return 1;
}

duk_ret_t native_GWMKey2(duk_context *ctx) {
  uint32_t t=GWMKey2(duk_get_int(ctx, -1));

  duk_idx_t arr_idx=duk_push_array(ctx);
  duk_push_uint(ctx,(t>>24 & 0xff));
  duk_put_prop_index(ctx,arr_idx,0);
  duk_push_uint(ctx,(t>>16 & 0xff));
  duk_put_prop_index(ctx,arr_idx,1);
  duk_push_uint(ctx,(t>>8 & 0xff));
  duk_put_prop_index(ctx,arr_idx,2);
  duk_push_uint(ctx,(t & 0xff));
  duk_put_prop_index(ctx,arr_idx,3);

  return 1;
}

duk_ret_t native_BCMKey(duk_context *ctx) {
  uint32_t t=BCMKey(duk_get_int(ctx, -1));

  duk_idx_t arr_idx=duk_push_array(ctx);
  duk_push_uint(ctx,(t>>24 & 0xff));
  duk_put_prop_index(ctx,arr_idx,0);
  duk_push_uint(ctx,(t>>16 & 0xff));
  duk_put_prop_index(ctx,arr_idx,1);
  duk_push_uint(ctx,(t>>8 & 0xff));
  duk_put_prop_index(ctx,arr_idx,2);
  duk_push_uint(ctx,(t & 0xff));
  duk_put_prop_index(ctx,arr_idx,3);

  return 1;
}

void register_auth_functions(duk_context *ctx) {
  DUK_ADD_FUNCTION("native_GWMKey1",native_GWMKey1,1);
  DUK_ADD_FUNCTION("native_GWMKey2",native_GWMKey2,1);
  DUK_ADD_FUNCTION("native_BCMKey",native_BCMKey,1);
}
