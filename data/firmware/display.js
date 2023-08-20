const WAIT_REPLY_TIMEOUT_MS = 200;
const MAX_TIMEOUT_COUNT = 30; // When reached, go back to wait for DC-DC state

const nRow = 5;
const nColumn = 4;
const gridStartX = 99;
const gridSizeX = 175;
const gridEndX = 799;
const gridStartY = 19;
const gridSizeY = 92;
const gridEndY = 479;

const CAN_ID_BMS = 0x7e5;
const CAN_ID_VCU = 0x7e3;
const CAN_ID_BCM = 0x740;
const CAN_ID_GWM = 0x710;
const CAN_ID_BROADCAST = 0x7df;

var timeoutCount = 0;

function setTopBarLeft(color, text) {
	gfx.setCursor(102, 0);
	gfx.setTextColor(color, TFT_CONST.COLOR.BLACK);
	gfx.setTextSize(2);
	gfx.print(padRight("                  ", text));
}

function setTopBarCenter(color, text) {
	gfx.setCursor(340, 0);
	gfx.setTextColor(color, TFT_CONST.COLOR.BLACK);
	gfx.setTextSize(2);
	var nPad = 18 - text.length;
	while (nPad > 0) {
		nPad--;
		text = ' ' + text;
		if (nPad > 0) {
			nPad--;
			text = text + ' ';
		}
	}
	gfx.print(text.substring(0, 18));
}

function setTopBarRight(color, text) {
	gfx.setCursor(580, 0);
	gfx.setTextColor(color, TFT_CONST.COLOR.BLACK);
	gfx.setTextSize(2);
	gfx.print(padLeft("                  ", text));
}

function initDisplay() {
	gfx.fillScreen(TFT_CONST.COLOR.BLACK);

	drawGrids();
	drawGridLabels();
	initActiveRequestList();
}

function padLeft(pad, str) {
	if (typeof str === 'undefined') return pad;
	return (pad + str).slice(-pad.length);
}

function padRight(pad, str) {
	if (typeof str === 'undefined') return pad;
	return (str + pad).substring(0, pad.length);
}

function noDecimal(n) {
	return (typeof n != 'number') ? '' : padLeft("         ", n.toFixed(0));
}

function oneDecimal(n) {
	return (typeof n != 'number') ? '' : padLeft("         ", n.toFixed(1));
}

function twoDecimal(n) {
	return (typeof n != 'number') ? '' : padLeft("         ", n.toFixed(2));
}

function threeDecimal(n) {
	return (typeof n != 'number') ? '' : padLeft("         ", n.toFixed(3));
}

function noFormat(s) {
	if (typeof s === 'undefined') return '         ';
	return s;
}

// valueNo 1-3 => min / current / max
function defaultColor(valueNo,value) {
	if (valueNo==2) return TFT_CONST.COLOR.WHITE;

	return TFT_CONST.COLOR.DARKGREY;
}

function defaultBgColor(valueNo,value) {
	return TFT_CONST.COLOR.BLACK;
}

function hvBatSoCColor(valueNo,value) {
	if (valueNo==2) {
		if (value>80) {
			return TFT_CONST.COLOR.GREEN;
		} else if (value>60) {
			return TFT_CONST.COLOR.GREENYELLOW;
		} else if (value>40) {
			return TFT_CONST.COLOR.YELLOW;
		} else if (value>20) {
			return TFT_CONST.COLOR.RED;
		} else {
			return TFT_CONST.COLOR.BLACK;
		}
	}
	return defaultColor(valueNo,value);
}

function hvBatSoCBgColor(valueNo,value) {
	if (valueNo==2) {
		if (value>20) {
			return TFT_CONST.COLOR.BLACK;
		} else {
			return TFT_CONST.COLOR.RED;
		}
	}
	return defaultBgColor(valueNo,value);
}

var requestList = {
	'HV Bat Volt': {
		label: 'HVB Volt (V)',
		disp: { r: 0, c: 0 },
		req: { canId: CAN_ID_BMS, msg: [0x03, 0x22, 0xb0, 0x42, 0xaa, 0xaa, 0xaa, 0xaa] },
		expect: { canId: CAN_ID_BMS + 8, prefix: [0x05, 0x62, 0xb0, 0x42] },
		decoder: function (id, m) { return ((m[4] << 8) | m[5]) * 0.25; },
		notify: ['HV Bat Power'],
		format: twoDecimal
	},
	'HV Bat Current': {
		label: 'HVB Curr. (A)',
		disp: { r: 0, c: 1 },
		req: { canId: CAN_ID_BMS, msg: [0x03, 0x22, 0xb0, 0x43, 0xaa, 0xaa, 0xaa, 0xaa] },
		expect: { canId: CAN_ID_BMS + 8, prefix: [0x05, 0x62, 0xb0, 0x43] },
		decoder: function (id, m) { return (((m[4] << 8) | m[5]) - 40000) * 0.025; },
		notify: ['HV Bat Power'],
		format: threeDecimal
	},
	'HV Bat Power': {
		label: 'HVB Power (W)',
		disp: { r: 0, c: 2 },
		decoder: function (id, m) { return (typeof (requestList['HV Bat Volt'].value) == 'number' && typeof (requestList['HV Bat Current'].value == 'number')) ? requestList['HV Bat Volt'].value * requestList['HV Bat Current'].value : null },
		format: twoDecimal
	},
	'HV Bat SoC': {
		label: 'HVB SoC (%)',
		disp: { r: 0, c: 3 },
		req: { canId: CAN_ID_BMS, msg: [0x03, 0x22, 0xb0, 0x46, 0xaa, 0xaa, 0xaa, 0xaa] },
		expect: { canId: CAN_ID_BMS + 8, prefix: [0x05, 0x62, 0xb0, 0x46] },
		decoder: function (id, m) { return ((m[4] << 8) | m[5]) * 0.1 },
		format: oneDecimal,
		color: hvBatSoCColor,
		bgColor: hvBatSoCBgColor
	},
	'HV Bat Cell Min': {
		label: 'HVB CellMin(V)',
		disp: { r: 1, c: 0 },
		req: { canId: CAN_ID_BMS, msg: [0x03, 0x22, 0xb0, 0x59, 0xaa, 0xaa, 0xaa, 0xaa] },
		expect: { canId: CAN_ID_BMS + 8, prefix: [0x06, 0x62, 0xb0, 0x59] },
		decoder: function (id, m) { return ((m[4] << 8) | m[5]) * 0.001 },
		notify: ['HV Cell Diff'],
		format: threeDecimal
	},
	'HV Bat Cell Max': {
		label: 'HVB CellMax(V)',
		disp: { r: 1, c: 1 },
		req: { canId: CAN_ID_BMS, msg: [0x03, 0x22, 0xb0, 0x58, 0xaa, 0xaa, 0xaa, 0xaa] },
		expect: { canId: CAN_ID_BMS + 8, prefix: [0x06, 0x62, 0xb0, 0x58] },
		decoder: function (id, m) { return ((m[4] << 8) | m[5]) * 0.001 },
		notify: ['HV Cell Diff'],
		format: threeDecimal
	},
	'HV Cell Diff': {
		label: 'HVB CellDif(V)',
		disp: { r: 1, c: 2 },
		decoder: function (id, m) {
			return (typeof (requestList['HV Bat Cell Min'].value) == 'number' && typeof (requestList['HV Bat Cell Max'].value == 'number'))
				? requestList['HV Bat Cell Max'].value - requestList['HV Bat Cell Min'].value : null
		},
		format: threeDecimal
	},

	'HV Bat SoH': {
		label: 'HVB SoH (%)',
		disp: { r: 1, c: 3 },
		req: { canId: CAN_ID_BMS, msg: [0x03, 0x22, 0xb0, 0x61, 0xaa, 0xaa, 0xaa, 0xaa] },
		expect: { canId: CAN_ID_BMS + 8, prefix: [0x05, 0x62, 0xb0, 0x61] },
		decoder: function (id, m) { return ((m[4] << 8) | m[5]) * 0.01 },
		format: twoDecimal
	},

	'DC-DC Volt': {
		label: '12V Volt (V)',
		disp: { r: 2, c: 0 },
		req: { canId: CAN_ID_VCU, msg: [0x03, 0x22, 0xb5, 0x84, 0xaa, 0xaa, 0xaa, 0xaa] },
		expect: { canId: CAN_ID_VCU + 8, prefix: [0x05, 0x62, 0xB5, 0x84] },
		decoder: function (id, m) { return ((m[4] << 8) | m[5]) * 0.1 },
		format: oneDecimal,
		notify: ['DC-DC Power']
	},
	'DC-DC Current': {
		label: 'DC-DC Out C(A)',
		disp: { r: 2, c: 1 },
		req: { canId: CAN_ID_VCU, msg: [0x03, 0x22, 0xb5, 0x83, 0xaa, 0xaa, 0xaa, 0xaa] },
		expect: { canId: CAN_ID_VCU + 8, prefix: [0x05, 0x62, 0xB5, 0x83] },
		decoder: function (id, m) { return ((m[4] << 8) | m[5]) * 0.1 },
		format: oneDecimal,
		notify: ['DC-DC Power']
	},
	'DC-DC Power': {
		label: 'DC-DC Out P(W)',
		disp: { r: 2, c: 2 },
		decoder: function (id, m) {
			return (typeof (requestList['DC-DC Volt'].value) == 'number' && typeof (requestList['DC-DC Current'].value == 'number'))
				? requestList['DC-DC Volt'].value * requestList['DC-DC Current'].value : null
		},
		format: oneDecimal
	},

	'DC-DC Temperature': {
		label: 'DC-DC Temp.(C)',
		disp: { r: 2, c: 3 },
		req: { canId: CAN_ID_VCU, msg: [0x03, 0x22, 0xb5, 0x87, 0xaa, 0xaa, 0xaa, 0xaa] },
		expect: { canId: CAN_ID_VCU + 8, prefix: [0x04, 0x62, 0xB5, 0x87] },
		decoder: function (id, m) { return m[4] - 40 },
		format: noDecimal
	},

	'Motor Torque': {
		label: 'Motor Torque',
		disp: { r: 3, c: 0 },
		req: { canId: CAN_ID_VCU, msg: [0x03, 0x22, 0xb4, 0x01, 0xaa, 0xaa, 0xaa, 0xaa] },
		expect: { canId: CAN_ID_VCU + 8, prefix: [0x05, 0x62, 0xB4, 0x01] },
		decoder: function (id, m) { return (((m[4] << 8) | m[5]) - 0x7fff) },
		format: noDecimal
	},
	'Speed': {
		label: 'Speed (km/hr)',
		disp: { r: 3, c: 1 },
		req: { canId: CAN_ID_VCU, msg: [0x03, 0x22, 0xba, 0x00, 0xaa, 0xaa, 0xaa, 0xaa] },
		expect: { canId: CAN_ID_VCU + 8, prefix: [0x05, 0x62, 0xBA, 0x00] },
		decoder: function (id, m) { return (((m[4] << 8) | m[5]) - 20000) * 0.01 },
		format: twoDecimal
	},
	'Motor Speed': {
		label: 'Motor Spd(RPM)',
		disp: { r: 3, c: 2 },
		req: { canId: CAN_ID_VCU, msg: [0x03, 0x22, 0xb4, 0x02, 0xaa, 0xaa, 0xaa, 0xaa] },
		expect: { canId: CAN_ID_VCU + 8, prefix: [0x05, 0x62, 0xB4, 0x02] },
		decoder: function (id, m) { return (((m[4] << 8) | m[5]) - 0x7fff) },
		format: noDecimal
	},
	'HV Insulation': {
		label: 'HV Insu (k O)',
		disp: { r: 3, c: 3 },
		req: { canId: CAN_ID_BMS, msg: [0x03, 0x22, 0xb0, 0x45, 0xaa, 0xaa, 0xaa, 0xaa] },
		expect: { canId: CAN_ID_BMS + 8, prefix: [0x05, 0x62, 0xb0, 0x45] },
		decoder: function (id, m) { return ((m[4] << 8) | m[5]) * 0.5 },
		format: oneDecimal
	},

	'Motor Temperature': {
		label: 'Motor Temp.(C)',
		disp: { r: 4, c: 0 },
		req: { canId: CAN_ID_VCU, msg: [0x03, 0x22, 0xb4, 0x05, 0xaa, 0xaa, 0xaa, 0xaa] },
		expect: { canId: CAN_ID_VCU + 8, prefix: [0x04, 0x62, 0xB4, 0x05] },
		decoder: function (id, m) { return m[4] - 40 },
		format: noDecimal
	},
	'Motor Coolant Temperature': {
		label: 'M. Cool. T.(C)',
		disp: { r: 4, c: 1 },
		req: { canId: CAN_ID_VCU, msg: [0x03, 0x22, 0xb3, 0x09, 0xaa, 0xaa, 0xaa, 0xaa] },
		expect: { canId: CAN_ID_VCU + 8, prefix: [0x04, 0x62, 0xB3, 0x09] },
		decoder: function (id, m) { return m[4] - 40 },
		format: noDecimal
	},
	'HV Temperature': {
		label: 'HV Temp. (C)',
		disp: { r: 4, c: 2 },
		req: { canId: CAN_ID_BMS, msg: [0x03, 0x22, 0xb0, 0x56, 0xaa, 0xaa, 0xaa, 0xaa] },
		expect: { canId: CAN_ID_BMS + 8, prefix: [0x05, 0x62, 0xb0, 0x56] },
		decoder: function (id, m) { return (m[4] * 0.5 - 40) },
		format: oneDecimal
	},
	'HV Coolant Temperature': {
		label: 'HV Cool. T.(C)',
		disp: { r: 4, c: 3 },
		req: { canId: CAN_ID_BMS, msg: [0x03, 0x22, 0xb0, 0x5c, 0xaa, 0xaa, 0xaa, 0xaa] },
		expect: { canId: CAN_ID_BMS + 8, prefix: [0x04, 0x62, 0xb0, 0x5c] },
		decoder: function (id, m) { return (m[4] * 0.5 - 40) },
		format: oneDecimal
	},

	'Outside Temperature': {
		label: 'Outside T (C)',
		// no display
		req: { canId: CAN_ID_VCU, msg: [0x03,0x22,0xbb,0x05,0xaa,0xaa,0xaa,0xaa] },
		expect: { canId: CAN_ID_VCU + 8, prefix: [0x04, 0x62, 0xbb, 0x05] },
		decoder: function (id, m) { return m[4] - 40 },
		format: oneDecimal
	},

	'Gear Position': {
		label: 'Gear Position',
		// no display
		req: { canId: CAN_ID_VCU, msg: [0x03, 0x22, 0xb9, 0x00, 0xaa, 0xaa, 0xaa, 0xaa] },
		expect: { canId: CAN_ID_VCU + 8, prefix: [0x04, 0x62, 0xb9, 0x00] },
		decoder: function (id, m) {
			switch (m[4]) {
				case 7: return 'R';
				case 6: return 'N';
				case 5: return 'D';
				case 8: return 'P';
				default: return "Unknown";
			}
		},
		notify: ['DRL Control']
	},
	'Charging Status': {
		label: 'Charg. Status',
		// no display
		req: { canId: CAN_ID_BMS, msg: [0x03, 0x22, 0xb0, 0x48, 0xaa, 0xaa, 0xaa, 0xaa] },
		expect: { canId: CAN_ID_BMS + 8, prefix: [0x04, 0x62, 0xb0, 0x48] },
		decoder: function (id, m) {
			switch (m[4]) {
				case 0: return "Connected unlocked";
				case 1: return "Idle";
				case 3: return "Running";
				case 6: return "Charging";
				case 7: return "Rapid charging";
				case 8: return "Sleep";
				case 10: return "Connected";
				case 12: return "Charge starting";
				default: return "Unknown";
			}
		},
		notify: ['DRL Control', 'State Watch']
	},
	'Odometer': {
		label: 'Odometer',
		// no display
		req: { canId: CAN_ID_VCU, msg: [0x03, 0x22, 0xe1, 0x01, 0xaa, 0xaa, 0xaa, 0xaa] },
		expect: { canId: CAN_ID_VCU + 8, prefix: [0x06, 0x62, 0xe1, 0x01] },
		decoder: function (id, m) { return (m[4] << 16) | (m[5] << 8) | m[6] }
	},
	'DRL Control': {
		label: 'DRL Control',
		// no display
		decoder: function (id, m) {
			var gearPos = typeof requestList['Gear Position'].value != 'undefined' ? requestList['Gear Position'].value : '';
			var chargingStatus = typeof requestList['Charging Status'].value != 'undefined' ? requestList['Charging Status'].value : '';

			if (gearPos == 'P' && chargingStatus == 'Running') {
				if (requestList['DRL Control'].lastDRL == '') {
					setTopBarCenter(TFT_CONST.COLOR.YELLOW, "Turning DRL off");
					if (drlOff(1000)) {
						requestList['DRL Control'].lastDRL = 'off';
						setTopBarCenter(TFT_CONST.COLOR.YELLOW, "DRL off");
					}
				} else {
					sendTesterPresent();
				}
			} else {
				if (requestList['DRL Control'].lastDRL == 'off') {
					requestList['DRL Control'].lastDRL = '';
					setTopBarCenter(TFT_CONST.COLOR.GREEN, "");
				}
			}
		},
		lastDRL: ''
	},
	'State Watch': {
		label: 'State Watch',
		// no display
		decoder: function (id, m) {
			var chargingStatus = typeof requestList['Charging Status'].value != 'undefined' ? requestList['Charging Status'].value : '';
			var color = TFT_CONST.COLOR.GREEN;

			if (chargingStatus != requestList['State Watch'].lastState) {
				if (!(chargingStatus == 'Running' || chargingStatus == 'Charging' || chargingStatus == 'Rapid charging')) {
					color = TFT_CONST.COLOR.YELLOW;
					currentState = APP_STATE_WAIT_DCDC;
					setTopBarRight(TFT_CONST.COLOR.RED, "LIMITED POLLING");
				}
				requestList['State Watch'].lastState = chargingStatus;
				setTopBarLeft(color, chargingStatus);
			}
		},
		lastState: ''
	}
};

var activeRequestList = null;
var currentRequest = null;
var toDraw = [];

function initActiveRequestList() {
	currentRequest = null;
	nextRequest = null;
	activeRequestList = [];

	for (var target in requestList) {
		var d = requestList[target];

		if (typeof d.req != 'undefined') {
			activeRequestList.push(target);
		}
	}
}

function prefixMatch(data, prefix) {
	if (data.length < prefix.length) {
		return false;
	}
	for (var i = 0; i < prefix.length; i++) {
		if (data[i] != prefix[i]) {
			return false;
		}
	}

	return true;
}

function getValueTextColor(requestName,valueNo,value) {
	if (typeof requestList[requestName].color=='function') {
		return requestList[requestName].color(valueNo,value);
	} else {
		return defaultColor(valueNo,value);
	}
}

function getValueTextBgColor(requestName,valueNo,value) {
	if (typeof requestList[requestName].bgColor=='function') {
		return requestList[requestName].bgColor(valueNo,value);
	} else {
		return defaultBgColor(valueNo,value);
	}
}

function doNextRequest() {
	// Draw the last request after the new request has been sent.

	if (activeRequestList.length == 0) {
		return;
	}

	if (currentRequest == null) {
		currentRequest = 0;
	} else {
		currentRequest++;

		if (currentRequest == activeRequestList.length) {
			currentRequest = 0;
		}
	}
	var currentRequestName = activeRequestList[currentRequest];
	var crq = requestList[currentRequestName];
	twai_tx(crq.req.canId, crq.req.msg);

	while (toDraw.length != 0) {
		// update the display
		var name=toDraw[0];
		var d = requestList[toDraw[0]];

		drawGridValue1(d.disp.c, d.disp.r, d.format(d.valueMin), getValueTextColor(name,1,d.valueMin), getValueTextBgColor(name,1,d.valueMin));
		drawGridValue2(d.disp.c, d.disp.r, d.format(d.value), getValueTextColor(name,2,d.valueMin), getValueTextBgColor(name,2,d.value));
		drawGridValue3(d.disp.c, d.disp.r, d.format(d.valueMax), getValueTextColor(name,3,d.valueMin), getValueTextBgColor(name,3,d.valueMax));

		toDraw.shift();
	}

	var sastified = false;
	var timeout = millis() + WAIT_REPLY_TIMEOUT_MS;

	if (typeof crq.expect != 'undefined') {
		do {
			var ret = twai.receive_f(10);

			if (typeof ret != 'number') {
				rxCount++;
				if (ret.identifier != crq.expect.canId) {
					//println("Not what I expected ", ret.identifier.toString(16), " != ", crq.expect.canId.toString(16));
					continue;
				} else if (prefixMatch(ret.data, crq.expect.prefix)) {
					var newValue = crq.decoder(ret.identifier, ret.data);

					updateValue(activeRequestList[currentRequest], newValue);
					sastified = true;
				} else {
					//println("prefix mismatched", JSON.stringify(ret.data));
				}
			}
		} while (!sastified && millis() < timeout);
		if (!sastified) {
			println("Timed out!");
			timeoutCount++;
			if (timeoutCount >= MAX_TIMEOUT_COUNT) {
				currentState = APP_STATE_WAIT_DCDC;
				setTopBarRight(TFT_CONST.COLOR.RED, "ECU NOT RESPONDING");
			}
		} else {
			timeoutCount=0;
		}
	}
}

function updateValue(target, newValue) {
	//println("Updating", target, "value to", newValue);
	requestList[target].value = newValue;
	var d = requestList[target];
	if (typeof newValue == 'number') {
		//println(JSON.stringify(d));
		if (typeof d.valueMin != 'number' || d.valueMin === null || isNaN(d.valueMin) || d.valueMin > newValue) {
			requestList[target].valueMin = newValue;
		}
		if (typeof d.valueMax != 'number' || d.valueMin === null || isNaN(d.valueMax) || d.valueMax < newValue) {
			requestList[target].valueMax = newValue;
		}
		//println(target, newValue, requestList[target].valueMin, requestList[target].valueMax);
	}
	if (typeof d.disp != 'undefined') {
		toDraw.push(target);
	}
	if (typeof d.notify != 'undefined') {
		for (var i = 0; i < d.notify.length; i++) {
			var notifyValue = requestList[d.notify[i]].decoder(null, null);
			updateValue(d.notify[i], notifyValue)
		}
	}
}

function drawGrids() {
	for (var row = 0; row <= 5; row++) {
		gfx.drawLine(gridStartX, gridStartY + gridSizeY * row, gridEndX, gridStartY + gridSizeY * row, TFT_CONST.COLOR.LIGHTGREY);
	}
	for (var column = 0; column <= 4; column++) {
		gfx.drawLine(gridStartX + gridSizeX * column, gridStartY, gridStartX + gridSizeX * column, gridEndY, TFT_CONST.COLOR.LIGHTGREY);
	}
}

function gridX(column) {
	return gridStartX + gridSizeX * column;
}

function gridY(row) {
	return gridStartY + gridSizeY * row;
}

function drawGridLabel(column, row, text, color) {
	var x = gridX(column) + 4;
	var y = gridY(row) + 2;

	gfx.setTextSize(2);
	gfx.setTextColor(color);
	gfx.setCursor(x, y);
	gfx.print(text);
}

function drawGridLabels() {
	for (var target in requestList) {
		var d = requestList[target];

		if (typeof d.disp != 'undefined') {
			drawGridLabel(d.disp.c, d.disp.r, d.label, TFT_CONST.COLOR.GREEN);
		}
	}
}

function drawGridValue1(column, row, text, color, bgcolor) {
	var x = gridX(column) + 8;
	var y = gridY(row) + 20;

	gfx.setTextSize(3);
	gfx.setTextColor(color, bgcolor);
	gfx.setCursor(x, y);
	gfx.print(text);
}

function drawGridValue2(column, row, text, color, bgcolor) {
	var x = gridX(column) + 8;
	var y = gridY(row) + 44;

	gfx.setTextSize(3);
	gfx.setTextColor(color, bgcolor);
	gfx.setCursor(x, y);
	gfx.print(text);
}

function drawGridValue3(column, row, text, color, bgcolor) {
	var x = gridX(column) + 8;
	var y = gridY(row) + 68;

	gfx.setTextSize(3);
	gfx.setTextColor(color, bgcolor);
	gfx.setCursor(x, y);
	gfx.print(text);
}