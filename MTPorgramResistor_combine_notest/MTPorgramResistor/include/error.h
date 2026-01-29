#ifndef MT_ERRNO_H_
#define MT_ERRNO_H_
//#include <errno.h>

#if EDOM > 0
# define MT_ERR(x) (-(x))
#else
# define MT_ERR(x) (x)
#endif


//**************************************************************************
// Constants Declarations
//**************************************************************************

//*********************************************
// MT5250 Error codes
//*********************************************
#define MT__SUCCESS			      (0L)
#define MT__UNKNOWN			      (-1000)
//MT5250 Error codes(-4001,-5000)
#define MT__EAI_SELF_CAL_ERR	                          (-4001)
#define MT__EAI_INV_VAL			                          (-4002)
#define MT__EAI_INV_ATTR		                          (-4003)
#define MT__EAI_INV_RANGE		                          (-4004)
#define MT__EAI_DEV_NOTFOUND	                          (-4005)
#define MT__EAI_EVENT_TIMEOUT	                          (-4006)
#define MT__EAI_INV_STATUS		                          (-4007)
#define MT__EAI_INV_OUTPUT_FUNCTION	                      (-4008)
#define MT__EAI_OPEN_FILE			                      (-4009)
#define MT__EAI_INV_SETTING_MODE	                      (-4010)
#define MT__EAI_OVER_CURRENT_PROTECTION	                  (-4011)
#define MT__EAI_OVER_VOLTAGE_PROTECTION	                  (-4012)
#define MT__EAI_DC_OVER_POWER_PROTECTION	              (-4013)
#define MT__EAI_PULSE_STEP1_OVER_POWER_PROTECTION	      (-4014)
#define MT__EAI_PULSE_STEP2_OVER_POWER_PROTECTION	      (-4015)
#define MT__EAI_HIGH_VOLTAGE_PROTECTION	                  (-4016)
#define MT__EAI_MALLOC_FAIL	                              (-4017)
#define MT__EVI_DECRYPTION_FAIL                           (-4018)
#define MT__EAI_INV_FLASH_PARAM                           (-4019)
#define MT__EAI_INV_DAC_PARM                              (-4020)
#define MT__ERROR_INV_SOURCE_DELAY_TIME                   (-4021)
#define MT__ERROR_INV_MEASUREMENT_PERIOD                  (-4022)
#define MT__ERROR_INV_LIMIT_VALUE                         (-4023)
#define MT__ERROR_CAL_PARAMS                              (-4024)
#define MT__ERROR_INV_VALUE_PROPERTY_SEQUENCE_STEP_DT     (-4025)
#define MT__ERROR_INV_OUTPUT_ENABLED_PROPERTY_SEQUENCE    (-4026)
#define MT__ERROR_INV_VALUE_PROPERTY_PULSE                (-4027)
#define MT__ERROR_INV_LEVEL_VALUE                         (-4028)
#define MT__ERROR_INV_PULSE_ON_TIME                                (-4029)
#define MT__ERROR_INV_PULSE_OFF_TIME                               (-4030)
#define MT__ERROR_EXCEED_POWER_LIMIT                               (-4033)
//VISA  Error codes(-5001,-6000)
#define MT__EVI_SYSTEM_ERROR           (-5001)
#define MT__EVI_INV_OBJECT             (-5002)
#define MT__EVI_RSRC_LOCKED            (-5003)
#define MT__EVI_INV_EXPR               (-5004)
#define MT__EVI_RSRC_NFOUND            (-5005)
#define MT__EVI_INV_RSRC_NAME          (-5006)
#define MT__EVI_INV_ACC_MODE           (-5007)
#define MT__EVI_TMO                    (-5008)
#define MT__EVI_CLOSING_FAILED         (-5009)
#define MT__EVI_INV_DEGREE             (-5010)
#define MT__EVI_INV_JOB_ID             (-5011)
#define MT__EVI_NSUP_ATTR              (-5012)
#define MT__EVI_NSUP_ATTR_STATE        (-5013)
#define MT__EVI_ATTR_READONLY          (-5014)
#define MT__EVI_INV_LOCK_TYPE          (-5015)
#define MT__EVI_INV_ACCESS_KEY         (-5016)
#define MT__EVI_INV_EVENT              (-5017)
#define MT__EVI_INV_MECH               (-5018)
#define MT__EVI_HNDLR_NINSTALLED       (-5019)
#define MT__EVI_INV_HNDLR_REF          (-5020)
#define MT__EVI_INV_CONTEXT            (-5021)
#define MT__EVI_QUEUE_OVERFLOW         (-5022)
#define MT__EVI_NENABLED               (-5023)
#define MT__EVI_ABORT                  (-5024)
#define MT__EVI_RAW_WR_PROT_VIOL       (-5025)
#define MT__EVI_RAW_RD_PROT_VIOL       (-5026)
#define MT__EVI_OUTP_PROT_VIOL         (-5027)
#define MT__EVI_INP_PROT_VIOL          (-5028)
#define MT__EVI_BERR                   (-5029)
#define MT__EVI_IN_PROGRESS            (-5030)
#define MT__EVI_INV_SETUP              (-5031)
#define MT__EVI_QUEUE_ERROR            (-5032)
#define MT__EVI_ALLOC                  (-5033)
#define MT__EVI_INV_MASK               (-5034)
#define MT__EVI_IO                     (-5035)
#define MT__EVI_INV_FMT                (-5036)
#define MT__EVI_NSUP_FMT               (-5037)
#define MT__EVI_LINE_IN_USE            (-5038)
#define MT__EVI_LINE_NRESERVED         (-5039)
#define MT__EVI_NSUP_MODE              (-5040)
#define MT__EVI_SRQ_NOCCURRED          (-5041)
#define MT__EVI_INV_SPACE              (-5042)
#define MT__EVI_INV_OFFSET             (-5043)
#define MT__EVI_INV_WIDTH              (-5044)
#define MT__EVI_NSUP_OFFSET            (-5045)
#define MT__EVI_NSUP_VAR_WIDTH         (-5046)
#define MT__EVI_WINDOW_NMAPPED         (-5047)
#define MT__EVI_RESP_PENDING           (-5048)
#define MT__EVI_NLISTENERS             (-5049)
#define MT__EVI_NCIC                   (-5050)
#define MT__EVI_NSYS_CNTLR             (-5051)
#define MT__EVI_NSUP_OPER              (-5052)
#define MT__EVI_INTR_PENDING           (-5053)
#define MT__EVI_ASRL_PARITY            (-5054)
#define MT__EVI_ASRL_FRAMING           (-5055)
#define MT__EVI_ASRL_OVERRUN           (-5056)
#define MT__EVI_TRIG_NMAPPED           (-5057)
#define MT__EVI_NSUP_ALIGN_OFFSET      (-5058)
#define MT__EVI_USER_BUF               (-5059)
#define MT__EVI_RSRC_BUSY              (-5060)
#define MT__EVI_NSUP_WIDTH             (-5061)
#define MT__EVI_INV_PARAMETER          (-5062)
#define MT__EVI_INV_PROT               (-5063)
#define MT__EVI_INV_SIZE               (-5064)
#define MT__EVI_WINDOW_MAPPED          (-5065)
#define MT__EVI_NIMPL_OPER             (-5066)
#define MT__EVI_INV_LENGTH             (-5067)
#define MT__EVI_INV_MODE               (-5068)
#define MT__EVI_SESN_NLOCKED           (-5069)
#define MT__EVI_MEM_NSHARED            (-5070)
#define MT__EVI_LIBRARY_NFOUND         (-5071)
#define MT__EVI_NSUP_INTR              (-5072)
#define MT__EVI_INV_LINE               (-5073)
#define MT__EVI_FILE_ACCESS            (-5074)
#define MT__EVI_FILE_IO                (-5075)
#define MT__EVI_NSUP_LINE              (-5076)
#define MT__EVI_NSUP_MECH              (-5077)
#define MT__EVI_INTF_NUM_NCONFIG       (-5078)
#define MT__EVI_CONN_LOST              (-5079)
#define MT__EVI_MACHINE_NAVAIL         (-5080)
#define MT__EVI_NPERMISSION            (-5081)


//*************************************************** 
// MTPorgramResistor Error codes(-3001,-4000)
//***************************************************  0xBFFC0800UL   //-1074011136
#define MTPR__SUCCESS				              (0L)
#define MTPR__ERROR_BASE				          (0xFFFFF060)   //-4000
#define MTPR__ERROR_BAD_SESSION		              (MTPR__ERROR_BASE + 0x0)     /* Not a valid MT session */                                     //在指定的会话上没有打开 MT卡
#define MTPR__ERROR_NO_INFO			              (MTPR__ERROR_BASE + 0x1)     /* Card information unobtainable - hardware problem */           //无法获取指定卡的信息
#define MTPR__ERROR_CARD_DISABLED                 (MTPR__ERROR_BASE + 0x2)     /* Card disabled - hardware problem */                           //指定的卡被禁用
#define MTPR__ERROR_BAD_SUB			              (MTPR__ERROR_BASE + 0x3)     /* Card has no sub-unit with specified number */                 //子单元值超出目标卡的范围
#define MTPR__ERROR_BAD_CHANNEL		              (MTPR__ERROR_BASE + 0x4)     /* Sub-unit has no channel with specified number */              //通道号超出目标子单元的范围
#define MTPR__ERROR_NO_CAL_DATA		              (MTPR__ERROR_BASE + 0x5)     /* Sub-unit has no calibration data to write/read */             //目标子单元没有校准数据可读/可写
#define MTPR__ERROR_BAD_ARRAY                     (MTPR__ERROR_BASE + 0x6)     /* Array type, size or shape is incorrect */                     //SafeArray类型、形状或大小不正确
#define MTPR__ERROR_MUX_ILLEGAL		              (MTPR__ERROR_BASE + 0x7)     /* Non-zero write data is illegal for MUX sub-unit */            //非零写入数据值对于MUX子单元是非法的
#define MTPR__ERROR_EXCESS_CLOSURE                (MTPR__ERROR_BASE + 0x8)     /* Sub-unit closure limit exceeded */                                  //执行将导致超出闭包限制
#define MTPR__ERROR_ILLEGAL_MASK		          (MTPR__ERROR_BASE + 0x9)     /* One or more of the specified channels cannot be masked */           //不能屏蔽一个或多个指定通道
#define MTPR__ERROR_OUTPUT_MASKED                 (MTPR__ERROR_BASE + 0xA)     /* Cannot activate an output that is masked */                         //无法激活被屏蔽的输出
#define MTPR__ERROR_FAILED_INIT		              (MTPR__ERROR_BASE + 0xB)     /* Cannot open a MT card with this resource name */                    //无法在指定位置打开 MT卡
#define MTPR__ERROR_READ_FAIL                     (MTPR__ERROR_BASE + 0xC)     /* Failed read from hardware */                                        //从硬件读取失败
#define MTPR__ERROR_WRITE_FAIL                    (MTPR__ERROR_BASE + 0xD)     /* Failed write to hardware */                                         //写入硬件失败
#define MTPR__ERROR_VISA_OP			              (MTPR__ERROR_BASE + 0xE)     /* VISA operation failed */                                            //VISA操作失败
#define MTPR__ERROR_VISA_VERSION		          (MTPR__ERROR_BASE + 0xF)     /* Incompatible VISA version */                                        //VISA版本不兼容
#define MTPR__ERROR_SUB_TYPE			          (MTPR__ERROR_BASE + 0x10)    /* Function call incompatible with sub-unit type or capabilities */    //与子单元类型不兼容
#define MTPR__ERROR_BAD_ROW			              (MTPR__ERROR_BASE + 0x11)    /* Matrix row value out of range */                                    //矩阵行值超出范围
#define MTPR__ERROR_BAD_COLUMN                    (MTPR__ERROR_BASE + 0x12)    /* Matrix column value out of range */                                 //矩阵列值超出范围
#define MTPR__ERROR_BAD_ATTEN                     (MTPR__ERROR_BASE + 0x13)    /* Attenuation value out of range */                                   //衰减值超出范围
#define MTPR__ERROR_BAD_VOLTAGE		              (MTPR__ERROR_BASE + 0x14)    /* Voltage value out of range */                                       //电压值超出范围
#define MTPR__ERROR_BAD_CAL_INDEX                 (MTPR__ERROR_BASE + 0x15)    /* Calibration reference out of range */                               //校准指数值超出范围
#define MTPR__ERROR_BAD_SEGMENT		              (MTPR__ERROR_BASE + 0x16)    /* Segment number out of range */                                      //段号超出范围
#define MTPR__ERROR_BAD_FUNC_CODE                 (MTPR__ERROR_BASE + 0x17)    /* Function code value out of range */                                 //功能代码值超出范围
#define MTPR__ERROR_BAD_SUBSWITCH                 (MTPR__ERROR_BASE + 0x18)    /* Subswitch value out of range */                                     //子开关值超出范围
#define MTPR__ERROR_BAD_ACTION                    (MTPR__ERROR_BASE + 0x19)    /* Action code out of range */                                         //操作代码超出范围
#define MTPR__ERROR_STATE_CORRUPT                 (MTPR__ERROR_BASE + 0x1A)    /* Cannot execute due to corrupt sub-unit state */                     //由于子单元状态损坏，无法执行
#define MTPR__ERROR_BAD_ATTR_CODE                 (MTPR__ERROR_BASE + 0x1B)    /* Unrecognised attribute code */                                      //无法识别的属性代码
#define MTPR__ERROR_EEPROM_WRITE_TMO	          (MTPR__ERROR_BASE + 0x1C)    /* Timeout writing to EEPROM */                                        //写入 EEPROM 超时
#define MTPR__ERROR_ILLEGAL_OP                    (MTPR__ERROR_BASE + 0x1D)    /* Operation is illegal in the sub-unit's current state */             //在子单元当前状态下，操作是非法的
#define MTPR__ERROR_BAD_POT			              (MTPR__ERROR_BASE + 0x1E)    /* Unrecognised pot number requested */                                //请求的 pot 号无法识别
#define MTPR__ERROR_MATRIXR_ILLEGAL	              (MTPR__ERROR_BASE + 0x1F)    /* Invalid write pattern for MATRIXR/MATRIXS sub-unit */               //MATRIXR 子单元的写入模式无效
#define MTPR__ERROR_MISSING_CHANNEL	              (MTPR__ERROR_BASE + 0x20)    /* Attempted operation on non-existent channel */                      //尝试在不存在的通道上进行操作
#define MTPR__ERROR_CARD_INACCESSIBLE             (MTPR__ERROR_BASE + 0x21)    /* Card cannot be accessed (failed/removed/unpowered) */               //卡无法访问（故障/已移除/未通电）
#define MTPR__ERROR_BAD_FP_FORMAT                 (MTPR__ERROR_BASE + 0x22)    /* Unsupported internal floating-point format (internal error) */      //不支持的内部浮点格式（内部错误）
#define MTPR__ERROR_UNCALIBRATED		          (MTPR__ERROR_BASE + 0x23)    /* Sub-unit is not calibrated */                                       //子单元未校准
#define MTPR__ERROR_BAD_RESISTANCE                (MTPR__ERROR_BASE + 0x24)    /* Unobtainable resistance value */                                    //无法获得的电阻值
#define MTPR__ERROR_BAD_STORE                     (MTPR__ERROR_BASE + 0x25)    /* Invalid calibration store number */                                 //校准存储编号无效
#define MTPR__ERROR_BAD_MODE			          (MTPR__ERROR_BASE + 0x26)    /* Invalid mode value */                                               //无效的模式值
#define MTPR__ERROR_SETTINGS_CONFLICT             (MTPR__ERROR_BASE + 0x27)    /* Conflicting device settings */                                      //调试设备设置
#define MTPR__ERROR_CARD_TYPE                     (MTPR__ERROR_BASE + 0x28)    /* Function call incompatible with card type or capabilities */        //函数调用与卡类型或功能不兼容
#define MTPR__ERROR_BAD_POLE                      (MTPR__ERROR_BASE + 0x29)    /* Switch pole value out of range */                                   //开关极值超出范围
#define MTPR__ERROR_MISSING_CAPABILITY            (MTPR__ERROR_BASE + 0x2A)    /* Attempted to activate a non-existent capability */                  //尝试激活不存在的功能
#define MTPR__ERROR_MISSING_HARDWARE	          (MTPR__ERROR_BASE + 0x2B)    /* Action requires hardware that is not present */                     //操作需要不存在的硬件
#define MTPR__ERROR_HARDWARE_FAULT		          (MTPR__ERROR_BASE + 0x2C)    /* Faulty hardware */                                                  //有故障的硬件
#define MTPR__ERROR_EXECUTION_FAIL		          (MTPR__ERROR_BASE + 0x2D)    /* Failed to execute (e.g. blocked by a hardware condition) */         //无法执行（例如被硬件条件阻塞）
#define MTPR__ERROR_BAD_CURRENT		              (MTPR__ERROR_BASE + 0x2E)    /* Current value out of range */                                       //电流值超出范围
#define MTPR__ERROR_BAD_RANGE			          (MTPR__ERROR_BASE + 0x2F)    /* Invalid range value */                                              //非法范围值
#define MTPR__ERROR_ATTR_UNSUPPORTED	          (MTPR__ERROR_BASE + 0x30)    /* Attribute not supported */                                          //不支持属性
#define MTPR__ERROR_BAD_REGISTER		          (MTPR__ERROR_BASE + 0x31)    /* Register number out of range */                                     //寄存器编号超出范围
#define MTPR__ERROR_MATRIXP_ILLEGAL	              (MTPR__ERROR_BASE + 0x32)    /* Invalid channel closure or write pattern for MATRIXP sub-unit */    //MATRIXP 子单元的通道关闭或写入模式无效
#define MTPR__ERROR_BUFFER_UNDERSIZE	          (MTPR__ERROR_BASE + 0x33)    /* Data buffer too small */                                            //数据缓冲区太小
#define MTPR__ERROR_ACCESS_MODE		              (MTPR__ERROR_BASE + 0x34)    /* Inconsistent shared access mode */                                  //共享访问模式不一致
#define MTPR__ERROR_POOR_RESISTANCE	              (MTPR__ERROR_BASE + 0x35)    /* Resistance outside limits */                                        //阻力超出限制
#define MTPR__ERROR_BAD_ATTR_VALUE 	              (MTPR__ERROR_BASE + 0x36)    /* Bad attribute value */                                              //错误的属性值
#define MTPR__ERROR_INVALID_POINTER 	          (MTPR__ERROR_BASE + 0x37)    /* Invalid pointer */                                                  //无效指针
#define MTPR__ERROR_ATTR_READ_ONLY 	              (MTPR__ERROR_BASE + 0x38)    /* Attribute is read only */                                           //属性为只读
#define MTPR__ERROR_ATTR_DISABLED 		          (MTPR__ERROR_BASE + 0x39)    /* Attribute is disabled */                                            //禁用属性
#define MTPR__ERROR_PSU_MAIN_DISABLED	          (MTPR__ERROR_BASE + 0x3A)    /* Main output is disabled, cannot enable the channel */               //主输出被禁用，无法启用通道
#define MTPR__ERROR_OUT_OF_MEMORY_HEAP	          (MTPR__ERROR_BASE + 0x3B)    /* Unable to allocate memory on Heap*/                                 //无法在堆上分配内存
#define MTPR__ERROR_INVALID_PROCESSID             (MTPR__ERROR_BASE + 0x3C)    /* Invalid ProcessID */                                                //无效的ProcessID 
#define MTPR__ERROR_SHARED_MEMORY                 (MTPR__ERROR_BASE + 0x3D)    /* Shared memory error */                                              //共享内存错误
#define MTPR__ERROR_CARD_OPENED_OTHER_PROCESS	  (MTPR__ERROR_BASE + 0x3E)    /* Card is opened by a process in exclusive mode */                    //卡被进程独占模式打开
#define MTPR__ERROR_DIO_PORT_DISABLED	          (MTPR__ERROR_BASE + 0x3F)    /* DIO card PORT is disabled due to Over-Current Scenario */           //过流场景导致DIO卡端口关闭
#define MTPR__ERROR_DIO_INVALID_FILE	          (MTPR__ERROR_BASE + 0x40)    /* DIO pattern file is invalid */                                      //DIO模式文件无效
#define MTPR__ERROR_DIO_DYNAMIC_ACTIVE            (MTPR__ERROR_BASE + 0x41)    /* DIO Dynamic operation is active, action not permissible */          //DIO动态操作激活，不允许操作
#define MTPR__ERROR_DIO_FILE_ENTRY_ERR	          (MTPR__ERROR_BASE + 0x42)    /* DIO File Entry has error, check the file entries.*/                 //DIO文件项有错误，请检查文件项
#define MTPR__ERROR_HW_INT_NOT_SUPPORTED          (MTPR__ERROR_BASE + 0x44)    /* Hardware Interlock feature not supported for the card */                     //该卡不支持硬件联锁功能
#define MTPR__ERROR_HW_INT_ERROR                  (MTPR__ERROR_BASE + 0x45)    /* Hardware Interlock is not detected on the card, cannot use the function */   //卡上没有检测到硬件联锁，无法使用该功能
#define MTPR__ERROR_SMS_CH_ENABLE_ERR             (MTPR__ERROR_BASE + 0x46)    /* Channel is disabled, so Set-Measure-Set cannot be used */                    //通道被禁用，所以Set-Measure-Set不能使用
#define MTPR__ERROR_MEAS_CURRENT                  (MTPR__ERROR_BASE + 0x47)    /* Measured Current out of range */                                             //测量电流超出量程
#define MTPR__ERROR_BAD_TEMPERATURE               (MTPR__ERROR_BASE + 0x48)    /* Temperature value out of range */                                            //温度值超出范围
#define MTPR__ERROR_CMP_ENGINE_ON		          (MTPR__ERROR_BASE + 0x49)    /* Capture Engine running - operation unobtainable */                           //捕获引擎运行-操作不可用
#define MTPR__ERROR_DDS_ON						  (MTPR__ERROR_BASE + 0x4A)    /* DDS Engine running - operation unobtainable   */
#define MTPR__ERROR_WAVEFORM_UNSUPPORTED		  (MTPR__ERROR_BASE + 0x4B)    /* Waveform unsupported   */
#define MTPR__ERROR_BAD_WAVEFORM				  (MTPR__ERROR_BASE + 0x4C)    /* Operation unavailable for selected waveform */
#define MTPR__ERROR_FILE_UNAVAILABLE			  (MTPR__ERROR_BASE + 0x4D)    /* File unavailable or does not exist */
#define MTPR__ERROR_FILE						  (MTPR__ERROR_BASE + 0x4E)    /* File content invalid */
#define MTPR__ERROR_AMPLITUDE					  (MTPR__ERROR_BASE + 0x4F)    /* Amplitude value out of range */
#define MTPR__ERROR_WRITEPATTERN_ILLEGAL          (MTPR__ERROR_BASE + 0x50)    /* Invalid write pattern */              //无效的写模式
#define MTPR__ERROR_DC_OFFSET					  (MTPR__ERROR_BASE + 0x51)    /* DC Offset value out of range */
#define MTPR__ERROR_FREQUENCY					  (MTPR__ERROR_BASE + 0x52)    /* Frequency value out of range */
#define MTPR__ERROR_PHASE						  (MTPR__ERROR_BASE + 0x53)    /* Phase value out of range */
#define MTPR__ERROR_DUTY_CYCLE					  (MTPR__ERROR_BASE + 0x54)    /* Duty Cycle value out of range */
#define MTPR__ERROR_PULSE_WIDTH				      (MTPR__ERROR_BASE + 0x55)    /* Pulse Width value out of range */
#define MTPR__ERROR_INVALID_COMP_FLAG		      (MTPR__ERROR_BASE + 0x56)    /* Invalid flag for setting compatibility logic*/
#define MTPR__ERROR_INV_PARAMETER                           (MTPR__ERROR_BASE + 0x57)    /* Invalid parameter value.*/

#define MTPR__ERROR_UNKNOWN                       (-1000L)   /* Unknown error code */      //未指定的错误

// Card status bits for MtPR__getCardStatus and MtPR__getSubStatus results  
// 函数 MtPR__getCardStatus 和 MtPR__getSubStatus 结果的卡状态位
#define MTPR__STAT_NO_CARD                     0x80000000UL    /* No MT card open on the session */                                  //在会话中没有MT卡打开      
#define MTPR__STAT_WRONG_DRIVER                0x40000000UL    /* Card requires later driver version */                              //卡需要更高版本的驱动
#define MTPR__STAT_EEPROM_ERR                  0x20000000UL    /* Error interpreting card EEPROM data */                             //解释卡EEPROM数据错误
#define MTPR__STAT_DISABLED                    0x10000000UL    /* Card is disabled */                                                //卡被禁用
#define MTPR__STAT_NO_SUB                      0x08000000UL    /* Card has no sub-unit with specified number */                      //卡片没有指定编号的子单元
#define MTPR__STAT_BUSY                        0x04000000UL    /* Busy (card or sub-unit) */                                         //忙（卡或子单元）
#define MTPR__STAT_HW_FAULT                    0x02000000UL    /* Hardware fault (eg. defective serial loop) */                      //硬件故障(例如：串行回路有缺陷)
#define MTPR__STAT_PARITY_ERROR                0x01000000UL    /* PCIbus parity error */                                             // PCIbus奇偶校验错误
#define MTPR__STAT_PSU_INHIBITED               0x00800000UL    /* PSU sub-unit - supply is disabled (by software) */                 //电源子单元电源被禁用（由软件）
#define MTPR__STAT_PSU_SHUTDOWN                0x00400000UL    /* PSU sub-unit - supply is shutdown (due to overload) */             // PSU子单元-电源关闭（由于过载）
#define MTPR__STAT_PSU_CURRENT_LIMIT           0x00200000UL    /* PSU sub-unit - supply is operating in current-limited mode */      // PSU子单元电源工作在限流模式
#define MTPR__STAT_CORRUPTED                   0x00100000UL    /* Sub-unit logical state is corrupted */                             //子单元逻辑状态损坏
#define MTPR__STAT_CARD_INACCESSIBLE           0x00080000UL    /* Card cannot be accessed (failed/removed/unpowered) */              //卡无法访问（失败/移除/未通电）
#define MTPR__STAT_UNCALIBRATED                0x00040000UL    /* Calibration data is faulty (card or sub-unit) */                   //校准数据故障（卡或子单元）
#define MTPR__STAT_CALIBRATION_DUE             0x00020000UL    /* Calibration is due (card or sub-unit) */                           //校准（卡或子单元）
#define MTPR__STAT_BIRST_ENABLED               0x00010000UL    /* BIRST is active (card or sub-unit) */                              // BIRST激活（卡或子单元）
#define MTPR__STAT_LID_CHECKSUM_FAIL           0x00008000UL    /* Checksum Verification for Legacy-ID feature failed*/               // Legacy-ID 功能的校验和验证失败
#define MTPR__STAT_OK                          0x00000000UL    //零值（MtPR__STAT_OK）                                                 //零值表示该卡功能正常且输出稳定。

// Driver operating mode control bits for pipx40_setDriverMode
#define MTPR__MODE_DEFAULT		0x00000000UL	/* Default, no special modes set */
#define MTPR__MODE_NO_WAIT		0x00000001UL	/* Function calls exclude output settling delay */
#define MTPR__MODE_UNLIMITED	0x00000002UL	/* Disable maximium closure limits */
#define MTPR__MODE_IGNORE_TEST	0x00000008UL	/* Enable card operation even if selftest fails */

// OUTPUT sub-unit type specifier codes (as obtained by MtPR__getSubInfo)
#define MTPR__TYPE_SW		1	/* Uncommitted switches */
#define MTPR__TYPE_MUX		2	/* Relay multiplexer (single-channel only) */
#define MTPR__TYPE_MUXM     3	/* Relay multiplexer (multi-channel capable) */
#define MTPR__TYPE_MAT		4	/* Standard matrix */
#define MTPR__TYPE_MATR     5	/* RF matrix */
#define MTPR__TYPE_DIG		6	/* Digital outputs */
#define MTPR__TYPE_RES		7	/* Programmable Resistor */
#define MTPR__TYPE_ATTEN	8	/* Programmable Attenuator */
#define MTPR__TYPE_PSUDC	9	/* Power supply - DC */
#define MTPR__TYPE_BATT     10	/* Battery simulator */
#define MTPR__TYPE_VSOURCE	11	/* Programmable voltage source */
#define MTPR__TYPE_MATP     12	/* Matrix with restricted operating modes */
#define MTPR__TYPE_MUXMS	13	/* Relay multiplexer (MUXM hardware emulated as MUX) */
#define MTPR__TYPE_FI		14	/* Fault insertion sub-unit */
#define MTPR__TYPE_DM		15	/* Displacement Module simulator */
#define MTPR__TYPE_PSOURCE  16	/* Power Source module */
#define MTPR__TYPE_DIO		17	/* DIO card with static and dynamic functionality */
#define MTPR__TYPE_MATS  	18	/* Matrix with self aligning capability */
#define MTPR__TYPE_DAC		19	/* DAC Subunit */

// Power supply capability flag bits for MtPR_psuInfo()
#define MTPR__PSU_CAP_OUTPUT_CONTROL		0x00000001UL	/* Has output on/off control */
#define MTPR__PSU_CAP_OUTPUT_SENSE			0x00000002UL	/* Has logic-level sensing of output active state */
#define MTPR__PSU_CAP_PROG_VOLTAGE			0x00000004UL	/* Output voltage is programmable */
#define MTPR__PSU_CAP_PROG_CURRENT			0x00000008UL	/* Output current is programmable */
#define MTPR__PSU_CAP_CURRENT_MODE_SENSE	0x00000010UL	/* Can sense if operating in current-limited mode */

// Note: there is only a single type of INPUT sub-unit; MtPR_getSubInfo always
// returns type code 1 for INPUT sub-units.

// Maximum lengths of string results
#define MTPR__MAX_DIAG_LENGTH           1024	/* pipx40_diagnostic() - PI specification */
#define MTPR__MAX_SELF_TEST_STR 	    256		/* pipx40_self_test() - VISA specification */
#define MTPR__MAX_DRIVER_REV_STR        256		/* pipx40_revision_query() - VISA specification */
#define MTPR__MAX_INSTR_REV_STR         256		/* pipx40_revision_query() - VISA specification */
#define MTPR__MAX_ERR_STR			    256		/* pipx40_error_message() - VISA specification */
#define MTPR__MAX_TS_DATA_DUMP_STR	    400		/* pipx40_GetAttribute( , ATTR_TS_DATA_DUMP, ) */
#define MTPR__MAX_ID_STR			    73		/* pipx40_getCardId() - PI specification, comparable to IEEE488.2 *IDN? */
#define MTPR__MAX_SUB_TYPE_STR		    80		/* pipx40_getSubType() - PI specification */
#define MTPR__MAX_PSU_TYPE_STR		    80		/* pipx40_psuGetType() - PI specification */
#define MTPR__MAX_ATTEN_TYPE_STR	    80		/* pipx40_attenGetType() - PI specification */

// Self-test fault codes
// MtPR__self_test------test_result------              //------------Description------------------
#define MTPR__FAULT_SUCCESS                     0      //Self-test passed with no errors                          //自检通过，没有错误
#define MTPR__FAULT_UNKNOWN                     1      //Unspecified fault                                        //未指明的错
#define MTPR__FAULT_WRONG_DRIVER                2      //Incompatible software driver version                     //软件驱动版本不兼容
#define MTPR__FAULT_EEPROM_ERROR                3      //EEPROM data error                                        //EEPROM数据错误
#define MTPR__FAULT_HARDWARE                    4      //Hardware defect                                          //硬件缺陷
#define MTPR__FAULT_PARITY                      5      //Parity error                                             //奇偶校验错误
#define MTPR__FAULT_CARD_INACCESSIBLE           6      //Card cannot be accessed(failed / removed / unpowered)    //卡无法访问（失败/移除/未通电）
#define MTPR__FAULT_UNCALIBRATED                7      //One or more sub - units is uncalibrated                  //一个或多个子单元未校准
#define MTPR__FAULT_CALIBRATION_DUE             8      //One or more sub - units is due for calibration           //一个或多个子单元需要校准
#define MTPR__FAULT_LID_CHECKSUM_FAIL           9      //Checksum Verification for Legacy-ID feature failed



//*********************************************
// Error codes(,)
//*********************************************



/*************************************************************************/
//					ERRNO_MAP  //ERRNO_MAP
/*************************************************************************/
#define MT_ERRNO_MAP(XX)                 \
	XX(MT__UNKNOWN, "UnKnow Error.")     \
	XX(MT__EVI_SYSTEM_ERROR, "Unknown system error (miscellaneous error).")    \
	XX(MT__EVI_INV_OBJECT, "The given session or object reference is invalid.")    \
	XX(MT__EVI_RSRC_LOCKED, "Specified type of lock cannot be obtained or specified operation cannot be performed, because the resource is locked.")    \
	XX(MT__EVI_INV_EXPR, "Invalid expression specified for search.")     \
	XX(MT__EVI_RSRC_NFOUND, "Insufficient location information or the device or resource is not present in the system.")     \
	XX(MT__EVI_INV_RSRC_NAME, "Invalid resource reference specified. Parsing error.")     \
	XX(MT__EVI_INV_ACC_MODE, "Invalid access mode.")     \
	XX(MT__EVI_TMO, "Timeout expired before operation completed.")     \
	XX(MT__EVI_CLOSING_FAILED, "Unable to deallocate the previously allocated data structures corresponding to this session or object reference.")     \
	XX(MT__EVI_INV_DEGREE, "Specified degree is invalid.")     \
	XX(MT__EVI_INV_JOB_ID, "Specified job identifier is invalid.")     \
	XX(MT__EVI_NSUP_ATTR, "The specified attribute is not defined or supported by the referenced session, event, or find list.")     \
	XX(MT__EVI_NSUP_ATTR_STATE, "The specified state of the attribute is not valid, or is not supported as defined by the session, event, or find list.")     \
	XX(MT__EVI_ATTR_READONLY, "The specified attribute is Read Only.")     \
	XX(MT__EVI_INV_LOCK_TYPE, "The specified type of lock is not supported by this resource.")     \
	XX(MT__EVI_INV_ACCESS_KEY, "The access key to the resource associated with this session is invalid.")     \
	XX(MT__EVI_INV_EVENT, "Specified event type is not supported by the resource.")     \
	XX(MT__EVI_INV_MECH, "A handler is not currently installed for the specified event.")     \
	XX(MT__EVI_HNDLR_NINSTALLED, "A handler is not currently installed for the specified event.")     \
	XX(MT__EVI_INV_HNDLR_REF, "The given handler reference is invalid.")     \
	XX(MT__EVI_INV_CONTEXT, "Specified event context is invalid.")     \
	XX(MT__EVI_QUEUE_OVERFLOW, "The event queue for the specified type has overflowed (usually due to previous events not having been closed).")     \
	XX(MT__EVI_NENABLED, "The session must be enabled for events of the specified type in order to receive them.")     \
	XX(MT__EVI_ABORT, "The operation was aborted.")     \
	XX(MT__EVI_RAW_WR_PROT_VIOL, "Violation of raw write protocol occurred during transfer.")     \
	XX(MT__EVI_RAW_RD_PROT_VIOL, "Violation of raw read protocol occurred during transfer.")     \
	XX(MT__EVI_OUTP_PROT_VIOL, "Device reported an output protocol error during transfer.")     \
	XX(MT__EVI_INP_PROT_VIOL, "Device reported an input protocol error during transfer.")     \
	XX(MT__EVI_BERR, "Bus error occurred during transfer.")     \
	XX(MT__EVI_IN_PROGRESS, "Unable to queue the asynchronous operation because there is already an operation in progress.")     \
	XX(MT__EVI_INV_SETUP, "Unable to start operation because setup is invalid (due to attributes being set to an inconsistent state).")     \
	XX(MT__EVI_QUEUE_ERROR, "Unable to queue asynchronous operation (usually due to the I/O completion event not being enabled or insufficient space in the session's queue).")     \
	XX(MT__EVI_ALLOC, "Insufficient system resources to perform necessary memory allocation.")     \
	XX(MT__EVI_INV_MASK, "Invalid buffer mask specified.")     \
	XX(MT__EVI_IO, "Could not perform operation because of I/O error.")     \
	XX(MT__EVI_INV_FMT, "A format specifier in the format string is invalid.")     \
	XX(MT__EVI_NSUP_FMT, "A format specifier in the format string is not supported.")     \
	XX(MT__EVI_LINE_IN_USE, "The specified trigger line is currently in use.")     \
	XX(MT__EVI_LINE_NRESERVED, "1")     \
	XX(MT__EVI_NSUP_MODE, "The specified mode is not supported by this VISA implementation.")     \
	XX(MT__EVI_SRQ_NOCCURRED, "Service request has not been received for the session.")     \
	XX(MT__EVI_INV_SPACE, "Invalid address space specified.")     \
	XX(MT__EVI_INV_OFFSET, "Invalid offset specified.")     \
	XX(MT__EVI_INV_WIDTH, "Invalid source or destination width specified.")     \
	XX(MT__EVI_NSUP_OFFSET, "Specified offset is not accessible from this hardware.")     \
	XX(MT__EVI_NSUP_VAR_WIDTH, "Cannot support source and destination widths that are different.")     \
	XX(MT__EVI_WINDOW_NMAPPED, "The specified session is not currently mapped.")     \
	XX(MT__EVI_RESP_PENDING, "A previous response is still pending, causing a multiple query error.")     \
	XX(MT__EVI_NLISTENERS, "No Listeners condition is detected (both NRFD and NDAC are deasserted).")     \
	XX(MT__EVI_NCIC, "The interface associated with this session is not currently the controller in charge.")     \
	XX(MT__EVI_NSYS_CNTLR, "The interface associated with this session is not the system controller.")     \
	XX(MT__EVI_NSUP_OPER, "The given session or object reference does not support this operation.")     \
	XX(MT__EVI_INTR_PENDING, "An interrupt is still pending from a previous call.")     \
	XX(MT__EVI_ASRL_PARITY, "A parity error occurred during transfer.")     \
	XX(MT__EVI_ASRL_FRAMING, "A framing error occurred during transfer.")     \
	XX(MT__EVI_ASRL_OVERRUN, "An overrun error occurred during transfer. A character was not read from the hardware before the next character arrived.")     \
	XX(MT__EVI_TRIG_NMAPPED, "The path from trigSrc to trigDest is not currently mapped.")     \
	XX(MT__EVI_NSUP_ALIGN_OFFSET, "The specified offset is not properly aligned for the access width of the operation.")     \
	XX(MT__EVI_USER_BUF, "A specified user buffer is not valid or cannot be accessed for the required size.")     \
	XX(MT__EVI_RSRC_BUSY, "The resource is valid, but VISA cannot currently access it.")     \
	XX(MT__EVI_NSUP_WIDTH, "Specified width is not supported by this hardware.")     \
	XX(MT__EVI_INV_PARAMETER, "The value of some parameter—which parameter is not known—is invalid.")     \
	XX(MT__EVI_INV_PROT, "The protocol specified is invalid.")     \
	XX(MT__EVI_INV_SIZE, "Invalid size of window specified.")     \
	XX(MT__EVI_WINDOW_MAPPED, "The specified session currently contains a mapped window.")     \
	XX(MT__EVI_NIMPL_OPER, "The given operation is not implemented.")     \
	XX(MT__EVI_INV_LENGTH, "Invalid length specified.")     \
	XX(MT__EVI_INV_MODE, "The specified mode is invalid.")     \
	XX(MT__EVI_SESN_NLOCKED, "The current session did not have any lock on the resource.")     \
	XX(MT__EVI_MEM_NSHARED, "The device does not export any memory.")     \
	XX(MT__EVI_LIBRARY_NFOUND, "A code library required by VISA could not be located or loaded.")     \
	XX(MT__EVI_NSUP_INTR, "The interface cannot generate an interrupt on the requested level or with the requested statusID value.")     \
	XX(MT__EVI_INV_LINE, "The value specified by the line parameter is invalid.")     \
	XX(MT__EVI_FILE_ACCESS, "An error occurred while trying to open the specified file. Possible reasons include an invalid path or lack of access rights.")     \
	XX(MT__EVI_FILE_IO, "An error occurred while performing I/O on the specified file.")     \
	XX(MT__EVI_NSUP_LINE, "One of the specified lines (trigSrc or trigDest) is not supported by this VISA implementation, or the combination of lines is not a valid mapping.")     \
	XX(MT__EVI_NSUP_MECH, "The interface type is valid but the specified interface number is not configured.")     \
	XX(MT__EVI_INTF_NUM_NCONFIG, "The connection for the given session has been lost.")     \
	XX(MT__EVI_CONN_LOST, "The connection for the given session has been lost.")     \
	XX(MT__EVI_MACHINE_NAVAIL, "The remote machine does not exist or is not accepting any connections.")     \
	XX(MT__EVI_NPERMISSION, "Access to the resource or remote machine is denied. This is due to lack of sufficient privileges for the current user or machine.")     \
	XX(MT__EAI_SELF_CAL_ERR, "Self calibration fail.")            \
	XX(MT__EAI_INV_VAL, "Invalid value.")                         \
	XX(MT__EAI_INV_ATTR, "Invalid attribute.")                    \
	XX(MT__EAI_INV_RANGE, "Invalid range.")                       \
	XX(MT__EAI_DEV_NOTFOUND, "Device not found.")                 \
	XX(MT__EAI_EVENT_TIMEOUT, "Event Time out.")                  \
	XX(MT__EAI_INV_STATUS, "Invalid Device status.")              \
	XX(MT__EAI_INV_OUTPUT_FUNCTION, "Invalid OutputFunction.")    \
	XX(MT__EAI_OPEN_FILE, "Open File Fail.")                      \
	XX(MT__EAI_INV_SETTING_MODE, "Setting Dynamic Attribute is not Allowed In Trigger Mode.")    \
	XX(MT__EAI_OVER_CURRENT_PROTECTION, "Over Current Protection,A fault confition occurs that would cause the device to sink or source current beyond its capabilities.")\
	XX(MT__EAI_OVER_VOLTAGE_PROTECTION, "Over Voltage Protection,A fault confition occurs that would cause the device to sink or source voltage beyond its capabilities.")\
	XX(MT__EAI_DC_OVER_POWER_PROTECTION, "Over Power Protection,A fault confition occurs that would cause the device to sink or source power beyond its capabilities in DC mode.")\
	XX(MT__EAI_PULSE_STEP1_OVER_POWER_PROTECTION, "Over Power Protection1,A fault confition occurs that would cause the device to sink or source power beyond its capabilities in pulse mode.")\
	XX(MT__EAI_PULSE_STEP2_OVER_POWER_PROTECTION, "Over Power Protection2,A fault confition occurs that would cause the device to sink or source power beyond its capabilities.")\
	XX(MT__EAI_HIGH_VOLTAGE_PROTECTION, "High Voltage Protection.")    \
	XX(MT__EAI_MALLOC_FAIL, "High Voltage Protection.")    \
	XX(MT__EVI_DECRYPTION_FAIL, "The DNA ID decryption of U44 FPGA failed.")    \
	XX(MT__EAI_INV_FLASH_PARAM, "Invalid Flash Parameter.")    \
	XX(MT__EAI_INV_DAC_PARM, "Invalid dac parameters.")    \
	XX(MT__ERROR_INV_SOURCE_DELAY_TIME, "The source delay time is invalid. Ensure that the source delay time is within the permitted range.")     \
	XX(MT__ERROR_INV_MEASUREMENT_PERIOD, "The measurement period overlaps with the falling edge of the pulse. The measurement period cannot overlap with the falling edge of the pulse if Measure When is configured to Automatically After Source Complete. Ensure that Measure Record Length Is Finite is set to True. Also ensure that Source Delay, Aperture Time, and Measure Record Length are configured such that the sum of the Source Delay and the measurement period is less than the Pulse On Time.")     \
	XX(MT__ERROR_INV_LIMIT_VALUE, "The value you requested is incompatible with the selected range. Change both the value and the corresponding Range attribute/property when the session is in the Uncommitted state or set the corresponding Autorange attribute/property to On. Minimum:1% of the full range, Maximum:the full range.")     \
	XX(MT__ERROR_CAL_PARAMS, "Invalid Cal Parameter.")     \
    XX(MT__ERROR_INV_VALUE_PROPERTY_SEQUENCE_STEP_DT, "Invalid value for parameter or property. Sequence Step dt: Maximum Value:60.0, Minimum Value:12.500e-6.")     \
	XX(MT__ERROR_INV_OUTPUT_ENABLED_PROPERTY_SEQUENCE, "The requested value for Output Enabled is not supported dynamically in Sequence mode.")     \
	XX(MT__ERROR_INV_VALUE_PROPERTY_PULSE, "Configuring properties while the session is running in either the Pulse Voltage or Pulse Current function is not permitted.")     \
    XX(MT__ERROR_INV_LEVEL_VALUE, "The value you requested is incompatible with the selected range. Change both the value and the corresponding Range attribute/property when the session is in the Uncommitted state or set the corresponding Autorange attribute/property to On. Minimum:-fullRange, Maximum:fullRange.")     \
    XX(MT__ERROR_INV_PULSE_ON_TIME, "The source delay cannot be longer than the pulse on time.")     \
	XX(MT__ERROR_INV_PULSE_OFF_TIME, "The pulse bias delay cannot be longer than the pulse off time.")     \
	XX(MT__ERROR_EXCEED_POWER_LIMIT, "The channel settings configured exceed the power limit. Adjust the voltage or current such that the channel settings will be within the allowed power limits. See your device documentation for more information.")     \	





int mt_translate_sys_error(int sys_errno);
#endif /* MT_ERRNO_H_ */

