#ifndef __MT_DCPRPOWER_CORE_HEADER
#define __MT_DCPRPOWER_CORE_HEADER

#include "MtPRcommon.h"


/******************************************************************************/
/*                  Instrument Driver Function Declarations                   */
/******************************************************************************/

//**************************************************************************
// Basic Function 
//**************************************************************************

/*
* 功   能：计算两电阻并联后的电阻值
* 输入参数：r1：电阻1
*         r2：电阻2
*
* 返回值：并联后的电阻值
*
*/
double calParallelResistance(double r1, double r2);

//根据设置电阻大小,通过算法计算,确定开关状态.
int calculate_switchState(ViSession vi, ViUInt32 subUnit, const ViReal64 resistance, 
	int switchState[], int switchSize, double calibrationValue[53]);


//**************************************************************************
//  Root Functions
//**************************************************************************
MT_DEVICE* findDevice(ViSession handle);

//**************************************************************************
// Initialize
//	Establishes communications with the instrument.
//		resourceName (in)    Instrument description VXI/LA
//		id_query (in)    VI_TRUE =  Perform in-system verification
//		                 VI_FALSE = Do not perform in-system verification
//		reset_instr (in) VI_TRUE =  Perform reset operation
//						 VI_FALSE = Do not perform reset operation
//		vi (out)		 Instrument handle
//		Return = ViStatus
//**************************************************************************
ViStatus MtPR__init(ViRsrc resourceName, ViBoolean id_query, ViBoolean reset_instr, ViSession *vi);


//**************************************************************************
// Close
//	Terminates the software connection to the instrument and deallocates
//	system resources.  
//		vi (in)			Instrument handle
//		Return = ViStatus
//**************************************************************************
ViStatus MtPR__Close(ViSession vi);


//**************************************************************************
// System Functions
//**************************************************************************

//**************************************************************************
// Route Functions
//**************************************************************************

//**************************************************************************
// Clear card
//	Clears all of a card's output sub-units.
//		Return = ViStatus
//**************************************************************************
ViStatus MtPR__clearCard(ViSession vi);


//**************************************************************************
// Clear a sub-unit
//  Clears all of an individual sub-unit's outputs, opening relays or
//  drivers, or setting logic '0'.
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		Return = ViStatus
//**************************************************************************
ViStatus MtPR__clearSub(ViSession vi, ViUInt32 subUnit);


//**************************************************************************
// Set the state of a single output
//  Sets the state of an individual output channel.
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		channel (in)	Channel number
//		state (in)		State to set (VI_OFF = channel OFF, VI_ON = channel ON)
//		Return = ViStatus
//**************************************************************************
ViStatus MtPR__setChannelState(ViSession vi, ViUInt32 subUnit,
	ViUInt32 channel, ViBoolean state);


////**************************************************************************
//// Set crosspoint state (added 26/07/02)
////  Sets the state of an individual matrix crosspoint.
////		vi (in)			Instrument handle
////		subUnit (in)	Sub-unit number
////		row (in)		Crosspoint row (Y) location
////		column (in)		Crosspoint column (X) location
////		state (in)		State to set (VI_OFF = crosspoint OFF, VI_ON = crosspoint ON)
////		Return = ViStatus
////**************************************************************************
//ViStatus pipx40_setCrosspointState(ViSession vi, ViUInt32 subUnit,
//	ViUInt32 row, ViUInt32 column, ViBoolean state);


//**************************************************************************
// Set all of a sub-unit's outputs.
//  Sets the states of all of a sub-unit's outputs.
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		pattern (in)	Array containing a bit-pattern representing
//						the desired output states
//		Return = ViStatus
//**************************************************************************
ViStatus MtPR__setChannelPattern(ViSession vi, ViUInt32 subUnit, ViAUInt32 pattern);


//**************************************************************************
// Get output channel state
//	Obtains the state of an individual output channel.
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		channel (in)	Channel number
//		state (out)		The state of the specified output channel
//						(VI_OFF = OFF or logic '0', VI_ON = ON or logic '1')
//		Return = ViStatus
//**************************************************************************
ViStatus MtPR__getChannelState(ViSession vi, ViUInt32 subUnit,
	ViUInt32 channel, ViPBoolean state);


////**************************************************************************
//// Get crosspoint state (added 26/07/02)
////	Obtains the state of an individual matrix crosspoint.
////		vi (in)			Instrument handle
////		subUnit (in)	Sub-unit number
////		row (in)		Crosspoint row (Y) location
////		column (in)		Crosspoint column (X) location
////		state (out)		The state of the specified output channel
////						(VI_OFF = OFF, VI_ON = ON)
////		Return = ViStatus
////**************************************************************************
//ViStatus pipx40_getCrosspointState(ViSession vi, ViUInt32 subUnit,
//	ViUInt32 row, ViUInt32 column, ViPBoolean state);


//**************************************************************************
// Get all of a sub-unit's outputs.
//  Gets the states of all of a sub-unit's outputs.
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		pattern (out)	Array to accept a bit-pattern representing
//						the sub-unit's output states
//		Return = ViStatus
//**************************************************************************
ViStatus MtPR__getChannelPattern(ViSession vi, ViUInt32 subUnit, ViAUInt32 pattern);


////**************************************************************************
//// Clear a sub-unit's mask
////  Unmasks all of an individual sub-unit's outputs.
////		vi (in)			Instrument handle
////		subUnit (in)	Sub-unit number
////		Return = ViStatus
////**************************************************************************
//ViStatus pipx40_clearMask(ViSession vi, ViUInt32 subUnit);
//
//
////**************************************************************************
//// Set the state of the mask for a single output
////  Sets the state of an individual output channel's mask.
////		vi (in)			Instrument handle
////		subUnit (in)	Sub-unit number
////		channel (in)	Channel number
////		state (in)		State to set (VI_OFF = channel unmasked, VI_ON = channel masked)
////		Return = ViStatus
////**************************************************************************
//ViStatus pipx40_setMaskState(ViSession vi, ViUInt32 subUnit,
//	ViUInt32 channel, ViBoolean state);
//
//
////**************************************************************************
//// Set crosspoint mask (added 26/07/02)
////  Sets the state of a matrix crosspoint's mask.
////		vi (in)			Instrument handle
////		subUnit (in)	Sub-unit number
////		row (in)		Crosspoint row (Y) location
////		column (in)		Crosspoint column (X) location
////		state (in)		State to set (VI_OFF = unmasked, VI_ON = masked)
////		Return = ViStatus
////**************************************************************************
//ViStatus pipx40_setCrosspointMask(ViSession vi, ViUInt32 subUnit,
//	ViUInt32 row, ViUInt32 column, ViBoolean state);
//
//
////**************************************************************************
//// Set the masks for all of a sub-unit's outputs.
////  Sets the states of all of a sub-unit's output masks.
////		vi (in)			Instrument handle
////		subUnit (in)	Sub-unit number
////		pattern (in)	Array containing a bit-pattern representing
////						the desired output masks
////		Return = ViStatus
////**************************************************************************
//ViStatus pipx40_setMaskPattern(ViSession vi, ViUInt32 subUnit,
//	ViAUInt32 pattern);
//
//
////**************************************************************************
//// Get output channel mask state
////	Obtains the state of an individual output channel's mask.
////		vi (in)			Instrument handle
////		subUnit (in)	Sub-unit number
////		channel (in)	Channel number
////		state (out)		The state of the specified output channel's mask
////						(VI_OFF = unmasked, VI_ON = masked)
////		Return = ViStatus
////**************************************************************************
//ViStatus pipx40_getMaskState(ViSession vi, ViUInt32 subUnit,
//	ViUInt32 channel, ViPBoolean state);
//
//
////**************************************************************************
//// Get crosspoint mask (added 26/07/02)
////	Obtains the state of a matrix crosspoint's mask.
////		vi (in)			Instrument handle
////		subUnit (in)	Sub-unit number
////		row (in)		Crosspoint row (Y) location
////		column (in)		Crosspoint column (X) location
////		state (out)		The state of the specified crosspoint's mask
////						(VI_OFF = unmasked, VI_ON = masked)
////		Return = ViStatus
////**************************************************************************
//ViStatus pipx40_getCrosspointMask(ViSession vi, ViUInt32 subUnit,
//	ViUInt32 row, ViUInt32 column, ViPBoolean state);
//
//
////**************************************************************************
//// Get all of a sub-unit's output masks.
////  Gets the states of all of a sub-unit's output masks.
////		vi (in)			Instrument handle
////		subUnit (in)	Sub-unit number
////		pattern (out)	Array to accept a bit-pattern representing
////						the sub-unit's output masks
////		Return = ViStatus
////**************************************************************************
//ViStatus pipx40_getMaskPattern(ViSession vi, ViUInt32 subUnit,
//	ViAUInt32 pattern);
//
//**************************************************************************

//**************************************************************************
// Get all of a sub-unit's output masks.
//  Gets the states of all of a sub-unit's output masks.
//		vi (in)			Instrument handle
//		corf (in)	Celcius Or Farenheit
//		Temperature (out)	Temperature Value
//		Return = ViStatus
//**************************************************************************
ViStatus pipx40_getCardTemperature(ViSession vi, ViUInt32 corf, ViPReal64 Temperature);


//**************************************************************************
// Utility Functions
//**************************************************************************

//**************************************************************************
// Reset 
//	Places instrument in default state.
//  All outputs are cleared; all outputs are unmasked.
//		vi (in)			Instrument handle
//		Return = ViStatus
//**************************************************************************
ViStatus MtPR__reset(ViSession vi);


//**************************************************************************
// Self-Test
//	Perform self-test and return result of the test.
//		vi (in)				Instrument handle
//		test_result  (out)	Numeric result of test; 0 = no error
//		test_message (out)	Self-test message
//		Return = ViStatus
//**************************************************************************
ViStatus MtPR__self_test(ViSession vi, ViPInt16 test_result, ViPString test_message);


//**************************************************************************
// Revision Query
//	Returns the revision of the instrument driver and instrument firmware.
//		vi (in)			Instrument handle
//		driver_rev (out)Driver revision
//		instr_rev (out) Instrument revision
//		Return = ViStatus
//**************************************************************************
ViStatus MtPR__revision_query(ViSession vi, ViPString driver_rev, ViPString instr_rev);


//**************************************************************************
// Error Query
//	Return an error code and corresponding message from the instrument's 
//	error queue.
//		vi (in)			Instrument handle
//		ecode (out)		Instrument error code
//		emessage (out)	Error message
//		Return = ViStatus
//**************************************************************************
ViStatus pipx40_error_query(ViSession vi, ViPInt32 ecode, ViChar message[256]);


//**************************************************************************
// Error Message
//  Translate the error return value from the instrument into a
//	user-readable string.
//		vi (in)				Instrument handle
//		status_code (in)	Instrument error code
//		message (out)		VISA or instrument driver error message
//		Return = ViStatus
//**************************************************************************
ViStatus MtPR__error_message(ViSession vi, ViStatus status_code, ViChar message[256]);


//**************************************************************************
// Get card ID
//	Obtains card identification in string format.
//		vi (in)			Instrument handle
//		id (out)		Identification string
//		Return = ViStatus
//**************************************************************************
ViStatus MtPR__getCardId(ViSession vi, ViPString id);


//**************************************************************************
// Get sub-unit closure limit
//	Obtains the maximum number of channel closures allowed on the sub-unit.
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		limit (out)		Closure limit
//		Return = ViStatus
//**************************************************************************
ViStatus pipx40_getClosureLimit(ViSession vi, ViUInt32 subUnit, ViPUInt32 limit);


//**************************************************************************
// Get card diagnostic 
//	Obtains diagnostic information in string format. 
//		vi (in)			Instrument handle
//		message (out)	Diagnostic string
//		Return = ViStatus
//**************************************************************************
ViStatus pipx40_getDiagnostic(ViSession vi, ViPString message);


//**************************************************************************
// Get sub-unit setting time
//	Obtains sub-unit settling time in microseconds.
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		time (out)		Settling time
//		Return = ViStatus
//**************************************************************************
ViStatus pipx40_getSettlingTime(ViSession vi, ViUInt32 subUnit, ViPUInt32 time);


//**************************************************************************
// Get sub-unit counts
//	Obtains counts of the number of input and output sub-units on the card.
//		vi (in)			Instrument handle
//		inSubs (out)	The number of input sub-units
//		outSubs (out)	The number of output sub-units
//		Return = ViStatus
//**************************************************************************
ViStatus MtPR__getSubCounts(ViSession vi, ViPUInt32 inSubs, ViPUInt32 outSubs);


//**************************************************************************
// Get sub-unit type
//	Obtains sub-unit type description, in string format.
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		out (in)		VI_TRUE to obtain output sub-unit information
//						VI_FALSE to obtain input sub-unit information
//		subType (out)		Identification string
//		Return = ViStatus
//**************************************************************************
ViStatus MtPR__getSubType(ViSession vi, ViUInt32 subUnit,
	ViBoolean out, ViPString subType);


//**************************************************************************
// Get sub-unit information
//	Obtains sub-unit type information, in numeric format.
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		out (in)		VI_TRUE to obtain output sub-unit information
//						VI_FALSE to obtain input sub-unit information
//		subType (out)	Sub-unit functionality
//		rows (out)		Sub-unit row dimension
//		columns (out)	Sub-unit column dimension
//		Return = ViStatus
//**************************************************************************
ViStatus MtPR__getSubInfo(ViSession vi, ViUInt32 subUnit,
	ViBoolean out, ViPUInt32 subType, ViPUInt32 rows, ViPUInt32 columns);


//**************************************************************************
// Get status information
//	Obtains card status as a set of bit-flags.
//		vi (in)			Instrument handle
//		status (out)	Bit-flags indicating the card's status
//		Return = ViStatus
//**************************************************************************
ViStatus MtPR__getCardStatus(ViSession vi, ViPUInt32 status);


//**************************************************************************
// Read calibration value
//	Reads from EEPROM the calibration value associated with
//  the specified index number.
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		idx (in)		Calibration value index number
//		data (out)		Calibration value read
//		Return = ViStatus
//**************************************************************************
ViStatus pipx40_readCalibration(ViSession vi, ViUInt32 subUnit,
	ViUInt32 idx, ViPUInt32 data);


//**************************************************************************
// Write calibration value
//	Writes to EEPROM the calibration value associated with
//  the specified index number.
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		idx (in)		Calibration value index number
//		data (in)		Calibration value to write
//		Return = ViStatus
//**************************************************************************
ViStatus pipx40_writeCalibration(ViSession vi, ViUInt32 subUnit,
	ViUInt32 idx, ViUInt32 data);


//**************************************************************************
// Get input channel state
//	Obtains the state of an individual input channel.
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		channel (in)	Channel number
//		state (out)		The state of the specified input channel
//						(VI_OFF = logic '0', VI_ON = logic '1')
//		Return = ViStatus
//**************************************************************************
ViStatus pipx40_readInputState(ViSession vi, ViUInt32 subUnit,
	ViUInt32 channel, ViPBoolean state);


//**************************************************************************
// Get a sub-unit's input pattern.
//  Gets the states of all of a sub-unit's inputs.
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		pattern (out)	Array to accept a bit-pattern representing
//						the sub-unit's input states
//		Return = ViStatus
//**************************************************************************
ViStatus pipx40_readInputPattern(ViSession vi, ViUInt32 subUnit, ViAUInt32 pattern);

//**************************************************************************
// Set driver mode
//  Sets (by means of a set of bit-flags) various driver operating
//  characteristics.
//		newMode (in)		The new operating mode flags to set
//		previousMode (out)	The previous operating mode flags
//		Return = ViStatus
//**************************************************************************
ViStatus pipx40_setDriverMode(ViUInt32 newMode, ViPUInt32 previousMode);


//**************************************************************************
// Additional general-purpose function.
//**************************************************************************

//**************************************************************************
// Get sub-unit status information as a set of bit-flags.  //获取子单元状态信息作为一组位标志。
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number   //子单元号
//		status (out)	Bit-flags indicating the sub-unit's status  //表示子单元状态的位标志
//		Return = ViStatus
//**************************************************************************
ViStatus pipx40_getSubStatus(ViSession vi, ViUInt32 subUnit, ViPUInt32 status);


//**************************************************************************
// Additional special-purpose switch functions.
//**************************************************************************

//**************************************************************************
// Obtain the value of a sub-unit attribute  //获取子单元属性值
//  Attribute values are relevant to specialised operation of certain card
//  types using pipx40_operateSwitch.  //属性值与特定卡片的特定操作相关 使用pipx40_operateSwitch。
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		out (in)		VI_TRUE to obtain output sub-unit information
//						VI_FALSE to obtain input sub-unit information (unsupported)
//		AttrCode (in)	A numeric code indicating the attribute to be queried
//		pAttrValue (out)	The value of the selected attribute
//		Return = ViStatus
//**************************************************************************
ViStatus pipx40_getSubAttribute(ViSession vi, ViUInt32 subUnit,
	ViBoolean out, ViUInt32 AttrCode, ViAddr const pAttrValue);


//**************************************************************************
// Sets the value of a sub-unit attribute
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		out (in)		VI_TRUE to obtain output sub-unit information
//						VI_FALSE to obtain input sub-unit information (unsupported)
//		AttrCode (in)	A numeric code indicating the attribute to be queried
//		pAttrValue (in) Pointer to value of the attribute
//		Return = ViStatus
//**************************************************************************
ViStatus pipx40_SetAttribute(ViSession vi, ViUInt32 subUnit,
	ViBoolean out, ViUInt32 AttrCode, ViAddr pAttrValue);


//**************************************************************************
// Gets the value of a sub-unit attribute
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		out (in)		VI_TRUE to obtain output sub-unit information
//						VI_FALSE to obtain input sub-unit information (unsupported)
//		AttrCode (in)	A numeric code indicating the attribute to be queried
//		pAttrValue (out) The value of the selected attribute
//		Return = ViStatus
//**************************************************************************
ViStatus pipx40_GetAttribute(ViSession vi, ViUInt32 subUnit,
	ViBoolean out, ViUInt32 AttrCode, ViAddr pAttrValue);


//**************************************************************************
// Operate an individual switch
//  Applicable to specialised operation of card types having auto-isolation
//  and/or auto loopthru features.
//		vi (in)				Instrument handle
//		subUnit (in)		Sub-unit number
//		switchFunc (in)		A code indicating the functional group of the switch
//		segNum (in)			The segment in which the switch is located
//		switchNum (in)		The logical number of the switch
//		subSwitch (in)		The logical sub-switch
//		switchAction (in)	A code indicating the action to perform
//		state (out)			The state of the switch (after performing any action)
//		Return = ViStatus
//**************************************************************************
ViStatus pipx40_operateSwitch(ViSession vi, ViUInt32 subUnit,
	ViUInt32 switchFunc, ViUInt32 segNum, ViUInt32 switchNum, ViUInt32 subSwitch,
	ViUInt32 switchAction, ViPBoolean state);


//**************************************************************************
// Floating-point calibration data functions.
//**************************************************************************

// Values for selecting User or Factory calibration stores
#define MT_CAL_STORE_USER		0
#define MT_CAL_STORE_FACTORY	1

//**************************************************************************
// Store floating-point calibration value(s) in EEPROM
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		store (in)		value indicating which store to access (user = 0, factory = 1)
//		offset (in)		offset in the sub-unit's calibration table at which to start (zero-based)
//		numValues (in)	the number of calibration values to write
//		data (in)		pointer to array of calibration values
//		Return = ViStatus
//**************************************************************************
ViStatus MtPR__writeCalibrationFP(ViSession vi, ViUInt32 subUnit,
	ViUInt32 store, ViUInt32 offset, ViUInt32 numValues, ViReal64 *data);


//**************************************************************************
// Read floating-point calibration value(s) from EEPROM
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		store (in)		value indicating which store to access (user = 0, factory = 1)
//		offset (in)		offset in the sub-unit's calibration table at which to start (zero-based)
//		numValues (in)	the number of calibration values to read
//		data (out)		pointer to array to receive calibration values
//		Return = ViStatus
//**************************************************************************
ViStatus MtPR__readCalibrationFP(ViSession vi, ViUInt32 subUnit,
	ViUInt32 store, ViUInt32 offset, ViUInt32 numValues, ViReal64 *dataArray);

ViStatus MtPR__InitReadCalibrationFP(ViSession vi, ViUInt32 subUnit,
	ViUInt32 store, ViUInt32 offset, ViUInt32 numValues, ViReal64* dataArray);

//**************************************************************************
// Write calibration date information to EEPROM
// Date information is obtained from the current system date.
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		store (in)		value indicating which store to access (user = 0, factory = 1)
//		interval (in)	the desired calibration interval (in days)
//		Return = ViStatus
//**************************************************************************
ViStatus MtPR__writeCalibrationDate(ViSession vi, ViUInt32 subUnit,
	ViUInt32 store, ViUInt32 interval);


//**************************************************************************
// Read calibration date information from EEPROM
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		store (in)		value indicating which store to access (user = 0, factory = 1)
//		year (out)		pointer to variable to receive year of calibration
//		day (out)		pointer to variable to receive day in year of calibration
//		interval (out)	pointer to variable to receive calibration interval (in days)
//		Return = ViStatus
//**************************************************************************
ViStatus MtPR__readCalibrationDate(ViSession vi, ViUInt32 subUnit,
	ViUInt32 store, ViPUInt32 year, ViPUInt32 day, ViPUInt32 interval);


//**************************************************************************
// Set a calibration point
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		idx (in)		index number of calibration point
//		Return = ViStatus
//**************************************************************************
ViStatus pipx40_setCalibrationPoint(ViSession vi, ViUInt32 subUnit, ViUInt32 idx);


//**************************************************************************
// Programmable resistor specific functions.
//**************************************************************************


//界面开关控制：通过设置界面的开关输出相应的电阻值.
ViStatus MtPR__setSwitchGetResistance(ViSession vi, ViUInt32 subUnit, int switchState[64]);


//**************************************************************************
// Set resistance as close as possible to the given value
//		vi (in)				Instrument handle
//		subUnit (in)		Sub-unit number
//		mode (in)			A value indicating how the given resistance value is to be applied
//		resistance (in)		the resistance value to set
//		Return = ViStatus
//**************************************************************************
ViStatus MtPR__resSetResistance(ViSession vi, ViUInt32 subUnit,
	ViUInt32 mode, const ViReal64 resistance);


//**************************************************************************
// Obtain the set resistance value
//		vi (in)				Instrument handle
//		subUnit (in)		Sub-unit number
//		resistance (out)	pointer to variable to receive the resistance value
//		Return = ViStatus
//**************************************************************************
ViStatus MtPR__resGetResistance(ViSession vi, ViUInt32 subUnit, ViPReal64 resistance);


//**************************************************************************
// Obtain detailed resistor information
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		minRes (out)	pointer to variable to receive the minimum resistance setting
//		maxRes (out)	pointer to variable to receive the maximum resistance setting
//		refRes (out)	pointer to variable to receive the reference resistance value
//		precPC (out)	pointer to variable to receive percentage precision (+/- percent)
//		precDelta (out)	pointer to variable to receive delta precision (+/- ohms)
//		int1 (out)		pointer to (currently unused) variable
//		intDelta (out)	pointer to variable to receive internal precision (+/- ohms)
//		capabilities (out)	pointer to variable to receive capabilities flags
//		Return = ViStatus
//**************************************************************************
ViStatus MtPR__resGetInfo(ViSession vi, ViUInt32 subUnit,
	ViPReal64 minRes, ViPReal64 maxRes, ViPReal64 refRes, ViPReal64 precPC,
	ViPReal64 precDelta, ViPReal64 int1, ViPReal64 intDelta, ViPUInt32 capabilities);


//**************************************************************************
// Clean up before terminating
//  Closes any cards that remain open, freeing all resources.
//		Return = ViStatus
//**************************************************************************
ViStatus pipx40__cleanUp(void);


/*********************************************************/
/*         MT（MicroTest） Basis Functions                */
/*********************************************************/

//写入 SN number
ViStatus MtPR__writeCardSNnumber(ViSession vi, ViUInt32 subUnit, ViUInt32 store, ViUInt32 SN);
//读取 SN-number 和 FwVersion
ViStatus MtPR__readCardSNnumber(ViSession vi, ViUInt32 subUnit, ViUInt32 store, ViPUInt32 SN, ViPUInt64 FwVersion);

//在线升级
ViStatus MtPR__OnlineUpdate_U45(ViSession vi, const char* path);
ViStatus MtPR__OnlineUpdateMultiple_U45(ViSession vi[], ViInt32 number, const char* path, ViStatus result[]);


//*********************************************
// Driver constant 
//*********************************************


#endif
