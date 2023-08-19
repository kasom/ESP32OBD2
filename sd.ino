duk_ret_t native_sd_cardSizeMiB(duk_context *duk_ctx) {
    duk_push_number(duk_ctx, (double)SD.cardSize() / (1024 * 1024));

    return 1;
}

duk_ret_t native_sd_totalMiB(duk_context *duk_ctx) {
    duk_push_number(duk_ctx, (double)SD.totalBytes() / (1024 * 1024));

    return 1;
}

duk_ret_t native_sd_usedMiB(duk_context *duk_ctx) {
    duk_push_number(duk_ctx, (double)SD.usedBytes() / (1024 * 1024));

    return 1;
}

void register_sd_functions(duk_context *duk_ctx) {
    DUK_ADD_FUNCTION("sd_cardSizeMiB", native_sd_cardSizeMiB, 0);
    DUK_ADD_FUNCTION("sd_totalMiB", native_sd_totalMiB, 0);
    DUK_ADD_FUNCTION("sd_usedMiB", native_sd_usedMiB, 0);
}
