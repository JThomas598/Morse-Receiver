#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include <esp_system.h>

#define UNIT 3500
#define POLL_TIME 3500
#define THRESHOLD 500


typedef enum State{
	WAITING,
	ON,
	OFF
}State;

char translate(char* symbol){
	if(!strcmp(".-", symbol)){
		return 'A'; }
	else if(!strcmp("-...", symbol)){
		return 'B';
	}
	else if(!strcmp("-.-.", symbol)){
		return 'C';
	}
	else if(!strcmp("-..", symbol)){
		return 'D';
	}
	else if(!strcmp(".", symbol)){
		return 'E';
	}
	else if(!strcmp("..-.", symbol)){
		return 'F';
	}
	else if(!strcmp("--.", symbol)){
		return 'G';
	}
	else if(!strcmp("....", symbol)){
		return 'H';
	}
	else if(!strcmp("..", symbol)){
		return 'I';
	}
	else if(!strcmp(".---", symbol)){
		return 'J';
	}
	else if(!strcmp("-.-", symbol)){
		return 'K';
	}
	else if(!strcmp(".-..", symbol)){
		return 'L';
	}
	else if(!strcmp("--", symbol)){
		return 'M';
	}
	else if(!strcmp("-.", symbol)){
		return 'N';
	}
	else if(!strcmp("---", symbol)){
		return 'O';
	}
	else if(!strcmp(".--.", symbol)){
		return 'P';
	}
	else if(!strcmp("--.-", symbol)){
		return 'Q';
	}
	else if(!strcmp(".-.", symbol)){
		return 'R';
	}
	else if(!strcmp("...", symbol)){
		return 'S';
	}
	else if(!strcmp("-", symbol)){
		return 'T';
	}
	else if(!strcmp("..-", symbol)){
		return 'U';
	}
	else if(!strcmp("...-", symbol)){
		return 'V';
	}
	else if(!strcmp(".--", symbol)){
		return 'W';
	}
	else if(!strcmp("-..-", symbol)){
		return 'X';
	}
	else if(!strcmp("-.--", symbol)){
		return 'Y';
	}
	else if(!strcmp("--..", symbol)){
		return 'Z';
	}
	else if(!strcmp(".----", symbol)){
		return '1';
	}
	else if(!strcmp("..---", symbol)){
		return '2';
	}
	else if(!strcmp("...--", symbol)){
		return '3';
	}
	else if(!strcmp("....-", symbol)){
		return '4';
	}
	else if(!strcmp(".....", symbol)){
		return '5';
	}
	else if(!strcmp("-....", symbol)){
		return '6';
	}
	else if(!strcmp("--...", symbol)){
		return '7';
	}
	else if(!strcmp("---..", symbol)){
		return '8';
	}
	else if(!strcmp("----.", symbol)){
		return '9';
	}
	else if(!strcmp("-----", symbol)){
		return '0';
	}
	else if(!strcmp("--..--", symbol)){
		return ',';
	}
	else if(!strcmp(".-.-.-", symbol)){
		return '.';
	}
	else if(!strcmp("..--..", symbol)){
		return '?';
	}
	else if(!strcmp("-..-.", symbol)){
		return '/';
	}
	else if(!strcmp("-....-", symbol)){
		return '-';
	}
	else if(!strcmp("-.--.", symbol)){
		return '(';
	}
	else if(!strcmp("-.--.-", symbol)){
		return ')';
	}
	else if(!strcmp("/", symbol)){
		return ' ';
	}
	return 0;
}

void app_main(void){
	adc1_config_width(12);
	adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_DB_11);
	uint32_t reading = 0;
	uint32_t ms_passed = 0;
	uint32_t time = 0;
	char buf[1024];
	int buf_ptr = 0;
	State state = WAITING;
	while(1){
		reading = adc1_get_raw(ADC1_CHANNEL_4);
		switch(state){
			case WAITING:
				//Message initiation detected
				if(reading > THRESHOLD){
					state = ON;
					break;
				}
				break;
			case ON:
				//High-Low transition detected 
				if(reading < THRESHOLD){
					if(ms_passed >= UNIT * 3){
						buf[buf_ptr] = '-';
					}
					else if(ms_passed >= UNIT){
						buf[buf_ptr] = '.';
					}
					state = OFF;
					buf_ptr++;
					ms_passed = 0; 
					break;
				}
				ms_passed += POLL_TIME; 
				time += POLL_TIME;
				break; 
			case OFF:
				//Signal low for an extended period of time. Flush message to stdout.
				if(ms_passed >= UNIT * 21){
					state = WAITING;
					buf[buf_ptr] = '\0';
					char sym = translate(buf);
					putchar(sym);
					putchar('\n');
					buf_ptr = 0;
					printf("Elapsed transmission time in microseconds: %ld\n", time - ms_passed);
					printf("Elapsed transmission time in seconds: %lf\n", (double)time / 1000000.0);
					ms_passed = 0; 
					time = 0;
					break;
				}
				//Low-High transition detected
				if(reading > THRESHOLD){
					//End of word detected
					if(ms_passed >= UNIT * 7){
						state = ON;
						buf[buf_ptr] = '\0';
						char sym = translate(buf);
						putchar(sym);
						putchar(' ');
						buf_ptr = 0;
					}
					//End of symbol detected
					else if(ms_passed >= UNIT * 3){
						state = ON;
						buf[buf_ptr] = '\0';
						char sym = translate(buf);
						putchar(sym);
						buf_ptr = 0;
					}
					//Still transmitting symbol
					else if(ms_passed >= UNIT){
						state = ON;
					}
					ms_passed = 0; 
					break;
				}
				ms_passed += POLL_TIME; 
				time += POLL_TIME;
				break;
		}
		esp_rom_delay_us(POLL_TIME);
	}
}
