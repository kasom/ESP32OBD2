const SINGLE_FRAME_WAIT_MS = 200;
const TWAI_WAIT_BEFORE_START_MILLIS = 100;
const FC_WAIT_MS = 200; // time to wait for a flow control frame
const CF_WAIT_MS = 200; // time to wait for a consecutive frame

const FRAME_TYPE = {
    SF: 0x00,
    FF: 0x10,
    CF: 0x20,
    FC: 0x30
}

function txrx_log(message) {
    //prinln(message);
}

function twai_tx(id, data) {
    var ret = twai.transmit_f(0, id, data);

    if (ret != ESP_OK) {
        switch (ret) {
            case ESP_OK: break;
            case ESP_ERR_INVALID_ARG: txrx_log("Invalid arguments"); break;
            case ESP_ERR_TIMEOUT: txrx_log("Timed out waiting for space on TX queue"); break;
            case ESP_FAIL: txrx_log("TX queue is disabled and another message is currently transmitting"); break;
            case ESP_ERR_INVALID_STATE: txrx_log("ESP_ERR_INVALID_STATE: TWAI driver is not in running state, or is not installed"); break;
            case ESP_ERR_NOT_SUPPORTED: txrx_log("ESP_ERR_NOT_SUPPORTED: Listen Only Mode does not support transmissions"); break;
            default: txrx_log("Unknown " + ret);
        }

        var twai_status = twai.get_status_info();
        //txrx_log(JSON.stringify(twai_status));

        switch (twai_status.state) {
            case twai.TWAI_STATE_STOPPED: txrx_log("stopped");
                if (millis() > twai_bus_off_millis + TWAI_WAIT_BEFORE_START_MILLIS) {
                    twai.start();
                } else {
                    // keep waiting
                }
                break;
            case twai.TWAI_STATE_RUNNING: txrx_log("running"); break;
            case twai.TWAI_STATE_BUS_OFF: txrx_log("bus off");
                twai.initiate_recovery();
                twai_bus_off_millis = millis();
                break;
            case twai.TWAI_STATE_RECOVERING: txrx_log("recovering"); break;
            default: println("Unknown state " + twai_status);
        }

        return false;
    }
    return true;
}

function msg_tx(id, data) {
    if (data.length <= 7) {
        // Single frame

        data.unshift(FRAME_TYPE.SF | data.length);
        while (data.length < 8) {
            data.push(0xAA);
        }

        twai_tx(id, data);
    } else if (data.length <= 4095) {
        // Multiple frames
        var frame_no = 0;
        var current_pos = 0;
        var remaining = data.length;
        var frame = data.split(0, 6);
        frame.unshift(data.length & 0xff);
        frame.unshift((FRAME_TYPE.FF | data.length >> 8))

        twai_tx(id, frame);
        frame_no++;
        current_pos += 6;
        remaining -= 6;

        while (remaining) {
            // wait for FC frame
            var ret = twai.receive(FC_WAIT_MS);

            // NOT DONE YET!
        }
    }
}
