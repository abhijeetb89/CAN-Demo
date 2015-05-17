#ifndef  CAN_DBC_H
#define  CAN_DBC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Messages */
#define MSG_INT_EXT_LIGHT		 		 0x350
#define MSG_Vehicle_Speed 			 0x250
#define MSG_Parking_Obstacle 		 0x200
#define MSG_Interior_Temperature 0x300
#define MSG_Ambient_Brightness   0x400
#define MSG_Airbag_Release 			 0x50
	 
/* DLCs */
#define DLC_INT_EXT_LIGHT		 		 2
#define DLC_Vehicle_Speed 			 1
#define DLC_Parking_Obstacle 		 1
#define DLC_Interior_Temperature 2
#define DLC_Ambient_Brightness   2
#define DLC_Airbag_Release 			 1
	 

/* Signals */
#define SIGNAL_HIGH_BEAM								0x01
#define SIGNAL_LOW_BEAM									0x02
#define SIGNAL_LEFT_TURN_INDICATOR			0x04
#define SIGNAL_RIGHT_TURN_INDICATOR			0x08
#define SIGNAL_MARKER_LIGHT							0x10
#define SIGNAL_DRL											0x20
#define SIGNAL_EMERGENCY_STOP						0x40	
#define SIGNAL_FRONT_FOG_LAMP						0x01
#define SIGNAL_REAR_FOG_LAMP						0x02
#define SIGNAL_TAIL_LAMP								0x04
#define SIGNAL_BRAKE_LAMP								0x08
#define SIGNAL_DOME_LAMP								0x10
#define SIGNAL_ROOM_TEMP								
#define SIGNAL_LUX_VALUE								
#define SIGNAL_VEHICLE_SPEED						0x01
#define SIGNAL_PARKING_SENSOR_REAR			0x01
#define SIGNAL_AIRBAG_RELEASE						0x01	 

#ifdef __cplusplus
}
#endif

#endif 
