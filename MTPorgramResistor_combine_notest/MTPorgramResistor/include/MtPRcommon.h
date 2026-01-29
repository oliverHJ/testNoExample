#ifndef __MT_PR_COMMON_HEADER
#define __MT_PR_COMMON_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include <visa.h>
#include <visatype.h>
#include <vpptype.h>


//**************************************************************************
//            //Global Variable definition
//**************************************************************************
//保存IEEE754格式的数据结构体
typedef struct DATA2IEEE754{
	uint32_t float2IEEE;       //保存float类型转换后的数据
	uint32_t double2IEEEHight; //保存double类型转换后的高32bit
	uint32_t double2IEEELower; //保存double类型转换后的低32bit,注意这里一定需要是无符号类型的数据类型，如果是有符号的话，就会出现问题，因为保存的低32位数据就是无符号，并且无符号可以保存的数据大小更大。
}Data2IEEE754;


#define MAX_DEVICES 50  //最大支持50个模块
typedef enum {
	EN_BOARD_TYPE_NONE,
	EN_BOARD_TYPE_5170B_01,
	EN_BOARD_TYPE_5170B_02,
	EN_BOARD_TYPE_5171,
	EN_BOARD_TYPE_5172,
}EN_BOARD_TYPE;


#define CALIBRATION_NUM_5170B  53//49 //校准值个数
typedef struct MT5170B_calibrationValue{
	double g_calibrationValue_CH[3][53];//校准值
	double g_currentSetR_CH[3];//当前设置的电阻值
	double R17_R18;//R17_R18并联
	double R19_R20;//R19_R20并联
	//K137=0,电阻断路; K137=1,K138=0,电阻接入; K137=1,K138=1,电阻短路; 
	int g_switchState37;//继电器K137的状态.0--断路(总体断路).
	int g_switchState38;//继电器K138的状态.
	//CH1(g_pre_pattern[0]、g_pre_pattern[1]); CH2(g_pre_pattern[2]、g_pre_pattern[3]); CH3(g_pre_pattern[4]、g_pre_pattern[5]).
	ViUInt32 g_pre_pattern[6];//记录上一次开关状态.
} MT5170B_CALIBRATION_VALUE;//两通道：MT5170B-01-->0x70A、（原MT5170C）MT5170B-02-->0x70C;


#define CALIBRATION_NUM_5171  33 //校准值个数
typedef struct MT5171_calibrationValue{
	double g_calibrationValue_CH[6][CALIBRATION_NUM_5171];//校准值
	double g_currentSetR_CH[6];//当前设置的电阻值
	int g_switchState37;//K137=0,电阻断路; K137=1,电阻短路; K137=2,电阻接入; 
	ViUInt32 g_pre_pattern[6];//记录上一次开关状态.
} MT5171_CALIBRATION_VALUE;//六通道：MT5171-->0x70B;


#define CALIBRATION_NUM_5172  33 //校准值个数
typedef struct MT5172_calibrationValue{
	double g_calibrationValue_CH[12][CALIBRATION_NUM_5172];//校准值
	double g_currentSetR_CH[12];//当前设置的电阻值
	int g_switchState37;//K137=0,电阻断路; K137=1,电阻短路; K137=2,电阻接入; 
	ViUInt32 g_pre_pattern[12];//记录上一次开关状态.
} MT5172_CALIBRATION_VALUE;//12通道：MT5172-12CH-->0x70D;


/* Device information struct */
typedef struct MT_DEVICE {
	ViSession handle;//device session
	char resourceName[20];
	EN_BOARD_TYPE board_type;
	int device_status;//设备状态
	int errorCode;//错误码
	char* errMsg;//错误信息
	//厂家信息
	ViUInt32 mtManfName;//厂家信息
	//程控电阻产品型号:0x70A-->MT5170B-01; 0x70C-->MT5170B-02; 0x70B-->MT5171; 0x70D-->MT5172;
	ViUInt32 mtModelName;//产品型号
	MT5170B_CALIBRATION_VALUE MT5170B_param;
	MT5171_CALIBRATION_VALUE MT5171_param;
	MT5172_CALIBRATION_VALUE MT5172_param;
} MT_DEVICE, *PMT_DEVICE;




#define LOW_8_BITS(x)	((x) & 0xFF)
#define BITS_PER_BYTE   8
#define EPSILON  1e-8


/*************************************************************************/
//							MT ATTR
/*************************************************************************/


/***********************************************/
/*          MT Attribute Value                 */
/***********************************************/
/*-  LCR_AUTOMATIC_LEVEL_CONTROL  -*/
#define MTDCPOWER_VAL_OFF         (0L)
#define MTDCPOWER_VAL_ON          (1L)


/*************************************************************************/
//								FPGA  VALUE
/*************************************************************************/

/////////////////////////////////////////////////////////////////////////
/*************************************************************************/
//					common part  //SPI FLASH
/*************************************************************************/
//SPI FLASH
#define MTDCPOWER_VAL_ERASE_SECTOR	                    0x20 //写擦除
#define MTDCPOWER_VAL_ERASE_BLOCK                       0xD8
#define MTDCPOWER_VAL_PAGE_PROGRAM_WRITE                0x02 //写数据
#define MTDCPOWER_VAL_READ_DATA                         0x03 //读数据
											            
//CSR模块 (0x0_0000-0x0_FFFF)-版本号与状态
#define CSR_BASE                                        0x00000000
#define CSR_ATTR_USER_LINK_UP                           CSR_BASE + 0x00000000  //版本号(Bit31-bit16  Version_High_Num：1; Bit15-bit0  Version_Low_Num：0;)
#define CSR_ATTR_VERSION_NUMBER_H                       CSR_BASE + 0x00000004  //FPGA版本号-高32bit,表示年份.(例如:32'h0000_2024,表示2024年)
#define CSR_ATTR_VERSION_NUMBER_L                       CSR_BASE + 0x00000008  //FPGA版本号-低32bit,表示月份日期时间.(例如:32'h1227_1714,表示12月27号17点14分)
#define CSR_ATTR_CHASSIS_SLOT                           CSR_BASE + 0x0000000C  //机箱槽位号.
#define CSR_ATTR_FRAM_WP_HOLD                           CSR_BASE + 0x00000010  //bit0:fram_wp：保持默认即可; bit1:fram_hold：保持默认即可.

//SPI0模块 (0x1_0000-0x1_FFFF)
#define SPI0_BASE                                       0x00010000
#define SPI0_ATTR_NWRST_FIFO                            SPI0_BASE + 0x00000000  //nwRst_FIFO：复位 SPI,写任意数,产生复位信号.：1位宽
#define SPI0_ATTR_CLOCK_DIV                             SPI0_BASE + 0x00000004  //CLK_Div：SPI 分频系数.：32位宽
#define SPI0_ATTR_SEND_DATA                             SPI0_BASE + 0x00000008  //SDI_Data_Single：SPI 发送的 32bit 数据.：32位宽
#define SPI0_ATTR_START_SEND                            SPI0_BASE + 0x0000000C  //Start：写任意数，SPI 开始工作.：32位宽
#define SPI0_ATTR_RECEIVE_DATA                          SPI0_BASE + 0x00000010  //SDO_Data_Single：SPI 接收的数据.：32位宽
#define SPI0_ATTR_RELAY_OUTPUT_EN                       SPI0_BASE + 0x00000014  //OE 控制,拉低,继电器输出使能.：32位宽
#define SPI0_ATTR_SEND_DATA_OVER                        SPI0_BASE + 0x00000018  //SDO_Data_EN：0x010 数据有效标志,也是 SPI 数据发送完成标志.：32位宽
#define SPI0_ATTR_DATA_CLEAR                            SPI0_BASE + 0x0000001C  //Data_Clear：写任意数,清除 SDO_Data_EN：32位宽

//SPI1模块 (0x1_1000-0x1_1FFF)
#define SPI1_BASE                                       0x00011000
#define SPI1_ATTR_NWRST_FIFO                            SPI1_BASE + 0x00000000  //nwRst_FIFO：复位 SPI,写任意数,产生复位信号.：1位宽
#define SPI1_ATTR_CLOCK_DIV                             SPI1_BASE + 0x00000004	//CLK_Div：SPI 分频系数.：32位宽
#define SPI1_ATTR_SEND_DATA                             SPI1_BASE + 0x00000008	//SDI_Data_Single：SPI 发送的 32bit 数据.：32位宽
#define SPI1_ATTR_START_SEND                            SPI1_BASE + 0x0000000C	//Start：写任意数，SPI 开始工作.：32位宽
#define SPI1_ATTR_RECEIVE_DATA                          SPI1_BASE + 0x00000010	//SDO_Data_Single：SPI 接收的数据.：32位宽
#define SPI1_ATTR_RELAY_OUTPUT_EN                       SPI1_BASE + 0x00000014	//OE 控制,拉低,继电器输出使能.：32位宽
#define SPI1_ATTR_SEND_DATA_OVER                        SPI1_BASE + 0x00000018	//SDO_Data_EN：0x010 数据有效标志,也是 SPI 数据发送完成标志.：32位宽
#define SPI1_ATTR_DATA_CLEAR                            SPI1_BASE + 0x0000001C	//Data_Clear：写任意数,清除 SDO_Data_EN：32位宽


/*************************************************************************/
//					MT5170B-01 和 MT5170B-02  //SPI FLASH
/*************************************************************************/
//SPI0模块 (0x1_0000-0x1_FFFF) (控制 16*5 = 80 个继电器)
#define SPI0_ATTR_DATA_U9U8_CH1                         SPI0_BASE + 0x00000020  //CH1-bit31~bit0-数据：32位宽
#define SPI0_ATTR_DATA_U10_CH1                          SPI0_BASE + 0x00000024  //CH1-bit39~bit32-数据：8位宽
#define SPI0_ATTR_DATA_U10_CH2                          SPI0_BASE + 0x00000028  //CH2-bit39~bit32-数据：8位宽
#define SPI0_ATTR_DATA_U12U11_CH2                       SPI0_BASE + 0x0000002C  //CH2-bit31~bit0-数据：32位宽
#define SPI0_ATTR_DATA_READ_U9U8_CH1                    SPI0_BASE + 0x00000030  //读回CH1-bit31~bit0-数据：32位宽
#define SPI0_ATTR_DATA_READ_U10_CH1                     SPI0_BASE + 0x00000034  //读回CH1-bit39~bit32-数据：8位宽
#define SPI0_ATTR_DATA_READ_U10_CH2                     SPI0_BASE + 0x00000038  //读回CH2-bit39~bit32-数据：8位宽
#define SPI0_ATTR_DATA_READ_U12U11_CH2                  SPI0_BASE + 0x0000003C  //读回CH2-bit31~bit0-数据：32位宽	
//SPI1模块 (0x1_1000-0x1_1FFF) (控制 16*4 = 64 个继电器)
#define SPI1_ATTR_CALIBRATION_DATA_U10                  SPI1_BASE + 0x00000020	//校准数据bit5~bit0：16位宽
#define SPI1_ATTR_DATA_U11U12_CH3                       SPI1_BASE + 0x00000024	//CH3数据bit31~bit0：32位宽
#define SPI1_ATTR_DATA_U13_CH3                          SPI1_BASE + 0x00000028	//CH3数据bit39~bit31：16位宽
#define SPI1_ATTR_CALIBRATION_DATA_READ_U10             SPI1_BASE + 0x00000030	//读回校准数据bit5~bit0：16位宽
#define SPI1_ATTR_DATA_READ_U11U12_CH3                  SPI1_BASE + 0x00000034  //读回CH3数据bit31~bit0：32位宽
#define SPI1_ATTR_DATA_READ_U13_CH3                     SPI1_BASE + 0x00000038  //读回CH3数据bit39~bit31：16位宽


/*************************************************************************/
//					MT5171  //SPI FLASH
/*************************************************************************/
//SPI0 模块(0x1_0000-0x1_0FFF)(控制 16*6 = 96 个继电器，实际 90 个)
#define SPI0_ATTR_DATA_5171_CH1                          SPI0_BASE + 0x00000020  //CH1-bit29~bit0-数据：32位宽
#define SPI0_ATTR_DATA_5171_CH2                          SPI0_BASE + 0x00000024  //CH2-bit29~bit0-数据：32位宽
#define SPI0_ATTR_DATA_5171_CH3                          SPI0_BASE + 0x00000028  //CH3-bit29~bit0-数据：32位宽
#define SPI0_ATTR_DATA_READ_5171_CH1                     SPI0_BASE + 0x00000030  //读回CH1-bit29~bit0-数据：32位宽
#define SPI0_ATTR_DATA_READ_5171_CH2                     SPI0_BASE + 0x00000034  //读回CH2-bit29~bit0-数据：32位宽
#define SPI0_ATTR_DATA_READ_5171_CH3                     SPI0_BASE + 0x00000038  //读回CH3-bit29~bit0-数据：32位宽
//SPI1 模块(0x1_1000-0x1_1FFF)(控制 16*6 = 96 个继电器，实际 90 个)
#define SPI1_ATTR_DATA_5171_CH4                          SPI1_BASE + 0x00000020	//CH4-bit29~bit0-数据：32位宽
#define SPI1_ATTR_DATA_5171_CH5                          SPI1_BASE + 0x00000024	//CH5-bit29~bit0-数据：32位宽
#define SPI1_ATTR_DATA_5171_CH6                          SPI1_BASE + 0x00000028	//CH6-bit29~bit0-数据：32位宽
#define SPI1_ATTR_DATA_READ_5171_CH4                     SPI1_BASE + 0x00000030	//读回CH4-bit29~bit0-数据：32位宽
#define SPI1_ATTR_DATA_READ_5171_CH5                     SPI1_BASE + 0x00000034  //读回CH5-bit29~bit0-数据：32位宽
#define SPI1_ATTR_DATA_READ_5171_CH6                     SPI1_BASE + 0x00000038  //读回CH6-bit29~bit0-数据：32位宽


/*************************************************************************/
//					MT5172  //SPI FLASH
/*************************************************************************/
//SPI0 模块(0x1_0000-0x1_0FFF)(控制 16*12 = 192 个继电器，实际 180 个)
#define SPI0_ATTR_DATA_5172_CH1                     SPI0_BASE + 0x00000020  //CH1-bit29~bit0-数据：32位宽
#define SPI0_ATTR_DATA_5172_CH2                     SPI0_BASE + 0x00000024  //CH2-bit29~bit0-数据：32位宽
#define SPI0_ATTR_DATA_5172_CH3                     SPI0_BASE + 0x00000028  //CH3-bit29~bit0-数据：32位宽
#define SPI0_ATTR_DATA_5172_CH4                     SPI0_BASE + 0x00000040  //CH4-bit29~bit0-数据：32位宽
#define SPI0_ATTR_DATA_5172_CH5                     SPI0_BASE + 0x00000044  //CH5-bit29~bit0-数据：32位宽
#define SPI0_ATTR_DATA_5172_CH6                     SPI0_BASE + 0x00000048  //CH6-bit29~bit0-数据：32位宽
#define SPI0_ATTR_DATA_READ_5172_CH1                SPI0_BASE + 0x00000030  //读回CH1-bit29~bit0-数据：32位宽
#define SPI0_ATTR_DATA_READ_5172_CH2                SPI0_BASE + 0x00000034  //读回CH2-bit29~bit0-数据：32位宽
#define SPI0_ATTR_DATA_READ_5172_CH3                SPI0_BASE + 0x00000038  //读回CH3-bit29~bit0-数据：32位宽
#define SPI0_ATTR_DATA_READ_5172_CH4                SPI0_BASE + 0x00000050  //读回CH4-bit29~bit0-数据：32位宽
#define SPI0_ATTR_DATA_READ_5172_CH5                SPI0_BASE + 0x00000054  //读回CH5-bit29~bit0-数据：32位宽
#define SPI0_ATTR_DATA_READ_5172_CH6                SPI0_BASE + 0x00000058  //读回CH6-bit29~bit0-数据：32位宽
//SPI1 模块(0x1_1000-0x1_1FFF)(控制 16*12 = 192 个继电器，实际 180 个)
#define SPI1_ATTR_DATA_5172_CH7                     SPI1_BASE + 0x00000020  //CH7-bit29~bit0-数据：32位宽
#define SPI1_ATTR_DATA_5172_CH8                     SPI1_BASE + 0x00000024  //CH8-bit29~bit0-数据：32位宽
#define SPI1_ATTR_DATA_5172_CH9                     SPI1_BASE + 0x00000028  //CH9-bit29~bit0-数据：32位宽
#define SPI1_ATTR_DATA_5172_CH10                    SPI1_BASE + 0x00000040  //CH10-bit29~bit0-数据：32位宽
#define SPI1_ATTR_DATA_5172_CH11                    SPI1_BASE + 0x00000044  //CH11-bit29~bit0-数据：32位宽
#define SPI1_ATTR_DATA_5172_CH12                    SPI1_BASE + 0x00000048  //CH12-bit29~bit0-数据：32位宽
#define SPI1_ATTR_DATA_READ_5172_CH7                SPI1_BASE + 0x00000030  //读回CH7-bit29~bit0-数据：32位宽
#define SPI1_ATTR_DATA_READ_5172_CH8                SPI1_BASE + 0x00000034  //读回CH8-bit29~bit0-数据：32位宽
#define SPI1_ATTR_DATA_READ_5172_CH9                SPI1_BASE + 0x00000038  //读回CH9-bit29~bit0-数据：32位宽
#define SPI1_ATTR_DATA_READ_5172_CH10               SPI1_BASE + 0x00000050  //读回CH10-bit29~bit0-数据：32位宽
#define SPI1_ATTR_DATA_READ_5172_CH11               SPI1_BASE + 0x00000054  //读回CH11-bit29~bit0-数据：32位宽
#define SPI1_ATTR_DATA_READ_5172_CH12               SPI1_BASE + 0x00000058  //读回CH12-bit29~bit0-数据：32位宽




/*************************************************************************/
//					common part  //SPI FLASH
/*************************************************************************/
//继电器次数统计模块 (0x2_0000-0x2_FFFF)
/******
该功能还没做
******/


//继电器次数修改地址空间 (0x3_0000-0x3_FFFF)
/******
该功能还没做
******/


//SPI_Flash模块 (0x5_0000-0x5_FFFF) 
#define MTDCPOWER_ATTR_SPI_BASE				        0x00050000
#define MTDCPOWER_ATTR_WRST_FIFO_FPGA		        MTDCPOWER_ATTR_SPI_BASE + 0x00000000  //wRst_FIFO：复位FIFO以及SPI功能,高有效.
#define MTDCPOWER_ATTR_SPI_CLK_TIME_FPGA	        MTDCPOWER_ATTR_SPI_BASE + 0x00000004  //SPI_CLK_Time：SPI时钟分频系数.
#define MTDCPOWER_ATTR_INSTRUCTION_LEN_FPGA	        MTDCPOWER_ATTR_SPI_BASE + 0x00000008  //Instruction_Len：指令字节长度.
#define MTDCPOWER_ATTR_ADDRESS_LEN_FPGA		        MTDCPOWER_ATTR_SPI_BASE + 0x0000000C  //Address_Len：地址字节长度.
#define MTDCPOWER_ATTR_DUMMY_LEN_FPGA		        MTDCPOWER_ATTR_SPI_BASE + 0x00000010  //Dummy_Len：虚假字节长度.
#define MTDCPOWER_ATTR_DATA_LEN_FPGA		        MTDCPOWER_ATTR_SPI_BASE + 0x00000014  //Data_Len：数据长度.
#define MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA	        MTDCPOWER_ATTR_SPI_BASE + 0x00000018  //Write_Data_FIFO：写FIFO.
#define MTDCPOWER_ATTR_READ_DATA_FIFO_FPGA	        MTDCPOWER_ATTR_SPI_BASE + 0x0000001C  //Read_Data_FIFO：读FIFO.
#define MTDCPOWER_ATTR_START_SPI_FPGA		        MTDCPOWER_ATTR_SPI_BASE + 0x00000020  //Start：SPI工作开始信号.
#define MTDCPOWER_ATTR_WR_RD_FLAG_FPGA		        MTDCPOWER_ATTR_SPI_BASE + 0x00000024  //WR_RD_Flag：读写标志.
#define MTDCPOWER_ATTR_EOD_SOFT_FPGA		        MTDCPOWER_ATTR_SPI_BASE + 0x00000028  //EOS_Soft：上电数据获取软触发.


//上电读系数模块 (0x6_0000-0x6_FFFF)  Power-on reading coefficient
#define MTDCPOWER_ATTR_POWERON_READ_FACTOR		  0x00060000

//存放工厂/用户校准数据.支持 360个 8字节(64bit)数据(720个 32bit数据).
#define SPI_FLASH_FACTORY1_DATA_BASE              0x00900000  //工厂校准数据-第i个数据存放地址(0x00900000+(i-1)*8)
#define SPI_FLASH_USER0_DATA_BASE                 0x00800000  //用户校准数据-第i个数据存放地址(0x00800000+(i-1)*8)
//写校准日期、时间间隔.addr=0x00900000 或 0x00800000, 从地址【addr = (subUnit + 2) * 64 * 1024 + addr】开始;

//板卡SN号
#define SPI_FLASH_CARD_SN_NUMBER		          0x00A00000



//**************************************************************************
// Function Declarations
//**************************************************************************

typedef void* (*mt_malloc_func)(size_t size);
typedef void* (*mt_realloc_func)(void* ptr, size_t size);
typedef void* (*mt_calloc_func)(size_t count, size_t size);
typedef void(*mt_free_func)(void* ptr);
typedef struct {
	mt_malloc_func local_malloc;
	mt_realloc_func local_realloc;
	mt_calloc_func local_calloc;
	mt_free_func local_free;
} mt_allocator_t;

static mt_allocator_t mt_allocator = {
	malloc,
	realloc,
	calloc,
	free,
};
void* mt_malloc(size_t size);
char* mt_strdup(const char* s);
const char* mt_unknown_err_code(int err);
int mt_strcpy(char *dest, size_t size, const char *src);
//单位:微秒
void mt_Sleep(ViUInt64 dwMilliseconds);

//**************************************************************************
// Function Declarations
//**************************************************************************

//获取系统日期和时间
int f_getSystemTime(struct tm& local_time);
//判断文件夹是否存在并创建
int check_and_create_dir(const char* dir_path);

void safe_strcpy(char *dest, const char *src, size_t size);
void safe_strcat(char *dest, const char *src, size_t size);

//============================================================
//函数名称：find_first_less
//函数功能：找到最后一个或第一个小于target的元素的位置.
//变   量：array--输入数组; length--数组长度;
//        direct--1数组升序排列,0数组降序排列;
//        target--目标值.
//
//返 回 值：0-降序,数组中第一个小于target的元素在数组中的位置.
//        1-升序,数组中最后一个小于target的元素在数组中的位置.
//============================================================
//int find_first_less(int array[], int length, int direct, int target)
int find_first_less(double array[], int low, int high, int direct, double target);

//二分法查找
int binary_search(double array[], int left, int right, double target);

//找出与目标数,误差最小的数.
int findClosest(double A[], int len, double b);

bool hasOne(int arr[], int size);

//两数据交换
void swap(double &a, double &b);
//冒泡排序,降序排列. flag=1降序排列, flag=0升序排列.
void bubble_sort(double arr[], size_t len, int flag);



int doubleZeroCompare(double value);

//uint64_t类型整数代表的 IEEE 754 双精度数的位表示。
//函数通过指针类型转换将该整数转换为 double 类型。
//============================================================
//函数名称：ieee754_to_double
//函数功能：函数通过指针类型转换将该整数转换为 double 类型。
//变   量：uint64_t类型整数代表的 IEEE 754 双精度数的位表示。
//      
//返 回 值：转换后的 double 类型.
//============================================================
double ieee754_to_double(uint64_t ieee754);

/** //将一个浮点数转为IEEE754格式的定点数,高32位表示整数部分,低32位表示小数部分.
* @双精度浮点数转为IEEE754格式的数据.
* @brief 将double数据转为IEEE754格式的数据
* @param converData IEEE754数据结构体，传出参数
* @param data 传入的数据
*/
void doubledata2IEEE754(Data2IEEE754* converData, double data);

/**
* @brief 将double数据转为IEEE754格式的数据
* @param binary：double数据的二进制形式; dst：double数据的十六进制形式.
* @param data 传入的数据
*/
void doubledata2Hex(int *binary, double data, ViUInt8 *dst);
/**
 * @brief 将两个32位无符号整数的字节顺序反转后合并成一个64位整数
 *
 * 此函数首先将输入的两个32位整数a1和b1的字节顺序进行反转，
 * 然后将反转后的a1作为高32位，反转后的b1作为低32位，
 * 合并成一个64位的无符号整数返回。
 *
 * @param a1 第一个32位无符号整数
 * @param b1 第二个32位无符号整数
 * @return 合并后的64位无符号整数，其中高32位是a1字节反转后的结果，低32位是b1字节反转后的结果
 */
uint64_t merge_reverse_bytes(uint32_t a1, uint32_t b1);


//将二进制数组转换为十六进制字符串（高位在前）
char* binaryArrayToHex(int array[], int length);

//将二进制数组转换为十六进制字符串（低位在前）
char* binaryToHex(int* array, int length);

// 函数：将单个十六进制字符转换为对应的二进制字符串
char* hexCharToBinary(char hexChar);//OK

// 函数：将十进制数转换为二进制字符串
void decimalToBinary(int decimal, char* binary);//OK

//函数：将十进制数转换为二进制整型数组.
void decimalToBinary2(int num, int binary[]);//OK

//函数：将64位无符号长整型数转换为64位二进制整型数组.
void convert_to_binary_array(uint64_t num, int binary_array[64]);


//限制数值在 [min, max] 之间，并四舍五入到小数点后3位.
double clamp_and_round(double value, double min, double max);


//写校准数据.多次写数据,只用首次写数据时,擦除一次（cnt=1时,起作用）.
ViStatus f_FlashWriteData(MT_DEVICE* device, int datasize, ViUInt32 addr, ViUInt32 dataArray[], int cnt);
ViStatus FlashWriteEnable(MT_DEVICE* device, bool enable);
ViStatus FlashWriteErase(MT_DEVICE* device, ViInt32 addr);
ViStatus FlashWriteData(MT_DEVICE* device, int datasize, ViInt32 addr, ViUInt32 dataArray[]);
//读取数据.
ViStatus FlashReadData(MT_DEVICE* device, int datasize, ViInt32 addr, ViUInt32* dataArray);
//批量读取数据.
ViStatus FlashReadData_BatchFetch(MT_DEVICE* device, int datasize, ViInt32 addr, ViUInt32* dataArray);

ViStatus FlashEraseU45(MT_DEVICE* device);
ViStatus FlashWriteEnableU45(MT_DEVICE* device, bool enable);
ViStatus FlashWriteEraseU45(MT_DEVICE* device, ViUInt32 addr);
ViStatus FlashWriteDataU45(MT_DEVICE* device, int datasize, ViUInt32 addr, ViUInt32 dataArray[]);




#endif

