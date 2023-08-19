
int ldrRead() {
#ifdef LDR_INPUT_PIN
    return analogRead(LDR_INPUT_PIN);
#else
    return 0;
#endif
}

int ldrReadMilliVolts() {
#ifdef LDR_INPUT_PIN
    return analogReadMilliVolts(LDR_INPUT_PIN);
#else
    return 0;
#endif
}

void setLedRgb(int r, int g, int b) {
#ifdef T35BOARD
    ledcWrite(LED_R_PWM_CHANNEL, r);
    ledcWrite(LED_G_PWM_CHANNEL, g);
    ledcWrite(LED_B_PWM_CHANNEL, b);
#endif
}

duk_ret_t native_setLedRgb(duk_context *duk_ctx) {
    int n = duk_get_top(duk_ctx);  // #args

    setLedRgb(duk_get_int(duk_ctx, -3), duk_get_int(duk_ctx, -2), duk_get_int(duk_ctx, -1));

    return 0;
}

duk_ret_t native_ldrRead(duk_context *duk_ctx) {
    duk_push_int(duk_ctx, ldrRead());
    return 1;
}

duk_ret_t native_ldrReadMilliVolts(duk_context *duk_ctx) {
    duk_push_int(duk_ctx, ldrReadMilliVolts());
    return 1;
}

void initLED_BL_LDR() {
#ifdef T35BOARD
    analogReadResolution(12);
    ledcSetup(LED_R_PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcSetup(LED_G_PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcSetup(LED_B_PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(LED_R_PIN, LED_R_PWM_CHANNEL);
    ledcAttachPin(LED_G_PIN, LED_G_PWM_CHANNEL);
    ledcAttachPin(LED_B_PIN, LED_B_PWM_CHANNEL);
    ledcWrite(LED_R_PWM_CHANNEL, 255);
    ledcWrite(LED_G_PWM_CHANNEL, 255);
    ledcWrite(LED_B_PWM_CHANNEL, 255);
#endif
    ledcSetup(LCD_BL_PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(LCD_BL_PIN, LCD_BL_PWM_CHANNEL);
    ledcWrite(LCD_BL_PWM_CHANNEL, 127);
}

duk_ret_t native_setBacklight(duk_context *duk_ctx) {
    int n = duk_get_top(duk_ctx);  // #args

    ledcWrite(LCD_BL_PWM_CHANNEL, duk_get_int(duk_ctx, -1));

    return 0;
}

void register_led_bl_functions(duk_context *duk_ctx) {
    duk_push_c_function(duk_ctx, native_setLedRgb, 3);
    duk_put_global_string(duk_ctx, "setLedRgb");
    duk_push_c_function(duk_ctx, native_ldrRead, 0);
    duk_put_global_string(duk_ctx, "ldrRead");
    duk_push_c_function(duk_ctx, native_ldrReadMilliVolts, 0);
    duk_put_global_string(duk_ctx, "ldrReadMilliVolts");
    duk_push_c_function(duk_ctx, native_setBacklight, 1);
    duk_put_global_string(duk_ctx, "setBacklight");
}
