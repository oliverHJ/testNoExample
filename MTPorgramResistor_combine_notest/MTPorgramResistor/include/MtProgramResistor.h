/****************************************************************************
*             MICROTEST Programmable Resistor
****************************************************************************/
#ifndef __MT_PROGRAM_RESISTOR_HEADER
#define __MT_PROGRAM_RESISTOR_HEADER

#include <visa.h>
#include <vpptype.h>


#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif


#if defined(_WIN32)
#define MTPR_EXPORT  __declspec(dllexport)
#else
#define MTPR_EXPORT  __declspec(dllimport)
#endif



//**************************************************************************
// Basic Function 
//**************************************************************************



//**************************************************************************
// Function Declarations
//**************************************************************************


//**************************************************************************
//  Root Functions
//**************************************************************************

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
MTPR_EXPORT ViStatus MtPR_init(ViRsrc resourceName, ViBoolean id_query, ViBoolean reset_instr, ViSession *vi);


//**************************************************************************
// Close
//	Terminates the software connection to the instrument and deallocates
//	system resources.  
//		vi (in)			Instrument handle
//		Return = ViStatus
//**************************************************************************
MTPR_EXPORT ViStatus MtPR_Close(ViSession vi);


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
MTPR_EXPORT ViStatus MtPR_clearCard(ViSession vi);


//**************************************************************************
// Clear a sub-unit
//  Clears all of an individual sub-unit's outputs, opening relays or
//  drivers, or setting logic '0'.
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		Return = ViStatus
//**************************************************************************
MTPR_EXPORT ViStatus MtPR_clearSub(ViSession vi, ViUInt32 subUnit);


//**************************************************************************
// Set the state of a single output
//  Sets the state of an individual output channel.
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		channel (in)	Channel number
//		state (in)		State to set (VI_OFF = channel OFF, VI_ON = channel ON)
//		Return = ViStatus
//**************************************************************************
MTPR_EXPORT ViStatus MtPR_setChannelState(ViSession vi, ViUInt32 subUnit,
	ViUInt32 channel, ViBoolean state);


//**************************************************************************
// Set all of a sub-unit's outputs.
//  Sets the states of all of a sub-unit's outputs.
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		pattern (in)	Array containing a bit-pattern representing
//						the desired output states
//		Return = ViStatus
//**************************************************************************
MTPR_EXPORT ViStatus MtPR_setChannelPattern(ViSession vi, ViUInt32 subUnit, ViAUInt32 pattern);


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
MTPR_EXPORT ViStatus MtPR_getChannelState(ViSession vi, ViUInt32 subUnit,
	ViUInt32 channel, ViPBoolean state);


//**************************************************************************
// Get all of a sub-unit's outputs.
//  Gets the states of all of a sub-unit's outputs.
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		pattern (out)	Array to accept a bit-pattern representing
//						the sub-unit's output states
//		Return = ViStatus
//**************************************************************************
MTPR_EXPORT ViStatus MtPR_getChannelPattern(ViSession vi, ViUInt32 subUnit, ViAUInt32 pattern);


//************该函数还未实现**************************************************************
// Get CardTemperature.
//		vi (in)			Instrument handle
//		corf (in)	Celcius Or Farenheit
//		Temperature (out)	Temperature Value
//		Return = ViStatus
//**************************************************************************
MTPR_EXPORT ViStatus MtPR_getCardTemperature(ViSession vi, ViUInt32 corf, ViPReal64 Temperature);


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
MTPR_EXPORT ViStatus MtPR_reset(ViSession vi);


//**************************************************************************
// Self-Test
//	Perform self-test and return result of the test.
//		vi (in)				Instrument handle
//		test_result  (out)	Numeric result of test; 0 = no error
//		test_message (out)	Self-test message
//		Return = ViStatus
//**************************************************************************
MTPR_EXPORT ViStatus MtPR_self_test(ViSession vi, ViPInt16 test_result, ViPString test_message);


//**************************************************************************
// Revision Query
//	Returns the revision of the instrument driver and instrument firmware.
//		vi (in)			Instrument handle
//		driver_rev (out)Driver revision
//		instr_rev (out) Instrument revision
//		Return = ViStatus
//**************************************************************************
MTPR_EXPORT ViStatus MtPR_revision_query(ViSession vi, ViPString driver_rev, ViPString instr_rev);


//**************************************************************************
// Error Query
//	Return an error code and corresponding message from the instrument's 
//	error queue.
//		vi (in)			Instrument handle
//		ecode (out)		Instrument error code
//		emessage (out)	Error message
//		Return = ViStatus
//**************************************************************************
MTPR_EXPORT ViStatus MtPR_error_query(ViSession vi, ViPInt32 ecode, ViString message);


//**************************************************************************
// Error Message
//  Translate the error return value from the instrument into a
//	user-readable string.
//		vi (in)				Instrument handle
//		status_code (in)	Instrument error code
//		message (out)		VISA or instrument driver error message
//		Return = ViStatus
//**************************************************************************
MTPR_EXPORT ViStatus MtPR_error_message(ViSession vi, ViStatus status_code, ViString message);


//**************************************************************************
// Get card ID
//	Obtains card identification in string format.
//		vi (in)			Instrument handle
//		id (out)		Identification string
//		Return = ViStatus
//**************************************************************************
MTPR_EXPORT ViStatus MtPR_getCardId(ViSession vi, ViPString id);


//************该函数还未实现**************************************************************
// Get sub-unit closure limit
//	Obtains the maximum number of channel closures allowed on the sub-unit.
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		limit (out)		Closure limit
//		Return = ViStatus
//**************************************************************************
MTPR_EXPORT ViStatus MtPR_getClosureLimit(ViSession vi, ViUInt32 subUnit, ViPUInt32 limit);


//************该函数还未实现**************************************************************
// Get card diagnostic 
//	Obtains diagnostic information in string format. 
//		vi (in)			Instrument handle
//		message (out)	Diagnostic string
//		Return = ViStatus
//**************************************************************************
MTPR_EXPORT ViStatus MtPR_getDiagnostic(ViSession vi, ViPString message);


//************该函数还未实现**************************************************************
// Get sub-unit setting time
//	Obtains sub-unit settling time in microseconds.
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		time (out)		Settling time
//		Return = ViStatus
//**************************************************************************
MTPR_EXPORT ViStatus MtPR_getSettlingTime(ViSession vi, ViUInt32 subUnit, ViPUInt32 time);


//**************************************************************************
// Get sub-unit counts
//	Obtains counts of the number of input and output sub-units on the card.
//		vi (in)			Instrument handle
//		inSubs (out)	The number of input sub-units
//		outSubs (out)	The number of output sub-units
//		Return = ViStatus
//**************************************************************************
MTPR_EXPORT ViStatus MtPR_getSubCounts(ViSession vi, ViPUInt32 inSubs, ViPUInt32 outSubs);


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
MTPR_EXPORT ViStatus MtPR_getSubType(ViSession vi, ViUInt32 subUnit,
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
MTPR_EXPORT ViStatus MtPR_getSubInfo(ViSession vi, ViUInt32 subUnit,
	ViBoolean out, ViPUInt32 subType, ViPUInt32 rows, ViPUInt32 columns);


//**************************************************************************
// Get status information
//	Obtains card status as a set of bit-flags.
//		vi (in)			Instrument handle
//		status (out)	Bit-flags indicating the card's status
//		Return = ViStatus
//**************************************************************************
MTPR_EXPORT ViStatus MtPR_getCardStatus(ViSession vi, ViPUInt32 status);


//************该函数还未实现**************************************************************
// Read calibration value
//	Reads from EEPROM the calibration value associated with
//  the specified index number.
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		idx (in)		Calibration value index number
//		data (out)		Calibration value read
//		Return = ViStatus
//**************************************************************************
MTPR_EXPORT ViStatus MtPR_readCalibration(ViSession vi, ViUInt32 subUnit,
	ViUInt32 idx, ViPUInt32 data);


//************该函数还未实现**************************************************************
// Write calibration value
//	Writes to EEPROM the calibration value associated with
//  the specified index number.
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		idx (in)		Calibration value index number
//		data (in)		Calibration value to write
//		Return = ViStatus
//**************************************************************************
MTPR_EXPORT ViStatus MtPR_writeCalibration(ViSession vi, ViUInt32 subUnit,
	ViUInt32 idx, ViUInt32 data);


//************该函数还未实现**************************************************************
// Get input channel state
//	Obtains the state of an individual input channel.
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		channel (in)	Channel number
//		state (out)		The state of the specified input channel
//						(VI_OFF = logic '0', VI_ON = logic '1')
//		Return = ViStatus
//**************************************************************************
MTPR_EXPORT ViStatus MtPR_readInputState(ViSession vi, ViUInt32 subUnit,
	ViUInt32 channel, ViPBoolean state);


//************该函数还未实现**************************************************************
// Get a sub-unit's input pattern.
//  Gets the states of all of a sub-unit's inputs.
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		pattern (out)	Array to accept a bit-pattern representing
//						the sub-unit's input states
//		Return = ViStatus
//**************************************************************************
MTPR_EXPORT ViStatus MtPR_readInputPattern(ViSession vi, ViUInt32 subUnit, ViAUInt32 pattern);


//************该函数还未实现**************************************************************
// Set driver mode
//  Sets (by means of a set of bit-flags) various driver operating
//  characteristics.
//		newMode (in)		The new operating mode flags to set
//		previousMode (out)	The previous operating mode flags
//		Return = ViStatus
//**************************************************************************
MTPR_EXPORT ViStatus MtPR_setDriverMode(ViUInt32 newMode, ViPUInt32 previousMode);


//**************************************************************************
// Additional general-purpose function.
//**************************************************************************

//************该函数还未实现**************************************************************
// Get sub-unit status information as a set of bit-flags.  //获取子单元状态信息作为一组位标志。
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number   //子单元号
//		status (out)	Bit-flags indicating the sub-unit's status  //表示子单元状态的位标志
//		Return = ViStatus
//**************************************************************************
MTPR_EXPORT ViStatus MtPR_getSubStatus(ViSession vi, ViUInt32 subUnit, ViPUInt32 status);


//**************************************************************************
// Additional special-purpose switch functions.
//**************************************************************************

//************该函数还未实现**************************************************************
// Obtain the value of a sub-unit attribute  //获取子单元属性值
//  Attribute values are relevant to specialised operation of certain card
//  types using MtPR_operateSwitch.  //属性值与特定卡片的特定操作相关 使用MtPR_operateSwitch。
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		out (in)		VI_TRUE to obtain output sub-unit information
//						VI_FALSE to obtain input sub-unit information (unsupported)
//		AttrCode (in)	A numeric code indicating the attribute to be queried
//		pAttrValue (out)	The value of the selected attribute
//		Return = ViStatus
//**************************************************************************
MTPR_EXPORT ViStatus MtPR_getSubAttribute(ViSession vi, ViUInt32 subUnit,
	ViBoolean out, ViUInt32 AttrCode, ViAddr const pAttrValue);


//************该函数还未实现**************************************************************
// Sets the value of a sub-unit attribute
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		out (in)		VI_TRUE to obtain output sub-unit information
//						VI_FALSE to obtain input sub-unit information (unsupported)
//		AttrCode (in)	A numeric code indicating the attribute to be queried
//		pAttrValue (in) Pointer to value of the attribute
//		Return = ViStatus
//**************************************************************************
MTPR_EXPORT ViStatus MtPR_SetAttribute(ViSession vi, ViUInt32 subUnit,
	ViBoolean out, ViUInt32 AttrCode, ViAddr pAttrValue);


//************该函数还未实现**************************************************************
// Gets the value of a sub-unit attribute
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		out (in)		VI_TRUE to obtain output sub-unit information
//						VI_FALSE to obtain input sub-unit information (unsupported)
//		AttrCode (in)	A numeric code indicating the attribute to be queried
//		pAttrValue (out) The value of the selected attribute
//		Return = ViStatus
//**************************************************************************
MTPR_EXPORT ViStatus MtPR_GetAttribute(ViSession vi, ViUInt32 subUnit,
	ViBoolean out, ViUInt32 AttrCode, ViAddr pAttrValue);


//************该函数还未实现**************************************************************
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
MTPR_EXPORT ViStatus MtPR_operateSwitch(ViSession vi, ViUInt32 subUnit,
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
MTPR_EXPORT ViStatus MtPR_writeCalibrationFP(ViSession vi, ViUInt32 subUnit,
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
MTPR_EXPORT ViStatus MtPR_readCalibrationFP(ViSession vi, ViUInt32 subUnit,
	ViUInt32 store, ViUInt32 offset, ViUInt32 numValues, ViReal64 *dataArray);


//**************************************************************************
// Write calibration date information to EEPROM
// Date information is obtained from the current system date.
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		store (in)		value indicating which store to access (user = 0, factory = 1)
//		interval (in)	the desired calibration interval (in days)
//		Return = ViStatus
//**************************************************************************
MTPR_EXPORT ViStatus MtPR_writeCalibrationDate(ViSession vi, ViUInt32 subUnit,
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
MTPR_EXPORT ViStatus MtPR_readCalibrationDate(ViSession vi, ViUInt32 subUnit,
	ViUInt32 store, ViPUInt32 year, ViPUInt32 day, ViPUInt32 interval);


//************该函数还未实现**************************************************************
// Set a calibration point
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		idx (in)		index number of calibration point
//		Return = ViStatus
//**************************************************************************
MTPR_EXPORT ViStatus MtPR_setCalibrationPoint(ViSession vi, ViUInt32 subUnit, ViUInt32 idx);


//**************************************************************************
// Programmable resistor specific functions.
//**************************************************************************

//界面开关控制：通过设置界面的开关输出相应的电阻值.
MTPR_EXPORT ViStatus MtPR_setSwitchGetResistance(ViSession vi, ViUInt32 subUnit, int switchState[64]);


//**************************************************************************
// Set resistance as close as possible to the given value
//		vi (in)				Instrument handle
//		subUnit (in)		Sub-unit number
//		mode (in)			A value indicating how the given resistance value is to be applied
//		resistance (in)		the resistance value to set
//		Return = ViStatus
//**************************************************************************
MTPR_EXPORT ViStatus MtPR_resSetResistance(ViSession vi, ViUInt32 subUnit,
	ViUInt32 mode, const ViReal64 resistance);


//**************************************************************************
// Obtain the set resistance value
//		vi (in)				Instrument handle
//		subUnit (in)		Sub-unit number
//		resistance (out)	pointer to variable to receive the resistance value
//		Return = ViStatus
//**************************************************************************
MTPR_EXPORT ViStatus MtPR_resGetResistance(ViSession vi, ViUInt32 subUnit, ViPReal64 resistance);


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
MTPR_EXPORT ViStatus MtPR_resGetInfo(ViSession vi, ViUInt32 subUnit,
	ViPReal64 minRes, ViPReal64 maxRes, ViPReal64 refRes, ViPReal64 precPC,
	ViPReal64 precDelta, ViPReal64 int1, ViPReal64 intDelta, ViPUInt32 capabilities);


//************该函数还未实现**************************************************************
// Clean up before terminating
//  Closes any cards that remain open, freeing all resources.
//		Return = ViStatus
//**************************************************************************
MTPR_EXPORT ViStatus MtPR_cleanUp(void);


/*********************************************************/
/*         MT（MicroTest） Basis Functions                */
/*********************************************************/

//写入 SN number
MTPR_EXPORT ViStatus  MtPR_writeCardSNnumber(ViSession vi, ViUInt32 subUnit, ViUInt32 store, ViUInt32 SN);
//读取 SN-number 和 FwVersion
MTPR_EXPORT ViStatus  MtPR_readCardSNnumber(ViSession vi, ViUInt32 subUnit, ViUInt32 store, ViPUInt32 SN, ViPUInt64 FwVersion);

//在线升级
MTPR_EXPORT ViStatus  MtPR_OnlineUpdate_U45(ViSession vi, const char* path);
MTPR_EXPORT ViStatus  MtPR_OnlineUpdateMultiple_U45(ViSession vi[], ViInt32 number, const char* path, ViStatus result[]);



//程控电阻卡进行校准测试
MTPR_EXPORT ViStatus MtPR_calibrationFactor_5170B(ViSession vi, ViUInt32 subUnit, ViUInt32 channel[], ViUInt32 pattern[], int datalen);
//程控电阻卡进行校准测试
MTPR_EXPORT ViStatus MtPR_calibrationFactor_5171(ViSession vi, ViUInt32 subUnit, ViUInt64 hexValue[], int datalen);
//程控电阻卡进行校准测试
MTPR_EXPORT ViStatus MtPR_calibrationFactor_5172(ViSession vi, ViUInt32 subUnit, ViUInt64 hexValue[], int datalen);



#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif
// End of MtProgramResistor.h
#endif

