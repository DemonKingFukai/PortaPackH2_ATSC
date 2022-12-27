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
void setup() {
// Initialize the Portapack and ATSC library
portapack_init();
atsc_init();

// Set the frequency to the desired ATSC channel
atsc_set_frequency(ATSC_CHANNEL_7);

// Start the ATSC receiver
atsc_start();
}

void loop() {
// Check if there is a new ATSC packet available
if (atsc_packet_available()) {
// Get the current ATSC packet
atsc_packet_t packet = atsc_get_packet();

// Display the channel information on the screen
portapack_lcd_printf(0, 0, "Channel: %d", packet.channel);
portapack_lcd_printf(0, 1, "Program: %d", packet.program_number);
portapack_lcd_printf(0, 2, "Program Name: %s", packet.program_name);


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
