function setBacklightNormal() {
	setBacklight(96);
}

function setBacklightNight() {
	setBacklight(48);
}

function setBacklightIdle() {
	setBacklight(16);
}

function setup() {
	setBacklightNormal();
	
	if (typeof Duktape !== 'object') {
		println('not Duktape');
	} else if (Duktape.version >= 20403) {
		println('Duktape 2.4.3 or higher',Duktape.version);
	} else if (Duktape.version >= 10500) {
		println('Duktape 1.5.0 or higher (but lower than 2.4.3)',Duktape.version);
	} else {
		println('Duktape lower than 1.5.0',Duktape.version);
	}

	twai.driver_install(
		GPIO_NUM_17, // TX
		GPIO_NUM_18, // RX
		twai.TWAI_MODE_NORMAL,
		twai.SPEED_500,
		{acceptance_code:0,acceptance_mask:0xFFFFFFFF,single_filter:true}
	);

	twai.start();

	pinMode(GPIO_NUM_0,INPUT);

	//twai.get_status_info();
	gfx.fillScreen(TFT_CONST.COLOR.BLACK);
	gfx.setTextSize(2);
	gfx.setTextColor(TFT_CONST.COLOR.WHITE,TFT_CONST.COLOR.BLACK);
	gfx.setCursor(0,0);
}
