#ifndef  CAN_DBC_H
#define  CAN_DBC_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

/* Messages */
#define ID_INT_EXT_LIGHT		 		 0x350
#define ID_VEHICLE_SPEED 				 0x250
#define ID_PARKING_OBSTACLE 		 0x200
#define ID_INTERIOR_TEMPERATURE  0x300
#define ID_AMBIENT_BRIGHTNESS    0x400
#define ID_AIRBAG_RELEASE 			 0x50

	 
/* DLCs */
#define DLC_INT_EXT_LIGHT		 		 2
#define DLC_Vehicle_Speed 			 1
#define DLC_Parking_Obstacle 		 1
#define DLC_Interior_Temperature 2
#define DLC_Ambient_Brightness   2
#define DLC_Airbag_Release 			 1
	 

/* Signals */
#define SIGNAL_HIGH_BEAM								0
#define SIGNAL_LOW_BEAM									1
#define SIGNAL_LEFT_TURN_INDICATOR			2
#define SIGNAL_RIGHT_TURN_INDICATOR			3
#define SIGNAL_MARKER_LIGHT							4
#define SIGNAL_DRL											5
#define SIGNAL_EMERGENCY_STOP						6	
#define SIGNAL_FRONT_FOG_LAMP						7
#define SIGNAL_REAR_FOG_LAMP						0
#define SIGNAL_TAIL_LAMP								1
#define SIGNAL_BRAKE_LAMP								2
#define SIGNAL_DOME_LAMP								3
#define SIGNAL_ROOM_TEMP								
#define SIGNAL_LUX_VALUE								
#define SIGNAL_VEHICLE_SPEED						0
#define SIGNAL_PARKING_SENSOR_REAR			0
#define SIGNAL_AIRBAG_RELEASE						0	 

typedef struct{
	
 uint16_t id;
 
 uint8_t dlc;
	
 bool signal_high_beam;								
 bool signal_low_beam;									
 bool signal_left_turn_indicator;			
 bool signal_right_turn_indicator;			
 bool signal_marker_light;							
 bool signal_drl;											
 bool signal_emergency_stop;							
 bool signal_front_fog_lamp;						
 bool signal_rear_fog_lamp;						
 bool signal_tail_lamp;								
 bool signal_brake_lamp;								
 bool signal_dome_lamp;

 uint16_t cycle_time;	
 
}struct_Msg_Init_Ext_Light;

typedef struct{
	
 uint16_t id;
	
 uint8_t dlc;
	
 bool signal_airbag_release;
	
 uint16_t cycle_time; 

}struct_Msg_Airbag_Release;

#ifdef __cplusplus
}
#endif

#endif 
