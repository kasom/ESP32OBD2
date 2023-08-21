const APP_STATE_WAIT_DCDC=0;
const APP_STATE_GWM_AUTH=1;
const APP_STATE_RUNNING=2;

const GWM_AUTH_TIMEOUT_MS = 1000;
const DCDC_POLL_DELAY_MS = 500;
const DCDC_POLL_TOO_MANY = 180;
var rxCount=0;

var currentState = APP_STATE_WAIT_DCDC;
var oldState = null;

var wait_DCDC_data = {
	millisEnteredWAIT_DCDC: 0,
	millisLastPollDCDC: 0,
	pollCount: 0
};

function processWaitDCDCState() {
	var now = millis();

	if (oldState !== currentState) {
		wait_DCDC_data.millisEnteredWAIT_DCDC = now;
		wait_DCDC_data.millisLastPollDCDC = 0;
		wait_DCDC_data.pollCount=0;
		oldState = currentState;
		setTopBarLeft(TFT_CONST.COLOR.YELLOW,"Waiting HVB live");
		setTopBarCenter(TFT_CONST.COLOR.GREEN, "");
		setTopBarRight(TFT_CONST.COLOR.GREEN, "FW V.: "+fwVersion());
		requestList['State Watch'].lastState='';
		requestList['DRL Control'].lastDRL == '';
	}

	if (now > wait_DCDC_data.millisLastPollDCDC + DCDC_POLL_DELAY_MS) {
		var ret=null;
		var pushed=null;
		var touched=null;
		if (wait_DCDC_data.pollCount>=DCDC_POLL_TOO_MANY) {
			setBacklightIdle();
			setTopBarLeft(TFT_CONST.COLOR.YELLOW,"Touch to activate");
			setTopBarRight(TFT_CONST.COLOR.RED, "WAIT FOR TRIGGER");

			// Don't poll. Wait for any trigger.
			do {
				var t=gfx.getTouches();
				touched=t.length==1;
				pushed=!digitalRead(GPIO_NUM_0);
				ret = twai.receive_f(10);
			} while (!touched && !pushed && typeof ret=='number')
			setBacklightNormal();
			wait_DCDC_data.pollCount=0;
		}

		wait_DCDC_data.millisLastPollDCDC = now;
		twai_tx(CAN_ID_BROADCAST, [0x03, 0x22, 0xb0, 0x48, 0xaa, 0xaa, 0xaa, 0xaa]);
		wait_DCDC_data.pollCount++;
		setTopBarCenter(TFT_CONST.COLOR.ORANGE, wait_DCDC_data.pollCount.toString()+"/"+DCDC_POLL_TOO_MANY);
	}

	do {
		var ret = twai.receive_f(0);

		if (typeof ret != 'number') {
			if (ret.identifier==CAN_ID_BMS+8) {
				if (prefixMatch(ret.data, [0x04, 0x62, 0xb0, 0x48])) {
					var chargingStatus=ret.data[4];
					switch (chargingStatus) {
						case 0: break; // "Connected unlocked";
						case 1: break; // "Idle";
						case 3: currentState=APP_STATE_GWM_AUTH; break; // "Running";
						case 6: currentState=APP_STATE_GWM_AUTH; break; // "Charging";
						case 7: currentState=APP_STATE_GWM_AUTH; break; // "Rapid charging";
						case 8: break; // "Sleep";
						case 10: break; // "Connected";
						case 12: break; // "Charge starting";
						default: return "Unknown";
					}
				}
			}
		}
	} while (typeof ret != 'number');
}

function processGWMAuthState() {
	if (oldState != currentState) {
		oldState = currentState;
		setTopBarLeft(TFT_CONST.COLOR.YELLOW,"AUTHENTICATING GWM");
		setBacklightNormal();
	}
	var authenticated = gwmAuth(GWM_AUTH_TIMEOUT_MS);
	currentState = authenticated ? APP_STATE_RUNNING : APP_STATE_WAIT_DCDC;
}

function processRunningState() {
	if (oldState != currentState) {
		oldState = currentState;
		initDisplay();
		setTopBarLeft(TFT_CONST.COLOR.YELLOW,"START POLLING...");
	}
	doNextRequest();
}

function loop() {
	switch (currentState) {
		case APP_STATE_WAIT_DCDC:
			processWaitDCDCState();
			break;
		case APP_STATE_GWM_AUTH:
			processGWMAuthState();
			break;
		case APP_STATE_RUNNING:
			processRunningState();
			break;
		default:
			println(JSON.stringify(currentState));
	}

/*
	var touches=gfx.getTouches();
	println(JSON.stringify(touches));
*/
/*
*/
}
