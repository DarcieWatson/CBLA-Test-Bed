#ifndef _TEENSY_UNIT_H
#define _TEENSY_UNIT_H

#include "Arduino.h"
#include "wave_table.h"
#include "i2c_t3.h"
#include "PWMDriver.h"

#define packet_size 64

class TeensyUnit{
	
	public:
	
		//===============================================
		//==== pin assignments ====
		//===============================================
		
		//--- Teensy on-board ---
		const uint8_t indicator_led_pin = 13;
		
		//--- Programming Pin ---
		const uint8_t PGM_DO_pin = 7;
		
		//--- Fast PWM pins ---
		const uint8_t FPWM_1_pin[2] = {3, 4};
		const uint8_t FPWM_2_pin[2] = {6, 5};
		const uint8_t FPWM_3_pin[0] = {};
		const uint8_t FPWM_4_pin[2] = {20, 21};
		const uint8_t FPWM_5_pin[2] = {25, 32};
		const uint8_t FPWM_6_pin[0] = {};
		const uint8_t* FPWM_pin[6];
		
		//--- Analogue pins ---
		const uint8_t Analog_1_pin[2] = {A11, A13};
		const uint8_t Analog_2_pin[2] = {A12, A15};
		const uint8_t Analog_3_pin[2] = {A16, A17};
		const uint8_t Analog_4_pin[2] = {A8, A9};
		const uint8_t Analog_5_pin[2] = {A2, A3};
		const uint8_t Analog_6_pin[2] = {A0, A1};
		const uint8_t* Analog_pin[6];
		
		//--- Multiplexer pins ---
		const uint8_t MUL_ADR_pin[3] = {2, 24, 33};
		
		//--- UART pins ---
		const uint8_t RX1_pin = 0;  //not being used
		const uint8_t TX1_pin = 1;  //not being used
		
		//--- SPI pins ---
		const uint8_t CE = 9;		//not being used
		const uint8_t CSN = 10;		//not being used
		const uint8_t MOSI = 11;	//not being used
		const uint8_t MISO = 12;	//not being used
		const uint8_t SCK = 13;		//not being used
		
		//--- Slow PWM pin ----
		const uint8_t SPWM_1_pin[2] = {0, 1};
		const uint8_t SPWM_2_pin[2] = {2, 3};
		const uint8_t SPWM_3_pin[4] = {4, 5, 6, 7};
		const uint8_t SPWM_4_pin[2] = {14, 15};
		const uint8_t SPWM_5_pin[2] = {12, 13};
		const uint8_t SPWM_6_pin[4] = {8, 9 , 10, 11};
		const uint8_t* SPWM_pin[6];
		
		
		
		//===============================================
		//==== Functions ====
		//===============================================
		
		//--- Constructor and destructor ---
		TeensyUnit();
		~TeensyUnit();

		//--- Initialization ---
		void init();
		
		//--- Communication functions ----
		bool receive_msg();
		void send_msg();
		virtual void parse_msg() = 0;
		virtual void compose_reply(byte front_signature, byte back_signature) = 0;
		
		
	protected:
		
		//==== constants ====
		const uint8_t num_outgoing_byte = packet_size;
		const uint8_t num_incoming_byte = packet_size;

		
		//==== COMMUNICATION variables =====
		byte send_data_buff[packet_size];
		byte recv_data_buff[packet_size];
		uint8_t request_type = 0;
		
		//==== Port Classes ====
		
		//--- Tentacle ---
		class TentaclePort{
		
			public:
			
				//~~constructor and destructor~~
				TentaclePort(TeensyUnit& teensy_parent, const uint8_t Port_Id, const bool All_Slow);
				~TentaclePort();

				//~~outputs~~
				void set_sma_level(const uint8_t id, const uint8_t level);
				void set_led_level(const uint8_t id, const uint8_t level);
				
				//~~inputs~~
				uint16_t read_analog_state(const uint8_t id);  //{IR 0, IR 1}
				uint16_t* read_acc_state();  // return array:{x, y, z}
				
				
				//~~configurations~~
				const uint8_t port_id;
				const bool is_all_slow;
				uint8_t sma_pins[2];
				uint8_t led_pins[2];
				uint8_t analog_pins[2];
				uint8_t acc_pin;
				
			private:
				TeensyUnit& teensy_unit;
				PWMDriver spwm;	
				
				void spwm_init(uint16_t freq=1000);
		};
	
	
		//--- Protocell Port ---
		class ProtocellPort{
			
			public:
			
				//~~constructor and destructor~~
				ProtocellPort(TeensyUnit& teensy_parent, const uint8_t Port_Id, const bool Slow);
				~ProtocellPort();

				//~~outputs~~
				void set_led_level(const uint8_t level);
				
				//~~inputs~~
				uint16_t read_analog_state(); 
				
				
				//~~configurations~~
				const uint8_t port_id;
				const bool is_slow;
				uint8_t led_pin;
				uint8_t analog_pin;
				
			private:
				TeensyUnit& teensy_unit;
				PWMDriver spwm;	
				
				void spwm_init(uint16_t freq=1000);
		
		};
		
		
		//===============================================
		//==== Ports ====
		//===============================================
		
		TentaclePort tentacle_0;
		TentaclePort tentacle_1;
		TentaclePort tentacle_2;
		ProtocellPort protocell;		
		
};


#endif
