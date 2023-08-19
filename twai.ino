enum TWAI_SPEED
{
    TWAI_SPEED_25,
    TWAI_SPEED_50,
    TWAI_SPEED_100,
    TWAI_SPEED_125,
    TWAI_SPEED_250,
    TWAI_SPEED_500,
    TWAI_SPEED_800,
    TWAI_SPEED_1000
};

// Library/Arduino15/packages/esp32/hardware/esp32/2.0.9/tools/sdk/esp32s3/include/driver/include/driver/twai.h 
// larger rx_queue_len
#define OUR_TWAI_GENERAL_CONFIG_DEFAULT(tx_io_num, rx_io_num, op_mode) {.mode = op_mode, .tx_io = tx_io_num, .rx_io = rx_io_num,        \
                                                                    .clkout_io = TWAI_IO_UNUSED, .bus_off_io = TWAI_IO_UNUSED,      \
                                                                    .tx_queue_len = 5, .rx_queue_len = 32,                           \
                                                                    .alerts_enabled = TWAI_ALERT_NONE,  .clkout_divider = 0,        \
                                                                    .intr_flags = ESP_INTR_FLAG_LEVEL1}


// tx_pin(-5), rx_pin(-4), mode(-3), speed(-2), filter(-1)
int native_twai_driver_install(duk_context *duk_ctx)
{
    // Initialize configuration structures using macro initializers
    twai_general_config_t g_config = OUR_TWAI_GENERAL_CONFIG_DEFAULT((gpio_num_t)duk_get_int(duk_ctx, -5), (gpio_num_t)duk_get_int(duk_ctx, -4), (twai_mode_t)duk_get_int(duk_ctx, -3));
    ///printf("tx=%d,  rx=%d, mode=%d\n", duk_get_int(duk_ctx, -5), duk_get_int(duk_ctx, -4), duk_get_int(duk_ctx, -3));
    twai_timing_config_t t_config;
    switch ((TWAI_SPEED)duk_get_int(duk_ctx, -2))
    {
    case TWAI_SPEED_25:
        t_config = TWAI_TIMING_CONFIG_25KBITS();
        break;
    case TWAI_SPEED_50:
        t_config = TWAI_TIMING_CONFIG_50KBITS();
        break;
    case TWAI_SPEED_100:
        t_config = TWAI_TIMING_CONFIG_100KBITS();
        break;
    case TWAI_SPEED_125:
        t_config = TWAI_TIMING_CONFIG_125KBITS();
        break;
    case TWAI_SPEED_250:
        t_config = TWAI_TIMING_CONFIG_250KBITS();
        break;
    case TWAI_SPEED_500:
        t_config = TWAI_TIMING_CONFIG_500KBITS();
        break;
    case TWAI_SPEED_800:
        t_config = TWAI_TIMING_CONFIG_800KBITS();
        break;
    case TWAI_SPEED_1000:
        t_config = TWAI_TIMING_CONFIG_1MBITS();
        break;
    default:
        printf("Invalid TWAI speed.\n");
    }

    duk_require_object(duk_ctx, -1);

    duk_get_prop_string(duk_ctx, -1, "acceptance_code");
    int acceptance_code = duk_get_uint(duk_ctx, -1);
    duk_pop(duk_ctx);

    duk_get_prop_string(duk_ctx, -1, "acceptance_mask");
    int acceptance_mask = duk_get_uint(duk_ctx, -1);
    duk_pop(duk_ctx);

    duk_get_prop_string(duk_ctx, -1, "single_filter");
    bool single_filter = duk_get_boolean(duk_ctx, -1);
    duk_pop(duk_ctx);

    //printf("acceptance_code=%08x, acceptance_mask=%08x, single_filter=%c\n", acceptance_code, acceptance_mask, single_filter ? 'T' : 'F');

    duk_pop(duk_ctx);

    twai_filter_config_t f_config = {.acceptance_code = acceptance_code, .acceptance_mask = acceptance_mask, .single_filter = single_filter};
    // TWAI_FILTER_CONFIG_ACCEPT_ALL() = {.acceptance_code = 0, .acceptance_mask = 0xFFFFFFFF, .single_filter = true}

    int ret = twai_driver_install(&g_config, &t_config, &f_config);

    //printf("twai_driver_install returned %d\n", ret);

    duk_push_int(duk_ctx, ret);

    return 1;
}

duk_ret_t native_twai_driver_uninstall(duk_context *duk_ctx)
{
    duk_push_int(duk_ctx, twai_driver_uninstall());

    return 1;
}

duk_ret_t native_twai_start(duk_context *duk_ctx)
{
    duk_push_int(duk_ctx, twai_start());

    return 1;
}

duk_ret_t native_twai_stop(duk_context *duk_ctx)
{
    duk_push_int(duk_ctx, twai_stop());

    return 1;
}

duk_ret_t native_twai_clear_transmit_queue(duk_context *duk_ctx)
{
    duk_push_int(duk_ctx, twai_clear_transmit_queue());

    return 1;
}

duk_ret_t native_twai_clear_receive_queue(duk_context *duk_ctx)
{
    duk_push_int(duk_ctx, twai_clear_receive_queue());

    return 1;
}

#ifdef UDP_LOG
void can2str(twai_message_t *message,char *messageBuffer) {
  sprintf(messageBuffer,"%x\t%x %x ",message->identifier,message->flags,message->data_length_code);
  
  if (message->data_length_code) {
    char *p=messageBuffer+strlen(messageBuffer);
  
    for (int i=0;i<message->data_length_code;i++) {
      sprintf(p,"%02x ",message->data[i]);
      p+=3;
    }
    *p=0;
  }
}
#endif

duk_ret_t native_twai_receive_f(duk_context *duk_ctx)
{
    twai_message_t message;

    esp_err_t ret = twai_receive(&message, pdMS_TO_TICKS(duk_get_int(duk_ctx, -1)));

    if (ret == ESP_OK)
    {
#ifdef UDP_LOG
        if (wifiOn) {
          char messageBuffer[256];
          char messageWithTimestamp[300];
  
          can2str(&message,messageBuffer);
    
          sprintf(messageWithTimestamp,"%lu\t%s",millis(),messageBuffer);
          wifiUDP.beginPacket(can2udpIP,can2udpPort);
          wifiUDP.write((const uint8_t *)messageWithTimestamp,strlen(messageWithTimestamp));
          wifiUDP.endPacket();
        }
#endif
      
        duk_idx_t obj_idx;
        obj_idx = duk_push_object(duk_ctx);
        duk_push_int(duk_ctx, message.identifier);
        duk_put_prop_string(duk_ctx, obj_idx, "identifier");

        duk_idx_t arr_idx = duk_push_array(duk_ctx);

        for (int i = 0; i < message.data_length_code; i++)
        {
            duk_push_int(duk_ctx, message.data[i]);
            duk_put_prop_index(duk_ctx, arr_idx, i);
        }

        duk_put_prop_string(duk_ctx, obj_idx, "data");
    }
    else
    {
        duk_push_int(duk_ctx, ret);
    }

    return 1;
}

duk_ret_t native_twai_receive(duk_context *duk_ctx)
{
    twai_message_t message;

    esp_err_t ret = twai_receive(&message, pdMS_TO_TICKS(duk_get_uint(duk_ctx, -1)));

    if (ret == ESP_OK)
    {
        duk_idx_t obj_idx;
        obj_idx = duk_push_object(duk_ctx);
        duk_push_int(duk_ctx, message.extd);
        duk_put_prop_string(duk_ctx, obj_idx, "extd");
        duk_push_int(duk_ctx, message.rtr);
        duk_put_prop_string(duk_ctx, obj_idx, "rtr");
        duk_push_int(duk_ctx, message.dlc_non_comp);
        duk_put_prop_string(duk_ctx, obj_idx, "dlc_non_comp");
        duk_push_int(duk_ctx, message.identifier);
        duk_put_prop_string(duk_ctx, obj_idx, "identifier");
        duk_push_int(duk_ctx, message.data_length_code);
        duk_put_prop_string(duk_ctx, obj_idx, "data_length_code");

        duk_idx_t arr_idx = duk_push_array(duk_ctx);

        for (int i = 0; i < message.data_length_code; i++)
        {
            duk_push_int(duk_ctx, message.data[i]);
            duk_put_prop_index(duk_ctx, arr_idx, i);
        }

        duk_put_prop_string(duk_ctx, obj_idx, "data");
    }
    else
    {
        duk_push_int(duk_ctx, ret);
    }

    return 1;
}

// timeout(-3), identifier (-2), msg[] (-1)
duk_ret_t native_twai_transmit_f(duk_context *duk_ctx)
{
    twai_message_t message;

    message.extd = 0;
    message.rtr = 0;
    message.ss = 1; // TWAI_MSG_FLAG_SS Transmit message using Single Shot Transmission
                    // (Message will not be retransmitted upon error or loss of arbitration)
    message.self = 0;
    message.identifier = duk_get_uint(duk_ctx, -2);

    duk_require_object(duk_ctx, -1);
    int len = duk_get_length(duk_ctx, -1);

    if (len > TWAI_FRAME_MAX_DLC)
        len = TWAI_FRAME_MAX_DLC;

    message.data_length_code = len;

    for (int i = 0; i < TWAI_FRAME_MAX_DLC; i++)
    {
        if (i < len)
        {
            duk_get_prop_index(duk_ctx, -1, i);
            unsigned b = duk_get_uint(duk_ctx, -1);

            duk_pop(duk_ctx);
            message.data[i] = b;
        }
        else
        {
            message.data[i] = 0xAA;
        }
    }
    duk_pop(duk_ctx);

    esp_err_t ret = twai_transmit(&message, pdMS_TO_TICKS(duk_get_int(duk_ctx, -3)));

    duk_push_int(duk_ctx, ret);

    return 1;
}

// timeout(-7), extd(-6), rtr(-5), ss(-4), self(-3), identifier (-2), msg[] (-1)
duk_ret_t native_twai_transmit(duk_context *duk_ctx)
{
    twai_message_t message;

    message.extd = duk_get_uint(duk_ctx, -6);
    message.rtr = duk_get_uint(duk_ctx, -5);
    message.ss = duk_get_uint(duk_ctx, -4);
    message.self = duk_get_uint(duk_ctx, -3);
    message.identifier = duk_get_uint(duk_ctx, -2);

    duk_require_object(duk_ctx, -1);
    int len = duk_get_length(duk_ctx, -1);

    if (len > TWAI_FRAME_MAX_DLC)
        len = TWAI_FRAME_MAX_DLC;

    message.data_length_code = len;

    for (int i = 0; i < TWAI_FRAME_MAX_DLC; i++)
    {
        if (i < len)
        {
            duk_get_prop_index(duk_ctx, -1, i);
            unsigned b = duk_get_uint(duk_ctx, -1);

            duk_pop(duk_ctx);
            message.data[i] = b;
        }
        else
        {
            message.data[i] = 0xAA;
        }
    }
    duk_pop(duk_ctx);

    esp_err_t ret = twai_transmit(&message, pdMS_TO_TICKS(duk_get_uint(duk_ctx, -7)));

    duk_push_int(duk_ctx, ret);

    return 1;
}

duk_ret_t native_twai_initiate_recovery(duk_context *duk_ctx)
{
    duk_push_int(duk_ctx, twai_initiate_recovery());

    return 1;
}

duk_ret_t native_twai_get_status_info(duk_context *duk_ctx)
{
    twai_status_info_t info;

    esp_err_t ret = twai_get_status_info(&info);

    if (ret == ESP_OK)
    {
        duk_idx_t obj_idx;
        obj_idx = duk_push_object(duk_ctx);

        duk_push_int(duk_ctx, info.state);
        duk_put_prop_string(duk_ctx, obj_idx, "state");

        duk_push_int(duk_ctx, info.msgs_to_tx);
        duk_put_prop_string(duk_ctx, obj_idx, "msgs_to_tx");

        duk_push_int(duk_ctx, info.msgs_to_rx);
        duk_put_prop_string(duk_ctx, obj_idx, "msgs_to_rx");

        duk_push_int(duk_ctx, info.tx_error_counter);
        duk_put_prop_string(duk_ctx, obj_idx, "tx_error_counter");

        duk_push_int(duk_ctx, info.rx_error_counter);
        duk_put_prop_string(duk_ctx, obj_idx, "rx_error_counter");

        duk_push_int(duk_ctx, info.tx_failed_count);
        duk_put_prop_string(duk_ctx, obj_idx, "tx_failed_count");

        duk_push_int(duk_ctx, info.rx_missed_count);
        duk_put_prop_string(duk_ctx, obj_idx, "rx_missed_count");

        duk_push_int(duk_ctx, info.arb_lost_count);
        duk_put_prop_string(duk_ctx, obj_idx, "arb_lost_count");

        duk_push_int(duk_ctx, info.bus_error_count);
        duk_put_prop_string(duk_ctx, obj_idx, "bus_error_count");
    }
    else
    {
        duk_push_int(duk_ctx, ret);
    }

    return 1;
}

duk_ret_t just_test(duk_context *duk_ctx)
{
    printf("This is a test.\n");
    duk_push_int(duk_ctx, 1);

    return 1;
}

void register_twai_functions(duk_context *duk_ctx)
{
    duk_idx_t o_idx = duk_push_object(duk_ctx);

    DUK_ADD_OBJ_FUNCTION(o_idx, "driver_install", native_twai_driver_install, 5);
    DUK_ADD_OBJ_FUNCTION(o_idx, "driver_uninstall", native_twai_driver_uninstall, 0);
    DUK_ADD_OBJ_FUNCTION(o_idx, "start", native_twai_start, 0);
    DUK_ADD_OBJ_FUNCTION(o_idx, "stop", native_twai_stop, 0);
    DUK_ADD_OBJ_FUNCTION(o_idx, "clear_transmit_queue", native_twai_clear_transmit_queue, 0);
    DUK_ADD_OBJ_FUNCTION(o_idx, "clear_receive_queue", native_twai_clear_receive_queue, 0);

    DUK_ADD_OBJ_FUNCTION(o_idx, "receive_f", native_twai_receive_f, 1);
    DUK_ADD_OBJ_FUNCTION(o_idx, "transmit_f", native_twai_transmit_f, 3);

    DUK_ADD_OBJ_FUNCTION(o_idx, "receive", native_twai_receive, 1);
    DUK_ADD_OBJ_FUNCTION(o_idx, "transmit", native_twai_transmit, 7);

    DUK_ADD_OBJ_FUNCTION(o_idx, "initiate_recovery", native_twai_initiate_recovery, 0);
    DUK_ADD_OBJ_FUNCTION(o_idx, "get_status_info", native_twai_get_status_info, 0);

    DUK_ADD_OBJ_INT(o_idx, "TWAI_MODE_NORMAL", TWAI_MODE_NORMAL);
    DUK_ADD_OBJ_INT(o_idx, "TWAI_MODE_NO_ACK", TWAI_MODE_NO_ACK);
    DUK_ADD_OBJ_INT(o_idx, "TWAI_MODE_LISTEN_ONLY", TWAI_MODE_LISTEN_ONLY);

    DUK_ADD_OBJ_INT(o_idx, "TWAI_STATE_STOPPED", TWAI_STATE_STOPPED);
    DUK_ADD_OBJ_INT(o_idx, "TWAI_STATE_RUNNING", TWAI_STATE_RUNNING);
    DUK_ADD_OBJ_INT(o_idx, "TWAI_STATE_BUS_OFF", TWAI_STATE_BUS_OFF);
    DUK_ADD_OBJ_INT(o_idx, "TWAI_STATE_RECOVERING", TWAI_STATE_RECOVERING);

    DUK_ADD_OBJ_INT(o_idx, "SPEED_25", TWAI_SPEED_25);
    DUK_ADD_OBJ_INT(o_idx, "SPEED_50", TWAI_SPEED_50);
    DUK_ADD_OBJ_INT(o_idx, "SPEED_100", TWAI_SPEED_100);
    DUK_ADD_OBJ_INT(o_idx, "SPEED_125", TWAI_SPEED_125);
    DUK_ADD_OBJ_INT(o_idx, "SPEED_250", TWAI_SPEED_250);
    DUK_ADD_OBJ_INT(o_idx, "SPEED_500", TWAI_SPEED_500);
    DUK_ADD_OBJ_INT(o_idx, "SPEED_800", TWAI_SPEED_800);
    DUK_ADD_OBJ_INT(o_idx, "SPEED_1000", TWAI_SPEED_1000);

    duk_put_global_string(duk_ctx, "twai");
}
