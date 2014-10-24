#include "HK_behaviours.h"


//===========================================================================
//===== CONSTRUCTOR and DECONSTRUCTOR =====
//===========================================================================

HK_Behaviours::HK_Behaviours():
		tentacle_ir_state{tentacle_0_ir_state, tentacle_1_ir_state, tentacle_2_ir_state},
		tentacle_ir_threshold{tentacle_0_ir_threshold, tentacle_1_ir_threshold, tentacle_2_ir_threshold},
		tentacle_cycle_period{tentacle_0_cycle_period, tentacle_1_cycle_period , tentacle_2_cycle_period}
{
	
}

HK_Behaviours::~HK_Behaviours(){
	
}

void HK_Behaviours::parse_msg(){

	uint16_t val = 0;
	
	// byte 1 --- type of request
	request_type = recv_data_buff[1];
        
	switch (request_type){
	
		//Teensy programming pin
		case 1: {
			bool program_teensy = recv_data_buff[2];
			if (program_teensy) {
				digitalWrite(PGM_DO_pin, 1);
				digitalWrite(13, 1);
			}
			break;
		}
		case 2: {
		
			// byte 2 to 33 --- indicator LED wave 
			for (short i = 0; i < wave_size; i++)
				test_wave.waveform[i] = recv_data_buff[i+2];
			break;
		}
		default:{
			// byte 2 --- indicator led on or off
			indicator_led_on = recv_data_buff[2];

			// byte 3 and 4 --- indicator led blinking frequency
			val = 0;
			for (short i = 0; i < 2 ; i++)
			  val += recv_data_buff[i+3] << (8*i);
			//indicator_led_blink_period = val;

			// byte 5 --- high power LED level
			//	high_power_led_level = recv_data_buff[5];
				
			// byte 6 and byte 7 --- high power LED reflex threshold
			val = 0;
			for (short i = 0; i < 2 ; i++)
			  val += recv_data_buff[i+6] << (8*i);
			//high_power_led_reflex_threshold = val;
			
			// byte 8 --- SMA 0 level
			//sma_0_level = recv_data_buff[8];
			
			// byte 9 --- SMA 1 level
			//sma_1_level = recv_data_buff[9];
			
			// byte 10 --- Reflex 0 level
			//reflex_0_level = recv_data_buff[10];
			
			// byte 11 --- Reflex 1 level
			//reflex_1_level = recv_data_buff[11];
			break;
		}
	}

}


//===========================================================================
//====== COMMUNICATION Protocol ======
//===========================================================================

void HK_Behaviours::compose_reply(byte front_signature, byte back_signature){


	// add the signatures to first and last byte
	send_data_buff[0] = front_signature;
	send_data_buff[num_outgoing_byte-1] = back_signature;
	
	// sample the sensors
	this->sample_inputs();
		

	switch (request_type){
	
		default:
			
			// >>> protocell --- byte 1 to 10
			
			send_data_buff[1] =  0;
			send_data_buff[2] =  0;
			
			// >>> tentacle_0 --- byte 11 to 20 
			// >>> tentacle_1 --- byte 21 to 30 
			// >>> tentacle_2 --- byte 31 to 40 
			
			for (uint8_t tentacle_id = 0; tentacle_id<3; tentacle_id++){
			
				// byte *1 and *2 --- ir 0 and ir 1
				send_data_buff[10*tentacle_id + 11] = tentacle_ir_state[tentacle_id][0];
				send_data_buff[10*tentacle_id + 12] = tentacle_ir_state[tentacle_id][1];

			}
		break;
	}

}

//===========================================================================
//====== Input functions ======
//===========================================================================

//--- Sampling function ---
void HK_Behaviours::sample_inputs(){

	
	//>>>tentacle<<<
	
	//~~IR sensors state~~
	for (uint8_t i=0; i<2; i++){
		tentacle_ir_state[0][i] = tentacle_0.read_analog_state(i);
		tentacle_ir_state[1][i] = tentacle_1.read_analog_state(i);
		tentacle_ir_state[2][i] = tentacle_2.read_analog_state(i);
	}

	

	if (Wire.frozen){
		//digitalWrite(PGM_DO_pin, 1);
		digitalWrite(13, 1);
	}
		


}

//===========================================================================
//============ BEHAVIOUR CODES =========
//===========================================================================

//---- test behaviour ----
void HK_Behaviours::test_behaviour(const uint32_t &curr_time) {
	
	//=== testing Tentacle ===
	for (uint8_t i=0; i<3; i++){
		uint8_t ir_range = tentacle[i].read_analog_state(0);
		if (ir_range > 100){
			tentacle[i].set_led_level(0, 250);
			tentacle[i].set_led_level(1, 250);
		}
		else{
			tentacle[i].set_led_level(0, 0);
			tentacle[i].set_led_level(1, 0);
		}
		
		ir_range = tentacle[i].read_analog_state(1);
		
		if (ir_range > 100){
			tentacle[i].set_sma_level(0, 250);
			tentacle[i].set_sma_level(1, 250);
			
		}
		else{
			tentacle[i].set_sma_level(0, 0);
			tentacle[i].set_sma_level(1, 0);
		}
	}		
	
	//=== testing extra lights ===
	uint8_t ir_range = tentacle[0].read_analog_state(0);
	if (ir_range > 100){
		extra_lights.set_led_level(0, 250);	
		extra_lights.set_led_level(1, 250);	
		extra_lights.set_sma_level(0, 250);	
		extra_lights.set_sma_level(1, 250);	
	}
	else{
		extra_lights.set_led_level(0, 0);	
		extra_lights.set_led_level(1, 0);	
		extra_lights.set_sma_level(0, 0);	
		extra_lights.set_sma_level(1, 0);	
	}
	
}

void HK_Behaviours::stress_test_behaviour(const uint32_t &curr_time)
{

	// LED stress test
	static wave_t sine_wave [32] = {127, 151, 175, 197, 216, 232, 244, 251, 254, 251, 244, 232, 216, 197, 175, 151, 127, 102, 78, 56, 37, 21, 9, 2, 0, 2, 9, 21, 37, 56, 78, 102};
	static WaveTable led_wave(1000, sine_wave);
	
	uint8_t led_level = led_wave.wave_function(curr_time);
	
	extra_lights.set_led_level(0, led_level);
	extra_lights.set_led_level(1, led_level);
	extra_lights.set_sma_level(0, led_level);
	extra_lights.set_sma_level(1, led_level);
	
	
	//vibration stress test
	for (uint8_t i = 0; i< 3; i++){
		tentacle[i].set_led_level(0, led_level);
		tentacle[i].set_led_level(1, led_level);
	}
	
	// SMA stress test
	//---- Tentacle cycling variables -----
	static bool tentacle_cycling[3] = {false, false, false};
	static uint32_t tentacle_phase_time[3] = {0, 0, 0};
	bool tentacle_on[3] = {false, false, false};
	
	//~~~ tentacle cycle~~~~
	for (uint8_t i=0; i<3; i++){
		
		// starting a cycle
		if (tentacle_cycling[i] == false){
			tentacle_cycling[i] = true;
			tentacle_phase_time[i] = millis();  
			tentacle[i].set_sma_level(0, 255);
			tentacle[i].set_sma_level(1, 255);					
		}
		else if (tentacle_cycling[i] == true){
			
			// if reaches the full period, restart cycle
			if ((curr_time - tentacle_phase_time[i]) > tentacle_cycle_period[i][1]*1000){
				tentacle_cycling[i]  = false;
			}
			// if reaches half the period, turn it off
			else if ((curr_time - tentacle_phase_time[i]) > tentacle_cycle_period[i][0]*1000){
				tentacle[i].set_sma_level(0, 0);
				tentacle[i].set_sma_level(1, 0);
			}	
		}
	}
}


//---- indicator LED -----

void HK_Behaviours::led_blink_behaviour(const uint32_t &curr_time) {

	//---- indicator LED blinking variables -----
	//~~indicator LED on~~
	static bool indicator_led_on_0 = 1;
	//~~indicator LED blink~~
	static bool indicator_led_blink_cycling = false;
	static uint32_t indicator_led_blink_phase_time= 0;

	if (indicator_led_on){
		
		// starting a blink cycle
		if (indicator_led_blink_cycling == false){
			indicator_led_blink_cycling = true;
			indicator_led_blink_phase_time = millis();      
			digitalWrite(indicator_led_pin, 1);
		}
		else if (indicator_led_blink_cycling == true){
			
			// if reaches the full period, restart cycle
			if ((curr_time - indicator_led_blink_phase_time) > indicator_led_blink_period){
				indicator_led_blink_cycling = false;
			}
			// if reaches half the period, turn it off
			else if ((curr_time - indicator_led_blink_phase_time) > indicator_led_blink_period>>1){
				digitalWrite(indicator_led_pin, 0);
			}	
		}
	}
	else{
	
		// if stopped in the middle of a cycle
		if (indicator_led_blink_cycling){
			indicator_led_blink_cycling = false;
			digitalWrite(indicator_led_pin, 0);
		}
	}
}

void HK_Behaviours::led_wave_behaviour(const uint32_t &curr_time){
	
	
	//static WaveTable test_wave(5);
	test_wave.set_duration(10000);
	uint8_t led_level = test_wave.wave_function(curr_time);
	//protocell.set_led_level(led_level);
	analogWrite(5, led_level);
	

}


//---- Tip IR primary action -----
void HK_Behaviours::tentacle_tip_ir_primary_action(const uint32_t &curr_time, const uint8_t* type){
	
	
	//---- Tentacle cycling variables -----
	static bool tentacle_cycling[3] = {false, false, false};
	static uint32_t tentacle_phase_time[3] = {0, 0, 0};
	static bool tentacle_on[3] = {false, false, false};

	//~~read IR sensors state~~
	// for (uint8_t i=0; i<3; i++){
		// tentacle_ir_state[i][1] = tentacle[i].read_analog_state(1);
	// }
	
	static uint16_t sma_offset_time[3] = {0, 0, 0};
	static uint8_t sma0[3] = {0, 0, 0};
	static uint8_t sma1[3] = {1, 1, 1};
	

	
	//~~~ tentacle cycle~~~~
	for (uint8_t i=0; i<3; i++){
	
		
		if (tentacle_ir_state[i][1] > tentacle_ir_threshold[i][1]){
			tentacle_on[i] = true;
		}

		
		if (tentacle_on[i]){
			
			// starting a cycle
			if (tentacle_cycling[i] == false){
			
				// behaviour Type
				switch (type[i]){
					case 1:
						sma0[i] = 0;
						sma1[i] = 1;
						sma_offset_time[i] = tentacle_cycle_offset[i];
					break;
					case 2:
						sma0[i] = 1;
						sma1[i] = 0;
						sma_offset_time[i] = tentacle_cycle_offset[i];
					break;
					default:
						sma0[i] = 0;
						sma1[i] = 1;
						sma_offset_time[i] = 0;
					break;
				}
				tentacle_cycling[i] = true;
				tentacle_phase_time[i] = millis();  
				
				// turn on the first sma
				tentacle[i].set_sma_level(sma0[i], 255);			
			}
			else if (tentacle_cycling[i] == true){
				
				
				volatile uint32_t cycle_time = curr_time - tentacle_phase_time[i];
				
				// if reaches the full period + offset, restart cycle
				if (cycle_time > (tentacle_cycle_period[i][1]*1000 + sma_offset_time[i])){
					tentacle_cycling[i]  = false;
					tentacle_on[i] = false;
				}
				
				//if reaches the on period + offset time
				else if (cycle_time > (tentacle_cycle_period[i][0]*1000 + sma_offset_time[i])){
					tentacle[i].set_sma_level(sma1[i], 0);
					tentacle[i].set_sma_level(sma0[i], 0);
	
				}
				
				// if reaches the on period, turn it off
				else if (cycle_time > tentacle_cycle_period[i][0]*1000){
					tentacle[i].set_sma_level(sma0[i], 0);
				}	
				

				//if reaches the offset time
				else if (cycle_time > sma_offset_time[i]){
					tentacle[i].set_sma_level(sma1[i], 255);
				}
			}
		}

	}
}


// //---- bottom IR primary action (soft) -----
// void HK_Behaviours::tentacle_bottom_ir_primary_action(const uint32_t &curr_time){


	// //---- Tentacle cycling variables -----
	// uint16_t protocell_cycle_period = 10000;

	// static wave_t sine_wave1 [32] = {0, 12, 23, 31, 36, 40, 43, 46, 48, 50, 51, 52, 53, 54, 55, 55, 55, 55, 55, 54, 53, 52, 51, 50, 48, 46, 43, 40, 36, 31, 23, 12};
	// static wave_t sine_wave2 [32] = {48, 50, 51, 52, 53, 54, 55, 55, 55, 55, 55, 54, 53, 52, 51, 50, 48, 46, 43, 40, 36, 31, 23, 12, 0, 12, 23, 31, 36, 40, 43, 46};
	// static WaveTable bot_ir_sine_wave(10000, sine_wave1);
	// static WaveTable bot_ir_cosine_wave(10000, sine_wave2);
	
	// //~~read IR sensors state~~
	// // for (uint8_t i=0; i<3; i++){
		// // tentacle_ir_state[i][0] = (uint8_t) tentacle[i].read_analog_state(0);
	// // }
	
	// uint8_t closest_ir = 0;
	// for (uint8_t i=0; i<3; i++){
		// if (tentacle_ir_state[i][0] > closest_ir)
			// closest_ir = tentacle_ir_state[i][0];
	// }
	

	// //if the object is very close
	// if (closest_ir > tentacle_ir_threshold[0][0]){
		// // for (uint8_t i = 0; i<2; i++)
			// // protocell[i].set_led_level(200);

		// return;	
	// }
	
	// uint8_t level_divider = 2;
	// // if there is no object detected
	// if (closest_ir < 80){
		// protocell_cycle_period = 10000;
		// //level_divider = 10;
		// // for (uint8_t i = 0; i<2; i++)
			// // protocell[i].set_led_level(0);
		// // return;
	// }
	// //otherwise, set the cycle period depending on distance
	// else if (closest_ir < 120)
		// protocell_cycle_period = 10*(255-closest_ir);
	// else
		// protocell_cycle_period = 10*(255-closest_ir);
		
	// if (protocell_cycle_period < 100)
		// protocell_cycle_period = 100;

	
	// bot_ir_sine_wave.set_duration(protocell_cycle_period);
	// bot_ir_cosine_wave.set_duration(protocell_cycle_period);
	
	
	// //~~~ tentacle cycle~~~~	
	// uint8_t led_level[2];
	// led_level[0] = bot_ir_cosine_wave.wave_function(curr_time) / level_divider;
	// led_level[1] = bot_ir_sine_wave.wave_function(curr_time) / level_divider;
	// for (uint8_t i = 0; i<2; i++){
		// protocell[i].set_led_level(led_level[i]);
	// }
	
// }