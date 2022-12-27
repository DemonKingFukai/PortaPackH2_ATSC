#ifndef __ATSC_APP_HPP__
#define __ATSC_APP_HPP__
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <portapack.h>
#include <atsc.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <sgpio.h>

#include <font.h>
#include <lcd.h>

#include <atsc_channels.h>

#define CHANNEL_COUNT sizeof(atsc_channels) / sizeof(atsc_channel_t)

static void vTaskATSC( void *pvParameters ) {
	static atsc_rx_t atsc_rx;
	static atsc_signal_t atsc_signal;
	
	static uint32_t channel_index = 0;
	
	lcd_clear();
	lcd_puts("Decoding ATSC...");
	
	atsc_init(&atsc_rx, &atsc_signal, portapack_memory, portapack_spi, portapack_io);
	
	while(1) {
		atsc_channel_t* atsc_channel = &atsc_channels[channel_index];
		
		atsc_set_channel(&atsc_rx, atsc_channel->frequency_hz);
		atsc_decode(&atsc_rx, &atsc_signal, &display_framebuffer, display_width, display_height);
		
		channel_index = (channel_index + 1) % CHANNEL_COUNT;
		
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

int main(void) {
	portapack_init();
	
	xTaskCreate(
		vTaskATSC,
		"ATSC",
		1024,
		NULL,
		tskIDLE_PRIORITY + 1,
		NULL
	);
	
	vTaskStartScheduler();
	
	while(1) {
		
	}
}
