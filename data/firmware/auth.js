function authDebug(message) {
	//gfx.println(message);
	//println(message);
}

function gwmAuth(timeout) {
	twai_tx(CAN_ID_GWM, [0x02, 0x10, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00]);
	authDebug("GWM_AUTH: Starting authentication.");

	var isSuccess = false;
	var deadline = millis() + timeout;

	do {
		var ret = twai.receive_f(0);
		if (typeof ret != 'number') {
			var d = ret.data;

			if (d.length != 8 || ret.identifier != (CAN_ID_GWM+8)) {
				authDebug("GWM_AUTH: Ignoring a message from " + ret.identifier.toString(16) + ", length " + d.length)
			} else {
				if (d[0] == 0x06 && (d[1] & 0xbf) == 0x10 && d[2] == 0x01) {
					twai_tx(CAN_ID_GWM, [0x02, 0x10, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00]);
					authDebug("GWM_AUTH: sending 1003");
				} else if (d[0] == 0x06 && (d[1] & 0xbf) == 0x10 && d[2] == 0x03) {
					twai_tx(CAN_ID_GWM, [0x06, 0x27, 0x41, 0x3e, 0xab, 0x00, 0x0d, 0x00]);
					authDebug("GWM_AUTH: Request seed1");
				} else if (d[0] == 0x06 && (d[1] & 0xbf) == 0x27 && d[2] == 0x41) {
					var seed = (d[3] << 24) | (d[4] << 16) | (d[5] << 8) | d[6];
					var key = native_GWMKey1(seed);
					twai_tx(CAN_ID_GWM, [0x06, 0x27, 0x42, key[0], key[1], key[2], key[3], 0x00]);
					authDebug("GWM auth: seed1 received, sending reply");
				} else if (d[0] == 0x02 && (d[1] & 0xbf) == 0x27 && d[2] == 0x42) {
					authDebug("GWM_AUTH: key1 accepted, requesting seed2");
					twai_tx(CAN_ID_GWM, [0x02, 0x27, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00]);
				} else if (d[0] == 0x06 && (d[1] & 0xbf) == 0x27 && d[2] == 0x01) {
					var seed = (d[3] << 24) | (d[4] << 16) | (d[5] << 8) | d[6];
					var key = native_GWMKey2(seed);
					twai_tx(CAN_ID_GWM, [0x06, 0x27, 0x02, key[0], key[1], key[2], key[3], 0x00]);
					authDebug("GWM_AUTH: seed2 received, sending reply");
				} else if (d[0] == 0x02 && (d[1] & 0xbf) == 0x27 && d[2] == 0x02) {
					twai_tx(CAN_ID_GWM, [0x05, 0x31, 0x01, 0xaa, 0xff, 0x00, 0x00, 0x00]);
					authDebug("GWM_AUTH: key2 accepted, starting routine");
				} else if (d[0] == 0x04 && (d[1] & 0xbf) == 0x31 && d[2] == 0x01) {
					twai_tx(CAN_ID_GWM, [0x04, 0x31, 0x03, 0xaa, 0xff, 0x00, 0x00, 0x00]);
					authDebug("GWM_AUTH: Routine started, request routine control");
				} else if (d[0] == 0x05 && (d[1] & 0xbf) == 0x31 && d[2] == 0x03) {
					authDebug("GWM_AUTH: Done");
					isSuccess = true;
				}
			}
		}
	} while (!isSuccess && (millis() < deadline));

	if (!isSuccess) {
		authDebug("GWM_AUTH: Failed.");
	}

	return isSuccess;
}

function sendTesterPresent() {
	twai_tx(CAN_ID_BCM, [0x02, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]);
}

function startDrlTurnOff() {
	twai_tx(CAN_ID_BCM, [0x10, 0x0c, 0x2f, 0xd1, 0x17, 0x03, 0x00, 0x00]);
	authDebug("BCM: DRL first frame");
}

function drlOff(timeout) {
	//gfx.fillScreen(TFT_CONST.COLOR.BLACK);
	//gfx.setCursor(0, 0);
	twai_tx(CAN_ID_BCM, [0x02, 0x10, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00]);
	authDebug("BCM: Starting authentication.");


	var isSuccess = false;
	var deadline = millis() + timeout;
	var firstFrameSent = 0;
	var drlResponse = [0, 0, 0, 0, 0];

	do {
		var ret = twai.receive_f(0);
		if (typeof ret != 'number') {
			var d = ret.data;

			if (d.length != 8 || ret.identifier != (CAN_ID_BCM+8)) {
				authDebug("BCM: Ignoring a message from " + ret.identifier.toString(16) + ", length " + d.length)
			} else {
				if (d[0] == 0x06 && (d[1] & 0xbf) == 0x10 && d[2] == 0x03) {
					authDebug("BCM: TESTERPRESENT & requesting seed");
					sendTesterPresent();
					twai_tx(CAN_ID_BCM, [0x02, 0x27, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00]);
				}  else if (d[0] == 0x06 && (d[1] & 0xbf) == 0x27 && d[2] == 0x01) {
					var seed = (d[3] << 24) | (d[4] << 16) | (d[5] << 8) | d[6];

					if (seed == 0) {
						authDebug("BCM: seed received=0. BCM authenticated, start DRL off control.");
						startDrlTurnOff();
						firstFrameSent = millis();
					} else {
						var key = native_BCMKey(seed);
						twai_tx(CAN_ID_BCM, [0x06, 0x27, 0x02, key[0], key[1], key[2], key[3], 0x00]);
						authDebug("BCM: seed received " + seed.toString(16) + ". Replying with key " + key[0] + " " + key[1] + " " + key[2] + " " + key[3]);
					}
				}  else if (d[0] == 0x02 && (d[1] & 0xbf) == 0x27 && d[2] == 0x02) {
					authDebug("BCM: key accepted, authentication complete");
					startDrlTurnOff();
					firstFrameSent = millis();
				} else if (d[0] == 0x30 && d[1] == 0x08) {
					var desiredSeparationTime = 0;
					if (d[2] <= 127) {
						desiredSeparationTime = d[2];
					} else if (d[2] >= 0xf1 && d[2] <= 0xf9) {
						desiredSeparationTime = 100 * (d[2] - 0xf0);
					}
					while (millis() < desiredSeparationTime); // just wait

					twai_tx(CAN_ID_BCM, [0x21, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00]);
					authDebug("BCM: DRL next frame");
				} else if (d[0] == 0x10 && d[1] == 0x08 && d[2] == 0x6f) {
					drlResponse[0] = d[4];
					drlResponse[1] = d[5];
					drlResponse[2] = d[6];
					twai_tx(CAN_ID_BCM, [0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]);
					authDebug("BCM: Send flow control and save the first half of the response");
				} else if (d[0] == 0x21) {
					drlResponse[3] = d[1];
					drlResponse[4] = d[2];

					authDebug("BCM: Second multi-frame response received. Total DRL response = "
						+ drlResponse[0].toString(16) + " " + drlResponse[1].toString(16) + " " + drlResponse[2].toString(16) + " " + drlResponse[3].toString(16) + " " + drlResponse[4].toString(16));

					sendTesterPresent();

					// make sure that nothing left?
					twai_tx(CAN_ID_BCM, [0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]);

					isSuccess=true;
				}
			}
		}
	} while (!isSuccess && (millis() < deadline));

	if (!isSuccess) {
		authDebug("BCM: Failed.");
	}

	return isSuccess;
}
