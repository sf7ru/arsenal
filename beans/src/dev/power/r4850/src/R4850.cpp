


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>


#include <R4850.h>


void r4850_data(uint8_t *frame, struct RectifierParameters *rp)
{
	uint32_t value = __builtin_bswap32(*(uint32_t *)&frame[4]);

	switch (frame[1]) {
		case R48xx_DATA_INPUT_POWER:
			rp->input_power = value / 1024.0;
			break;

		case R48xx_DATA_INPUT_FREQ:
			rp->input_frequency = value / 1024.0;
			break;

		case R48xx_DATA_INPUT_CURRENT:
			rp->input_current = value / 1024.0;
			break;

		case R48xx_DATA_OUTPUT_POWER:
			rp->output_power = value / 1024.0;
			break;

		case R48xx_DATA_EFFICIENCY:
			rp->efficiency = value / 1024.0;
			break;

		case R48xx_DATA_OUTPUT_VOLTAGE:
			rp->output_voltage = value / 1024.0;
			break;

		case R48xx_DATA_OUTPUT_CURRENT_MAX:
			rp->max_output_current = value / MAX_CURRENT_MULTIPLIER;
			break;

		case R48xx_DATA_INPUT_VOLTAGE:
			rp->input_voltage = value / 1024.0;
			break;

		case R48xx_DATA_OUTPUT_TEMPERATURE:
			rp->output_temp = value / 1024.0;
			break;

		case R48xx_DATA_INPUT_TEMPERATURE:
			rp->input_temp = value / 1024.0;
			break;

		case R48xx_DATA_OUTPUT_CURRENT1:
			//printf("Output Current(1) %.02fA\r\n", value / 1024.0);
			//rp->output_current = value / 1024.0;
			break;

		case R48xx_DATA_OUTPUT_CURRENT:
			rp->output_current = value / 1024.0;

			/* This is normally the last parameter received. Print */
			//r4850_print_parameters(rp);

			break;

		default:
			//printf("Unknown parameter 0x%02X, 0x%04X\r\n",frame[1], value);
			break;

	}
}

void r4850_Ahr(uint8_t *frame, struct RectifierParameters *rp)
{
	uint16_t value = __builtin_bswap16(*(uint16_t *)&frame[6]);
	//printf("Instantaneous current = %.02fA\r\n", (float)value / 20);
	rp->amp_hour += (((float)value / 20) * 0.377);

}

void r4850_description(uint8_t *frame)
{
	printf("%c%c%c%c%c%c", frame[2], frame[3], frame[4], frame[5], frame[6], frame[7]);
}

int r4850_request_data(int s)
{
/*	
	struct can_frame frame;

	frame.can_id = 0x108040FE | CAN_EFF_FLAG;
	// 0x108140FE also works
	frame.can_dlc = 8;
	frame.data[0] = 0;
	frame.data[1] = 0;
	frame.data[2] = 0;
	frame.data[3] = 0;
	frame.data[4] = 0;
	frame.data[5] = 0;
	frame.data[6] = 0;
	frame.data[7] = 0;

	if (write(s, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
		perror("Write");
		return 1;
	}
*/	
	return 0;
}

int r4850_set_voltage(int s, float voltage, bool nonvolatile)
{
/*	
	struct can_frame frame;

	uint16_t value = voltage * 1024;
	//printf("Voltage = 0x%04X\n",value);

	uint8_t command;

	if (nonvolatile) command = 0x01;	// Off-line mode
	else		 command = 0x00;	// On-line mode

	frame.can_id = 0x108180FE | CAN_EFF_FLAG;
	frame.can_dlc = 8;
	frame.data[0] = 0x01;
	frame.data[1] = command;
	frame.data[2] = 0x00;
	frame.data[3] = 0x00;
	frame.data[4] = 0x00;
	frame.data[5] = 0x00;
	frame.data[6] = (value & 0xFF00) >> 8;
	frame.data[7] = value & 0xFF;

	if (write(s, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
		perror("Write");
		return 1;
	}
*/	
	return 0;
}

int r4850_set_current(int s, float current, bool nonvolatile)
{
/*	
	struct can_frame frame;

	uint16_t value = current * MAX_CURRENT_MULTIPLIER;
	//printf("Current = 0x%04X\n",value);

	uint8_t command;

	if (nonvolatile) command = 0x04;	// Off-line mode
	else		 command = 0x03;	// On-line mode

	frame.can_id = 0x108180FE | CAN_EFF_FLAG;
	frame.can_dlc = 8;
	frame.data[0] = 0x01;
	frame.data[1] = command;
	frame.data[2] = 0x00;
	frame.data[3] = 0x00;
	frame.data[4] = 0x00;
	frame.data[5] = 0x00;
	frame.data[6] = (value & 0xFF00) >> 8;
	frame.data[7] = value & 0xFF;

	if (write(s, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
		perror("Write");
		return 1;
	}
*/	
	return 0;
}

void r4850_ack(uint8_t *frame)
{
	bool error = frame[0] & 0x20;
	uint32_t value = __builtin_bswap32(*(uint32_t *)&frame[4]);

	switch (frame[1]){
		case 0x00:
			printf("%s setting on-line voltage to %.02fV\n", error?"Error":"Success", value / 1024.0);
			break;
		case 0x01:
			printf("%s setting non-volatile (off-line) voltage to %.02fV\n", error?"Error":"Success", value / 1024.0);
			break;
		case 0x02:
			printf("%s setting overvoltage protection to %.02fV\n", error?"Error":"Success", value / 1024.0);
			break;
		case 0x03:
			printf("%s setting on-line current to %.02fA\n", error?"Error":"Success", (float) value / MAX_CURRENT_MULTIPLIER);
			break;
		case 0x04:
			printf("%s setting non-volatile (off-line) current to %.02fA\n", error?"Error":"Success", (float) value / MAX_CURRENT_MULTIPLIER);
			break;
		default:
			printf("%s setting unknown parameter (0x%02X)\n", error?"Error":"Success", frame[1]);
			break;
	}
}

// void thread_handler(union sigval sv) {
// 	r4850_request_data(sv.sival_int);
// }

int ____main(int argc, char **argv)
{
/*	
	int s, i;
	int frame_number;
	int nbytes;
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct can_frame frame;
	struct RectifierParameters rp;

	rp.amp_hour = 0;

	float voltage;
	bool setvoltage = false;
	float current;
	bool setcurrent = false;
	bool nonvolatile = false;
	int opt;
	char *ptr;

	if (setvoltage) {
		r4850_set_voltage(s, voltage, false);
		if (nonvolatile)
			r4850_set_voltage(s, voltage, true);
	}

	if (setcurrent) {
		r4850_set_current(s, current, false);
		if (nonvolatile)
			r4850_set_current(s, current, true);
	}

	timer_t timerid;

	struct sigevent sev;
	memset(&sev, 0, sizeof(struct sigevent));
	sev.sigev_notify = SIGEV_THREAD;
	sev.sigev_notify_function = &thread_handler;
	//sev.sigev_value.sival_ptr = &data;
	sev.sigev_value.sival_int = s;
	timer_create(CLOCK_REALTIME, &sev, &timerid);

        struct itimerspec trigger;
        memset(&trigger, 0, sizeof(struct itimerspec));
        trigger.it_value.tv_sec = 1;
        trigger.it_interval.tv_sec = 1;
        timer_settime(timerid, 0, &trigger, NULL);

	r4850_request_data(s);

	do {

		nbytes = read(s, &frame, sizeof(struct can_frame));

		if (nbytes < 0) {
			perror("Read");
			return 1;
		}
#if debug
		printf("0x%03X [%d] ",(frame.can_id & 0x1FFFFFFF), frame.can_dlc);
		for (i = 0; i < frame.can_dlc; i++)
			printf("%02X ",frame.data[i]);
		printf("\r\n");
#endif

		switch (frame.can_id & 0x1FFFFFFF){

			case 0x1081407F:
				r4850_data((uint8_t *)&frame.data, &rp);
				break;

			case 0x1081407E:
				// Acknowledgment
				break;

			case 0x1081D27F:
				r4850_description((uint8_t *)&frame.data);
				break;

			case 0x1081807E:
				// Acknowledgement 
				r4850_ack((uint8_t *)&frame.data);
				break;

			case 0x1001117E:
				r4850_Ahr((uint8_t *)&frame.data, &rp);
				// Normally 00 01 00 0s 00 00 xx xx 
				// xx = Whr meter, send every 377mS 
				// s = 1 when output disabled 
				break;

			case 0x100011FE:
				// Normally 00 02 00 00 00 00 00 00 
				break;

			case 0x108111FE:
				// Normally 00 03 00 00 00 0s 00 00 
				// s = 1 when output disabled 
				//if (frame.data[5] == 1)
				//	printf("Output Enabled\n");
				//else 	printf("Output Disabled\n");
				break;

			case 0x108081FE:
				// Normally 01 13 00 01 00 00 00 00 
				break;

			default:
				printf("Unknown frame 0x%03X [%d] ",(frame.can_id & 0x1FFFFFFF), frame.can_dlc);
				for (i = 0; i < frame.can_dlc; i++)
					printf("%02X ",frame.data[i]);
				printf("\r\n");

		}

	} while(1);

	if (close(s) < 0) {
		perror("Close");
		return 1;
	}

	timer_delete(timerid);
*/
	return 0;
}
