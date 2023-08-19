
duk_ret_t native_tft_setTextColor(duk_context *duk_ctx) {
    int n = duk_get_top(duk_ctx);  // #args

    if (n == 1) {
        gfx->setTextColor(duk_get_int(duk_ctx, -1));
    } else if (n >= 2) {
        gfx->setTextColor(duk_get_int(duk_ctx, -2), duk_get_int(duk_ctx, -1));
    }

    return 0;
}

duk_ret_t native_tft_setTextSize(duk_context *duk_ctx) {
    int n = duk_get_top(duk_ctx);  // #args

    if (n == 1) {
        gfx->setTextSize(duk_get_int(duk_ctx, -1));
    } else if (n == 2) {
        gfx->setTextSize(duk_get_int(duk_ctx, -2), duk_get_int(duk_ctx, -1));
    } else if (n > 2) {
        gfx->setTextSize(duk_get_int(duk_ctx, -3), duk_get_int(duk_ctx, -2), duk_get_int(duk_ctx, -1));
    }

    return 0;
}

duk_ret_t native_tft_fillScreen(duk_context *duk_ctx) {
    gfx->fillScreen(duk_get_int(duk_ctx, -1));

    return 0;
}

duk_ret_t native_tft_setCursor(duk_context *duk_ctx) {
    int n = duk_get_top(duk_ctx);  // #args

    if (n >= 2) {
        gfx->setCursor(duk_get_int(duk_ctx, -2), duk_get_int(duk_ctx, -1));
    }

    return 0;
}

duk_ret_t native_tft_print(duk_context *duk_ctx) {
    duk_push_string(duk_ctx, " ");
    duk_insert(duk_ctx, 0);
    duk_join(duk_ctx, duk_get_top(duk_ctx) - 1);
    gfx->print(duk_to_string(duk_ctx, -1));

    return 0;
}

duk_ret_t native_tft_println(duk_context *duk_ctx) {
    duk_push_string(duk_ctx, " ");
    duk_insert(duk_ctx, 0);
    duk_join(duk_ctx, duk_get_top(duk_ctx) - 1);
    gfx->println(duk_to_string(duk_ctx, -1));

    return 0;
}

duk_ret_t native_tft_getCursorX(duk_context *duk_ctx) {
    duk_push_int(duk_ctx, gfx->getCursorX());
    return 1;
}

duk_ret_t native_tft_getCursorY(duk_context *duk_ctx) {
    duk_push_int(duk_ctx, gfx->getCursorY());
    return 1;
}

duk_ret_t native_tft_setTextWrap(duk_context *duk_ctx) {
    gfx->setTextWrap(duk_get_boolean(duk_ctx, -1));

    return 0;
}

duk_ret_t native_tft_width(duk_context *duk_ctx) {
    duk_push_int(duk_ctx, gfx->width());

    return 1;
}

duk_ret_t native_tft_height(duk_context *duk_ctx) {
    duk_push_int(duk_ctx, gfx->height());

    return 1;
}

duk_ret_t native_tft_drawPixel(duk_context *duk_ctx) {
    int n = duk_get_top(duk_ctx);  // #args

    gfx->drawPixel(duk_get_int(duk_ctx, -3), duk_get_int(duk_ctx, -2), duk_get_int(duk_ctx, -1));

    return 0;
}

duk_ret_t native_tft_drawLine(duk_context *duk_ctx) {
    gfx->drawLine(duk_get_int(duk_ctx, -5), duk_get_int(duk_ctx, -4), duk_get_int(duk_ctx, -3), duk_get_int(duk_ctx, -2), duk_get_int(duk_ctx, -1));

    return 0;
}

duk_ret_t native_tft_drawRect(duk_context *duk_ctx) {
    gfx->drawRect(duk_get_int(duk_ctx, -5), duk_get_int(duk_ctx, -4), duk_get_int(duk_ctx, -3), duk_get_int(duk_ctx, -2), duk_get_int(duk_ctx, -1));

    return 0;
}

duk_ret_t native_tft_drawFastVLine(duk_context *duk_ctx) {
    gfx->drawFastVLine(duk_get_int(duk_ctx, -4), duk_get_int(duk_ctx, -3), duk_get_int(duk_ctx, -2), duk_get_int(duk_ctx, -1));

    return 0;
}

duk_ret_t native_tft_drawFastHLine(duk_context *duk_ctx) {
    gfx->drawFastHLine(duk_get_int(duk_ctx, -4), duk_get_int(duk_ctx, -3), duk_get_int(duk_ctx, -2), duk_get_int(duk_ctx, -1));

    return 0;
}

duk_ret_t native_tft_fillRect(duk_context *duk_ctx) {
    gfx->fillRect(duk_get_int(duk_ctx, -5), duk_get_int(duk_ctx, -4), duk_get_int(duk_ctx, -3), duk_get_int(duk_ctx, -2), duk_get_int(duk_ctx, -1));

    return 0;
}

duk_ret_t native_tft_drawCircle(duk_context *duk_ctx) {
    gfx->drawCircle(duk_get_int(duk_ctx, -4), duk_get_int(duk_ctx, -3), duk_get_int(duk_ctx, -2), duk_get_int(duk_ctx, -1));

    return 0;
}

duk_ret_t native_tft_fillCircle(duk_context *duk_ctx) {
    gfx->fillCircle(duk_get_int(duk_ctx, -4), duk_get_int(duk_ctx, -3), duk_get_int(duk_ctx, -2), duk_get_int(duk_ctx, -1));

    return 0;
}

duk_ret_t native_tft_drawEllipse(duk_context *duk_ctx) {
    gfx->drawEllipse(duk_get_int(duk_ctx, -5), duk_get_int(duk_ctx, -4), duk_get_int(duk_ctx, -3), duk_get_int(duk_ctx, -2), duk_get_int(duk_ctx, -1));

    return 0;
}

duk_ret_t native_tft_fillEllipse(duk_context *duk_ctx) {
    gfx->fillEllipse(duk_get_int(duk_ctx, -5), duk_get_int(duk_ctx, -4), duk_get_int(duk_ctx, -3), duk_get_int(duk_ctx, -2), duk_get_int(duk_ctx, -1));

    return 0;
}

duk_ret_t native_tft_drawTriangle(duk_context *duk_ctx) {
    gfx->drawTriangle(duk_get_int(duk_ctx, -7), duk_get_int(duk_ctx, -6), duk_get_int(duk_ctx, -5), duk_get_int(duk_ctx, -4), duk_get_int(duk_ctx, -3), duk_get_int(duk_ctx, -2), duk_get_int(duk_ctx, -1));

    return 0;
}

duk_ret_t native_tft_fillTriangle(duk_context *duk_ctx) {
    gfx->fillTriangle(duk_get_int(duk_ctx, -7), duk_get_int(duk_ctx, -6), duk_get_int(duk_ctx, -5), duk_get_int(duk_ctx, -4), duk_get_int(duk_ctx, -3), duk_get_int(duk_ctx, -2), duk_get_int(duk_ctx, -1));

    return 0;
}

duk_ret_t native_tft_setRotation(duk_context *duk_ctx) {
    gfx->setRotation(duk_get_int(duk_ctx, -1));

    return 0;
}

duk_ret_t native_tft_getRotation(duk_context *duk_ctx) {
#ifdef TOUCH_SCL
    duk_push_int(duk_ctx, gfx->getRotation());
#else
    duk_push_int(0);
#endif

    return 1;
}

duk_ret_t native_setTouchRotation(duk_context *duk_ctx) {
#ifdef TOUCH_SCL
    tp.setRotation(duk_get_int(duk_ctx, -1));
#endif

    return 0;
}

duk_ret_t native_getTouches(duk_context *duk_ctx) {
    duk_idx_t arr_idx = duk_push_array(duk_ctx);

#ifdef TOUCH_SCL
    tp.read();
    if (tp.isTouched) {
        for (int i = 0; i < tp.touches; i++) {
            duk_idx_t obj_idx;

            obj_idx = duk_push_object(duk_ctx);

            duk_push_int(duk_ctx, tp.points[i].x);
            duk_put_prop_string(duk_ctx, obj_idx, "x");

            duk_push_int(duk_ctx, tp.points[i].y);
            duk_put_prop_string(duk_ctx, obj_idx, "y");

            //      duk_pop(duk_ctx);

            duk_put_prop_index(duk_ctx, arr_idx, i);
        }
    }
#endif

    return 1;
}

void register_tft_functions(duk_context *duk_ctx) {
    duk_idx_t o_idx = duk_push_object(duk_ctx);

    DUK_ADD_OBJ_FUNCTION(o_idx, "setTextColor", native_tft_setTextColor, DUK_VARARGS);
    DUK_ADD_OBJ_FUNCTION(o_idx, "setTextSize", native_tft_setTextSize, DUK_VARARGS);
    DUK_ADD_OBJ_FUNCTION(o_idx, "fillScreen", native_tft_fillScreen, 1);
    DUK_ADD_OBJ_FUNCTION(o_idx, "setTouchRotation", native_setTouchRotation, 1);
    DUK_ADD_OBJ_FUNCTION(o_idx, "getTouches", native_getTouches, 0);
    DUK_ADD_OBJ_FUNCTION(o_idx, "setCursor", native_tft_setCursor, DUK_VARARGS);
    DUK_ADD_OBJ_FUNCTION(o_idx, "getCursorX", native_tft_getCursorX, 0);
    DUK_ADD_OBJ_FUNCTION(o_idx, "getCursorY", native_tft_getCursorY, 0);
    DUK_ADD_OBJ_FUNCTION(o_idx, "print", native_tft_print, DUK_VARARGS);
    DUK_ADD_OBJ_FUNCTION(o_idx, "println", native_tft_println, DUK_VARARGS);
    DUK_ADD_OBJ_FUNCTION(o_idx, "width", native_tft_width, 0);
    DUK_ADD_OBJ_FUNCTION(o_idx, "height", native_tft_height, 0);
    DUK_ADD_OBJ_FUNCTION(o_idx, "drawPixel", native_tft_drawPixel, 3);
    DUK_ADD_OBJ_FUNCTION(o_idx, "drawCircle", native_tft_drawCircle, 4);
    DUK_ADD_OBJ_FUNCTION(o_idx, "fillCircle", native_tft_fillCircle, 4);
    DUK_ADD_OBJ_FUNCTION(o_idx, "drawEllipse", native_tft_drawEllipse, 5);
    DUK_ADD_OBJ_FUNCTION(o_idx, "fillEllipse", native_tft_fillEllipse, 5);
    DUK_ADD_OBJ_FUNCTION(o_idx, "drawTriangle", native_tft_drawTriangle, 7);
    DUK_ADD_OBJ_FUNCTION(o_idx, "fillTriangle", native_tft_fillTriangle, 7);
    DUK_ADD_OBJ_FUNCTION(o_idx, "drawLine", native_tft_drawLine, 5);
    DUK_ADD_OBJ_FUNCTION(o_idx, "drawRect", native_tft_drawRect, 5);
    DUK_ADD_OBJ_FUNCTION(o_idx, "drawFastVLine", native_tft_drawFastVLine, 4);
    DUK_ADD_OBJ_FUNCTION(o_idx, "drawFastHLine", native_tft_drawFastHLine, 4);
    DUK_ADD_OBJ_FUNCTION(o_idx, "fillRect", native_tft_fillRect, 5);
    DUK_ADD_OBJ_FUNCTION(o_idx, "setTextWrap", native_tft_setTextWrap, 1);
    DUK_ADD_OBJ_FUNCTION(o_idx, "setRotation", native_tft_setRotation, 1);
    DUK_ADD_OBJ_FUNCTION(o_idx, "getRotation", native_tft_getRotation, 0);

    duk_put_global_string(duk_ctx, "gfx");
}
