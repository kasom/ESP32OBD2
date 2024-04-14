const APP_STATE_WAIT_DCDC=0;
const APP_STATE_GWM_AUTH=1;
const APP_STATE_RUNNING=2;
const APP_STATE_CAN_MONITOR=3;

const GWM_AUTH_TIMEOUT_MS = 1000;
const DCDC_POLL_DELAY_MS = 500;
const DCDC_POLL_TOO_MANY = 180;

const CAN_MONITOR_MAX_LINE = 30;
var rxCount=0;

var currentState = APP_STATE_WAIT_DCDC;
var oldState = null;

var canMonitorCurrentLine=0;
var canMonitorStartMillis;

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

	var pushed=!digitalRead(GPIO_NUM_0);

	if (now > wait_DCDC_data.millisLastPollDCDC + DCDC_POLL_DELAY_MS) {
		var ret=null;
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

	if (pushed) {
		currentState=APP_STATE_CAN_MONITOR;
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

function processCanMonitor() {
	var now = millis();

	if (oldState != currentState) {
		oldState = currentState;
		canMonitorCurrentLine=0;
	        gfx.fillScreen(TFT_CONST.COLOR.BLACK);
		gfx.setTextSize(2);
		gfx.setTextColor(TFT_CONST.COLOR.WHITE,TFT_CONST.COLOR.BLACK);
		gfx.setCursor(0,0);
		gfx.println("CAN monitor mode. Reset to exit.");
		canMonitorCurrentLine++;
		canMonitorStartMillis=now;
	}

	var ret = twai.receive_f(10);
        
	if (typeof ret != 'number') {
		if (canMonitorCurrentLine==CAN_MONITOR_MAX_LINE) {
			canMonitorCurrentLine=1;
			gfx.setCursor(0,16);
		}

		gfx.setTextColor(TFT_CONST.COLOR.YELLOW,TFT_CONST.COLOR.BLACK);
		gfx.print(padLeft("            ", now-canMonitorStartMillis));
		gfx.setTextColor(TFT_CONST.COLOR.WHITE,TFT_CONST.COLOR.BLACK);
		gfx.print(" ",padLeft("000",ret.identifier.toString(16)));
		gfx.setTextColor(TFT_CONST.COLOR.GREEN,TFT_CONST.COLOR.BLACK);

		for (var i=0;i<8;i++) {
			if (i<ret.data.length) {
				gfx.print(" ",padLeft("00",ret.data[i].toString(16)));
			} else {
				gfx.print("   ");
			}
		}
		gfx.println("");
		canMonitorCurrentLine++;
	}
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
		case APP_STATE_CAN_MONITOR:
			processCanMonitor();
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
