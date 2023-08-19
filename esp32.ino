duk_ret_t native_getHeapSize(duk_context *duk_ctx) {
    duk_push_int(duk_ctx, ESP.getHeapSize());
    return 1;
}

duk_ret_t native_getFreeHeap(duk_context *duk_ctx) {
    duk_push_int(duk_ctx, ESP.getFreeHeap());
    return 1;
}

duk_ret_t native_getMinFreeHeap(duk_context *duk_ctx) {
    duk_push_int(duk_ctx, ESP.getMinFreeHeap());
    return 1;
}

duk_ret_t native_getMaxAllocHeap(duk_context *duk_ctx) {
    duk_push_int(duk_ctx, ESP.getMaxAllocHeap());
    return 1;
}

duk_ret_t native_getPsramSize(duk_context *duk_ctx) {
    duk_push_int(duk_ctx, ESP.getPsramSize());
    return 1;
}

duk_ret_t native_getFreePsram(duk_context *duk_ctx) {
    duk_push_int(duk_ctx, ESP.getFreePsram());
    return 1;
}

duk_ret_t native_getMinFreePsram(duk_context *duk_ctx) {
    duk_push_int(duk_ctx, ESP.getMinFreePsram());
    return 1;
}

duk_ret_t native_getMaxAllocPsram(duk_context *duk_ctx) {
    duk_push_int(duk_ctx, ESP.getMaxAllocPsram());
    return 1;
}

duk_ret_t native_deepSleepStart(duk_context *duk_ctx) {
    esp_deep_sleep_start();
    // never return
    return 0;
}

duk_ret_t native_pinMode(duk_context *duk_ctx) {
  pinMode(duk_get_int(duk_ctx,-2),duk_get_int(duk_ctx,-1));

  return 0;
}

duk_ret_t native_digitalWrite(duk_context *duk_ctx) {
  digitalWrite(duk_get_int(duk_ctx,-2),duk_get_int(duk_ctx,-1));

  return 0;
}

duk_ret_t native_digitalRead(duk_context *duk_ctx) {
  duk_push_boolean(duk_ctx,digitalRead(duk_get_int(duk_ctx,-1)));

  return 1;
}

void register_esp32_functions(duk_context *duk_ctx) {
    duk_push_c_function(duk_ctx, native_getHeapSize, 0);
    duk_put_global_string(duk_ctx, "getHeapSize");
    duk_push_c_function(duk_ctx, native_getFreeHeap, 0);
    duk_put_global_string(duk_ctx, "getFreeHeap");
    duk_push_c_function(duk_ctx, native_getMinFreeHeap, 0);
    duk_put_global_string(duk_ctx, "getMinFreeHeap");
    duk_push_c_function(duk_ctx, native_getMaxAllocHeap, 0);
    duk_put_global_string(duk_ctx, "getMaxAllocHeap");

    duk_push_c_function(duk_ctx, native_getPsramSize, 0);
    duk_put_global_string(duk_ctx, "getPsramSize");
    duk_push_c_function(duk_ctx, native_getFreePsram, 0);
    duk_put_global_string(duk_ctx, "getFreePsram");
    duk_push_c_function(duk_ctx, native_getMinFreePsram, 0);
    duk_put_global_string(duk_ctx, "getMinFreePsram");
    duk_push_c_function(duk_ctx, native_getMaxAllocPsram, 0);
    duk_put_global_string(duk_ctx, "getMaxAllocPsram");

    duk_push_c_function(duk_ctx, native_deepSleepStart, 0);
    duk_put_global_string(duk_ctx, "deepSleepStart");

    duk_push_c_function(duk_ctx, native_pinMode, 2);
    duk_put_global_string(duk_ctx, "pinMode");
    duk_push_c_function(duk_ctx, native_digitalWrite, 2);
    duk_put_global_string(duk_ctx, "digitalWrite");
    duk_push_c_function(duk_ctx, native_digitalRead, 1);
    duk_put_global_string(duk_ctx, "digitalRead");

    DUK_ADD_INT("INPUT", INPUT);
    DUK_ADD_INT("OUTPUT", OUTPUT);

    DUK_ADD_INT("ESP_OK", ESP_OK);
    DUK_ADD_INT("ESP_FAIL", ESP_FAIL);
    DUK_ADD_INT("ESP_ERR_NO_MEM", ESP_ERR_NO_MEM);
    DUK_ADD_INT("ESP_ERR_NOT_SUPPORTED", ESP_ERR_NOT_SUPPORTED);
    DUK_ADD_INT("ESP_ERR_INVALID_STATE", ESP_ERR_INVALID_STATE);
    DUK_ADD_INT("ESP_ERR_INVALID_ARG", ESP_ERR_INVALID_ARG);
    DUK_ADD_INT("ESP_ERR_TIMEOUT", ESP_ERR_TIMEOUT);

    DUK_ADD_INT("GPIO_NUM_NC", -1);
#ifdef CONFIG_IDF_TARGET_ESP32S3
    for (int i = 0; i < 49; i++) {
        char buf[20];
        sprintf(buf, "GPIO_NUM_%d", i);
        DUK_ADD_INT(buf, i);
    }
#endif
}
