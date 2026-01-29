#include "MtPRcore.h"
#include "MtPRcommon.h"
#include "error.h"
#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#pragma warning(disable:4996)


//#define Debug_ReasoningProcess  //调试用
//#define Debug_calswitchState_CSV  //调试用
//#define Debug_readORwriteCaliFP_TXT  //调试用



//**************************************************************************
//           common part   //Global Variable definition
//**************************************************************************
static MT_DEVICE* devices[MAX_DEVICES] = { NULL };
static int deviceCount = 0;
int deviceInitCalFlag = 0;


//**************************************************************************
// Basic Function 
//**************************************************************************
/******************************************/
/*          Basics Function               */
/******************************************/
MT_DEVICE* findDevice(ViSession handle)
{
	for (int i = 0; i < MAX_DEVICES; i++) {
		if (devices[i] && devices[i]->handle == handle) {
			return devices[i];
		}
	}
	return NULL;
}

/*
* 功   能：计算两电阻并联后的电阻值
* 输入参数：r1：电阻1
*         r2：电阻2
* 返回值：并联后的电阻值
*/
double calParallelResistance(double r1, double r2)
{
	if (0 == r1 && 0 == r2)
	{
		return 0;
	}
	return ((r1 * r2) / (r1 + r2));
}

//逻辑5计算
int calculate_circuit5_switchState(ViSession vi, ViUInt32 subUnit, double &input_tempR,
	double shortValue, int switchState[64], double tempResis[64], double calibrationValue[53])
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;
	}

	//根据属性判断板卡类型
	ViUInt32 iManf = 0, iModel = 0;
	//厂家信息：MT-->0x89E
	status = viGetAttribute(vi, VI_ATTR_MANF_ID, &iManf);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//产品型号：5170B-->0x70A; 5170C-->0x70C
	status = viGetAttribute(vi, VI_ATTR_MODEL_CODE, &iModel);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	////逻辑5计算
	double midValue = 0;
	int i = 0, istart = 0, iend = 0;
	int index = 0;
	istart = 5, iend = 14;
	if (input_tempR >= calibrationValue[iend])
	{
		//printf(" input_tempR5 = %f.\n", input_tempR);
		//逻辑5大电阻接入电路,必须是：K127断开（0）,K128闭合（1）.
		switchState[26] = 0;//K127断开（0）.
		switchState[27] = 1;//K128闭合（1）.

		if (0x89E == iManf)
		{
			if (0x70A == iModel)//MT5170B-01
			{
				if (1 == subUnit)
				{
					tempResis[50] = 0.213;
				}
				else if (2 == subUnit)
				{
					tempResis[50] = 0.213;
				}
				else if (3 == subUnit)
				{
					tempResis[50] = 0.217;
				}
				else
				{
					tempResis[50] = 0.213;
				}
				input_tempR -= tempResis[50];
			}
			else if (0x70C == iModel)//MT5170C：MT5170B-02
			{
				if (1 == subUnit)
				{
					tempResis[50] = 0.213;
				}
				else if (2 == subUnit)
				{
					tempResis[50] = 0.213;
				}
				else if (3 == subUnit)
				{
					tempResis[50] = 0.217;
				}
				else
				{
					tempResis[50] = 0.213;
				}
				input_tempR -= tempResis[50];
			}
		}
#ifdef Debug_ReasoningProcess
		printf("circuit5_大电阻接入电路：input_tempR = %f,,,tempResis[50] = %f\n", input_tempR, tempResis[50]);
#endif

		if ((input_tempR) > shortValue)//
		{
			index = find_first_less(calibrationValue, istart, iend, 0, input_tempR);
			if ((input_tempR - calibrationValue[index]) < (fabs(calibrationValue[14] - shortValue)))//g_calibrationValue[3])//
			{
				index = index + 1;
			}
			istart = index;
			for (i = istart; i <= iend; i++)
			{//K119~K126
				midValue = input_tempR - calibrationValue[i];
				if (midValue >= (fabs(calibrationValue[14] - shortValue)))//5.0)// > 0)
				{
					tempResis[i] = calibrationValue[i];
					input_tempR -= calibrationValue[i];
					if (5 == i || 6 == i)
					{
						switchState[44 - i] = 0;
					}
					else
					{
						switchState[32 - i] = 0;
					}
#ifdef Debug_ReasoningProcess
					printf("     tempResis[%d] = %f\n", i, tempResis[i]);
					printf(" input_tempR5 = %f.\n", input_tempR);
#endif
				}
				if ((midValue) < shortValue && midValue > 0)
				{
					//printf("【 g_calibrationValue[%d] = %f\n", i, MT5170B_param->g_calibrationValue[i]);
					//printf("midValue < g_calibrationValue[3]:midValue = %lf < g_calibrationValue[3] = %lf 】\n\n", midValue, MT5170B_param->g_calibrationValue[3]);
				}
			}
		}
	}
	else //1.大电阻断开,小电阻闭合; 2.大电阻全闭合(实际不存在这种情况).
	{
		//printf(" 大电阻断开,小电阻闭合. \n");
		//printf(" input_tempR_5 = %f.\n", input_tempR);
		//逻辑5大电阻不接入电路,必须是：K127闭合（1）.
		switchState[26] = 1;//K127闭合（1）.
		//switchState[27] = 0;//K128断开（0）.

		switchState[38] = 0;//K139断开（0）.
		switchState[39] = 0;//K140断开（0）.
		for (int jj = 18; jj <= 25; jj++)
		{
			switchState[jj] = 0;
		}
		//tempResis[1] = g_calibrationValue[1];//大电阻断开,小电阻闭合.
		//input_tempR -= g_calibrationValue[1];

		if (0x89E == iManf)
		{
			if (0x70A == iModel)//MT5170B-01
			{
				if (0.0 == input_tempR)
				{
				}
				else
				{
					if (1 == subUnit)
					{
						tempResis[50] = 0.05;//大电阻断开,小电阻闭合.
					}
					else if (2 == subUnit)
					{
						tempResis[50] = 0.05;//大电阻断开,小电阻闭合.
					}
					else if (3 == subUnit)
					{
						tempResis[50] = 0.0387;//大电阻断开,小电阻闭合.
					}
					else
					{
						tempResis[50] = 0.05;//大电阻断开,小电阻闭合.
					}
					input_tempR -= tempResis[50];
				}
			}
			if (0x70C == iModel)//MT5170C：MT5170B-02
			{
				if (0.0 == input_tempR)
				{
				}
				else
				{
					if (1 == subUnit)
					{
						tempResis[50] = 1.03;//大电阻断开,小电阻闭合.
					}
					else if (2 == subUnit)
					{
						tempResis[50] = 1.03;//大电阻断开,小电阻闭合.
					}
					else if (3 == subUnit)
					{
						tempResis[50] = 1.012;//大电阻断开,小电阻闭合.
					}
					else
					{
						tempResis[50] = 1.0;//大电阻断开,小电阻闭合.
					}
					input_tempR -= tempResis[50];
				}
			}
		}
#ifdef Debug_ReasoningProcess
		printf("circuit5_大电阻断开,小电阻闭合：input_tempR = %f,,,tempResis[50] = %f\n", input_tempR, tempResis[50]);
#endif
	}
#ifdef Debug_ReasoningProcess
	printf("circuit5：input_tempR = %f,,,tempResis[50] = %f\n", input_tempR, tempResis[50]);
#endif

	return 0;
}
//逻辑4计算
int calculate_circuit4_switchState(ViSession vi, double &input_tempR,
	int switchState[64], double tempResis[64], double calibrationValue[53])
{
	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;
	}

	double midValue = 0;
	int i = 0, istart = 0, iend = 0;
	//逻辑4计算
	istart = 15, iend = 20;
	if (input_tempR >= calibrationValue[iend])
	{
		//printf(" input_tempR4 = %f.\n", input_tempR);

		for (i = istart; i <= iend; i++)
		{//K113~K118
			if (calibrationValue[i] <= input_tempR)
			{
				midValue = input_tempR - calibrationValue[i];
				input_tempR -= calibrationValue[i];
				if (i == istart)
				{
					tempResis[i] = calibrationValue[i];
				}
				else if (i != istart)
				{
					tempResis[i] = calibrationValue[i];
					switchState[33 - i] = 1;
				}
				//printf("     tempResis[%d] = %f\n", i, tempResis[i]);
				//printf(" input_tempR4 = %f.\n", input_tempR);
				break;
			}
		}
	}
	else
	{
		//printf(" input_tempR_4 = %f.\n", input_tempR);
		switchState[12] = 1;
	}
#ifdef Debug_ReasoningProcess
	printf("circuit4：input_tempR = %f,,,tempResis[%d] = %f\n", input_tempR, i, tempResis[i]);
#endif

	return 0;
}
//逻辑3计算
int calculate_circuit3_switchState(ViSession vi, double &input_tempR,
	int switchState[64], double tempResis[64], double calibrationValue[53])
{
	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;
	}

	double midValue = 0;
	int i = 0, istart = 0, iend = 0;
	//逻辑3计算
	istart = 21, iend = 27;
	if (input_tempR >= calibrationValue[iend])
	{
		for (i = istart; i <= iend; i++)
		{//K106~K112
			if (calibrationValue[i] <= input_tempR)
			{
				midValue = input_tempR - calibrationValue[i];
				input_tempR -= calibrationValue[i];
				if (i == istart)
				{
					tempResis[i] = calibrationValue[i];
				}
				else if (i != istart)
				{
					tempResis[i] = calibrationValue[i];
					switchState[33 - i] = 1;
				}
				//printf("     tempResis[%d] = %f\n", i, tempResis[i]);
				//printf(" input_tempR3 = %f.\n", input_tempR);
				break;
			}
		}
	}
	else
	{
		//printf(" input_tempR_3 = %f.\n", input_tempR);
		switchState[5] = 1;
	}
#ifdef Debug_ReasoningProcess
	printf("circuit3：input_tempR = %f,,,tempResis[%d] = %f\n", input_tempR, i, tempResis[i]);
#endif

	return 0;
}
//逻辑2计算
int calculate_circuit2_switchState(ViSession vi, double &input_tempR,
	int switchState[64], double tempResis[64], double calibrationValue[53])
{
	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;
	}

	double midValue = 0;
	int i = 0, istart = 0, iend = 0;
	//逻辑2计算
	istart = 28, iend = 32;
	if (input_tempR >= calibrationValue[iend])
	{
		//printf(" input_tempR2 = %f.\n", input_tempR);
		for (i = istart; i <= iend; i++)
		{//K101~K105
			if (calibrationValue[i] <= input_tempR)
			{
				midValue = input_tempR - calibrationValue[i];
				input_tempR -= calibrationValue[i];
				if (i == istart)
				{
					tempResis[i] = calibrationValue[i];
				}
				else if (i != istart)
				{
					tempResis[i] = calibrationValue[i];
					switchState[33 - i] = 1;
				}
				//printf("     tempResis[%d] = %f\n", i, tempResis[i]);
				//printf(" input_tempR2 = %f.\n", input_tempR);
				break;
			}
		}
	}
	else
	{
		//printf(" input_tempR_2 = %f.\n", input_tempR);
		switchState[0] = 1;
	}
#ifdef Debug_ReasoningProcess
	printf("circuit2：input_tempR = %f,,,tempResis[%d] = %f\n", input_tempR, i, tempResis[i]);
#endif

	return 0;
}
//逻辑1计算
double calculate_circuit1_switchState(ViSession vi, ViUInt32 subUnit, double &y_logic1, 
	double &input_tempR, int switchState[64], double tempResis[64], int &dstindex, double calibrationValue[53])
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;
	}

	//double R17_R18 = 0.0;//R17_R18并联
	//double R19_R20 = 0.0;//R19_R20并联
	device->MT5170B_param.R17_R18 = 0.0;//R17_R18并联
	device->MT5170B_param.R19_R20 = 0.0;//R19_R20并联
	double RK133 = 0.800118, RK134 = 1.60714;
	double RK135 = 3.195, RK136 = 6.36704;
	double RK129 = 0.050, RK130 = 0.100;
	double RK131 = 0.200, RK132 = 0.400315;
	
	//根据属性判断板卡类型
	if (0x89E == device->mtManfName)
	{
		if (0x70A == device->mtModelName)//MT5170B-01
		{//1==subUnit、2==subUnit、3==subUnit

			device->MT5170B_param.R17_R18 = 4.272727272727;//R17_R18并联
			device->MT5170B_param.R19_R20 = 1.111111111111;//R19_R20并联

			RK133 = 0.800118, RK134 = 1.60704;
			RK135 = 3.194580, RK136 = 6.36704;
			RK129 = 0.050, RK130 = 0.100;
			RK131 = 0.200, RK132 = 0.400;
		}
		else if (0x70C == device->mtModelName)//MT5170B-02
		{//1==subUnit、2==subUnit、3==subUnit

			device->MT5170B_param.R17_R18 = 56.0;//R17_R18并联
			device->MT5170B_param.R19_R20 = 10.0;//R19_R20并联

			RK133 = 8.00118, RK134 = 16.0714;
			RK135 = 31.9458, RK136 = 63.6704;
			RK129 = 0.500, RK130 = 1.000;
			RK131 = 2.000, RK132 = 4.00315;
		}
	}
	double y1_logicScale = RK129 + RK130 + RK131 + RK132;

	double x1_fineSction = 0;//x1是精调电路中要开的值.
	double y_logic2 = input_tempR;//y是电路2中开完后余下的值.
	double y1_logic = 0;//y1是电路1中要开的值.
	double y1_logic_up = 0;
	double y1_logic_dw = 0;
	//逻辑1计算 //精调,小电阻(K129~K136)默认全闭合.取精调最小值.
	double midValue = 0;
	int i = 0, istart = 0, iend = 0;
	int index = 0;
	midValue = 0;
	istart = 33, iend = 49;
	calibrationValue[49] = 0;//20250328
	//if (input_tempR <= calibrationValue[istart])
	if (input_tempR >0 && input_tempR <= calibrationValue[istart])
	{
		//printf(" input_tempR1 = %f.\n", input_tempR);

		index = find_first_less(calibrationValue, istart, iend, 0, input_tempR);
		if (index > 33)
		{
			index = index - 1;
		}
		istart = index;
		for (i = istart; i <= iend; i++)
		{//K133~K136
			//if (g_calibrationValue[i] <= input_tempR)
			{
				midValue = input_tempR - calibrationValue[i];
				input_tempR -= calibrationValue[i];
				//printf(" midValue = %f \n", midValue);
				//printf(" input_tempR1 = %f.\n", input_tempR);

				if (i == 33)
				{
					y_logic1 = RK133 + RK134 + RK135 + RK136;//y1是电路1中要开的电阻.
					//33~36全断开(全为0).
					switchState[32] = 0;
					switchState[33] = 0;
					switchState[34] = 0;
					switchState[35] = 0;
					break;
				}
				else if (i == 34)
				{
					y_logic1 = RK134 + RK135 + RK136;//y1是电路1中要开的电阻.
					switchState[32] = 1;
					switchState[33] = 0;
					switchState[34] = 0;
					switchState[35] = 0;
					break;
				}
				else if (i == 35)
				{
					y_logic1 = RK133 + RK135 + RK136;//y1是电路1中要开的电阻.
					switchState[32] = 0;
					switchState[33] = 1;
					switchState[34] = 0;
					switchState[35] = 0;
					break;
				}
				else if (i == 36)
				{
					y_logic1 = RK135 + RK136;//y1是电路1中要开的电阻.
					switchState[32] = 1;
					switchState[33] = 1;
					switchState[34] = 0;
					switchState[35] = 0;
					break;
				}
				else if (i == 37)
				{
					y_logic1 = RK133 + RK134 + RK135;//y1是电路1中要开的电阻.
					switchState[32] = 0;
					switchState[33] = 0;
					switchState[34] = 1;
					switchState[35] = 0;
					break;
				}
				else if (i == 38)
				{
					y_logic1 = RK133 + RK135;//y1是电路1中要开的电阻.
					switchState[32] = 1;
					switchState[33] = 0;
					switchState[34] = 1;
					switchState[35] = 0;
					break;
				}
				else if (i == 39)
				{
					y_logic1 = RK133 + RK134;//y1是电路1中要开的电阻.
					switchState[32] = 0;
					switchState[33] = 1;
					switchState[34] = 1;
					switchState[35] = 0;
					break;
				}
				else if (i == 40)
				{
					y_logic1 = RK136;//y1是电路1中要开的电阻.
					switchState[32] = 1;
					switchState[33] = 1;
					switchState[34] = 1;
					switchState[35] = 0;
					break;
				}
				else if (i == 41)
				{
					y_logic1 = RK133 + RK134 + RK135;//y1是电路1中要开的电阻.
					switchState[32] = 0;
					switchState[33] = 0;
					switchState[34] = 0;
					switchState[35] = 1;
					break;
				}
				else if (i == 42)
				{
					y_logic1 = RK134 + RK135;//y1是电路1中要开的电阻.
					switchState[32] = 1;
					switchState[33] = 0;
					switchState[34] = 0;
					switchState[35] = 1;
					break;
				}
				else if (i == 43)
				{
					y_logic1 = RK133 + RK135;//y1是电路1中要开的电阻.
					switchState[32] = 0;
					switchState[33] = 1;
					switchState[34] = 0;
					switchState[35] = 1;
					break;
				}
				else if (i == 44)
				{
					y_logic1 = RK135;//y1是电路1中要开的电阻.
					switchState[32] = 1;
					switchState[33] = 1;
					switchState[34] = 0;
					switchState[35] = 1;
					break;
				}
				else if (i == 45)
				{
					y_logic1 = RK133 + RK134;//y1是电路1中要开的电阻.
					switchState[32] = 0;
					switchState[33] = 0;
					switchState[34] = 1;
					switchState[35] = 1;
					break;
				}
				else if (i == 46)
				{
					y_logic1 = RK134;//y1是电路1中要开的电阻.
					switchState[32] = 1;
					switchState[33] = 0;
					switchState[34] = 1;
					switchState[35] = 1;
					break;
				}
				else if (i == 47)
				{
					y_logic1 = RK133;//y1是电路1中要开的电阻.
					switchState[32] = 0;
					switchState[33] = 1;
					switchState[34] = 1;
					switchState[35] = 1;
					break;
				}
				else if (i == 48)
				{
					y_logic1 = 0;//y1是电路1中要开的电阻.
					switchState[32] = 1;
					switchState[33] = 1;
					switchState[34] = 1;
					switchState[35] = 1;
					break;
				}
			}
		}
		dstindex = i;
		tempResis[i] = calibrationValue[i];
		//printf("  circuit1 tempResis[%d] = %f\n", i, tempResis[i]);

		//if (i == 33)
		//{
		//	printf("     【circuit2 coarse Section ERROR!!!】 \n", i, tempResis[i]);
		//}
		//if (i > 33 && i < 49)
		{
			y1_logic = calibrationValue[i];//y1是电路1中要开的值.
			y1_logic_up = calibrationValue[i];
			y1_logic_dw = calibrationValue[i + 1];
		}
		//x1是精调电路中要开的值.
		x1_fineSction = (y_logic2 - y1_logic_dw) / (y1_logic_up - y1_logic_dw) * y1_logicScale;
		//printf(" x1_fineSction = %.9lf.\n", x1_fineSction);
	}
	else //if(input_tempR > calibrationValue[istart])
	{//K133~K136默认全闭合.

		//printf("     【coarse Section ERROR!!!】 \n", i, tempResis[i]);
		//printf(" input_tempR_1 = %f.\n", input_tempR);
		//printf("\r There is no suitable value in circuit1. \n");

		return 0;
	}
#ifdef Debug_ReasoningProcess
	printf("circuit1：input_tempR = %f,,,tempResis[%d] = %f\n", input_tempR, i, tempResis[i]);
	printf(" x1_fineSction = %.9lf.\n", x1_fineSction);
#endif

	return x1_fineSction;
}
//精调计算
double calculate_finePart_switchState(ViSession vi, ViUInt32 subUnit, double x0_fineSction, 
	double &input_tempR, int switchState[64], double tempResis[64], int dstindex, double calibrationValue[53])
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;
	}

	double fineSction[16] = { 0 };//精调电路中电阻取值集合.
	double fineSction_B_123[16] = { 0.75, 0.7, 0.65, 0.6, 0.55, 0.5, 0.45, 0.4, 0.35, 0.3, 0.25, 0.2, 0.15, 0.1, 0.05, 0 };
	double fineSction_C_123[16] = { 7.50, 7.0, 6.50, 6.0, 5.50, 5.0, 4.50, 4.0, 3.50, 3.0, 2.50, 2.0, 1.50, 1.0, 0.50, 0 };
	size_t leng = sizeof(fineSction_B_123) / sizeof(fineSction_B_123[0]);

	//根据属性判断板卡类型
	if (0x89E == device->mtManfName)
	{
		if (0x70A == device->mtModelName)//MT5170B-01
		{//精调电路中电阻取值集合
			memcpy(fineSction, fineSction_B_123, leng*sizeof(double));//将src中的Size个数据复制到dst中.
			//memmove(fineSction, fineSction_B_123, leng*sizeof(double));//将src中的Size个数据复制到dst中.
		}
		else if (0x70C == device->mtModelName)//MT5170B-02
		{//精调电路中电阻取值集合
			memcpy(fineSction, fineSction_C_123, leng*sizeof(double));
		}
	}
	
	double midValue = 0;
	int istart = 0, iend = 0;
	int index = 0;
	//精调计算
	istart = 0, iend = 15;
	midValue = 0;
	//精调,小电阻(K129~K136)默认全闭合.取精调最小值.
	if (x0_fineSction <= fineSction[0])
	{//K29~K32全闭合,即switchState[28]~[31]全为1.
		//printf(" input_tempR00 = %f.\n", input_tempR);
		//index = find_first_less(fineSction, istart, iend, 0, x0_fineSction);//OK
		index = findClosest(fineSction, 16, x0_fineSction);//OK
		if (index == 0)
		{
			//K29~K32全断开,即switchState[28]~[31]全为0.
			switchState[28] = 0;
			switchState[29] = 0;
			switchState[30] = 0;
			switchState[31] = 0;
		}
		if (index == 1)
		{
			//K29闭合，30、31、32全断开.
			switchState[28] = 1;
			switchState[29] = 0;
			switchState[30] = 0;
			switchState[31] = 0;
		}
		if (index == 2)
		{
			//K30闭合，29、31、32全断开.
			switchState[28] = 0;
			switchState[29] = 1;
			switchState[30] = 0;
			switchState[31] = 0;
		}
		if (index == 3)
		{
			//K29、30闭合，31、32全断开.
			switchState[28] = 1;
			switchState[29] = 1;
			switchState[30] = 0;
			switchState[31] = 0;
		}
		if (index == 4)
		{
			//K31闭合，29、30、32全断开.
			switchState[28] = 0;
			switchState[29] = 0;
			switchState[30] = 1;
			switchState[31] = 0;
		}
		if (index == 5)
		{
			//K29、31闭合，30、32全断开.
			switchState[28] = 1;
			switchState[29] = 0;
			switchState[30] = 1;
			switchState[31] = 0;
		}
		if (index == 6)
		{
			//K30-31闭合，29、32断开.
			switchState[28] = 0;
			switchState[29] = 1;
			switchState[30] = 1;
			switchState[31] = 0;
		}
		if (index == 7)
		{
			//K29-31闭合，32断开.
			switchState[28] = 1;
			switchState[29] = 1;
			switchState[30] = 1;
			switchState[31] = 0;
		}
		if (index == 8)
		{
			//K32闭合，29-31全断开.
			switchState[28] = 0;
			switchState[29] = 0;
			switchState[30] = 0;
			switchState[31] = 1;
		}
		if (index == 9)
		{
			//K29、32闭合，30、31全断开.
			switchState[28] = 1;
			switchState[29] = 0;
			switchState[30] = 0;
			switchState[31] = 1;
		}
		if (index == 10)
		{
			//K30、32闭合，29、31全断开.
			switchState[28] = 0;
			switchState[29] = 1;
			switchState[30] = 0;
			switchState[31] = 1;
		}
		if (index == 11)
		{
			//K29、30、32闭合，31断开.
			switchState[28] = 1;
			switchState[29] = 1;
			switchState[30] = 0;
			switchState[31] = 1;
		}
		if (index == 12)
		{
			//K31、32闭合，29、30全断开.
			switchState[28] = 0;
			switchState[29] = 0;
			switchState[30] = 1;
			switchState[31] = 1;
		}
		if (index == 13)
		{
			//K29、31-32闭合，30断开.
			switchState[28] = 1;
			switchState[29] = 0;
			switchState[30] = 1;
			switchState[31] = 1;
		}
		if (index == 14)
		{
			//K30-32闭合，29断开.
			switchState[28] = 0;
			switchState[29] = 1;
			switchState[30] = 1;
			switchState[31] = 1;
		}
		if (index == 15)
		{
			//K29-32全闭合.
			switchState[28] = 1;
			switchState[29] = 1;
			switchState[30] = 1;
			switchState[31] = 1;
		}
		double tempValue = (calibrationValue[dstindex] - calibrationValue[dstindex + 1]);
		midValue = (fineSction[0] - fineSction[index]) / fineSction[0] * tempValue;
		//printf("fineSction[%d] = %f\n", index, fineSction[index]);
		//printf(" input_tempR0 = %f.\n", input_tempR - fineSction[index]);
	}
	else
	{
		return 0;
	}
	tempResis[63] = -midValue;
#ifdef Debug_ReasoningProcess
	printf("finePart：input_tempR0_0 = %f,,,tempResis[63] = %f\n", input_tempR, tempResis[63]);
#endif

	return fineSction[index];
}

//根据设置电阻大小,通过算法计算,确定开关状态.
int calculate_switchState(ViSession vi, ViUInt32 subUnit, const ViReal64 resistance,
	int switchState[], int switchSize, double calibrationValue[53])
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;//MT__EAI_DEV_NOTFOUND;//
	}

	double input_resistance = resistance;
	double tempResis[64];
	for (int j = 0; j < 64; j++)
	{
		tempResis[j] = 0;
	}
	tempResis[0] = calibrationValue[0];
	tempResis[3] = calibrationValue[3];
	//tempResis[2] = calibrationValue[2];
	//tempResis[1] = calibrationValue[1];


	//默认大电阻全闭合,即K119~K126,K139~K140,全闭合.

	//234断开最大值
	//double shortValue = calibrationValue[15] + calibrationValue[21] + calibrationValue[28];//方法一：
	double shortValue = calibrationValue[15] + calibrationValue[21] + calibrationValue[28] + calibrationValue[33];//方法二：
	//如果期望的电阻在输出范围内，则计算出需要闭合的开关位置.否则断路.
	//精调部分K129~K136,全部闭合（1）,最小电阻接入电路.
	//double fineShort = calibrationValue[3] + calibrationValue[0];
	double fineShort = calibrationValue[3] + calibrationValue[0];// + calibrationValue[2] - calibrationValue[1] + 0.03;//
	double input_tempR = input_resistance - fineShort;
	if (0.0 == input_resistance)
	{
		input_tempR = 0.0;
	}
	//电阻要接入电路，K138断开（0），K137闭合（1），K127、K128至少闭合一个；
	switchState[36] = 1;//K137闭合（1）.
	switchState[37] = 0;//K138断开（0）.

	device->MT5170B_param.g_switchState37 = 1;//继电器K137的状态.
	device->MT5170B_param.g_switchState38 = 0;//继电器K138的状态.

	//printf("\r\n calibrationValue[0] = %f,,calibrationValue[3] = %f,,\n", calibrationValue[0], calibrationValue[3]);
	////printf("\r\n calibrationValue[2] = %f,,calibrationValue[1] = %f,,\n", calibrationValue[2], calibrationValue[1]);
	//printf("shortValue = %f,,fineShort = %f \n", shortValue, fineShort);
	//printf("input_resistance = %f,,input_tempR0 = %f \n", input_resistance, input_tempR);

	//逻辑5计算
	calculate_circuit5_switchState(vi, subUnit, input_tempR, shortValue, switchState, tempResis, calibrationValue);

	//逻辑4计算
	calculate_circuit4_switchState(vi, input_tempR, switchState, tempResis, calibrationValue);

	//逻辑3计算
	calculate_circuit3_switchState(vi, input_tempR, switchState, tempResis, calibrationValue);

	//逻辑2计算
	calculate_circuit2_switchState(vi, input_tempR, switchState, tempResis, calibrationValue);

	////---------------just test---------------------
	//input_tempR = 0.15;
	////---------------just test---------------------
	double y_logic2 = input_tempR;//y是电路2中开完后余下的值.

	//逻辑1计算
	double y_logic1 = 0.0;//y1是电路1中要开的电阻.
	double x_fineSction = 0.0;//x1是精调电路中要开的值.
	int dstindex = 0;
	if (input_tempR > 0)
	{
		x_fineSction = calculate_circuit1_switchState(vi, subUnit, y_logic1, input_tempR, switchState, tempResis, dstindex, calibrationValue);
		//printf(" y_logic1 = %.9lf,,x_fineSction = %.9lf\n", y_logic1, x_fineSction);
	}

	double fineValue = 0;
	////精调计算//
	//if (x_fineSction >= g_calibrationValue[3])
	if (dstindex >= 33 && dstindex <= 48)
	{
		fineValue = calculate_finePart_switchState(vi, subUnit, x_fineSction, input_tempR, switchState, tempResis, dstindex, calibrationValue);
	}
	//printf(" fineValue = %f \n", fineValue);

	double currentSetR = 0;//当前设置的电阻值
	device->MT5170B_param.g_currentSetR_CH[subUnit - 1] = 0;
	////----------------------------------------////
	for (int j = 0; j < 64; j++)
	{
		currentSetR += tempResis[j];
	}
	device->MT5170B_param.g_currentSetR_CH[subUnit - 1] = currentSetR;

	//---------------------------------------------------//
	{
		double absolute_error = input_resistance - currentSetR;
#ifdef Debug_ReasoningProcess
		printf("input_tempR = %f,,,currentSetR = %f,,,absolute_error = %f.\r\n", input_tempR, currentSetR, absolute_error);
#endif
#ifdef Debug_calswitchState_CSV
		////save_csv(resistance, switchState, switchSize);
		FILE* fp_setR;
		char filename_setR[256];
	    ////获取系统日期和时间
		struct tm local_time;
		//结构体初始化
		local_time.tm_sec = 0;     /* seconds after the minute - [0,59] */
		local_time.tm_min = 0;     /* minutes after the hour - [0,59] */
		local_time.tm_hour = 0;    /* hours since midnight - [0,23] */
		local_time.tm_mday = 1;    /* day of the month - [1,31] */
		local_time.tm_mon = 0;     /* months since January - [0,11] */
		local_time.tm_year = 0;    /* years since 1900 */
		local_time.tm_wday = 0;    /* days since Sunday - [0,6] */
		local_time.tm_yday = 0;    /* days since January 1 - [0,365] */
		local_time.tm_isdst = 0;   /* daylight savings time flag */
		f_getSystemTime(local_time);
		printf("本地时间: %d-%02d-%02d %02d:%02d:%02d  \n\n",
			local_time.tm_year + 1900,  // 年份从1900开始计数
			local_time.tm_mon + 1,      // 月份范围0~11，需+1
			local_time.tm_mday,         // 日
			local_time.tm_hour,         // 时
			local_time.tm_min,          // 分
			local_time.tm_sec);
		const char* dir_path = "..\\..\\calibration_result";
		int rtn = check_and_create_dir(dir_path);
		if (rtn != 0)
		{
			printf("创建目录失败！\n");
			return -1;
		}
		sprintf(filename_setR, "..\\..\\calibration_result\\test_setR_switchStatus_Model0x%X_ch%d_%d%02d%02d%02d%02d%02d.csv", device->mtModelName, subUnit,
			local_time.tm_year + 1900, local_time.tm_mon + 1, local_time.tm_mday,
			local_time.tm_hour, local_time.tm_min, local_time.tm_sec);
		fp_setR = fopen(filename_setR, "w");//"a"：以“追加”方式打开文件。"w"：以“清空后写入”方式打开文件。
		if (NULL == fp_setR) {
			printf("fopen(filename) error!!!");
			return -1;
		}
		fprintf(fp_setR, "SerialNumber,status,g_calibrationValue[i],setResistance,currentSetR,absolute_error\n");//写入数据
		//fclose(fp_setR);

		//写入数据到CSV文件
		int value = 0;
		int sum = 0;
		static int cnt = 0;
		int kk = 0; int jj = 0;
		cnt++;
		for (jj = 0; jj < switchSize; jj++)
		{
			value = switchState[jj];
			if (0 == jj)
			{
				fprintf(fp_setR, "%d, %d, %.6lf, %.6lf, %.6lf, %.6lf\n",
					(jj + 1), value, tempResis[jj], resistance, currentSetR, absolute_error);//写入数据
			}
			else
			{
				fprintf(fp_setR, "%d, %d, %.6lf\n", (jj + 1), value, tempResis[jj]);//写入数据
			}
			//缓冲区管理：定期调用 fflush(fp) 刷新缓冲区,防止内存占用过高.
			//if (jj % 1000 == 0)  fflush(fp_setR);//每1000行刷新一次
		}
		fclose(fp_setR);//关闭文件
#endif
	}//---------------------------------------------------//

	return status;
}

//根据设置电阻大小,通过算法计算,确定开关状态.
int calculate_switchState_5171(ViSession vi, ViUInt32 subUnit, const ViReal64 resistance,
	int switchState[], int switchSize, double calibrationValue[53])
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;//MT__EAI_DEV_NOTFOUND;//
	}

	double midValue1 = 0;
	double midValue2 = 0;
	double input_resistance = resistance;
	double tempResis[CALIBRATION_NUM_5171 - 1];
	for (int j = 0; j < CALIBRATION_NUM_5171 - 1; j++)
	{
		tempResis[j] = 0;
	}
	device->MT5171_param.g_switchState37 = 2;//K137=0,电阻断路; K137=1,电阻短路; K137=2,电阻接入; 

	//短路最小值
	double shortValue = calibrationValue[26] + calibrationValue[27] + calibrationValue[28] + calibrationValue[29];
	double sumline1 = 0;
	for (int i = 0; i <= 8; i++)
	{
		sumline1 += calibrationValue[i];;
	}
	double sumline2 = 0;
	for (int i = 9; i <= 16; i++)
	{
		sumline2 += calibrationValue[i];;
	}
	double sumline3 = 0;
	for (int i = 17; i <= 24; i++)
	{
		sumline3 += calibrationValue[i];;
	}

	double input_tempR = input_resistance - shortValue;
	if (0.0 == input_tempR)
	{
		input_tempR = 0.0;
	}
#ifdef Debug_ReasoningProcess
	printf("\r\n-------shortValue = %6.3lf,,,input_resistance = %6.3lf-------\n", shortValue, input_tempR);
#endif

	int line1flag = 0, line2flag = 0, line3flag = 0;
	//逻辑计算
	int istart = 0, iend = 0;
	int i = 0, index = 0;
	istart = 17, iend = 24;
	//if (input_tempR >= calibrationValue[istart])
	while (input_tempR >= calibrationValue[istart] && iend >= istart)
	{
		index = find_first_less(calibrationValue, istart, iend, 1, input_tempR);

		input_tempR -= calibrationValue[index];
		tempResis[index] = calibrationValue[index];
		line3flag = 1;

		iend = index - 1;
		i = index;
		switchState[i + 3] = 0;

#ifdef Debug_ReasoningProcess
		printf("sumline3,,,input_tempR = %f,,,tempResis[%d] = %f \n", input_tempR, index, tempResis[index]);
#endif
	}
	if (0 == line3flag)
	{
		input_tempR = input_tempR + calibrationValue[29];
	}
	else
	{
		//闭合RL29
		switchState[28] = 0;
		tempResis[29] = calibrationValue[29];

#ifdef Debug_ReasoningProcess
		printf("sumline3,,,input_tempR = %f,,,tempResis[29] = %f \n", input_tempR, tempResis[29]);
#endif
	}

	istart = 9, iend = 16;
	while (input_tempR >= calibrationValue[istart] && iend >= istart)
	{
		index = find_first_less(calibrationValue, istart, iend, 1, input_tempR);
		//if (iend >= index)
		//{
		//	midValue1 = fabs(input_tempR - calibrationValue[index]);
		//	midValue2 = fabs(input_tempR - calibrationValue[index + 1]);
		//	if (0.012 > midValue2 && (index + 1) <= iend)
		//	{
		//		index = index + 1;
		//	}
		//}

		input_tempR -= calibrationValue[index];
		tempResis[index] = calibrationValue[index];
		line2flag = 1;

		iend = index - 1;
		i = index;
		switchState[i + 1] = 0;

#ifdef Debug_ReasoningProcess
		printf("sumline2,,,input_tempR = %f,,,tempResis[%d] = %f \n", input_tempR, index, tempResis[index]);
#endif
	}
	if (0 == line2flag)
	{
		input_tempR = input_tempR + calibrationValue[28];
	}
	else
	{
		//闭合RL19
		switchState[18] = 0;
		tempResis[28] = calibrationValue[28];

#ifdef Debug_ReasoningProcess
		printf("sumline2,,,input_tempR = %f,,,tempResis[28] = %f \n", input_tempR, tempResis[28]);
#endif
	}

	istart = 1, iend = 8;
	while (input_tempR >= (calibrationValue[istart] / 2) && iend >= istart)
	{
		if (input_tempR >= calibrationValue[istart])
		{
			index = find_first_less(calibrationValue, istart, iend, 1, input_tempR);
			if (iend >= index)
			{
				midValue1 = fabs(input_tempR - calibrationValue[index]);
				midValue2 = fabs(input_tempR - calibrationValue[index + 1]);
				if (0.012 > midValue2 && (index + 1) <= iend)
				{
					index = index + 1;
				}
			}
		}
		else
		{
			//找出与目标数,误差最小的数.
			double current_error = 0;
			double closest = calibrationValue[istart];
			double min_error = fabs(calibrationValue[istart] - input_tempR);
			for (int ik = 1; ik <= iend; ik++)
			{
				current_error = fabs(calibrationValue[ik] - input_tempR);
				if (current_error <= min_error)
				{
					min_error = current_error;
					closest = calibrationValue[ik];
					index = ik;
				}
			}
		}
		input_tempR -= calibrationValue[index];
		tempResis[index] = calibrationValue[index];
		line1flag = 1;

		iend = index - 1;
		i = index;
		switchState[i - 1] = 0;

#ifdef Debug_ReasoningProcess
		printf("sumline1,,,input_tempR = %f,,,tempResis[%d] = %f \n", input_tempR, index, tempResis[index]);
#endif
	}
	if (0 == line1flag)
	{
		//input_tempR = input_tempR + calibrationValue[27];
		tempResis[27] = calibrationValue[27];//2025.08.14由于分辨率卡点,即使该线路上不取值,calibrationValue[27]也参与计算.
	}
	else
	{
		//闭合RL9
		switchState[8] = 0;
		tempResis[27] = calibrationValue[27];

#ifdef Debug_ReasoningProcess
		printf("sumline1,,,input_tempR = %f,,,tempResis[27] = %f \n", input_tempR, tempResis[27]);
#endif
	}
	//短路最小值
	tempResis[26] = calibrationValue[26];

	double currentSetR = 0;//当前设置的电阻值
	for (int i = 0; i < 6; i++)
	{
		device->MT5171_param.g_currentSetR_CH[i] = 0;
	}
	////double temp_sum = 0;//g_currentSetR
	for (int j = 0; j < (CALIBRATION_NUM_5171 - 1); j++)
	{
		currentSetR += tempResis[j];
	}
	device->MT5171_param.g_currentSetR_CH[subUnit - 1] = currentSetR;
	double absolute_error = input_resistance - currentSetR;
#ifdef Debug_ReasoningProcess
	printf("input_tempR = %f,,,currentSetR = %f,,,absolute_error = %f.\n\n", input_tempR, currentSetR, absolute_error);
#endif
#ifdef Debug_calswitchState_CSV
	////save_csv(resistance, switchState, switchSize);
	FILE* fp_setR;
	char filename_setR[256];
	////获取系统日期和时间
	struct tm local_time;
	//结构体初始化
	local_time.tm_sec = 0;     /* seconds after the minute - [0,59] */
	local_time.tm_min = 0;     /* minutes after the hour - [0,59] */
	local_time.tm_hour = 0;    /* hours since midnight - [0,23] */
	local_time.tm_mday = 1;    /* day of the month - [1,31] */
	local_time.tm_mon = 0;     /* months since January - [0,11] */
	local_time.tm_year = 0;    /* years since 1900 */
	local_time.tm_wday = 0;    /* days since Sunday - [0,6] */
	local_time.tm_yday = 0;    /* days since January 1 - [0,365] */
	local_time.tm_isdst = 0;   /* daylight savings time flag */
	f_getSystemTime(local_time);
	printf("本地时间: %d-%02d-%02d %02d:%02d:%02d  \n\n",
		local_time.tm_year + 1900,  // 年份从1900开始计数
		local_time.tm_mon + 1,      // 月份范围0~11，需+1
		local_time.tm_mday,         // 日
		local_time.tm_hour,         // 时
		local_time.tm_min,          // 分
		local_time.tm_sec);
	const char* dir_path = "..\\..\\calibration_result";
	int rtn = check_and_create_dir(dir_path);
	if (rtn != 0)
	{
		printf("创建目录失败！\n");
		return -1;
	}
	sprintf(filename_setR, "..\\..\\calibration_result\\test_setR_switchStatus_Model0x%X_ch%d_%d%02d%02d%02d%02d%02d.csv", device->mtModelName, subUnit,
		local_time.tm_year + 1900, local_time.tm_mon + 1, local_time.tm_mday,
		local_time.tm_hour, local_time.tm_min, local_time.tm_sec);
	fp_setR = fopen(filename_setR, "w");//"a"：以“追加”方式打开文件。"w"：以“清空后写入”方式打开文件。
	if (NULL == fp_setR) {
		printf("fopen(filename) error!!!");
		return -1;
	}
	fprintf(fp_setR, "SerialNumber,status,g_calibrationValue[i],setResistance,currentSetR,absolute_error\n");//写入数据
	//fclose(fp_setR);

	//写入数据到CSV文件
	int value = 0;
	int sum = 0;
	static int cnt = 0;
	int kk = 0; int jj = 0;
	cnt++;
	for (jj = 0; jj < switchSize; jj++)
	{
		value = switchState[jj];
		if (0 == jj)
		{
			fprintf(fp_setR, "%d, %d, %.6lf, %.6lf, %.6lf, %.6lf\n",
				(jj + 1), value, tempResis[jj], resistance, currentSetR, absolute_error);//写入数据
		}
		else
		{
			fprintf(fp_setR, "%d, %d, %.6lf\n", (jj + 1), value, tempResis[jj]);//写入数据
		}
		//缓冲区管理：定期调用 fflush(fp) 刷新缓冲区,防止内存占用过高.
		//if (jj % 1000 == 0)  fflush(fp_setR);//每1000行刷新一次
	}
	fclose(fp_setR);//关闭文件
#endif

	return status;
}

//根据设置电阻大小,通过算法计算,确定开关状态.
int calculate_switchState_5172(ViSession vi, ViUInt32 subUnit, const ViReal64 resistance,
	int switchState[], int switchSize, double calibrationValue[53])
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;
	}

	double midValue1 = 0;
	double midValue2 = 0;
	double input_resistance = resistance;
	double tempResis[CALIBRATION_NUM_5172 - 1];
	for (int j = 0; j < CALIBRATION_NUM_5172 - 1; j++)
	{
		tempResis[j] = 0;
	}
	device->MT5172_param.g_switchState37 = 2;//K137=0,电阻断路; K137=1,电阻短路; K137=2,电阻接入; 

	double sumcal26cal27 = calibrationValue[26] + calibrationValue[27];
	//短路最小值
	double shortValue = calibrationValue[26] + calibrationValue[27] + calibrationValue[28] + calibrationValue[29];
	double input_tempR = 0;
	double sumline1 = 0;
	for (int i = 0; i <= 8; i++)
	{
		sumline1 += calibrationValue[i];;
	}
	double sumline2 = 0;
	for (int i = 9; i <= 16; i++)
	{
		sumline2 += calibrationValue[i];;
	}
	double sumline3 = 0;
	for (int i = 17; i <= 24; i++)
	{
		sumline3 += calibrationValue[i];;
	}

	if (doubleZeroCompare(input_resistance - (sumcal26cal27 - 0.4)) <= 0 )
	{
#ifdef Debug_ReasoningProcess
		printf("\r\n-------calibrationValue[26] = %6.3lf,,,input_resistance = %6.3lf-------\n", calibrationValue[26], input_tempR);
#endif
		for (int j = 0; j < 32; j++)
		{
			switchState[j] = 0;
		}
		//短路最小值，闭合RL10、RL9、RL19、RL29.
		switchState[9] = 1;
		switchState[8] = 1;
		switchState[18] = 1;
		switchState[28] = 1;
		tempResis[26] = calibrationValue[26];
	}
	else if (doubleZeroCompare(input_resistance - (sumcal26cal27 + 0.25)) < 0 && doubleZeroCompare(input_resistance - (sumcal26cal27 - 0.4)) > 0)
	{
#ifdef Debug_ReasoningProcess
		printf("\r\n-------sumcal26cal27 = %6.3lf,,,input_resistance = %6.3lf-------\n", sumcal26cal27, input_tempR);
#endif
		for (int j = 0; j < 32; j++)
		{
			switchState[j] = 0;
		}
		//短路值1,闭合RL10、RL1~RL8、RL19、RL29.
		switchState[9] = 1;
		for (int i = 0; i <= 7; i++)
		{
			switchState[i] = 1;
		}
		switchState[18] = 1;
		switchState[28] = 1;
		tempResis[27] = calibrationValue[27];
	} 
	else
	{
		input_tempR = input_resistance - shortValue;
		if (0.0 == input_tempR)
		{
			input_tempR = 0.0;
		}
#ifdef Debug_ReasoningProcess
		printf("\r\n-------shortValue = %6.3lf,,,input_resistance = %6.3lf-------\n", shortValue, input_tempR);
#endif

		int line1flag = 0, line2flag = 0, line3flag = 0;
		//逻辑计算
		int istart = 0, iend = 0;
		int i = 0, index = 0;
		istart = 17, iend = 24;
		//if (input_tempR >= calibrationValue[istart])
		while (input_tempR >= calibrationValue[istart] && iend >= istart)
		{
			index = find_first_less(calibrationValue, istart, iend, 1, input_tempR);

			input_tempR -= calibrationValue[index];
			tempResis[index] = calibrationValue[index];
			line3flag = 1;

			iend = index - 1;
			i = index;
			switchState[i + 3] = 0;

#ifdef Debug_ReasoningProcess
			printf("sumline3,,,input_tempR = %f, tempResis[%d] = %f \n", input_tempR, index, tempResis[index]);
#endif
		}
		if (0 == line3flag)
		{
			input_tempR = input_tempR + calibrationValue[29];
#ifdef Debug_ReasoningProcess
			printf("sumline3-0, input_tempR = %f, tempResis[29] = %f, calibrationValue[29] = %f \n", input_tempR, tempResis[29], calibrationValue[29]);
#endif
		}
		else
		{
			switchState[28] = 0;//断开RL29
			tempResis[29] = calibrationValue[29];
#ifdef Debug_ReasoningProcess
		printf("sumline3-1, input_tempR = %f, tempResis[29] = %f \n", input_tempR, tempResis[29]);
#endif
		}

		istart = 9, iend = 16;
		while (input_tempR >= calibrationValue[istart] && iend >= istart)
		{
			index = find_first_less(calibrationValue, istart, iend, 1, input_tempR);
			//if (iend >= index)
			//{
			//	midValue1 = fabs(input_tempR - calibrationValue[index]);
			//	midValue2 = fabs(input_tempR - calibrationValue[index + 1]);
			//	if (0.012 > midValue2 && (index + 1) <= iend)
			//	{
			//		index = index + 1;
			//	}
			//}

			input_tempR -= calibrationValue[index];
			tempResis[index] = calibrationValue[index];
			line2flag = 1;

			iend = index - 1;
			i = index;
			switchState[i + 1] = 0;

#ifdef Debug_ReasoningProcess
			printf("sumline2,,,input_tempR = %f, tempResis[%d] = %f \n", input_tempR, index, tempResis[index]);
#endif
		}
		if (0 == line2flag)
		{
			input_tempR = input_tempR + calibrationValue[28];
#ifdef Debug_ReasoningProcess
			printf("sumline2-0, input_tempR = %f, tempResis[28] = %f, calibrationValue[28] = %f \n", input_tempR, tempResis[28], calibrationValue[28]);
#endif
		}
		else
		{
			switchState[18] = 0;//断开RL19
			tempResis[28] = calibrationValue[28];
#ifdef Debug_ReasoningProcess
		printf("sumline2-1, input_tempR = %f, tempResis[28] = %f \n", input_tempR, tempResis[28]);
#endif
		}

		istart = 1, iend = 8;
		while (input_tempR >= (calibrationValue[istart] / 2) && iend >= istart)
		{
			if (input_tempR >= calibrationValue[istart])
			{
				//找出与目标数,误差最小的数.
				double current_error = 0;
				double closest = calibrationValue[istart];
				double min_error = fabs(calibrationValue[istart] - input_tempR);
				for (int ik = 1; ik <= iend; ik++)
				{
					current_error = fabs(calibrationValue[ik] - input_tempR);
					if (current_error <= min_error)
					{
						min_error = current_error;
						closest = calibrationValue[ik];
						index = ik;
					}
				}
				if (doubleZeroCompare(min_error - calibrationValue[istart]) > 0)
				{
					index = find_first_less(calibrationValue, istart, iend, 1, input_tempR);
					if (iend >= index)
					{
						midValue1 = fabs(input_tempR - calibrationValue[index]);
						midValue2 = fabs(input_tempR - calibrationValue[index + 1]);
						if (0.012 > midValue2 && (index + 1) <= iend)
						{
							index = index + 1;
						}
					}
				}
			}
			else
			{
				//找出与目标数,误差最小的数.
				double current_error = 0;
				double closest = calibrationValue[istart];
				double min_error = fabs(calibrationValue[istart] - input_tempR);
				for (int ik = 1; ik <= iend; ik++)
				{
					current_error = fabs(calibrationValue[ik] - input_tempR);
					if (current_error <= min_error)
					{
						min_error = current_error;
						closest = calibrationValue[ik];
						index = ik;
					}
				}
			}
			input_tempR -= calibrationValue[index];
			tempResis[index] = calibrationValue[index];
			line1flag = 1;

			iend = index - 1;
			i = index;
			switchState[i - 1] = 0;

#ifdef Debug_ReasoningProcess
			printf("sumline1,,,input_tempR = %f, tempResis[%d] = %f \n", input_tempR, index, tempResis[index]);
#endif
		}
		if (0 == line1flag)
		{
			//input_tempR = input_tempR + calibrationValue[27];
			switchState[8] = 0;//断开RL9
			tempResis[27] = calibrationValue[27];//2025.08.14由于分辨率卡点,即使该线路上不取值,calibrationValue[27]也参与计算.
#ifdef Debug_ReasoningProcess
			printf("sumline1-0, input_tempR = %f, tempResis[27] = %f, calibrationValue[27] = %f \n", input_tempR, tempResis[27], calibrationValue[27]);
#endif
		}
		else
		{
			switchState[8] = 0;//断开RL9
			tempResis[27] = calibrationValue[27];
#ifdef Debug_ReasoningProcess
			printf("sumline1-1, input_tempR = %f, tempResis[27] = %f \n", input_tempR, tempResis[27]);
#endif
		}
	}
	tempResis[26] = calibrationValue[26];//calibrationValue[26]短路最小值
	input_tempR -= calibrationValue[26];
#ifdef Debug_ReasoningProcess
	printf("sumline1-2, input_tempR = %f, tempResis[26] = %f \n", input_tempR, tempResis[26]);
#endif
	double currentSetR = 0;//当前设置的电阻值
	for (int i = 0; i < 12; i++)
	{
		device->MT5172_param.g_currentSetR_CH[i] = 0;
	}
	for (int j = 0; j < (CALIBRATION_NUM_5172 - 1); j++)
	{
		currentSetR += tempResis[j];
	}
	if (subUnit > 0 && subUnit < 13) {
		device->MT5172_param.g_currentSetR_CH[subUnit - 1] = currentSetR;
	}
	double absolute_error = input_resistance - currentSetR;
#ifdef Debug_ReasoningProcess
	printf("input_tempR = %f, currentSetR = %f, absolute_error = %f.\n\n", input_tempR, currentSetR, absolute_error);
#endif
#ifdef Debug_calswitchState_CSV
	////save_csv(resistance, switchState, switchSize);
	FILE* fp_setR;
	char filename_setR[256];
	////获取系统日期和时间
	struct tm local_time;
	//结构体初始化
	local_time.tm_sec = 0;     /* seconds after the minute - [0,59] */
	local_time.tm_min = 0;     /* minutes after the hour - [0,59] */
	local_time.tm_hour = 0;    /* hours since midnight - [0,23] */
	local_time.tm_mday = 1;    /* day of the month - [1,31] */
	local_time.tm_mon = 0;     /* months since January - [0,11] */
	local_time.tm_year = 0;    /* years since 1900 */
	local_time.tm_wday = 0;    /* days since Sunday - [0,6] */
	local_time.tm_yday = 0;    /* days since January 1 - [0,365] */
	local_time.tm_isdst = 0;   /* daylight savings time flag */
	f_getSystemTime(local_time);
	printf("本地时间: %d-%02d-%02d %02d:%02d:%02d  \n\n",
		local_time.tm_year + 1900,  // 年份从1900开始计数
		local_time.tm_mon + 1,      // 月份范围0~11，需+1
		local_time.tm_mday,         // 日
		local_time.tm_hour,         // 时
		local_time.tm_min,          // 分
		local_time.tm_sec);
	const char* dir_path = "..\\..\\calibration_result";
	int rtn = check_and_create_dir(dir_path);
	if (rtn != 0)
	{
		printf("创建目录失败！\n");
		return -1;
	}
	sprintf(filename_setR, "..\\..\\calibration_result\\test_setR_switchStatus_Model0x%X_ch%d_%d%02d%02d%02d%02d%02d.csv", device->mtModelName, subUnit,
		local_time.tm_year + 1900, local_time.tm_mon + 1, local_time.tm_mday,
		local_time.tm_hour, local_time.tm_min, local_time.tm_sec);
	fp_setR = fopen(filename_setR, "w");//"a"：以“追加”方式打开文件。"w"：以“清空后写入”方式打开文件。
	if (NULL == fp_setR) {
		printf("fopen(filename) error!!!");
		return -1;
	}
	fprintf(fp_setR, "SerialNumber,status,g_calibrationValue[i],setResistance,currentSetR,absolute_error\n");//写入数据
	//fclose(fp_setR);

	//写入数据到CSV文件
	int value = 0;
	int sum = 0;
	static int cnt = 0;
	int kk = 0; int jj = 0;
	cnt++;
	for (jj = 0; jj < switchSize; jj++)
	{
		value = switchState[jj];
		if (0 == jj)
		{
			fprintf(fp_setR, "%d, %d, %.6lf, %.6lf, %.6lf, %.6lf\n",
				(jj + 1), value, tempResis[jj], resistance, currentSetR, absolute_error);//写入数据
		}
		else
		{
			fprintf(fp_setR, "%d, %d, %.6lf\n", (jj + 1), value, tempResis[jj]);//写入数据
		}
		//缓冲区管理：定期调用 fflush(fp) 刷新缓冲区,防止内存占用过高.
		//if (jj % 1000 == 0)  fflush(fp_setR);//每1000行刷新一次
	}
	fclose(fp_setR);//关闭文件
#endif
	//---------------------------------------------------//

	return status;
}


//**************************************************************************
// Function Implementation
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
ViStatus MtPR__init(ViRsrc resourceName, ViBoolean id_query, ViBoolean reset_instr, ViSession *vi)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	ViSession defaultRM;
	// Begin by initializing the system.
	status = viOpenDefaultRM(&defaultRM);
	if (status) {
		return mt_translate_sys_error(status);
	}
	// Open communication with Device.
	status = viOpen(defaultRM, resourceName, VI_NULL, VI_NULL, vi);
	if (status) {
		return mt_translate_sys_error(status);
	}

	EN_BOARD_TYPE type = EN_BOARD_TYPE_NONE;
	//根据属性判断板卡类型
	ViUInt32 iManf = 0, iModel = 0;
	//厂家信息：MT-->0x89E
	status = viGetAttribute(*vi, VI_ATTR_MANF_ID, &iManf);
	if (status) {
		return mt_translate_sys_error(status);
	}
	//产品型号：0x70A-->MT5170B-01; 0x70C-->MT5170B-02; 0x70B-->MT5171; 0x70D-->MT5172; 
	status = viGetAttribute(*vi, VI_ATTR_MODEL_CODE, &iModel);
	if (status) {
		return mt_translate_sys_error(status);
	}
	if (0x89E == iManf)
	{
		if (0x70A == iModel)
		{//srcName = "MT5170B-01";
			type = EN_BOARD_TYPE_5170B_01;
		}
		else if (0x70C == iModel)
		{//srcName = "MT5170B-02";
			type = EN_BOARD_TYPE_5170B_02;
		}
		else if (0x70B == iModel)
		{//srcName = "MT5171";
			type = EN_BOARD_TYPE_5171;
		}
		else if (0x70D == iModel)
		{//srcName = "MT5172";
			type = EN_BOARD_TYPE_5172;
		}
	}
	if (type == EN_BOARD_TYPE_NONE)
	{
		return MT__EAI_DEV_NOTFOUND;
	}

	MT_DEVICE* device = findDevice(*vi);
	if (device == NULL)
	{
		device = (MT_DEVICE*)malloc(sizeof(MT_DEVICE));
		if (device == NULL) { // 处理内存分配失败
			return MTPR__ERROR_OUT_OF_MEMORY_HEAP;
		}
		memset(device, 0, sizeof(MT_DEVICE)); 
		device->handle = *vi; // 使用设备地址作为唯一句柄
		for (int i = 0; i < MAX_DEVICES; i++)
		{
			if ((devices[i] == NULL) || (devices[i]->handle == 0))
			{
				safe_strcpy(device->resourceName, resourceName, 20);
				device->board_type = type;
				devices[i] = device;
				break;
			}
		}
	}
	device->mtManfName = iManf;//厂家信息
	device->mtModelName = iModel;//产品型号

	//程控电阻:0x70A-->MT5170B-01; 0x70C-->MT5170B-02; 0x70B-->MT5171; 0x70D-->MT5172;
	if (0x70A == device->mtModelName || 0x70C == device->mtModelName)
	{//程控电阻:0x70A-->MT5170B-01; 0x70C-->MT5170B-02;
		//变量初始化
		MT5170B_CALIBRATION_VALUE MT5170B_Value;
		MT5170B_Value.R17_R18 = 0;//R17_R18并联
		MT5170B_Value.R19_R20 = 0;//R19_R20并联
		//K137=0,电阻断路; K137=1,K138=0,电阻接入; K137=1,K138=1,电阻短路; 
		MT5170B_Value.g_switchState37 = 0;//继电器K137的状态.0--断路(总体断路).
		MT5170B_Value.g_switchState38 = 0;//继电器K138的状态.
		
		//CH1(g_pre_pattern[0]、g_pre_pattern[1]); CH2(g_pre_pattern[2]、g_pre_pattern[3]); CH3(g_pre_pattern[4]、g_pre_pattern[5]).
		memset(MT5170B_Value.g_pre_pattern, 0, sizeof(MT5170B_Value.g_pre_pattern));
		for (int i = 0; i < 3; i++)
		{
			//校准值  //将一块内存区域的每个字节设置为指定的值.
			memset(MT5170B_Value.g_calibrationValue_CH[i], 0, sizeof(MT5170B_Value.g_calibrationValue_CH[0]));//将每一行都设置为0.
			MT5170B_Value.g_currentSetR_CH[i] = 0;//当前设置的电阻值
		}
		
		device->MT5170B_param = MT5170B_Value;
	}
	else if (0x70B == device->mtModelName)
	{//程控电阻:0x70B-->MT5171; 
		//变量初始化
		MT5171_CALIBRATION_VALUE MT5171_Value;
		//K137=0,电阻断路; K137=1,电阻短路; K137=2,电阻接入; 
		MT5171_Value.g_switchState37 = 0;//继电器K137的状态.0--断路(总体断路).

		memset(MT5171_Value.g_pre_pattern, 0, sizeof(MT5171_Value.g_pre_pattern));
		for (int i = 0; i < 6; i++)
		{
			//校准值  //将一块内存区域的每个字节设置为指定的值.
			memset(MT5171_Value.g_calibrationValue_CH[i], 0, sizeof(MT5171_Value.g_calibrationValue_CH[0]));//将每一行都设置为0.
			MT5171_Value.g_currentSetR_CH[i] = 0;//当前设置的电阻值
		}

		device->MT5171_param = MT5171_Value;
	}
	else if (0x70D == device->mtModelName)
	{//程控电阻:0x70D-->MT5172;
		//变量初始化
		MT5172_CALIBRATION_VALUE MT5172_Value;
		//K137=0,电阻断路; K137=1,电阻短路; K137=2,电阻接入; 
		MT5172_Value.g_switchState37 = 0;//继电器K137的状态.0--断路(总体断路).

		memset(MT5172_Value.g_pre_pattern, 0, sizeof(MT5172_Value.g_pre_pattern));
		for (int i = 0; i < 12; i++)
		{
			//校准值  //将一块内存区域的每个字节设置为指定的值.
			memset(MT5172_Value.g_calibrationValue_CH[i], 0, sizeof(MT5172_Value.g_calibrationValue_CH[0]));//将每一行都设置为0.
			MT5172_Value.g_currentSetR_CH[i] = 0;//当前设置的电阻值
		}

		device->MT5172_param = MT5172_Value;
	}

	//断开卡所有通道.
	status = MtPR__clearCard(*vi);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}

	ViUInt32 subUnit = 1;
	ViUInt32 store = 1;//0-用户数据;1-工厂数据.
	ViUInt32 offset = 0;
	ViUInt32 numValues = 53;
	//程控电阻:0x70A-->MT5170B-01; 0x70C-->MT5170B-02; 0x70B-->MT5171; 0x70D-->MT5172;
	if (0x70A == device->mtModelName || 0x70C == device->mtModelName)
	{//程控电阻:0x70A-->MT5170B-01; 0x70C-->MT5170B-02; 
		numValues = CALIBRATION_NUM_5170B;//校准数据个数
		subUnit = 1; store = 1; offset = 0;
		////读取校准数据
		//for (int i = 0; i < 2; i++)
		//{
		//	subUnit = i + 1;
		//	status = MtPR__readCalibrationFP(*vi, subUnit, store, offset, numValues, device->MT5170B_param.g_calibrationValue_CH[i]);//分通道读取.
		//	if (status) {
		//		device->errorCode = mt_translate_sys_error(status);
		//		return device->errorCode;
		//	}
		//}
		double calValue_allCH[53 * 2] = { 0 };//MT5170B有2个通道,每个通道53个校准数据.
		status = MtPR__InitReadCalibrationFP(*vi, subUnit, store, offset, numValues, calValue_allCH);//批量读取,一次读取2个通道的校准数据.
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		for (int i = 0; i < 2; i++)
		{
			memcpy(device->MT5170B_param.g_calibrationValue_CH[i], calValue_allCH + numValues * i, numValues * sizeof(double));
		}
	}
	else if (0x70B == device->mtModelName)
	{//程控电阻:0x70B-->MT5171;
		numValues = CALIBRATION_NUM_5171;//校准数据个数
		////读取校准数据
		//subUnit = 1; store = 1; offset = 0;
		//for (int i = 0; i < 6; i++)
		//{
		//	subUnit = i + 1;
		//	status = MtPR__readCalibrationFP(*vi, subUnit, store, offset, numValues, device->MT5171_param.g_calibrationValue_CH[i]);
		//	if (status) {
		//		device->errorCode = mt_translate_sys_error(status);
		//		return device->errorCode;
		//	}
		//}
		double calValue_allCH[33 * 6] = { 0 };//MT5171有6个通道,每个通道33个校准数据.
		status = MtPR__InitReadCalibrationFP(*vi, subUnit, store, offset, numValues, calValue_allCH);//批量读取,一次读取6个通道的校准数据.
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		for (int i = 0; i < 6; i++)
		{
			memcpy(device->MT5171_param.g_calibrationValue_CH[i], calValue_allCH + numValues * i, numValues * sizeof(double));
		}
	}
	else if (0x70D == device->mtModelName)
	{//程控电阻:MT5172_12CH-->0x70D;
		numValues = CALIBRATION_NUM_5172;//校准数据个数
		////读取校准数据
		//subUnit = 1; store = 1; offset = 0;
		//for (int i = 0; i < 12; i++)
		//{
		//	subUnit = i + 1;
		//	status = MtPR__readCalibrationFP(*vi, subUnit, store, offset, numValues, device->MT5172_param.g_calibrationValue_CH[i]);
		//	if (status) {
		//		device->errorCode = mt_translate_sys_error(status);
		//		return device->errorCode;
		//	}
		//}
		double calValue_allCH[33 * 12] = { 0 };//MT5172有12个通道,每个通道33个校准数据.
		status = MtPR__InitReadCalibrationFP(*vi, subUnit, store, offset, numValues, calValue_allCH);//批量读取,一次读取12个通道的校准数据.
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		for (int i = 0; i < 12; i++)
		{
			memcpy(device->MT5172_param.g_calibrationValue_CH[i], calValue_allCH + numValues * i, numValues * sizeof(double));
		}
	}

	return status;
}


//**************************************************************************
// Close
//	Terminates the software connection to the instrument and deallocates
//	system resources.  
//		vi (in)			Instrument handle
//		Return = ViStatus
//**************************************************************************
//终止与仪器的软件连接，并释放与仪器相关的系统资源。
ViStatus MtPR__Close(ViSession vi)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;//MT__EAI_DEV_NOTFOUND;//
	}

	status = viClose(vi);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}

	return status;
}


//**************************************************************************
// System Functions
//**************************************************************************

//**************************************************************************
// Route Functions
//**************************************************************************

//清除（取消能量或设置为逻辑‘0’）卡上所有子单元的所有输出通道。
ViStatus MtPR__clearCard_5170B(ViSession vi)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;//MT__EAI_DEV_NOTFOUND;//
	}

	device->MT5170B_param.g_switchState37 = 0;//继电器K137的状态.
	device->MT5170B_param.g_switchState38 = 0;//继电器K138的状态.
	device->MT5170B_param.g_pre_pattern[0] = 0x0;
	device->MT5170B_param.g_pre_pattern[1] = 0x0;
	device->MT5170B_param.g_pre_pattern[2] = 0x0;
	device->MT5170B_param.g_pre_pattern[3] = 0x0;
	//subUnit-1、subUnit-2 电阻通道开关全断开(0).	
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_NWRST_FIFO, 0x1);//SPI0复位
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_CLOCK_DIV, 0xA);//分频系数
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//U12~U8,继电器断开.
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_U10_CH1, 0x0);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_U9U8_CH1, 0x0);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_U12U11_CH2, 0x0);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_U10_CH2, 0x0);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//开始发送数据
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_START_SEND, 0x1);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//查询状态
	ViUInt32 validFlag = 0;
	status = viIn32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_SEND_DATA_OVER, &validFlag);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//打开继电器输出使能
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_RELAY_OUTPUT_EN, 0x0);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//清除 SDO_Data_EN
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_CLEAR, 0x1);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}

	device->MT5170B_param.g_pre_pattern[4] = 0x0;
	device->MT5170B_param.g_pre_pattern[5] = 0x0;
	//subUnit-3 校准通道开关全断开(0).
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_NWRST_FIFO, 0x1);//SPI1复位
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_CLOCK_DIV, 0xA);//分频系数
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//U10~U13,继电器断开.
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_DATA_U13_CH3, 0x0);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_DATA_U11U12_CH3, 0x0);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_CALIBRATION_DATA_U10, 0x0);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//开始发送数据
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_START_SEND, 0x1);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//查询状态
	ViUInt32 validFlag1 = 0;
	status = viIn32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_SEND_DATA_OVER, &validFlag1);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//打开继电器输出使能
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_RELAY_OUTPUT_EN, 0x0);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//清除 SDO_Data_EN
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_DATA_CLEAR, 0x1);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}

	return status;
}
//清除（取消能量或设置为逻辑‘0’）卡上所有子单元的所有输出通道。
ViStatus MtPR__clearCard_5171(ViSession vi)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;//MT__EAI_DEV_NOTFOUND;//
	}

	device->MT5171_param.g_pre_pattern[0] = 0x0;
	device->MT5171_param.g_pre_pattern[1] = 0x0;
	device->MT5171_param.g_pre_pattern[2] = 0x0;
	//subUnit-1、subUnit-2、subUnit-3 电阻通道开关全断开(0).	
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_NWRST_FIFO, 0x1);//SPI0复位
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_CLOCK_DIV, 0xA);//分频系数
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//CH1,继电器断开.
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_5171_CH1, 0x0);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//CH2,继电器断开.
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_5171_CH2, 0x0);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//CH3,继电器断开.
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_5171_CH3, 0x0);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//开始发送数据
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_START_SEND, 0x1);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//查询状态
	ViUInt32 validFlag = 0;
	status = viIn32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_SEND_DATA_OVER, &validFlag);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//打开继电器输出使能
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_RELAY_OUTPUT_EN, 0x0);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//清除 SDO_Data_EN
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_CLEAR, 0x1);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}

	//subUnit-4、subUnit-5、subUnit-6 电阻通道开关全断开(0).	
	device->MT5171_param.g_pre_pattern[3] = 0x0;
	device->MT5171_param.g_pre_pattern[4] = 0x0;
	device->MT5171_param.g_pre_pattern[5] = 0x0;
	//subUnit-3 校准通道开关全断开(0).
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_NWRST_FIFO, 0x1);//SPI1复位
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_CLOCK_DIV, 0xA);//分频系数
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//CH4,继电器断开.
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_DATA_5171_CH4, 0x0);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//CH5,继电器断开.
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_DATA_5171_CH5, 0x0);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//CH6,继电器断开.
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_DATA_5171_CH6, 0x0);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//开始发送数据
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_START_SEND, 0x1);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//查询状态
	ViUInt32 validFlag1 = 0;
	status = viIn32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_SEND_DATA_OVER, &validFlag1);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//打开继电器输出使能
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_RELAY_OUTPUT_EN, 0x0);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//清除 SDO_Data_EN
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_DATA_CLEAR, 0x1);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	
	//g_currentSetR = HUGE_VAL;//当前设置的电阻值

	return status;
}
//清除（取消能量或设置为逻辑‘0’）卡上所有子单元的所有输出通道。
ViStatus MtPR__clearCard_5172(ViSession vi)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;
	}

	device->MT5172_param.g_pre_pattern[0] = 0x0;
	device->MT5172_param.g_pre_pattern[1] = 0x0;
	device->MT5172_param.g_pre_pattern[2] = 0x0;
	//subUnit-1、subUnit-2、subUnit-3 电阻通道开关全断开(0).	
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_NWRST_FIFO, 0x1);//SPI0复位
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_CLOCK_DIV, 0xA);//分频系数
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//CH1,继电器断开.
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_5172_CH1, 0x0);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//CH2,继电器断开.
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_5172_CH2, 0x0);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//CH3,继电器断开.
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_5172_CH3, 0x0);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//CH4,继电器断开.
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_5172_CH4, 0x0);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//CH5,继电器断开.
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_5172_CH5, 0x0);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//CH6,继电器断开.
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_5172_CH6, 0x0);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//开始发送数据
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_START_SEND, 0x1);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//查询状态
	ViUInt32 validFlag = 0;
	status = viIn32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_SEND_DATA_OVER, &validFlag);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//打开继电器输出使能
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_RELAY_OUTPUT_EN, 0x0);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//清除 SDO_Data_EN
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_CLEAR, 0x1);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}

	//subUnit-4、subUnit-5、subUnit-6 电阻通道开关全断开(0).	
	device->MT5172_param.g_pre_pattern[3] = 0x0;
	device->MT5172_param.g_pre_pattern[4] = 0x0;
	device->MT5172_param.g_pre_pattern[5] = 0x0;
	//subUnit-3 校准通道开关全断开(0).
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_NWRST_FIFO, 0x1);//SPI1复位
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_CLOCK_DIV, 0xA);//分频系数
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//CH7,继电器断开.
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_DATA_5172_CH7, 0x0);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//CH8,继电器断开.
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_DATA_5172_CH8, 0x0);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//CH9,继电器断开.
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_DATA_READ_5172_CH9, 0x0);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//CH10,继电器断开.
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_DATA_READ_5172_CH10, 0x0);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//CH11,继电器断开.
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_DATA_READ_5172_CH11, 0x0);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//CH12,继电器断开.
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_DATA_READ_5172_CH12, 0x0);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//开始发送数据
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_START_SEND, 0x1);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//查询状态
	ViUInt32 validFlag1 = 0;
	status = viIn32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_SEND_DATA_OVER, &validFlag1);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//打开继电器输出使能
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_RELAY_OUTPUT_EN, 0x0);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//清除 SDO_Data_EN
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_DATA_CLEAR, 0x1);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}

	//g_currentSetR = HUGE_VAL;//当前设置的电阻值

	return status;
}
//**************************************************************************
// Clear card
//	Clears all of a card's output sub-units.
//		Return = ViStatus
//**************************************************************************
//清除（取消能量或设置为逻辑‘0’）卡上所有子单元的所有输出通道。
ViStatus MtPR__clearCard(ViSession vi)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;//MT__EAI_DEV_NOTFOUND;//
	}

	//程控电阻:0x70A-->MT5170B-01; 0x70C-->MT5170B-02; 0x70B-->MT5171; 0x70D-->MT5172; 
	if (0x70A == device->mtModelName || 0x70C == device->mtModelName)
	{
		status = MtPR__clearCard_5170B(vi);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else if (0x70B == device->mtModelName)
	{
		status = MtPR__clearCard_5171(vi);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else if (0x70D == device->mtModelName)
	{
		status = MtPR__clearCard_5172(vi);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else
	{
		device->errorCode = MTPR__ERROR_BAD_SESSION;
		return device->errorCode;
	}

	//g_currentSetR = HUGE_VAL;//当前设置的电阻值

	return status;
}


//清除（取消激活或设置为逻辑‘0’）子单元的所有输出通道。
ViStatus MtPR__clearSub_5170B(ViSession vi, ViUInt32 subUnit)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;//MT__EAI_DEV_NOTFOUND;//
	}
	if (subUnit > 2 || subUnit < 1)
	{
		device->errorCode = MTPR__ERROR_BAD_SUB;
		return device->errorCode;
	}

	device->MT5170B_param.g_switchState37 = 0;//继电器K137的状态.
	device->MT5170B_param.g_switchState38 = 0;//继电器K138的状态.

	//清除（取消激活或设置为逻辑‘0’）子单元的所有输出通道。
	//sub=1,只清除子单元1的所有输出通道,其他子单元的转态不变.
	//sub=2,只清除子单元2的所有输出通道,其他子单元的转态不变.
	//sub=3,只清除子单元3的所有输出通道,其他子单元的转态不变.

	if (1 == subUnit)
	{
		device->MT5170B_param.g_pre_pattern[0] = 0x0;
		device->MT5170B_param.g_pre_pattern[1] = 0x0;

		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_NWRST_FIFO, 0x1);//SPI0复位
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_CLOCK_DIV, 0xA);//分频系数
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//U12~U8,继电器断开.
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_U10_CH1, 0x0);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_U9U8_CH1, 0x0);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//开始发送数据
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_START_SEND, 0x1);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//查询状态
		ViUInt32 val32 = 0;
		status = viIn32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_SEND_DATA_OVER, &val32);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//打开继电器输出使能
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_RELAY_OUTPUT_EN, 0x0);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//清除 SDO_Data_EN
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_CLEAR, 0x1);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else if (2 == subUnit)
	{
		device->MT5170B_param.g_pre_pattern[2] = 0x0;
		device->MT5170B_param.g_pre_pattern[3] = 0x0;

		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_NWRST_FIFO, 0x1);//SPI0复位
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_CLOCK_DIV, 0xA);//分频系数
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//U12~U8,继电器断开.
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_U12U11_CH2, 0x0);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_U10_CH2, 0x0);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//开始发送数据
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_START_SEND, 0x1);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//查询状态
		ViUInt32 val32 = 0;
		status = viIn32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_SEND_DATA_OVER, &val32);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//打开继电器输出使能
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_RELAY_OUTPUT_EN, 0x0);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//清除 SDO_Data_EN
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_CLEAR, 0x1);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else if (3 == subUnit)
	{
		device->MT5170B_param.g_pre_pattern[4] = 0x0;
		device->MT5170B_param.g_pre_pattern[5] = 0x0;

		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_NWRST_FIFO, 0x1);//SPI1复位
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_CLOCK_DIV, 0xA);//分频系数
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//U10~U13,继电器断开.
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_DATA_U13_CH3, 0x0);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_DATA_U11U12_CH3, 0x0);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//开始发送数据
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_START_SEND, 0x1);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//查询状态
		ViUInt32 validFlag = 0;
		status = viIn32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_SEND_DATA_OVER, &validFlag);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//打开继电器输出使能
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_RELAY_OUTPUT_EN, 0x0);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//清除 SDO_Data_EN
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_DATA_CLEAR, 0x1);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}

	return status;
}
//清除（取消激活或设置为逻辑‘0’）子单元的所有输出通道。
ViStatus MtPR__clearSub_5171(ViSession vi, ViUInt32 subUnit)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;//MT__EAI_DEV_NOTFOUND;//
	}
	if (subUnit > 6 || subUnit < 1)
	{
		device->errorCode = MTPR__ERROR_BAD_SUB;
		return device->errorCode;
	}

	//清除（取消激活或设置为逻辑‘0’）子单元的所有输出通道。
	//sub=1,只清除子单元1的所有输出通道,其他子单元的转态不变.
	//sub=2,只清除子单元2的所有输出通道,其他子单元的转态不变.
	//sub=3,只清除子单元3的所有输出通道,其他子单元的转态不变.

	if (1 == subUnit || 2 == subUnit || 3 == subUnit)
	{
		//subUnit-1、subUnit-2、subUnit-3 电阻通道开关全断开(0).	
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_NWRST_FIFO, 0x1);//SPI0复位
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_CLOCK_DIV, 0xA);//分频系数
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else if (4 == subUnit || 5 == subUnit || 6 == subUnit)
	{
		//subUnit-4、subUnit-5、subUnit-6 电阻通道开关全断开(0).	
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_NWRST_FIFO, 0x1);//SPI1复位
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_CLOCK_DIV, 0xA);//分频系数
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	//写入数据
	ViBusAddress offset_5171CHi = SPI0_ATTR_DATA_5171_CH1;
	if (1 == subUnit)
	{
		offset_5171CHi = SPI0_ATTR_DATA_5171_CH1;
	}
	else if (2 == subUnit)
	{
		offset_5171CHi = SPI0_ATTR_DATA_5171_CH2;
	}
	else if (3 == subUnit)
	{
		offset_5171CHi = SPI0_ATTR_DATA_5171_CH3;
	}
	else if (4 == subUnit)
	{
		offset_5171CHi = SPI1_ATTR_DATA_5171_CH4;
	}
	else if (5 == subUnit)
	{
		offset_5171CHi = SPI1_ATTR_DATA_5171_CH5;
	}
	else if (6 == subUnit)
	{
        offset_5171CHi = SPI1_ATTR_DATA_5171_CH6;
	}
	//写入数据
	if (subUnit>=1 && subUnit <= 6)
	{
		device->MT5171_param.g_pre_pattern[subUnit - 1] = 0x0;
		//CHi,继电器断开.
		status = viOut32(vi, VI_PXI_BAR0_SPACE, offset_5171CHi, 0x0);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}

	//发送数据
	if (1 == subUnit || 2 == subUnit || 3 == subUnit)
	{
		//开始发送数据
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_START_SEND, 0x1);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//查询状态
		ViUInt32 validFlag = 0;
		status = viIn32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_SEND_DATA_OVER, &validFlag);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//打开继电器输出使能
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_RELAY_OUTPUT_EN, 0x0);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//清除 SDO_Data_EN
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_CLEAR, 0x1);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else if (4 == subUnit || 5 == subUnit || 6 == subUnit)
	{
		//开始发送数据
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_START_SEND, 0x1);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//查询状态
		ViUInt32 validFlag1 = 0;
		status = viIn32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_SEND_DATA_OVER, &validFlag1);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//打开继电器输出使能
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_RELAY_OUTPUT_EN, 0x0);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//清除 SDO_Data_EN
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_DATA_CLEAR, 0x1);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}

	//g_currentSetR = HUGE_VAL;//当前设置的电阻值

	return status;
}
//清除（取消激活或设置为逻辑‘0’）子单元的所有输出通道。
ViStatus MtPR__clearSub_5172(ViSession vi, ViUInt32 subUnit)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;//MT__EAI_DEV_NOTFOUND;//
	}
	if (subUnit > 12 || subUnit < 1)
	{
		device->errorCode = MTPR__ERROR_BAD_SUB;
		return device->errorCode;
	}

	//SPI复位
	if (subUnit >= 1 || subUnit <= 6)
	{
		//subUnit-1、subUnit-2、subUnit-3、subUnit-4、subUnit-5、subUnit-6 电阻通道开关全断开(0).	
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_NWRST_FIFO, 0x1);//SPI0复位
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_CLOCK_DIV, 0xA);//分频系数
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else if (subUnit >= 7 || subUnit <= 12)
	{
		//电阻通道开关全断开(0).	
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_NWRST_FIFO, 0x1);//SPI1复位
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_CLOCK_DIV, 0xA);//分频系数
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	//写入数据
	ViBusAddress offset_5172CHi = SPI0_ATTR_DATA_5172_CH1;
	if (1 == subUnit)
	{
		offset_5172CHi = SPI0_ATTR_DATA_5172_CH1;
	}
	else if (2 == subUnit)
	{
		offset_5172CHi = SPI0_ATTR_DATA_5172_CH2;
	}
	else if (3 == subUnit)
	{
		offset_5172CHi = SPI0_ATTR_DATA_5172_CH3;
	}
	else if (4 == subUnit)
	{
		offset_5172CHi = SPI0_ATTR_DATA_5172_CH4;
	}
	else if (5 == subUnit)
	{
		offset_5172CHi = SPI0_ATTR_DATA_5172_CH5;
	}
	else if (6 == subUnit)
	{
		offset_5172CHi = SPI0_ATTR_DATA_5172_CH6;
	}
	else if (7 == subUnit)
	{
		offset_5172CHi = SPI1_ATTR_DATA_5172_CH7;
	}
	else if (8 == subUnit)
	{
		offset_5172CHi = SPI1_ATTR_DATA_5172_CH8;
	}
	else if (9 == subUnit)
	{
		offset_5172CHi = SPI1_ATTR_DATA_5172_CH9;
	}
	else if (10 == subUnit)
	{
		offset_5172CHi = SPI1_ATTR_DATA_5172_CH10;
	}
	else if (11 == subUnit)
	{
		offset_5172CHi = SPI1_ATTR_DATA_5172_CH11;
	}
	else if (12 == subUnit)
	{
		offset_5172CHi = SPI1_ATTR_DATA_5172_CH12;
	}
	//写入数据
	if (subUnit >= 1 && subUnit <= 12)
	{
		device->MT5172_param.g_pre_pattern[subUnit - 1] = 0x0;
		//CHi,继电器断开.
		status = viOut32(vi, VI_PXI_BAR0_SPACE, offset_5172CHi, 0x0);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	//发送数据
	if (subUnit >= 1 && subUnit <= 6)
	{
		//开始发送数据
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_START_SEND, 0x1);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//查询状态
		ViUInt32 validFlag = 0;
		status = viIn32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_SEND_DATA_OVER, &validFlag);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//打开继电器输出使能
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_RELAY_OUTPUT_EN, 0x0);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//清除 SDO_Data_EN
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_CLEAR, 0x1);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else if (subUnit >= 7 && subUnit <= 12)
	{
		//开始发送数据
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_START_SEND, 0x1);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//查询状态
		ViUInt32 validFlag1 = 0;
		status = viIn32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_SEND_DATA_OVER, &validFlag1);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//打开继电器输出使能
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_RELAY_OUTPUT_EN, 0x0);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//清除 SDO_Data_EN
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_DATA_CLEAR, 0x1);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}

	return status;
}
//**************************************************************************
// Clear a sub-unit
//  Clears all of an individual sub-unit's outputs, opening relays or
//  drivers, or setting logic '0'.
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		Return = ViStatus
//**************************************************************************
//清除（取消激活或设置为逻辑‘0’）子单元的所有输出通道。
ViStatus MtPR__clearSub(ViSession vi, ViUInt32 subUnit)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;//MT__EAI_DEV_NOTFOUND;//
	}

	//产品型号: 0x70A-->MT5170B-01; 0x70C-->MT5170B-02; 0x70B-->MT5171; 0x70D-->MT5172; 
	if (0x70A == device->mtModelName || 0x70C == device->mtModelName)
	{
		status = MtPR__clearSub_5170B(vi, subUnit);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else if (0x70B == device->mtModelName)
	{
		status = MtPR__clearSub_5171(vi, subUnit);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else if (0x70D == device->mtModelName)
	{
		status = MtPR__clearSub_5172(vi, subUnit);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else
	{
		device->errorCode = MTPR__ERROR_BAD_SESSION;
		return device->errorCode;
	}

	//g_currentSetR = HUGE_VAL;//当前设置的电阻值

	return status;
}


ViStatus MtPR__setChannelState_5170B(ViSession vi, ViUInt32 subUnit,
	ViUInt32 channel, ViBoolean state)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;
	}

	////MT5170-校准通道为：subUnit=4,是校准时用的MUX通道.
	//if (4 != subUnit)
	//{
	//	return -1;
	//}

	//校准通道-SPI1：U10、U11、U12、U13.
	int switchState[16];
	//初始化初始开关状态
	for (int j = 0; j < 16; j++)
	{
		switchState[j] = 0;
		//MT5170B_param->g_pre_setChannelState[j] = 0;
	}

	//校准通道-SPI1:R1:1-2-3; R2:4-5-6; R3:7-8-9; 
	if (1 == channel)
	{
		if (1 == state)
		{
			switchState[0] = 1;
			switchState[2] = 1;
		}
		else
		{
			switchState[0] = 0;
			switchState[2] = 0;
		}
	}
	else if (2 == channel)
	{
		if (1 == state)
		{
			switchState[0] = 1;
			switchState[1] = 1;
		}
		else
		{
			switchState[0] = 0;
			switchState[1] = 0;
		}
	}
	else if (3 == channel)
	{
		if (1 == state)
		{
			switchState[1] = 1;
			switchState[2] = 1;
			switchState[9] = 1;
		}
		else
		{
			switchState[1] = 0;
			switchState[2] = 0;
			switchState[9] = 0;
		}
	}
	else if (4 == channel)
	{
		if (1 == state)
		{
			switchState[3] = 1;
			switchState[5] = 1;
		}
		else
		{
			switchState[3] = 0;
			switchState[5] = 0;
		}
	}
	else if (5 == channel)
	{
		if (1 == state)
		{
			switchState[3] = 1;
			switchState[4] = 1;
		}
		else
		{
			switchState[3] = 0;
			switchState[4] = 0;
		}
	}
	else if (6 == channel)
	{
		if (1 == state)
		{
			switchState[4] = 1;
			switchState[5] = 1;
			switchState[9] = 1;
		}
		else
		{
			switchState[4] = 0;
			switchState[5] = 0;
			switchState[9] = 0;
		}
	}
	else if (7 == channel)
	{
		if (1 == state)
		{
			switchState[6] = 1;
			switchState[8] = 1;
		}
		else
		{
			switchState[6] = 0;
			switchState[8] = 0;
		}
	}
	else if (8 == channel)
	{
		if (1 == state)
		{
			switchState[6] = 1;
			switchState[7] = 1;
		}
		else
		{
			switchState[6] = 0;
			switchState[7] = 0;
		}
	}
	else if (9 == channel)
	{
		if (1 == state)
		{
			switchState[7] = 1;
			switchState[8] = 1;
			switchState[9] = 1;
		}
		else
		{
			switchState[7] = 0;
			switchState[8] = 0;
			switchState[9] = 0;
		}
	}
	else
	{
	    device->errorCode = MTPR__ERROR_BAD_CHANNEL;
		return device->errorCode;
	}
	int length = sizeof(switchState) / sizeof(switchState[0]);
	//char* hexStr = binaryArrayToHex(switchState, 16);
	char* hexStr = binaryToHex(switchState, length);
	if (hexStr == NULL)
	{
		//printf(" binaryArrayToHex ERROR.");
		device->errorCode = MTPR__ERROR_INV_PARAMETER;
		return device->errorCode;
	}
	ViUInt64 value = strtoull(hexStr, NULL, length);//转换为64位整数
	ViUInt32 high32 = (ViUInt32)(value >> 32);//提取高32位和低32位
	ViUInt32 low32 = (ViUInt32)(value & 0xFFFFFFFF);
	free(hexStr);//释放内存
	//printf("high32 = %lu,low32 = %lu.\n", high32, low32);
	//g_pre_setChannelState_pattern[0] = low32;
	//g_pre_setChannelState_pattern[1] = high32;

	//设置校准通道状态.
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_NWRST_FIFO, 0x1);//SPI1复位
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_CLOCK_DIV, 0xA);//分频系数
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//U10~U13,继电器开关.
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_CALIBRATION_DATA_U10, low32);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//开始发送数据
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_START_SEND, 0x1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//查询状态
	ViUInt32 validFlag;
	status = viIn32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_SEND_DATA_OVER, &validFlag);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//打开继电器输出使能
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_RELAY_OUTPUT_EN, 0x0);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//清除 SDO_Data_EN
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_DATA_CLEAR, 0x1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}

	return status;
}
ViStatus MtPR__setChannelState_5171(ViSession vi, ViUInt32 subUnit,
	ViUInt32 channel, ViBoolean state)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;
	}


	return status;
}
ViStatus MtPR__setChannelState_5172(ViSession vi, ViUInt32 subUnit,
	ViUInt32 channel, ViBoolean state)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;
	}


	return status;
}
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
	ViUInt32 channel, ViBoolean state)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;//MT__EAI_DEV_NOTFOUND;//
	}

	//程控电阻:0x70A-->MT5170B-01; 0x70C-->MT5170B-02; 0x70B-->MT5171; 
	if (0x70A == device->mtModelName || 0x70C == device->mtModelName)
	{
		status = MtPR__setChannelState_5170B(vi, subUnit, channel, state);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else if (0x70B == device->mtModelName)
	{
	}
	else if (0x70D == device->mtModelName)
	{
	}
	else
	{
		return MTPR__ERROR_BAD_SESSION;
	}

	return status;
}


ViStatus MtPR__setChannelPattern_5170B(ViSession vi, ViUInt32 subUnit, ViAUInt32 pattern)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;//MT__EAI_DEV_NOTFOUND;//
	}
	if (subUnit > 2 || subUnit < 1)
	{
		device->errorCode = MTPR__ERROR_BAD_SUB;
		return device->errorCode;
	}

	ViUInt32 validFlag = 0;
	if (1 == subUnit)
	{
		device->MT5170B_param.g_pre_pattern[0] = pattern[0];
		device->MT5170B_param.g_pre_pattern[1] = pattern[1];

		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_NWRST_FIFO, 0x1);//SPI0复位
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_CLOCK_DIV, 0xA);//分频系数
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//U12~U8,继电器断开.
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_U9U8_CH1, pattern[0]);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_U10_CH1, pattern[1]);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//开始发送数据
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_START_SEND, 0x1);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//查询状态
		status = viIn32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_SEND_DATA_OVER, &validFlag);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//打开继电器输出使能
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_RELAY_OUTPUT_EN, 0x0);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//清除 SDO_Data_EN
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_CLEAR, 0x1);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else if (2 == subUnit)
	{
		device->MT5170B_param.g_pre_pattern[2] = pattern[0];
		device->MT5170B_param.g_pre_pattern[3] = pattern[1];

		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_NWRST_FIFO, 0x1);//SPI0复位
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_CLOCK_DIV, 0xA);//分频系数
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//U12~U8,继电器断开.
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_U12U11_CH2, pattern[0]);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_U10_CH2, pattern[1]);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//开始发送数据
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_START_SEND, 0x1);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//查询状态
		status = viIn32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_SEND_DATA_OVER, &validFlag);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//打开继电器输出使能
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_RELAY_OUTPUT_EN, 0x0);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//清除 SDO_Data_EN
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_CLEAR, 0x1);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else if (3 == subUnit)
	{
		device->MT5170B_param.g_pre_pattern[4] = pattern[0];
		device->MT5170B_param.g_pre_pattern[5] = pattern[1];

		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_NWRST_FIFO, 0x1);//SPI1复位
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_CLOCK_DIV, 0xA);//分频系数
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//U12~U8,继电器断开.
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_DATA_U11U12_CH3, pattern[0]);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_DATA_U13_CH3, pattern[1]);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//开始发送数据
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_START_SEND, 0x1);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//查询状态
		status = viIn32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_SEND_DATA_OVER, &validFlag);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//打开继电器输出使能
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_RELAY_OUTPUT_EN, 0x0);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//清除 SDO_Data_EN
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_DATA_CLEAR, 0x1);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}

	return status;
}
ViStatus MtPR__setChannelPattern_5171(ViSession vi, ViUInt32 subUnit, ViAUInt32 pattern)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;//MT__EAI_DEV_NOTFOUND;//
	}
	if (subUnit > 6 || subUnit < 1)
	{
		device->errorCode = MTPR__ERROR_BAD_SUB;
		return device->errorCode;
	}

	if (1 == subUnit || 2 == subUnit || 3 == subUnit)
	{
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_NWRST_FIFO, 0x1);//SPI0复位
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_CLOCK_DIV, 0xA);//分频系数
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else if (4 == subUnit || 5 == subUnit || 6 == subUnit)
	{
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_NWRST_FIFO, 0x1);//SPI1复位
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_CLOCK_DIV, 0xA);//分频系数
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
		}
	}
	//写入数据
	device->MT5171_param.g_pre_pattern[subUnit - 1] = pattern[0];
	if (1 == subUnit)
	{
		//CH1,继电器断开.
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_5171_CH1, pattern[0]);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else if (2 == subUnit)
	{
		//CH2,继电器断开.
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_5171_CH2, pattern[0]);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else if (3 == subUnit)
	{
		//CH3,继电器断开.
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_5171_CH3, pattern[0]);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else if (4 == subUnit)
	{
		//CH4,继电器断开.
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_DATA_5171_CH4, pattern[0]);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else if (5 == subUnit)
	{
		//CH5,继电器断开.
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_DATA_5171_CH5, pattern[0]);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else if (6 == subUnit)
	{
		//CH6,继电器断开.
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_DATA_5171_CH6, pattern[0]);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}

	ViUInt32 validFlag = 0;
	if (1 == subUnit || 2 == subUnit || 3 == subUnit)
	{
		//开始发送数据
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_START_SEND, 0x1);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//查询状态
		status = viIn32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_SEND_DATA_OVER, &validFlag);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//打开继电器输出使能
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_RELAY_OUTPUT_EN, 0x0);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//清除 SDO_Data_EN
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_CLEAR, 0x1);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else if (4 == subUnit || 5 == subUnit || 6 == subUnit)
	{
		//开始发送数据
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_START_SEND, 0x1);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//查询状态
		status = viIn32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_SEND_DATA_OVER, &validFlag);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//打开继电器输出使能
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_RELAY_OUTPUT_EN, 0x0);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//清除 SDO_Data_EN
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_DATA_CLEAR, 0x1);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}

	return status;
}
ViStatus MtPR__setChannelPattern_5172(ViSession vi, ViUInt32 subUnit, ViAUInt32 pattern)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;
	}
	if (subUnit > 12 || subUnit < 1)
	{
		device->errorCode = MTPR__ERROR_BAD_SUB;
		return device->errorCode;
	}

	ViBusAddress offset_5172CHi = SPI0_ATTR_DATA_5172_CH1;
	if (1 == subUnit)
	{
		offset_5172CHi = SPI0_ATTR_DATA_5172_CH1;
	}
	else if (2 == subUnit)
	{
		offset_5172CHi = SPI0_ATTR_DATA_5172_CH2;
	}
	else if (3 == subUnit)
	{
		offset_5172CHi = SPI0_ATTR_DATA_5172_CH3;
	}
	else if (4 == subUnit)
	{
		offset_5172CHi = SPI0_ATTR_DATA_5172_CH4;
	}
    else if (5 == subUnit)
	{
		offset_5172CHi = SPI0_ATTR_DATA_5172_CH5;
	}
	else if (6 == subUnit)
	{
		offset_5172CHi = SPI0_ATTR_DATA_5172_CH6;
	}
	else if (7 == subUnit)
	{
		offset_5172CHi = SPI1_ATTR_DATA_5172_CH7;
	}
	else if (8 == subUnit)
	{
		offset_5172CHi = SPI1_ATTR_DATA_5172_CH8;
	}
	else if (9 == subUnit)
	{
		offset_5172CHi = SPI1_ATTR_DATA_5172_CH9;
	}
	else if (10 == subUnit)
	{
		offset_5172CHi = SPI1_ATTR_DATA_5172_CH10;
	}
	else if (11 == subUnit)
	{
		offset_5172CHi = SPI1_ATTR_DATA_5172_CH11;
	}
	else if (12 == subUnit)
	{
		offset_5172CHi = SPI1_ATTR_DATA_5172_CH12;
	}

	if (subUnit>=1 && subUnit<=6)
	{
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_NWRST_FIFO, 0x1);//SPI0复位
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_CLOCK_DIV, 0xA);//分频系数
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	} 
	else if(subUnit >=7 && subUnit <= 12)
	{
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_NWRST_FIFO, 0x1);//SPI0复位
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_CLOCK_DIV, 0xA);//分频系数
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	//写入数据, CHi,继电器断开.
	device->MT5172_param.g_pre_pattern[subUnit - 1] = pattern[0];
	status = viOut32(vi, VI_PXI_BAR0_SPACE, offset_5172CHi, pattern[0]);
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	ViUInt32 validFlag = 0;
	if (subUnit >=1 && subUnit <= 6)
	{
		//开始发送数据
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_START_SEND, 0x1);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//查询状态
		status = viIn32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_SEND_DATA_OVER, &validFlag);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//打开继电器输出使能
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_RELAY_OUTPUT_EN, 0x0);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//清除 SDO_Data_EN
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_CLEAR, 0x1);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else if (subUnit >=7 && subUnit <= 12)
	{
		//开始发送数据
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_START_SEND, 0x1);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//查询状态
		status = viIn32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_SEND_DATA_OVER, &validFlag);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//打开继电器输出使能
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_RELAY_OUTPUT_EN, 0x0);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//清除 SDO_Data_EN
		status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_DATA_CLEAR, 0x1);
		if (status)
		{
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}

	return status;
}
//**************************************************************************
// Set all of a sub-unit's outputs.//设置子单元的所有输出。
//  Sets the states of all of a sub-unit's outputs.//设置子单元所有输出的状态。
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		pattern (in)	Array containing a bit-pattern representing
//						the desired output states
//		Return = ViStatus
//**************************************************************************
ViStatus MtPR__setChannelPattern(ViSession vi, ViUInt32 subUnit, ViAUInt32 pattern)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;
	}

	//程控电阻:0x70A-->MT5170B-01; 0x70C-->MT5170B-02; 0x70B-->MT5171; 
	if (0x70A == device->mtModelName || 0x70C == device->mtModelName)
	{
		status = MtPR__setChannelPattern_5170B(vi, subUnit, pattern);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else if (0x70B == device->mtModelName)
	{
		status = MtPR__setChannelPattern_5171(vi, subUnit, pattern);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else if (0x70D == device->mtModelName)
	{
		status = MtPR__setChannelPattern_5172(vi, subUnit, pattern);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else
	{
		device->errorCode = MTPR__ERROR_BAD_SESSION;
		return device->errorCode;
	}

	return status;
}


ViStatus MtPR__getChannelState_5170B(ViSession vi, ViUInt32 subUnit,
	ViUInt32 channel, ViPBoolean state)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;
	}

	////如果不是校准通道,则输出0;
	//if (4 != subUnit)
	//{//MT5170-subUnit为4时,是校准时用的MUX通道.
	//	*state = VI_OFF;
	//	return MTPR__ERROR_BAD_SUB;
	//}

	//如果不是校准通道,则输出0;
	if (channel>9 || channel<1)
	{
		device->errorCode = MTPR__ERROR_BAD_CHANNEL;
		return device->errorCode;
	}

	//校准通道-SPI1：U10、U11、U12、U13.
	int switchState[16] = { 0 };
	//初始化初始开关状态
	for (int j = 0; j < 16; j++)
	{
		switchState[j] = 0;
	}
	////读取校准通道,位数据. 方式一：
	//decimalToBinary2(g_pre_setChannelState_pattern[0], switchState);//OK

	//读取校准通道,位数据. 方式二：
	status = MtPR__setChannelState(vi, subUnit, channel, VI_ON);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	ViUInt32 low32 = 0;
	status = viIn32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_CALIBRATION_DATA_READ_U10, &low32);//OK
	if (status)
	{
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	decimalToBinary2(low32, switchState);
	/*///-------test 查看校准通道的开关状态---------------//
	for (int j = 0; j < 16; j++)
	{
	printf("switchState[%d] = %d \n", j, switchState[j]);
	}
	printf("\r-----------getChannelState----End--------------------\n\n");
	//-------test 查看校准通道的开关状态---------------///*/

	//校准通道-SPI1:R1:1-2-3; R2:4-5-6; R3:7-8-9; 
	if (1 == channel)
	{
		if (1 == switchState[0] && 1 == switchState[2])
		{
			*state = VI_ON;
		}
		else
		{
			*state = VI_OFF;
		}
	}
	else if (2 == channel)
	{
		if (1 == switchState[0] && 1 == switchState[1])
		{
			*state = VI_ON;
		}
		else
		{
			*state = VI_OFF;
		}
	}
	else if (3 == channel)
	{
		if (1 == switchState[1] && 1 == switchState[2] && 1 == switchState[9])
		{
			*state = VI_ON;
		}
		else
		{
			*state = VI_OFF;
		}
	}
	else if (4 == channel)
	{
		if (1 == switchState[3] && 1 == switchState[5])
		{
			*state = VI_ON;
		}
		else
		{
			*state = VI_OFF;
		}
	}
	else if (5 == channel)
	{
		if (1 == switchState[3] && 1 == switchState[4])
		{
			*state = VI_ON;
		}
		else
		{
			*state = VI_OFF;
		}
	}
	else if (6 == channel)
	{
		if (1 == switchState[4] && 1 == switchState[5] && 1 == switchState[9])
		{
			*state = VI_ON;
		}
		else
		{
			*state = VI_OFF;
		}
	}
	else if (7 == channel)
	{
		if (1 == switchState[6] && 1 == switchState[8])
		{
			*state = VI_ON;
		}
		else
		{
			*state = VI_OFF;
		}
	}
	else if (8 == channel)
	{
		if (1 == switchState[6] && 1 == switchState[7])
		{
			*state = VI_ON;
		}
		else
		{
			*state = VI_OFF;
		}
	}
	else if (9 == channel)
	{
		if (1 == switchState[6] && 1 == switchState[8] && 1 == switchState[9])
		{
			*state = VI_ON;
		}
		else
		{
			*state = VI_OFF;
		}
	}
	//*state = hasOne(switchState, 16);

	return status;
}
ViStatus MtPR__getChannelState_5171(ViSession vi, ViUInt32 subUnit,
	ViUInt32 channel, ViPBoolean state)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;//MT__EAI_DEV_NOTFOUND;//
	}



	return status;
}
ViStatus MtPR__getChannelState_5172(ViSession vi, ViUInt32 subUnit,
	ViUInt32 channel, ViPBoolean state)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;//MT__EAI_DEV_NOTFOUND;//
	}



	return status;
}
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
	ViUInt32 channel, ViPBoolean state)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;
	}


	if (0x70A == device->mtModelName || 0x70C == device->mtModelName)
	{
		status = MtPR__getChannelState_5170B(vi, subUnit, channel, state);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else if (0x70B == device->mtModelName)
	{
		status = MtPR__getChannelState_5171(vi, subUnit, channel, state);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else if (0x70D == device->mtModelName)
	{
		status = MtPR__getChannelState_5172(vi, subUnit, channel, state);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else
	{
		return MTPR__ERROR_BAD_SESSION;
	}

	return status;
}


//获取子单元所有输出的状态。
ViStatus MtPR__getChannelPattern_5170B(ViSession vi, ViUInt32 subUnit, ViAUInt32 pattern)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;//MT__EAI_DEV_NOTFOUND;//
	}
	if (subUnit > 2 || subUnit < 1)
	{
		device->errorCode = MTPR__ERROR_BAD_SUB;
		return device->errorCode;
	}

	ViUInt32 pre_set_pattern[2] = { 0, 0 };

	////读取正常功能通道,位数据. 方法一: 
	//if (1 == subUnit)
	//{
	//	pattern[0] = g_pre_pattern[0];
	//	pattern[1] = g_pre_pattern[1];
	//	pre_set_pattern[0] = g_pre_pattern[0];
	//	pre_set_pattern[1] = g_pre_pattern[1];
	//}
	//else if (2 == subUnit)
	//{
	//	pattern[0] = g_pre_pattern[2];
	//	pattern[1] = g_pre_pattern[3];
	//	pre_set_pattern[0] = g_pre_pattern[2];
	//	pre_set_pattern[1] = g_pre_pattern[3];
	//}
	//else if (3 == subUnit)
	//{
	//	pattern[0] = g_pre_pattern[4];
	//	pattern[1] = g_pre_pattern[5];
	//	pre_set_pattern[0] = g_pre_pattern[4];
	//	pre_set_pattern[1] = g_pre_pattern[5];
	//}
	//else
	//{
	//	pattern[0] = 0;
	//	pattern[1] = 0;
	//	pre_set_pattern[0] = 0;
	//	pre_set_pattern[1] = 0;
	//}


	//读取正常功能通道,位数据. 方法二: 
	if (1 == subUnit)
	{
		pre_set_pattern[0] = device->MT5170B_param.g_pre_pattern[0];
		pre_set_pattern[1] = device->MT5170B_param.g_pre_pattern[1];
	}
	else if (2 == subUnit)
	{
		pre_set_pattern[0] = device->MT5170B_param.g_pre_pattern[2];
		pre_set_pattern[1] = device->MT5170B_param.g_pre_pattern[3];
	}
	else if (3 == subUnit)
	{
		pre_set_pattern[0] = device->MT5170B_param.g_pre_pattern[4];
		pre_set_pattern[1] = device->MT5170B_param.g_pre_pattern[5];
	}
	status = MtPR__setChannelPattern(vi, subUnit, pre_set_pattern);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	if (1 == subUnit)
	{
		status = viIn32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_READ_U9U8_CH1, &pattern[0]);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		status = viIn32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_READ_U10_CH1, &pattern[1]);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else if (2 == subUnit)
	{
		status = viIn32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_READ_U12U11_CH2, &pattern[0]);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		status = viIn32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_READ_U10_CH2, &pattern[1]);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else if (3 == subUnit)
	{
		status = viIn32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_DATA_READ_U11U12_CH3, &pattern[0]);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		status = viIn32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_DATA_READ_U13_CH3, &pattern[1]);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}

	return status;
}
//获取子单元所有输出的状态。
ViStatus MtPR__getChannelPattern_5171(ViSession vi, ViUInt32 subUnit, ViAUInt32 pattern)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;//MT__EAI_DEV_NOTFOUND;//
	}
	if (subUnit > 6 || subUnit < 1)
	{
		device->errorCode = MTPR__ERROR_BAD_SUB;
		return device->errorCode;
	}

	ViUInt32 pre_set_pattern[2] = { 0, 0 };


	return status;
}
//获取子单元所有输出的状态。
ViStatus MtPR__getChannelPattern_5172(ViSession vi, ViUInt32 subUnit, ViAUInt32 pattern)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;//MT__EAI_DEV_NOTFOUND;//
	}
	if (subUnit > 12 || subUnit < 1)
	{
		device->errorCode = MTPR__ERROR_BAD_SUB;
		return device->errorCode;
	}

	ViUInt32 pre_set_pattern[2] = { 0, 0 };


	return status;
}
//**************************************************************************
// Get all of a sub-unit's outputs.
//  Gets the states of all of a sub-unit's outputs.
//		vi (in)			Instrument handle
//		subUnit (in)	Sub-unit number
//		pattern (out)	Array to accept a bit-pattern representing
//						the sub-unit's output states
//		Return = ViStatus
//**************************************************************************
ViStatus MtPR__getChannelPattern(ViSession vi, ViUInt32 subUnit, ViAUInt32 pattern)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;
	}


	if (0x70A == device->mtModelName || 0x70C == device->mtModelName)
	{
		status = MtPR__getChannelPattern_5170B(vi, subUnit, pattern);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else if (0x70B == device->mtModelName)
	{
		status = MtPR__getChannelPattern_5171(vi, subUnit, pattern);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else if (0x70D == device->mtModelName)
	{
		status = MtPR__getChannelPattern_5172(vi, subUnit, pattern);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else
	{
		device->errorCode = MTPR__ERROR_BAD_SESSION;
		return MTPR__ERROR_BAD_SESSION;
	}

	return status;
}



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
ViStatus MtPR__reset(ViSession vi)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;//MT__EAI_DEV_NOTFOUND;//
	}

	status = MtPR__clearCard(vi);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}

	return status;
}


ViStatus MtPR__self_test_5170B(ViSession vi, ViPInt16 test_result, ViPString test_message)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;
	}

	//subUnit-1、subUnit-2 校准通道开关全断开(0).
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_NWRST_FIFO, 0x1);//SPI0复位
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_CLOCK_DIV, 0xA);//分频系数
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//U12~U8,继电器断开.
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_U10_CH1, 0x12);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_U9U8_CH1, 0x23);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_U12U11_CH2, 0x34);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_U10_CH2, 0x45);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//关闭继电器输出使能
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_RELAY_OUTPUT_EN, 0x1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//开始发送数据
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_START_SEND, 0x1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//查询状态
	ViUInt32 validFlag = 0;
	status = viIn32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_SEND_DATA_OVER, &validFlag);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//清除 SDO_Data_EN
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_CLEAR, 0x1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}

	//回读自检数据
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_START_SEND, 0x1);//开始发送数据
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	ViUInt32 value1 = 0;
	status = viIn32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_READ_U10_CH1, &value1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	ViUInt32 value2 = 0;
	status = viIn32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_READ_U9U8_CH1, &value2);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	ViUInt32 value3 = 0;
	status = viIn32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_READ_U12U11_CH2, &value3);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	ViUInt32 value4 = 0;
	status = viIn32(vi, VI_PXI_BAR0_SPACE, SPI0_ATTR_DATA_READ_U10_CH2, &value4);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	if (0x12 != value1 && 0x23 != value2 && 0x34 != value3 && 0x45 != value4)
	{
		device->errorCode = MTPR__ERROR_HARDWARE_FAULT;
		return device->errorCode;
	}


	//subUnit-3 校准通道开关全断开(0).
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_NWRST_FIFO, 0x1);//SPI1复位
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_CLOCK_DIV, 0xA);//分频系数
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//U10~U13,继电器断开.
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_DATA_U13_CH3, 0x10);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_DATA_U11U12_CH3, 0x11);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_CALIBRATION_DATA_U10, 0x12);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//关闭继电器输出使能
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_RELAY_OUTPUT_EN, 0x1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//开始发送数据
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_START_SEND, 0x1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//查询状态
	ViUInt32 validFlag1 = 0;
	status = viIn32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_SEND_DATA_OVER, &validFlag1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//清除 SDO_Data_EN
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_DATA_CLEAR, 0x1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//回读自检数据
	status = viOut32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_START_SEND, 0x1);//开始发送数据
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	ViUInt32 value10 = 0;
	status = viIn32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_DATA_READ_U13_CH3, &value10);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	ViUInt32 value11 = 0;
	status = viIn32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_DATA_READ_U11U12_CH3, &value11);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	ViUInt32 value12 = 0;
	status = viIn32(vi, VI_PXI_BAR0_SPACE, SPI1_ATTR_CALIBRATION_DATA_READ_U10, &value12);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	if (0x10 != value10 && 0x11 != value11 && 0x12 != value12)
	{
		device->errorCode = MTPR__ERROR_HARDWARE_FAULT;
		return device->errorCode;
	}

	return status;
}
ViStatus MtPR__self_test_5171(ViSession vi, ViPInt16 test_result, ViPString test_message)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;
	}

	return status;
}
ViStatus MtPR__self_test_5172(ViSession vi, ViPInt16 test_result, ViPString test_message)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;
	}

	return status;
}
//**************************************************************************
// Self-Test
//	Perform self-test and return result of the test.
//		vi (in)				Instrument handle
//		test_result  (out)	Numeric result of test; 0 = no error
//		test_message (out)	Self-test message
//		Return = ViStatus
//**************************************************************************
ViStatus MtPR__self_test(ViSession vi, ViPInt16 test_result, ViPString test_message)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		mt_strcpy(test_message, 256, "Card cannot be accessed (failed/removed/unpowered).");
		*test_result = MTPR__FAULT_CARD_INACCESSIBLE;
		return MTPR__FAULT_CARD_INACCESSIBLE;
	}

	if (0x70A == device->mtModelName || 0x70C == device->mtModelName)
	{
		status = MtPR__self_test_5170B(vi, test_result, test_message);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else if (0x70B == device->mtModelName)
	{
		status = MtPR__self_test_5171(vi, test_result, test_message);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else if (0x70D == device->mtModelName)
	{
		status = MtPR__self_test_5172(vi, test_result, test_message);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else
	{
		device->errorCode = MTPR__ERROR_BAD_SESSION;
		return device->errorCode;
	}
	*test_result = (ViInt16)status;
	return MtPR__error_message(vi, status, test_message);
}


//**************************************************************************
// Revision Query  //修改查询：返回仪器驱动程序和仪器固件的版本。
//	Returns the revision of the instrument driver and instrument firmware.
//		vi (in)			Instrument handle
//		driver_rev (out)Driver revision      //驱动版本
//		instr_rev (out) Instrument revision  //仪器版本
//		Return = ViStatus
//**************************************************************************
ViStatus MtPR__revision_query(ViSession vi, ViPString driver_rev, ViPString instr_rev)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;
	}

	//size_t len1 = strlen(DRIVER_VERSION);
	//size_t len2 = strlen(PRODUCT_VERSION);
	////使用 snprintf 安全拼接
	status = snprintf(driver_rev, 256, "%s", DRIVER_VERSION);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = snprintf(instr_rev, 256, "%s", PRODUCT_VERSION);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}

	return status;
}


//**************************************************************************
// Error Query  //查询错误：返回一个错误代码和相应的信息
//	Return an error code and corresponding message from the instrument's 
//	error queue.
//		vi (in)			Instrument handle
//		ecode (out)		Instrument error code  //仪器错误码
//		emessage (out)	Error message          //错误消息
//		Return = ViStatus
//**************************************************************************
ViStatus pipx40_error_query(ViSession vi, ViPInt32 ecode, ViChar message[256])
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;//MT__EAI_DEV_NOTFOUND;//
	}



	return status;
}


//**************************************************************************
// Error Message  //错误信息：将仪器的错误返回值转换为用户可读的字符串。
//  Translate the error return value from the instrument into a
//	user-readable string.
//		vi (in)				Instrument handle
//		status_code (in)	Instrument error code  //仪器错误码
//		message (out)		VISA or instrument driver error message  //visa 或仪器驱动程序错误消息
//		Return = ViStatus
//**************************************************************************
ViStatus MtPR__error_message(ViSession vi, ViStatus status_code, ViChar message[256])
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
        mt_strcpy(message, 256, "Card cannot be accessed (failed/removed/unpowered).");
		return MTPR__ERROR_CARD_INACCESSIBLE;
	}

	switch (status_code)
	{
	case 0:
        mt_strcpy(message, 256, "SUCCESS./STAT_OK.");
		break;
	case MTPR__ERROR_BAD_SESSION:
        mt_strcpy(message, 256, "No MT VTECHSEMI card is open on the session specified.");
		break;
	case MTPR__ERROR_NO_INFO:
        mt_strcpy(message, 256, "Cannot obtain information for specified card.");
		break;
	case MTPR__ERROR_CARD_DISABLED:
        mt_strcpy(message, 256, "Specified card is disabled.");
		break;
	case MTPR__ERROR_BAD_SUB:
        mt_strcpy(message, 256, "sub-unit value out-of-range for target card.");
		break;
	case MTPR__ERROR_BAD_CHANNEL:
        mt_strcpy(message, 256, "Channel number out-of-range for target sub-unit.");
		break;
	case MTPR__ERROR_NO_CAL_DATA:
        mt_strcpy(message, 256, "Target sub-unit has no calibration data to read / write.");
		break;
	case MTPR__ERROR_BAD_ARRAY:
        mt_strcpy(message, 256, "SafeArray type, shape or size is incorrect.");
		break;
	case MTPR__ERROR_MUX_ILLEGAL:
        mt_strcpy(message, 256, "Non-zero write data is illegal for MUX sub-unit ");
		break;
	case MTPR__ERROR_EXCESS_CLOSURE:
        mt_strcpy(message, 256, "Execution would cause closure limit to be exceeded.");
		break;
	case MTPR__ERROR_ILLEGAL_MASK:
        mt_strcpy(message, 256, "One or more of the specified channels cannot be masked.");
		break;
	case MTPR__ERROR_OUTPUT_MASKED:
        mt_strcpy(message, 256, "Cannot activate an output that is masked.");
		break;
	case MTPR__ERROR_FAILED_INIT:
        mt_strcpy(message, 256, "Cannot open a MT card at the specified location.");
		break;
	case MTPR__ERROR_READ_FAIL:
        mt_strcpy(message, 256, "Failed read from hardware.");
		break;
	case MTPR__ERROR_WRITE_FAIL:
        mt_strcpy(message, 256, "Failed write to hardware.");
		break;
	case MTPR__ERROR_VISA_OP:
        mt_strcpy(message, 256, "VISA operation failed.");
		break;
	case MTPR__ERROR_VISA_VERSION:
        mt_strcpy(message, 256, "Incompatible VISA version.");
		break;
	case MTPR__ERROR_SUB_TYPE:
        mt_strcpy(message, 256, "Incompatible with sub-unit type.");
		break;
	case MTPR__ERROR_BAD_ROW:
        mt_strcpy(message, 256, "Matrix row value out of range.");
		break;
	case MTPR__ERROR_BAD_COLUMN:
        mt_strcpy(message, 256, "Matrix column value out of range.");
		break;
	case MTPR__ERROR_BAD_ATTEN:
        mt_strcpy(message, 256, "Attenuation value out of range.");
		break;
	case MTPR__ERROR_BAD_VOLTAGE:
        mt_strcpy(message, 256, "Voltage value out of range.");
		break;
	case MTPR__ERROR_BAD_CAL_INDEX:
        mt_strcpy(message, 256, "Calibration index value out-of-range.");
		break;
	case MTPR__ERROR_BAD_SEGMENT:
        mt_strcpy(message, 256, "Segment number out of range.");
		break;
	case MTPR__ERROR_BAD_FUNC_CODE:
        mt_strcpy(message, 256, "Function code value out of range.");
		break;
	case MTPR__ERROR_BAD_SUBSWITCH:
        mt_strcpy(message, 256, "Sub-switch value out of range.");
		break;
	case MTPR__ERROR_BAD_ACTION:
        mt_strcpy(message, 256, "Action code out of range.");
		break;
	case MTPR__ERROR_STATE_CORRUPT:
        mt_strcpy(message, 256, "Cannot execute due to corrupt sub-unit state.");
		break;
	case MTPR__ERROR_BAD_ATTR_CODE:
        mt_strcpy(message, 256, "Unrecognised attribute code.");
		break;
	case MTPR__ERROR_EEPROM_WRITE_TMO:
        mt_strcpy(message, 256, "Timeout writing to EEPROM.");
		break;
	case MTPR__ERROR_ILLEGAL_OP:
        mt_strcpy(message, 256, "Operation is illegal in the sub-unit's current state.");
		break;
	case MTPR__ERROR_BAD_POT:
		mt_strcpy(message, 256, "Unrecognised pot number requested.");
		break;
	case MTPR__ERROR_MATRIXR_ILLEGAL:
        mt_strcpy(message, 256, "Invalid write pattern for MATRIXR/MATRIXS sub-unit.");
		break;
	case MTPR__ERROR_MISSING_CHANNEL:
        mt_strcpy(message, 256, "Attempted operation on non-existent channel.");
		break;
	case MTPR__ERROR_CARD_INACCESSIBLE:
        mt_strcpy(message, 256, "Card cannot be accessed (failed/removed/unpowered).");
		break;
	case MTPR__ERROR_BAD_FP_FORMAT:
        mt_strcpy(message, 256, "Unsupported internal floating-point format (internal error).");
		break;
	case MTPR__ERROR_UNCALIBRATED:
        mt_strcpy(message, 256, "Sub-unit is not calibrated.");
		break;
	case MTPR__ERROR_BAD_RESISTANCE:
        mt_strcpy(message, 256, "Unobtainable resistance value.");
		break;
	case MTPR__ERROR_BAD_STORE:
        mt_strcpy(message, 256, "Invalid calibration store number.");
		break;
	case MTPR__ERROR_BAD_MODE:
        mt_strcpy(message, 256, "Invalid mode value.");
		break;
	case MTPR__ERROR_SETTINGS_CONFLICT:
        mt_strcpy(message, 256, "Conflicting device settings.");
		break;
	case MTPR__ERROR_CARD_TYPE:
        mt_strcpy(message, 256, "Function call incompatible with card type or capabilities.");
		break;
	case MTPR__ERROR_BAD_POLE:
        mt_strcpy(message, 256, "Switch pole value out of range.");
		break;
	case MTPR__ERROR_MISSING_CAPABILITY:
        mt_strcpy(message, 256, "Attempted to activate a non-existent capability.");
		break;
	case MTPR__ERROR_MISSING_HARDWARE:
        mt_strcpy(message, 256, "Action requires hardware that is not present.");
		break;
	case MTPR__ERROR_HARDWARE_FAULT:
        mt_strcpy(message, 256, "Faulty hardware.");
		break;
	case MTPR__ERROR_EXECUTION_FAIL:
        mt_strcpy(message, 256, "Failed to execute (e.g. blocked by a hardware condition).");
		break;
	case MTPR__ERROR_BAD_CURRENT:
        mt_strcpy(message, 256, "Current value out of range.");
		break;
	case MTPR__ERROR_BAD_RANGE:
        mt_strcpy(message, 256, "Invalid range value.");
		break;
	case MTPR__ERROR_ATTR_UNSUPPORTED:
        mt_strcpy(message, 256, "Attribute not supported.");
		break;
	case MTPR__ERROR_BAD_REGISTER:
        mt_strcpy(message, 256, "Register number out of range.");
		break;
	case MTPR__ERROR_MATRIXP_ILLEGAL:
        mt_strcpy(message, 256, "Invalid channel closure or write pattern for MATRIXP sub-unit.");
		break;
	case MTPR__ERROR_BUFFER_UNDERSIZE:
        mt_strcpy(message, 256, "Data buffer too small.");
		break;
	case MTPR__ERROR_ACCESS_MODE:
        mt_strcpy(message, 256, "Inconsistent shared access mode.");
		break;
	case MTPR__ERROR_POOR_RESISTANCE:
		mt_strcpy(message, 256, "Resistance outside limits.");
		break;
	case MTPR__ERROR_BAD_ATTR_VALUE:
        mt_strcpy(message, 256, "Bad attribute value.");
		break;
	case MTPR__ERROR_INVALID_POINTER:
        mt_strcpy(message, 256, "Invalid pointer.");
		break;
	case MTPR__ERROR_ATTR_READ_ONLY:
        mt_strcpy(message, 256, "Attribute is read only.");
		break;
	case MTPR__ERROR_ATTR_DISABLED:
        mt_strcpy(message, 256, "Attribute is disabled.");
		break;
	case MTPR__ERROR_PSU_MAIN_DISABLED:
        mt_strcpy(message, 256, "Main output is disabled, cannot enable the channel.");
		break;
	case MTPR__ERROR_OUT_OF_MEMORY_HEAP:
        mt_strcpy(message, 256, "Unable to allocate memory on Heap.");
		break;
	case MTPR__ERROR_INVALID_PROCESSID:
        mt_strcpy(message, 256, "Invalid ProcessID.");
		break;
	case MTPR__ERROR_SHARED_MEMORY:
        mt_strcpy(message, 256, "Shared memory error.");
		break;
	case MTPR__ERROR_CARD_OPENED_OTHER_PROCESS:
        mt_strcpy(message, 256, "Card is opened by a process in exclusive mode.");
		break;
	case MTPR__ERROR_DIO_PORT_DISABLED:
        mt_strcpy(message, 256, "DIO card PORT is disabled due to Over-Current Scenario.");
		break;
	case MTPR__ERROR_DIO_INVALID_FILE:
        mt_strcpy(message, 256, "DIO pattern file is invalid.");
		break;
	case MTPR__ERROR_DIO_DYNAMIC_ACTIVE:
        mt_strcpy(message, 256, "DIO File Entry has error, check the file entries.");
		break;
	case MTPR__ERROR_DIO_FILE_ENTRY_ERR:
        mt_strcpy(message, 256, "The interface associated with this session is not the system controller.");
		break;
	case MTPR__ERROR_HW_INT_NOT_SUPPORTED:
        mt_strcpy(message, 256, "Hardware Interlock feature not supported for the card.");
		break;
	case MTPR__ERROR_HW_INT_ERROR:
        mt_strcpy(message, 256, "Hardware Interlock is not detected on the card, cannot use the function.");
		break;
	case MTPR__ERROR_SMS_CH_ENABLE_ERR:
        mt_strcpy(message, 256, "Channel is disabled, so Set-Measure-Set cannot be used.");
		break;
	case MTPR__ERROR_MEAS_CURRENT:
        mt_strcpy(message, 256, "Measured Current out of range.");
		break;
	case MTPR__ERROR_BAD_TEMPERATURE:
        mt_strcpy(message, 256, "Temperature value out of range.");
		break;
	case MTPR__ERROR_CMP_ENGINE_ON:
        mt_strcpy(message, 256, "Capture Engine running - operation unobtainable.");
		break;
	case MTPR__ERROR_WRITEPATTERN_ILLEGAL:
        mt_strcpy(message, 256, "Invalid write pattern.");
		break;
	case MTPR__ERROR_DC_OFFSET:
        mt_strcpy(message, 256, "DC Offset value out of range.");
		break;
	case MTPR__ERROR_FREQUENCY:
        mt_strcpy(message, 256, "Frequency value out of range.");
        break;
	case MTPR__ERROR_PHASE:
        mt_strcpy(message, 256, "Phase value out of range.");
        break;
	case MTPR__ERROR_DUTY_CYCLE:
        mt_strcpy(message, 256, "Duty Cycle value out of range.");
        break;
	case MTPR__ERROR_PULSE_WIDTH:
        mt_strcpy(message, 256, "Pulse Width value out of range.");
        break;
	case MTPR__ERROR_INVALID_COMP_FLAG:
        mt_strcpy(message, 256, "Invalid flag for setting compatibility logic.");
        break;
	case MTPR__ERROR_INV_PARAMETER:
        mt_strcpy(message, 256, "Invalid Value.");
        break;

	case MTPR__ERROR_UNKNOWN:
		mt_strcpy(message, 256, "Unspecified Error.");
		break;
	case MTPR__STAT_NO_CARD:
        mt_strcpy(message, 256, "No MT card open on the session.");
		break;
	case MTPR__STAT_WRONG_DRIVER:
        mt_strcpy(message, 256, "Card requires later driver version.");
		break;
	case MTPR__STAT_EEPROM_ERR:
        mt_strcpy(message, 256, "Error interpreting card EEPROM data.");
		break;
	case MTPR__STAT_DISABLED:
        mt_strcpy(message, 256, "Card is disabled.");
		break;
	case MTPR__STAT_NO_SUB:
        mt_strcpy(message, 256, "Card has no sub-unit with specified number.");
		break;
	case MTPR__STAT_BUSY:
        mt_strcpy(message, 256, "Busy (card or sub-unit) .");
		break;
	case MTPR__STAT_HW_FAULT:
        mt_strcpy(message, 256, "Hardware fault (eg. defective serial loop).");
		break;
	case MTPR__STAT_PARITY_ERROR:
        mt_strcpy(message, 256, "PCIbus parity error.");
		break;
	case MTPR__STAT_PSU_INHIBITED:
        mt_strcpy(message, 256, "PSU sub-unit - supply is disabled (by software).");
		break;
	case MTPR__STAT_PSU_SHUTDOWN:
        mt_strcpy(message, 256, "PSU sub-unit - supply is shutdown (due to overload).");
		break;
	case MTPR__STAT_PSU_CURRENT_LIMIT:
        mt_strcpy(message, 256, "PSU sub-unit - supply is operating in current-limited mode.");
		break;
	case MTPR__STAT_CORRUPTED:
        mt_strcpy(message, 256, "Sub-unit logical state is corrupted.");
		break;
	case MTPR__STAT_CARD_INACCESSIBLE:
        mt_strcpy(message, 256, "Card cannot be accessed (failed/removed/unpowered).");
		break;
	case MTPR__STAT_UNCALIBRATED:
        mt_strcpy(message, 256, "Calibration data is faulty (card or sub-unit).");
		break;
	case MTPR__STAT_CALIBRATION_DUE:
        mt_strcpy(message, 256, "Calibration is due (card or sub-unit).");
		break;
	case MTPR__STAT_BIRST_ENABLED:
        mt_strcpy(message, 256, "BIRST is active (card or sub-unit).");
		break;
	default:
		mt_strcpy(message, 256, "Unknown error.");
	}

	return status;
}


//**************************************************************************
// Get card ID
//	Obtains card identification in string format.
//		vi (in)			Instrument handle
//		id (out)		Identification string
//		Return = ViStatus
//**************************************************************************
ViStatus MtPR__getCardId(ViSession vi, ViPString id)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;//MT__EAI_DEV_NOTFOUND;//
	}
	//1.读取厂家  //2.读取产品型号  //3.读取产品SN号  //4.板卡版本号

	//方式一：
	//1.读取厂家
	char CorporName[16];//缓冲区用于存储型号信息
	status = viGetAttribute(vi, VI_ATTR_MANF_NAME, &CorporName);//读厂家信息
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	else
	{
		//2.读取产品型号
		char modelName[16];//缓冲区用于存储型号信息
		status = viGetAttribute(vi, VI_ATTR_MODEL_NAME, &modelName);//读产品型号
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		else
		{
			//使用 snprintf 安全拼接
			snprintf(id, 256, "%s,%s", CorporName, modelName);//snprintf函数(可以兼容linux和windows)
			//printf("---【 拼接后的字符串: %s 】\n", id);
			//输出: ---【 拼接后的字符串: MT VTECHSEMI,5170B 】
		}
	}

	return status;
}


//**************************************************************************
// Get card diagnostic      //获取卡诊断信息
//	Obtains diagnostic information in string format.    //以字符串格式获取诊断信息。
//		vi (in)			Instrument handle
//		message (out)	Diagnostic string
//		Return = ViStatus
//**************************************************************************
ViStatus pipx40_getDiagnostic(ViSession vi, ViPString message)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;//MT__EAI_DEV_NOTFOUND;//
	}



	return status;
}


//**************************************************************************
// Get sub-unit counts
//	Obtains counts of the number of input and output sub-units on the card.
//		vi (in)			Instrument handle
//		inSubs (out)	The number of input sub-units
//		outSubs (out)	The number of output sub-units
//		Return = ViStatus
//**************************************************************************
ViStatus MtPR__getSubCounts(ViSession vi, ViPUInt32 inSubs, ViPUInt32 outSubs)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;//MT__EAI_DEV_NOTFOUND;//
	}

	//程控电阻:0x70A-->MT5170B-01; 0x70C-->MT5170B-02; 0x70B-->MT5171; 
	if (0x70A == device->mtModelName || 0x70C == device->mtModelName)
	{
		//*inSubs = 3;
		*outSubs = 3;
	}
	else if (0x70B == device->mtModelName)
	{
		//*inSubs = 6;
		*outSubs = 6;
	}
	else if (0x70D == device->mtModelName)
	{
		//*inSubs = 12;
		*outSubs = 12;
	}

	return status;
}


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
	ViBoolean out, ViPString subType)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;//MT__EAI_DEV_NOTFOUND;//
	}

	if (VI_FALSE == out)
	{       
		status = snprintf(subType, 256, "%s", "");//snprintf函数 (兼容Linux系统)
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else
	{
		//根据属性判断板卡类型
		if (0x89E == device->mtManfName)
		{
			//程控电阻:0x70A-->MT5170B-01; 0x70C-->MT5170B-02; 
			if (0x70A == device->mtModelName || 0x70C == device->mtModelName)
			{
				status = snprintf(subType, 256, "%s", "RES(40)");
				if (status) {
					device->errorCode = mt_translate_sys_error(status);
					return device->errorCode;
				}
				//printf("dest_size = %d,,(char*)subType: %s\n", strlen(card_subType), (char*)subType);
			}
			else if (0x70B == device->mtModelName)
			{
				status = snprintf(subType, 256, "%s", "RES(30)");
				if (status) {
					device->errorCode = mt_translate_sys_error(status);
					return device->errorCode;
				}
				//printf("dest_size = %d,,(char*)subType: %s\n", strlen(card_subType), (char*)subType);
			}
			else if (0x70D == device->mtModelName)
			{
				status = snprintf(subType, 256, "%s", "RES(30)");
				if (status) {
					device->errorCode = mt_translate_sys_error(status);
					return device->errorCode;
				}
				//printf("dest_size = %d,,(char*)subType: %s\n", strlen(card_subType), (char*)subType);
			}
		}
	}

	return status;
}


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
	ViBoolean out, ViPUInt32 subType, ViPUInt32 rows, ViPUInt32 columns)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;//MT__EAI_DEV_NOTFOUND;//
	}

	if (VI_FALSE == out)
	{
		*subType = 0;
		*rows = 0; *columns = 0;
	}
	else
	{
		//根据属性判断板卡类型
		if (0x89E == device->mtManfName)
		{
			//产品型号: 0x70A-->MT5170B-01; 0x70C-->MT5170B-02; 0x70B-->MT5171; 0x70D-->MT5172; 
			if (0x70A == device->mtModelName || 0x70C == device->mtModelName)
			{
				*subType = 7;
				*rows = 1; *columns = 40;
			}
			else if (0x70B == device->mtModelName)
			{
				*subType = 7;
				*rows = 1; *columns = 30;
			}
			else if (0x70D == device->mtModelName)
			{
				*subType = 7;
				*rows = 1; *columns = 30;
			}
		}
	}

	return status;
}

//**************************************************************************
// Get status information   //获取状态信息
//	Obtains card status as a set of bit-flags.  //获取卡的状态作为一组位标志。
//		vi (in)			Instrument handle
//		status (out)	Bit-flags indicating the card's status  //表示卡状态的位标志
//		Return = ViStatus
//**************************************************************************
ViStatus MtPR__getCardStatus(ViSession vi, ViPUInt32 status)
{
	ViStatus istatus = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;//MT__EAI_DEV_NOTFOUND;//
	}




	return istatus;
}



//**************************************************************************
// Floating-point calibration data functions.
//**************************************************************************

// Store floating-point calibration value(s) in EEPROM
ViStatus MtPR__writeCalibrationFP_5170B(ViSession vi, ViUInt32 subUnit,
	ViUInt32 store, ViUInt32 offset, ViUInt32 numValues, ViReal64 *data)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;//MT__EAI_DEV_NOTFOUND;//
	}

	ViUInt32 addr = 0x00800000;
	if (0 == store)
	{
		addr = SPI_FLASH_USER0_DATA_BASE;
	}
	else if (1 == store)
	{
		addr = SPI_FLASH_FACTORY1_DATA_BASE;
	}
	else
	{
	}

#ifdef Debug_readORwriteCaliFP_TXT
	//读取 SN number
	ViUInt32 serialNumber = 0;
	ViUInt64 FwVersion = 0;
	status = MtPR__readCardSNnumber(vi, subUnit, 1, &serialNumber, &FwVersion);
	if (status < VI_SUCCESS) {
		printf("读取设备序列号，错误代码: 0x%x\n", status);
	}
	else
	{
		printf("\r\r--------readSN = %lu,,,FwVersion = %llx--------\r\n", serialNumber, FwVersion);
	}
	////获取系统日期和时间
	struct tm local_time;
	//结构体初始化
	local_time.tm_sec = 0;     // seconds after the minute - [0,59]
	local_time.tm_min = 0;     // minutes after the hour - [0,59]
	local_time.tm_hour = 0;    // hours since midnight - [0,23]
	local_time.tm_mday = 1;    // day of the month - [1,31] 
	local_time.tm_mon = 0;     // months since January - [0,11] 
	local_time.tm_year = 0;    // years since 1900 
	local_time.tm_wday = 0;    // days since Sunday - [0,6] 
	local_time.tm_yday = 0;    // days since January 1 - [0,365] 
	local_time.tm_isdst = 0;   // daylight savings time flag 
	f_getSystemTime(local_time);
	printf("本地时间: %d-%02d-%02d %02d:%02d:%02d  \n\n",
		local_time.tm_year + 1900,  // 年份从1900开始计数
		local_time.tm_mon + 1,      // 月份范围0~11，需+1
		local_time.tm_mday,         // 日
		local_time.tm_hour,         // 时
		local_time.tm_min,          // 分
		local_time.tm_sec);         // 秒
	const char* dir_path = "..\\..\\calibration_result";
	int rtn = check_and_create_dir(dir_path);
	if (rtn != 0)
	{
		printf("创建目录失败！\n");
		return -1;
	}
	//保存校准数据 //可选：保存到文件
	FILE* fp = NULL;
	char filename[256];
	sprintf(filename, "..\\..\\calibration_result\\writeCalibrationF_Model0x%X_SN%lu_ch%d_%d%02d%02d%02d%02d%02d.txt",
		device->mtModelName, serialNumber, subUnit, 
		local_time.tm_year + 1900, local_time.tm_mon + 1, local_time.tm_mday, local_time.tm_hour, local_time.tm_min, local_time.tm_sec);
	fp = fopen(filename, "w");//"a"：以“追加”方式打开文件。"w"：以“清空后写入”方式打开文件。
	if (NULL == fp) {
		printf("fopen(filename) error!!!");
		return -1;
	}
	fclose(fp);
#endif

	printf("\r\n writeCalibrationFP Start! \r\n");
	////将double类型的数据转成十六进制数.
	ViUInt32 dataArray[106] = { 0 };//53*2=106（每个通道53个校准数据,每个数据分为2个32bit存入,有2个通道.）
	ViUInt32 hex_8bit1[256] = { 0 };//53*8=424（每个通道53个校准数据,viOut32每次写入8bit.）
	ViUInt32 hex_8bit2[168] = { 0 };//168=424-256
	// 424个数据分两次写,第一次写256个,第二次写168个.
	Data2IEEE754 converData;
	converData.double2IEEEHight = 0;
	converData.double2IEEELower = 0;
	int j = 0, jj = -1, jj2 = -1;
	for (size_t i = 0; i < numValues; i++)
	{
		doubledata2IEEE754(&converData, data[i]);//将一个浮点数转为IEEE754格式的定点数,高32位表示整数部分,低32位表示小数部分.
		if (jj <= 255)
		{
			dataArray[j] = converData.double2IEEEHight;
			j++;
			dataArray[j] = converData.double2IEEELower;
			j++;
			//将两个32bit的数据分成8个8bit的数据.
			//拆分操作
			jj++;
			hex_8bit1[jj] = (converData.double2IEEEHight >> 24) & 0xFF;
			jj++;
			hex_8bit1[jj] = (converData.double2IEEEHight >> 16) & 0xFF;
			jj++;
			hex_8bit1[jj] = (converData.double2IEEEHight >> 8) & 0xFF;
			jj++;
			hex_8bit1[jj] = converData.double2IEEEHight & 0xFF;
			//拆分操作
			jj++;
			hex_8bit1[jj] = (converData.double2IEEELower >> 24) & 0xFF;
			jj++;
			hex_8bit1[jj] = (converData.double2IEEELower >> 16) & 0xFF;
			jj++;
			hex_8bit1[jj] = (converData.double2IEEELower >> 8) & 0xFF;
			jj++;
			hex_8bit1[jj] = converData.double2IEEELower & 0xFF;
		}
		else
		{
			dataArray[j] = converData.double2IEEEHight;
			j++;
			dataArray[j] = converData.double2IEEELower;
			j++;
			//将两个32bit的数据分成8个8bit的数据.
			//拆分操作
			jj2++;
			hex_8bit2[jj2] = (converData.double2IEEEHight >> 24) & 0xFF;
			jj2++;
			hex_8bit2[jj2] = (converData.double2IEEEHight >> 16) & 0xFF;
			jj2++;
			hex_8bit2[jj2] = (converData.double2IEEEHight >> 8) & 0xFF;
			jj2++;
			hex_8bit2[jj2] = converData.double2IEEEHight & 0xFF;
			//拆分操作
			jj2++;
			hex_8bit2[jj2] = (converData.double2IEEELower >> 24) & 0xFF;
			jj2++;
			hex_8bit2[jj2] = (converData.double2IEEELower >> 16) & 0xFF;
			jj2++;
			hex_8bit2[jj2] = (converData.double2IEEELower >> 8) & 0xFF;
			jj2++;
			hex_8bit2[jj2] = converData.double2IEEELower & 0xFF;
		}

#ifdef Debug_readORwriteCaliFP_TXT
		uint64_t combine_result[66] = { 0 };//106=53*2;
		combine_result[i] = ((uint64_t)dataArray[j - 2] << 32) | dataArray[j - 1];//将两个32位整数拼接成一个64位整数.
		printf("double_Hight -- %x,,,", converData.double2IEEEHight);
		printf("double_Lower -- %x \n", converData.double2IEEELower);
		printf("    data[%d] -- %.9lf \n", i, data[i]);
		printf("hex[%d]: ", i);
		printf("0x%016llx, %x %x \n", combine_result[i], dataArray[j - 2], dataArray[j - 1]);
		////可选：保存到文件
		//FILE* fp = NULL;
		fp = fopen(filename, "a");//"a"：以“追加”方式打开文件。"w"：以“清空后写入”方式打开文件。
		if (NULL == fp) {
			printf("fopen(filename) error!!!");
			return -1;
		}
		else
		{
			fprintf(fp, "calidata[%d]-- %.9lf,\t data[%d] -- 0x%016llx \n", i, data[i], i, combine_result[i]);
			//fprintf(fp, "data[%d]-- %.9lf,\t 0x%x %x \n", i, data[i], dataArray[j - 2], dataArray[j - 1]);
			//if (jj < 256)
			//{
			//	fprintf(fp, "%x %x %x %x   %x %x %x %x  \n",
			//		hex_8bit1[jj - 7], hex_8bit1[jj - 6], hex_8bit1[jj - 5], hex_8bit1[jj - 4],
			//		hex_8bit1[jj - 3], hex_8bit1[jj - 2], hex_8bit1[jj - 1], hex_8bit1[jj - 0]);
			//}
			//if (jj2 > 0)
			//{
			//	fprintf(fp, "%x %x %x %x   %x %x %x %x  \n",
			//		hex_8bit2[jj2 - 7], hex_8bit2[jj2 - 6], hex_8bit2[jj2 - 5], hex_8bit2[jj2 - 4],
			//		hex_8bit2[jj2 - 3], hex_8bit2[jj2 - 2], hex_8bit2[jj2 - 1], hex_8bit2[jj2 - 0]);
			//	fprintf(fp, "--------------------------------\n");
			//}
		}
		fclose(fp);
#endif
		if (jj == 255)
		{
			jj = 256;
		}
	}
#ifdef Debug_readORwriteCaliFP_TXT
	//printf("\r\n doubledata2IEEE754 Finished! \r\n");
	fp = fopen(filename, "a");//"a"：以“追加”方式打开文件。"w"：以“清空后写入”方式打开文件。
	if (NULL == fp) {
		printf("fopen(filename) error!!!");
		return -1;
	}
	fprintf(fp, "-----------Finished-------------\n");
	fclose(fp);
#endif

	//写校准数据
	int datasize = 256;
	addr = (subUnit - 1) * 64 * 1024 + addr;
	//第一次写入.
	status = f_FlashWriteData(device, datasize, addr, hex_8bit1, 1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//第二次写入.
	//datasize = 136;//136=392-256
	datasize = 168;//168=424-256
	status = f_FlashWriteData(device, datasize, addr + 256, hex_8bit2, 2);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	printf("\r\n writeCalibrationFP Finished! \r\n");

	return status;
}

ViStatus MtPR__writeCalibrationFP_5171(ViSession vi, ViUInt32 subUnit,
	ViUInt32 store, ViUInt32 offset, ViUInt32 numValues, ViReal64 *data)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;//MT__EAI_DEV_NOTFOUND;//
	}

	ViUInt32 addr = 0x00800000;
	if (0 == store)
	{
		addr = SPI_FLASH_USER0_DATA_BASE;
	}
	else if (1 == store)
	{
		addr = SPI_FLASH_FACTORY1_DATA_BASE;
	}
	else
	{
	}

#ifdef Debug_readORwriteCaliFP_TXT
	//读取 SN number
	ViUInt32 serialNumber = 0;
	ViUInt64 FwVersion = 0;
	status = MtPR__readCardSNnumber(vi, subUnit, 1, &serialNumber, &FwVersion);
	if (status < VI_SUCCESS) {
		printf("读取设备序列号，错误代码: 0x%x\n", status);
	}
	else
	{
		printf("\r\r--------readSN = %lu,,,FwVersion = %llx--------\r\n", serialNumber, FwVersion);
	}
	////获取系统日期和时间
	struct tm local_time;
	//结构体初始化
	local_time.tm_sec = 0;     // seconds after the minute - [0,59]
	local_time.tm_min = 0;     // minutes after the hour - [0,59]
	local_time.tm_hour = 0;    // hours since midnight - [0,23]
	local_time.tm_mday = 1;    // day of the month - [1,31] 
	local_time.tm_mon = 0;     // months since January - [0,11] 
	local_time.tm_year = 0;    // years since 1900 
	local_time.tm_wday = 0;    // days since Sunday - [0,6] 
	local_time.tm_yday = 0;    // days since January 1 - [0,365] 
	local_time.tm_isdst = 0;   // daylight savings time flag 
	f_getSystemTime(local_time);
	printf("本地时间: %d-%02d-%02d %02d:%02d:%02d  \n\n",
		local_time.tm_year + 1900,  // 年份从1900开始计数
		local_time.tm_mon + 1,      // 月份范围0~11，需+1
		local_time.tm_mday,         // 日
		local_time.tm_hour,         // 时
		local_time.tm_min,          // 分
		local_time.tm_sec);         // 秒
	const char* dir_path = "..\\..\\calibration_result";
	int rtn = check_and_create_dir(dir_path);
	if (rtn != 0)
	{
		printf("创建目录失败！\n");
		return -1;
	}
	//保存校准数据 //可选：保存到文件
	FILE* fp = NULL;
	char filename[256];
	sprintf(filename, "..\\..\\calibration_result\\writeCalibrationF_Model0x%X_SN%lu_ch%d_%d%02d%02d%02d%02d%02d.txt", 
		device->mtModelName, serialNumber, subUnit, 
		local_time.tm_year + 1900, local_time.tm_mon + 1, local_time.tm_mday, local_time.tm_hour, local_time.tm_min, local_time.tm_sec);
	fp = fopen(filename, "w");//"a"：以“追加”方式打开文件。"w"：以“清空后写入”方式打开文件。
	if (NULL == fp) {
		printf("fopen(filename) error!!!");
		return -1;
	}
	fclose(fp);////
#endif

	printf("\r\n writeCalibrationFP Start! \r\n");
	////将double类型的数据转成十六进制数.
	ViUInt32 dataArray[66] = { 0 };//33*2=66
	ViUInt32 hex_8bit1[256] = { 0 };//33*64/8=264
	ViUInt32 hex_8bit2[8] = { 0 };//8=264-256
	// 264个数据分两次写,第一次写256个,第二次写8个.
	Data2IEEE754 converData;
	converData.double2IEEEHight = 0;
	converData.double2IEEELower = 0;
	int j = 0, jj = -1, jj2 = -1;
	for (size_t i = 0; i < numValues; i++)
	{
		//printf("data[%d] -- %.9lf \n", i, data[i]);

		doubledata2IEEE754(&converData, data[i]);
		if (jj <= 255)
		{
			dataArray[j] = converData.double2IEEEHight;
			j++;
			dataArray[j] = converData.double2IEEELower;
			j++;
			//将两个32bit的数据分成8个8bit的数据.
			//拆分操作
			jj++;
			hex_8bit1[jj] = (converData.double2IEEEHight >> 24) & 0xFF;
			jj++;
			hex_8bit1[jj] = (converData.double2IEEEHight >> 16) & 0xFF;
			jj++;
			hex_8bit1[jj] = (converData.double2IEEEHight >> 8) & 0xFF;
			jj++;
			hex_8bit1[jj] = converData.double2IEEEHight & 0xFF;
			//拆分操作
			jj++;
			hex_8bit1[jj] = (converData.double2IEEELower >> 24) & 0xFF;
			jj++;
			hex_8bit1[jj] = (converData.double2IEEELower >> 16) & 0xFF;
			jj++;
			hex_8bit1[jj] = (converData.double2IEEELower >> 8) & 0xFF;
			jj++;
			hex_8bit1[jj] = converData.double2IEEELower & 0xFF;
		}
		else
		{
			//printf(",,,,,,,,,jj -- %d,,,,,,,,,", jj);
			dataArray[j] = converData.double2IEEEHight;
			j++;
			dataArray[j] = converData.double2IEEELower;
			j++;
			//将两个32bit的数据分成8个8bit的数据.
			//拆分操作
			jj2++;
			hex_8bit2[jj2] = (converData.double2IEEEHight >> 24) & 0xFF;
			jj2++;
			hex_8bit2[jj2] = (converData.double2IEEEHight >> 16) & 0xFF;
			jj2++;
			hex_8bit2[jj2] = (converData.double2IEEEHight >> 8) & 0xFF;
			jj2++;
			hex_8bit2[jj2] = converData.double2IEEEHight & 0xFF;
			//拆分操作
			jj2++;
			hex_8bit2[jj2] = (converData.double2IEEELower >> 24) & 0xFF;
			jj2++;
			hex_8bit2[jj2] = (converData.double2IEEELower >> 16) & 0xFF;
			jj2++;
			hex_8bit2[jj2] = (converData.double2IEEELower >> 8) & 0xFF;
			jj2++;
			hex_8bit2[jj2] = converData.double2IEEELower & 0xFF;
		}

#ifdef Debug_readORwriteCaliFP_TXT
		uint64_t combine_result[66] = { 0 };//106=53*2;
		combine_result[i] = ((uint64_t)dataArray[j - 2] << 32) | dataArray[j - 1];//将两个32位整数拼接成一个64位整数.
		printf("double_Hight -- %x,,,", converData.double2IEEEHight);
		printf("double_Lower -- %x \n", converData.double2IEEELower);
		printf("    data[%d] -- %.9lf \n", i, data[i]);
		printf("hex[%d]: ", i);
		printf("0x%016llx, %x %x \n", combine_result[i], dataArray[j - 2], dataArray[j - 1]);
		////可选：保存到文件
		//FILE* fp = NULL;
		fp = fopen(filename, "a");//"a"：以“追加”方式打开文件。"w"：以“清空后写入”方式打开文件。
		if (NULL == fp) {
			printf("fopen(filename) error!!!");
			return -1;
		}
		else
		{
			fprintf(fp, "calidata[%d]-- %.9lf,\t data[%d] -- 0x%016llx \n", i, data[i], i, combine_result[i]);
			//fprintf(fp, "data[%d]-- %.9lf,\t 0x%x %x \n", i, data[i], dataArray[j - 2], dataArray[j - 1]);
			//if (jj < 256)
			//{
			//	fprintf(fp, "%x %x %x %x   %x %x %x %x  \n",
			//		hex_8bit1[jj - 7], hex_8bit1[jj - 6], hex_8bit1[jj - 5], hex_8bit1[jj - 4],
			//		hex_8bit1[jj - 3], hex_8bit1[jj - 2], hex_8bit1[jj - 1], hex_8bit1[jj - 0]);
			//}
			//if (jj2 > 0)
			//{
			//	fprintf(fp, "%x %x %x %x   %x %x %x %x  \n",
			//		hex_8bit2[jj2 - 7], hex_8bit2[jj2 - 6], hex_8bit2[jj2 - 5], hex_8bit2[jj2 - 4],
			//		hex_8bit2[jj2 - 3], hex_8bit2[jj2 - 2], hex_8bit2[jj2 - 1], hex_8bit2[jj2 - 0]);
			//	fprintf(fp, "--------------------------------\n");
			//}
		}
		fclose(fp);
#endif
		if (jj == 255)
		{
			jj = 256;
		}
	}
#ifdef Debug_readORwriteCaliFP_TXT
	//printf("\r\n doubledata2IEEE754 Finished! \r\n");
	fp = fopen(filename, "a");//"a"：以“追加”方式打开文件。"w"：以“清空后写入”方式打开文件。
	if (NULL == fp) {
		printf("fopen(filename) error!!!");
		return -1;
	}
	fprintf(fp, "-----------Finished-------------\n");
	fclose(fp);
#endif

	//写校准数据
	int datasize = 256;
	addr = (subUnit - 1) * 64 * 1024 + addr;
	//第一次写入.
	status = f_FlashWriteData(device, datasize, addr, hex_8bit1, 1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//第二次写入.
	datasize = 8;//168=264-256
	status = f_FlashWriteData(device, datasize, addr + 256, hex_8bit2, 2);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}

	printf("\r\n writeCalibrationFP Finished! \r\n");
	return status;
}

ViStatus MtPR__writeCalibrationFP_5172(ViSession vi, ViUInt32 subUnit,
	ViUInt32 store, ViUInt32 offset, ViUInt32 numValues, ViReal64 *data)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;//MT__EAI_DEV_NOTFOUND;//
	}

	ViUInt32 addr = 0x00800000;
	if (0 == store)
	{
		addr = SPI_FLASH_USER0_DATA_BASE;
	}
	else if (1 == store)
	{
		addr = SPI_FLASH_FACTORY1_DATA_BASE;
	}
	else
	{
	}

#ifdef Debug_readORwriteCaliFP_TXT
	//读取 SN number
	ViUInt32 serialNumber = 0;
	ViUInt64 FwVersion = 0;
	status = MtPR__readCardSNnumber(vi, subUnit, 1, &serialNumber, &FwVersion);
	if (status < VI_SUCCESS) {
		printf("读取设备序列号，错误代码: 0x%x\n", status);
	}
	else
	{
		printf("\r\r--------readSN = %lu,,,FwVersion = %llx--------\r\n", serialNumber, FwVersion);
	}
	////获取系统日期和时间
	struct tm local_time;
	//结构体初始化
	local_time.tm_sec = 0;     // seconds after the minute - [0,59]
	local_time.tm_min = 0;     // minutes after the hour - [0,59]
	local_time.tm_hour = 0;    // hours since midnight - [0,23]
	local_time.tm_mday = 1;    // day of the month - [1,31] 
	local_time.tm_mon = 0;     // months since January - [0,11] 
	local_time.tm_year = 0;    // years since 1900 
	local_time.tm_wday = 0;    // days since Sunday - [0,6] 
	local_time.tm_yday = 0;    // days since January 1 - [0,365] 
	local_time.tm_isdst = 0;   // daylight savings time flag 
	f_getSystemTime(local_time);
	printf("本地时间: %d-%02d-%02d %02d:%02d:%02d  \n\n",
		local_time.tm_year + 1900,  // 年份从1900开始计数
		local_time.tm_mon + 1,      // 月份范围0~11，需+1
		local_time.tm_mday,         // 日
		local_time.tm_hour,         // 时
		local_time.tm_min,          // 分
		local_time.tm_sec);         // 秒
	const char* dir_path = "..\\..\\calibration_result";
	int rtn = check_and_create_dir(dir_path);
	if (rtn != 0)
	{
		printf("创建目录失败！\n");
		return -1;
}
	//保存校准数据 //可选：保存到文件
	FILE* fp = NULL;
	char filename[256];
	sprintf(filename, "..\\..\\calibration_result\\writeCalibrationF_Model0x%X_SN%lu_ch%d_%d%02d%02d%02d%02d%02d.txt", 
		device->mtModelName, serialNumber, subUnit, 
		local_time.tm_year + 1900, local_time.tm_mon + 1, local_time.tm_mday, local_time.tm_hour, local_time.tm_min, local_time.tm_sec);
	fp = fopen(filename, "w");//"a"：以“追加”方式打开文件。"w"：以“清空后写入”方式打开文件。
	if (NULL == fp) {
		printf("fopen(filename) error!!!");
		return -1;
	}
	fclose(fp);
#endif

	printf("\r\n writeCalibrationFP Start! \r\n");
	////将double类型的数据转成十六进制数.
	ViUInt32 dataArray[66] = { 0 };//33*2=66
	ViUInt32 hex_8bit1[256] = { 0 };//33*64/8=264
	ViUInt32 hex_8bit2[8] = { 0 };//8=264-256
	// 264个数据分两次写,第一次写256个,第二次写8个.
	Data2IEEE754 converData;
	converData.double2IEEEHight = 0;
	converData.double2IEEELower = 0;
	int j = 0, jj = -1, jj2 = -1;
	for (size_t i = 0; i < numValues; i++)
	{
		//printf("data[%d] -- %.9lf \n", i, data[i]);

		doubledata2IEEE754(&converData, data[i]);
		if (jj <= 255)
		{
			dataArray[j] = converData.double2IEEEHight;
			j++;
			dataArray[j] = converData.double2IEEELower;
			j++;
			//将两个32bit的数据分成8个8bit的数据.
			//拆分操作
			jj++;
			hex_8bit1[jj] = (converData.double2IEEEHight >> 24) & 0xFF;
			jj++;
			hex_8bit1[jj] = (converData.double2IEEEHight >> 16) & 0xFF;
			jj++;
			hex_8bit1[jj] = (converData.double2IEEEHight >> 8) & 0xFF;
			jj++;
			hex_8bit1[jj] = converData.double2IEEEHight & 0xFF;
			//拆分操作
			jj++;
			hex_8bit1[jj] = (converData.double2IEEELower >> 24) & 0xFF;
			jj++;
			hex_8bit1[jj] = (converData.double2IEEELower >> 16) & 0xFF;
			jj++;
			hex_8bit1[jj] = (converData.double2IEEELower >> 8) & 0xFF;
			jj++;
			hex_8bit1[jj] = converData.double2IEEELower & 0xFF;
		}
		else
		{
			//printf(",,,,,,,,,jj -- %d,,,,,,,,,", jj);
			dataArray[j] = converData.double2IEEEHight;
			j++;
			dataArray[j] = converData.double2IEEELower;
			j++;
			//将两个32bit的数据分成8个8bit的数据.
			//拆分操作
			jj2++;
			hex_8bit2[jj2] = (converData.double2IEEEHight >> 24) & 0xFF;
			jj2++;
			hex_8bit2[jj2] = (converData.double2IEEEHight >> 16) & 0xFF;
			jj2++;
			hex_8bit2[jj2] = (converData.double2IEEEHight >> 8) & 0xFF;
			jj2++;
			hex_8bit2[jj2] = converData.double2IEEEHight & 0xFF;
			//拆分操作
			jj2++;
			hex_8bit2[jj2] = (converData.double2IEEELower >> 24) & 0xFF;
			jj2++;
			hex_8bit2[jj2] = (converData.double2IEEELower >> 16) & 0xFF;
			jj2++;
			hex_8bit2[jj2] = (converData.double2IEEELower >> 8) & 0xFF;
			jj2++;
			hex_8bit2[jj2] = converData.double2IEEELower & 0xFF;
		}

#ifdef Debug_readORwriteCaliFP_TXT
		uint64_t combine_result[66] = { 0 };//106=53*2;
		combine_result[i] = ((uint64_t)dataArray[j - 2] << 32) | dataArray[j - 1];//将两个32位整数拼接成一个64位整数.
		printf("double_Hight -- %x,,,", converData.double2IEEEHight);
		printf("double_Lower -- %x \n", converData.double2IEEELower);
		printf("    data[%d] -- %.9lf \n", i, data[i]);
		printf("hex[%d]: ", i);
		printf("0x%016llx, %x %x \n", combine_result[i], dataArray[j - 2], dataArray[j - 1]);
		////可选：保存到文件
		//FILE* fp = NULL;
		fp = fopen(filename, "a");//"a"：以“追加”方式打开文件。"w"：以“清空后写入”方式打开文件。
		if (NULL == fp) {
			printf("fopen(filename) error!!!");
			return -1;
		}
		else
		{
			fprintf(fp, "calidata[%d]-- %.9lf,\t data[%d] -- 0x%016llx \n", i, data[i], i, combine_result[i]);
			//fprintf(fp, "data[%d]-- %.9lf,\t 0x%x %x \n", i, data[i], dataArray[j - 2], dataArray[j - 1]);
			//if (jj < 256)
			//{
			//	fprintf(fp, "%x %x %x %x   %x %x %x %x  \n",
			//		hex_8bit1[jj - 7], hex_8bit1[jj - 6], hex_8bit1[jj - 5], hex_8bit1[jj - 4],
			//		hex_8bit1[jj - 3], hex_8bit1[jj - 2], hex_8bit1[jj - 1], hex_8bit1[jj - 0]);
			//}
			//if (jj2 > 0)
			//{
			//	fprintf(fp, "%x %x %x %x   %x %x %x %x  \n",
			//		hex_8bit2[jj2 - 7], hex_8bit2[jj2 - 6], hex_8bit2[jj2 - 5], hex_8bit2[jj2 - 4],
			//		hex_8bit2[jj2 - 3], hex_8bit2[jj2 - 2], hex_8bit2[jj2 - 1], hex_8bit2[jj2 - 0]);
			//	fprintf(fp, "--------------------------------\n");
			//}
		}
		fclose(fp);
#endif
		if (jj == 255)
		{
			jj = 256;
		}
	}
#ifdef Debug_readORwriteCaliFP_TXT
	//printf("\r\n doubledata2IEEE754 Finished! \r\n");
	fp = fopen(filename, "a");//"a"：以“追加”方式打开文件。"w"：以“清空后写入”方式打开文件。
	if (NULL == fp) {
		printf("fopen(filename) error!!!");
		return -1;
	}
	fprintf(fp, "-----------Finished-------------\n");
	fclose(fp);
#endif

	//写校准数据
	int datasize = 256;
	addr = (subUnit - 1) * 64 * 1024 + addr;
	//第一次写入.
	status = f_FlashWriteData(device, datasize, addr, hex_8bit1, 1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//第二次写入.
	datasize = 8;//168=264-256
	status = f_FlashWriteData(device, datasize, addr + 256, hex_8bit2, 2);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}

	printf("\r\n writeCalibrationFP Finished! \r\n");
	return status;
}
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
	ViUInt32 store, ViUInt32 offset, ViUInt32 numValues, ViReal64 *data)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;
	}

	//程控电阻:0x70A-->MT5170B-01; 0x70C-->MT5170B-02; 0x70B-->MT5171; 0x70D-->MT5172; 
	if (0x70A == device->mtModelName || 0x70C == device->mtModelName)
	{
		status = MtPR__writeCalibrationFP_5170B(vi, subUnit, store, offset, numValues, data);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else if (0x70B == device->mtModelName)
	{
		status = MtPR__writeCalibrationFP_5171(vi, subUnit, store, offset, numValues, data);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else if (0x70D == device->mtModelName)
	{
		status = MtPR__writeCalibrationFP_5172(vi, subUnit, store, offset, numValues, data);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else
	{
		device->errorCode = MTPR__ERROR_BAD_SESSION;
		return device->errorCode;
	}

	return status;
}


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
	ViUInt32 store, ViUInt32 offset, ViUInt32 numValues, ViReal64 *dataArray)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;
	}

	ViUInt32 addr = 0x00800000;
	if (0 == store)
	{
		addr = SPI_FLASH_USER0_DATA_BASE;
	}
	else
	{
		addr = SPI_FLASH_FACTORY1_DATA_BASE;
	}

	int k = -1;
	uint64_t combine_result[53] = { 0 };//每8bit一组进行组合.
	if (0x70A == device->mtModelName || 0x70C == device->mtModelName)
	{//0x70A-->MT5170B-01; 0x70C-->MT5170B-02;
		if (subUnit > 2 || subUnit < 1)
		{
			device->errorCode = MTPR__ERROR_BAD_SUB;
			return device->errorCode;
		}
		//读取数据
		double calidata[53] = { 0 };
		ViUInt32 arraydata[424] = { 0 };//424=53*8
		addr = (subUnit - 1) * 64 * 1024 + addr;
		status = FlashReadData(device, 424, addr, arraydata);//OK
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		for (int i = 0; i < 424; i += 8)
		{
			uint64_t result = 0;
			for (size_t j = 0; j < 8; j++)
			{
				result = result | static_cast<unsigned long long>(arraydata[i + j]) << ((7 - j) * 8);
			}
			k++;
			calidata[k] = ieee754_to_double(result);
			combine_result[k] = result;
			dataArray[k] = calidata[k];
		}
	}
	else if (0x70B == device->mtModelName)
	{//0x70B-->MT5171; 
		if (subUnit > 6 || subUnit < 1)
		{
			device->errorCode = MTPR__ERROR_BAD_SUB;
			return device->errorCode;
		}
		//读取数据
		double calidata[33] = { 0 };
		ViUInt32 arraydata[264] = { 0 };//264=33*8
		addr = (subUnit - 1) * 64 * 1024 + addr;
		status = FlashReadData(device, 264, addr, arraydata);//OK
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		for (int i = 0; i < 264; i += 8)
		{
			uint64_t result = 0;
			for (size_t j = 0; j < 8; j++)
			{
				result = result | static_cast<unsigned long long>(arraydata[i + j]) << ((7 - j) * 8);
			}
			k++;
			calidata[k] = ieee754_to_double(result);
			combine_result[k] = result;
			dataArray[k] = calidata[k];
		}
	}
	else if (0x70D == device->mtModelName)
	{//0x70D-->MT5172; 
		if (subUnit > 12 || subUnit < 1)
		{
			device->errorCode = MTPR__ERROR_BAD_SUB;
			return device->errorCode;
		}
		//读取数据
		double calidata[33] = { 0 };
		ViUInt32 arraydata[264] = { 0 };//264=33*8
		addr = (subUnit - 1) * 64 * 1024 + addr;
		status = FlashReadData(device, 264, addr, arraydata);//OK
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		for (int i = 0; i < 264; i += 8)
		{
			uint64_t result = 0;
			for (size_t j = 0; j < 8; j++)
			{
				result = result | static_cast<unsigned long long>(arraydata[i + j]) << ((7 - j) * 8);
			}
			k++;
			calidata[k] = ieee754_to_double(result);
			combine_result[k] = result;
			dataArray[k] = calidata[k];
		}
	}
	else
	{
		device->errorCode = MTPR__ERROR_BAD_SESSION;
		return device->errorCode;
	}

#ifdef Debug_readORwriteCaliFP_TXT
	//读取 SN number
	ViUInt32 serialNumber = 0;
	ViUInt64 FwVersion = 0;
	status = MtPR__readCardSNnumber(vi, subUnit, 1, &serialNumber, &FwVersion);
	if (status < VI_SUCCESS) {
		printf("读取设备序列号，错误代码: 0x%x\n", status);
	}
	else
	{
		printf("\r\r--------readSN = %lu,,,FwVersion = %llx--------\r\n", serialNumber, FwVersion);
	}
	//static int calCNT = 0;
	//if (0 == calCNT)
	{
		//获取系统日期和时间
		struct tm g_local_time;
		//结构体初始化
		g_local_time.tm_sec = 0;     // seconds after the minute - [0,59]
		g_local_time.tm_min = 0;     // minutes after the hour - [0,59]
		g_local_time.tm_hour = 0;    // hours since midnight - [0,23]
		g_local_time.tm_mday = 1;    // day of the month - [1,31] 
		g_local_time.tm_mon = 0;     // months since January - [0,11] 
		g_local_time.tm_year = 0;    // years since 1900 
		g_local_time.tm_wday = 0;    // days since Sunday - [0,6] 
		g_local_time.tm_yday = 0;    // days since January 1 - [0,365] 
		g_local_time.tm_isdst = 0;   // daylight savings time flag 
		f_getSystemTime(g_local_time);
		printf("本地时间: %d-%02d-%02d %02d:%02d:%02d  \n\n",
			g_local_time.tm_year + 1900,  // 年份从1900开始计数
			g_local_time.tm_mon + 1,      // 月份范围0~11，需+1
			g_local_time.tm_mday,         // 日
			g_local_time.tm_hour,         // 时
			g_local_time.tm_min,          // 分
			g_local_time.tm_sec);         // 秒
		const char* dir_path = "..\\..\\calibration_result";
		int rtn = check_and_create_dir(dir_path);
		if (rtn != 0)
		{
			printf("创建目录失败！\n");
			return -1;
		}
		//保存校准数据 //可选：保存到文件
		FILE* fp = NULL;
		char filename[256];
		sprintf(filename, "..\\..\\calibration_result\\readCalibrationF_iModel0x%X_SN%lu_ch%d_%d%02d%02d%02d%02d%02d.txt", 
			device->mtModelName, serialNumber, subUnit, 
			g_local_time.tm_year + 1900, g_local_time.tm_mon + 1, g_local_time.tm_mday, g_local_time.tm_hour, g_local_time.tm_min, g_local_time.tm_sec);
		fp = fopen(filename, "w");//"a"：以“追加”方式打开文件。"w"：以“清空后写入”方式打开文件。
		if (NULL == fp) {
			printf("fopen(filename) error!!!");
			return -1;
		}
		//每8个一组进行组合.
		for (size_t i = 0; i < numValues; i++)
		{
			//fprintf(fp, "calidata[%d]-- %.9lf \n", i, dataArray[i]);
			fprintf(fp, "calidata[%d]-- %.9lf,\t data[%d] -- 0x%016llx \n", i, dataArray[i], i, combine_result[i]);
			////可选：保存到文件
			//FILE* fptxt = NULL;
			//fptxt = fopen("readCalibrationF.txt", "a");
			//if (NULL == fptxt) {
			//	printf("fopen(filename) error!!!");
			//	return;
			//}
			//fprintf(fptxt, "data[%d] -- 0x%016llx \n", i, combine_result[i]);//64位十六进制数.完整输出 64位十六进制数.
			//fclose(fptxt);
		}
		fclose(fp);
	}
#endif

	return status;
}

ViStatus MtPR__InitReadCalibrationFP(ViSession vi, ViUInt32 subUnit,
	ViUInt32 store, ViUInt32 offset, ViUInt32 numValues, ViReal64* dataArray)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;
	}

	ViUInt32 addr = 0x00800000;
	if (0 == store)
	{
		addr = SPI_FLASH_USER0_DATA_BASE;
	}
	else
	{
		addr = SPI_FLASH_FACTORY1_DATA_BASE;
	}

	int kk = -1;
	ViUInt32 subUnitNum = 1;
	ViUInt32 caliValueNum = 1;
	uint64_t combine_result[396] = { 0 };//最多396个数据.
	if (0x70A == device->mtModelName || 0x70C == device->mtModelName)
	{//0x70A-->MT5170B-01; 0x70C-->MT5170B-02;
		if (subUnit > 2 || subUnit < 1)
		{
			device->errorCode = MTPR__ERROR_BAD_SUB;
			return device->errorCode;
		}
		subUnitNum = 2;
		caliValueNum = CALIBRATION_NUM_5170B* subUnitNum;
		//批量读取数据
		double calidata[106] = { 0 };//每个通道53个校准数据,有2个通道.
		ViUInt32 arraydata[212] = { 0 };//每个通道53个校准数据,每个数据分为2个32bit存入,有2个通道.
		addr = MTDCPOWER_ATTR_POWERON_READ_FACTOR;
		status = FlashReadData_BatchFetch(device, 212, addr, arraydata);//同时读取5170B的2个通道的校准数据.
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//uint64_t combine_result[106] = { 0 };//每个通道53个校准数据,有2个通道.
		for (int i = 0; i < 211; i += 2)
		{
			uint64_t result = merge_reverse_bytes(arraydata[i], arraydata[i + 1]);
			kk++;
			calidata[kk] = ieee754_to_double(result);
			combine_result[kk] = result;
			dataArray[kk] = calidata[kk];
		}
	}
	else if (0x70B == device->mtModelName)
	{//0x70B-->MT5171; 
		if (subUnit > 6 || subUnit < 1)
		{
			device->errorCode = MTPR__ERROR_BAD_SUB;
			return device->errorCode;
		}
		subUnitNum = 6;
		caliValueNum = CALIBRATION_NUM_5171* subUnitNum;
		//批量读取数据
		double calidata[198] = { 0 };//每个通道33个校准数据,有6个通道.
		ViUInt32 arraydata[396] = { 0 };//每个通道33个校准数据,每个数据分为2个32bit存入,有6个通道.
		addr = MTDCPOWER_ATTR_POWERON_READ_FACTOR;
		status = FlashReadData_BatchFetch(device, 396, addr, arraydata);//同时读取5171的6个通道的校准数据.
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//uint64_t combine_result[198] = { 0 };//每个通道33个校准数据,有6个通道.
		for (int i = 0; i < 395; i += 2)
		{
			uint64_t result = merge_reverse_bytes(arraydata[i], arraydata[i + 1]);
			kk++;
			calidata[kk] = ieee754_to_double(result);
			combine_result[kk] = result;
			dataArray[kk] = calidata[kk];
		}
	}
	else if (0x70D == device->mtModelName)
	{//0x70D-->MT5172; 
		if (subUnit > 12 || subUnit < 1)
		{
			device->errorCode = MTPR__ERROR_BAD_SUB;
			return device->errorCode;
		}
		subUnitNum = 12;
		caliValueNum = CALIBRATION_NUM_5172* subUnitNum;
		//批量读取数据
		double calidata[396] = { 0 };//每个通道33个校准数据,有12个通道.
		ViUInt32 arraydata[792] = { 0 };//每个通道33个校准数据,每个数据分为2个32bit存入,有12个通道.
		addr = MTDCPOWER_ATTR_POWERON_READ_FACTOR;
		status = FlashReadData_BatchFetch(device, 792, addr, arraydata);//同时读取5172的12个通道的校准数据.
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//uint64_t combine_result[396] = { 0 };//每个通道33个校准数据,有12个通道.
		for (int i = 0; i < 792; i += 2)
		{
			uint64_t result = merge_reverse_bytes(arraydata[i], arraydata[i + 1]);
			kk++;
			calidata[kk] = ieee754_to_double(result);
			combine_result[kk] = result;
			dataArray[kk] = calidata[kk];
		}
	}
	else
	{
		device->errorCode = MTPR__ERROR_BAD_SESSION;
		return device->errorCode;
	}

#ifdef Debug_readORwriteCaliFP_TXT
	//读取 SN number
	ViUInt32 serialNumber = 0;
	ViUInt64 FwVersion = 0;
	status = MtPR__readCardSNnumber(vi, subUnit, 1, &serialNumber, &FwVersion);
	if (status < VI_SUCCESS) {
		printf("读取设备序列号，错误代码: 0x%x\n", status);
	}
	else
	{
		printf("\r\r--------readSN = %lu,,,FwVersion = %llx--------\r\n", serialNumber, FwVersion);
	}
	//static int calCNT = 0;
	//if (0 == calCNT)
	{
		//获取系统日期和时间
		struct tm g_local_time;
		//结构体初始化
		g_local_time.tm_sec = 0;     // seconds after the minute - [0,59]
		g_local_time.tm_min = 0;     // minutes after the hour - [0,59]
		g_local_time.tm_hour = 0;    // hours since midnight - [0,23]
		g_local_time.tm_mday = 1;    // day of the month - [1,31] 
		g_local_time.tm_mon = 0;     // months since January - [0,11] 
		g_local_time.tm_year = 0;    // years since 1900 
		g_local_time.tm_wday = 0;    // days since Sunday - [0,6] 
		g_local_time.tm_yday = 0;    // days since January 1 - [0,365] 
		g_local_time.tm_isdst = 0;   // daylight savings time flag 
		f_getSystemTime(g_local_time);
		printf("本地时间: %d-%02d-%02d %02d:%02d:%02d  \n\n",
			g_local_time.tm_year + 1900,  // 年份从1900开始计数
			g_local_time.tm_mon + 1,      // 月份范围0~11，需+1
			g_local_time.tm_mday,         // 日
			g_local_time.tm_hour,         // 时
			g_local_time.tm_min,          // 分
			g_local_time.tm_sec);         // 秒
		const char* dir_path = "..\\..\\calibration_result";
		int rtn = check_and_create_dir(dir_path);
		if (rtn != 0)
		{
			printf("创建目录失败！\n");
			return -1;
		}
		//保存校准数据 //可选：保存到文件
		FILE* fp = NULL;
		char filename[256];
		sprintf(filename, "..\\..\\calibration_result\\Init_readCalibrationF_iModel0x%X_SN%lu_ch%d_%d%02d%02d%02d%02d%02d.txt", 
			device->mtModelName, serialNumber, subUnit, 
			g_local_time.tm_year + 1900, g_local_time.tm_mon + 1, g_local_time.tm_mday, g_local_time.tm_hour, g_local_time.tm_min, g_local_time.tm_sec);
		fp = fopen(filename, "w");//"a"：以“追加”方式打开文件。"w"：以“清空后写入”方式打开文件。
		if (NULL == fp) {
			printf("fopen(filename) error!!!");
			return -1;
		}
		//每8个一组进行组合.
		for (size_t j = 0, i = 0; j < caliValueNum; j++, i++)
		{
			//fprintf(fp, "calidata[%d]-- %.9lf \n", j, dataArray[j]);
			fprintf(fp, "calidata[%d]-- %.9lf,\t data[%d] -- 0x%016llx \n", i, dataArray[j], i, combine_result[j]);
			////可选：保存到文件
			//FILE* fptxt = NULL;
			//fptxt = fopen("readCalibrationF.txt", "a");
			//if (NULL == fptxt) {
			//	printf("fopen(filename) error!!!");
			//	return;
			//}
			//fprintf(fptxt, "data[%d] -- 0x%016llx \n", i, combine_result[i]);//64位十六进制数.完整输出 64位十六进制数.
			//fclose(fptxt);

			if (0 == ((j + 1) % numValues)) {
				i = -1;
				fprintf(fp, "--------------------\n");
			}
		}
		fclose(fp);
	}
#endif

	return status;
}

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
	ViUInt32 store, ViUInt32 interval)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;//MT__EAI_DEV_NOTFOUND;//
	}


	//获取系统日期和时间
	struct tm local_time;
	//结构体初始化
	local_time.tm_sec = 0;     /* seconds after the minute - [0,59] */
	local_time.tm_min = 0;     /* minutes after the hour - [0,59] */
	local_time.tm_hour = 0;    /* hours since midnight - [0,23] */
	local_time.tm_mday = 1;    /* day of the month - [1,31] */
	local_time.tm_mon = 0;     /* months since January - [0,11] */
	local_time.tm_year = 0;    /* years since 1900 */
	local_time.tm_wday = 0;    /* days since Sunday - [0,6] */
	local_time.tm_yday = 0;    /* days since January 1 - [0,365] */
	local_time.tm_isdst = 0;   /* daylight savings time flag */
	f_getSystemTime(local_time);
	//printf("本地时间: %d-%02d-%02d %02d:%02d:%02d  \n\n",
	//	local_time.tm_year + 1900,  // 年份从1900开始计数
	//	local_time.tm_mon + 1,      // 月份范围0~11，需+1
	//	local_time.tm_mday,         // 日
	//	local_time.tm_hour,         // 时
	//	local_time.tm_min,          // 分
	//	local_time.tm_sec);         // 秒

	//写校准日期
	ViUInt32 yeardata = 0;
	ViUInt32 monthdata = 0;
	ViUInt32 datedata = 0;
	ViUInt32 timedata = 0;
	yeardata = local_time.tm_year + 1900;//2025
	monthdata = (local_time.tm_mon + 1) * 100 + local_time.tm_mday;//0403
	datedata = yeardata * 10000 + monthdata;
	timedata = local_time.tm_hour * 100 + local_time.tm_min;//1715

	ViUInt32 addr = SPI_FLASH_USER0_DATA_BASE;
	if (0 == store)
	{
		addr = SPI_FLASH_USER0_DATA_BASE;
	}
	else
	{
		addr = SPI_FLASH_FACTORY1_DATA_BASE;
	}

	ViUInt32 hex_8bit1[12] = { 0 };//12=3*32/8
	int jj = -1;
	//将一个32bit的数据分成4个8bit的数据.
	//拆分操作
	jj++;
	hex_8bit1[jj] = (yeardata >> 24) & 0xFF;
	jj++;
	hex_8bit1[jj] = (yeardata >> 16) & 0xFF;
	jj++;
	hex_8bit1[jj] = (yeardata >> 8) & 0xFF;
	jj++;
	hex_8bit1[jj] = yeardata & 0xFF;
	//拆分操作
	jj++;
	hex_8bit1[jj] = (monthdata >> 24) & 0xFF;
	jj++;
	hex_8bit1[jj] = (monthdata >> 16) & 0xFF;
	jj++;
	hex_8bit1[jj] = (monthdata >> 8) & 0xFF;
	jj++;
	hex_8bit1[jj] = monthdata & 0xFF;
	//拆分操作
	jj++;
	hex_8bit1[jj] = (interval >> 24) & 0xFF;
	jj++;
	hex_8bit1[jj] = (interval >> 16) & 0xFF;
	jj++;
	hex_8bit1[jj] = (interval >> 8) & 0xFF;
	jj++;
	hex_8bit1[jj] = interval & 0xFF;


	int datasize = 12;
	addr = (subUnit + 2) * 64 * 1024 + addr;
	status = f_FlashWriteData(device, datasize, addr, hex_8bit1, 1);//第一次写入.
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//printf("\r\n jj=%d -------------writeCalibrationDate-----END-------------\n", jj);

	return status;
}

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
	ViUInt32 store, ViPUInt32 year, ViPUInt32 day, ViPUInt32 interval)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;
	}

	ViUInt32 addr = SPI_FLASH_USER0_DATA_BASE;
	if (0 == store)
	{
		addr = SPI_FLASH_USER0_DATA_BASE;
	}
	else
	{
		addr = SPI_FLASH_FACTORY1_DATA_BASE;
	}

	//读取数据
	int datasize = 12;//12=3*8/2
	ViUInt32 arraydata[12] = { 0 };//12=3*8/2
	addr = (subUnit + 2) * 64 * 1024 + addr;
	status = FlashReadData(device, datasize, addr, arraydata);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}

	int k = -1;
	//每4个一组进行组合.
	ViUInt32 combine_result[3] = { 0 };
	for (int i = 0; i < datasize; i += 4)
	{
		ViUInt32 result = 0;
		for (size_t j = 0; j < 4; j++)
		{
			result = result | static_cast<unsigned long>(arraydata[i + j]) << ((3 - j) * 8);
		}
		k++;
		combine_result[k] = result;
	}
	*year = combine_result[0];
	*day = combine_result[1];
	*interval = combine_result[2];

	return status;
}


//**************************************************************************
// Programmable resistor specific functions.
//**************************************************************************
//界面开关控制：通过设置界面的开关输出相应的电阻值.
ViStatus MtPR__setSwitchGetResistance(ViSession vi, ViUInt32 subUnit, int switchState[64])
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;//MT__EAI_DEV_NOTFOUND;//
	}

	char* hexStr = NULL;
	ViUInt64 value = 0;
	ViUInt32 high32 = 0;
	ViUInt32 low32 = 0;
	int length = 64;
	ViUInt32 set_pattern[2] = { 0x0, 0x0 };//十六进制表示40个开关的闭合情况.

	hexStr = binaryToHex(switchState, length);
	if (hexStr == NULL)
	{
		//printf(" binaryArrayToHex ERROR. ");
		device->errorCode = MTPR__ERROR_INV_PARAMETER;
		return device->errorCode;
	}
	value = strtoull(hexStr, NULL, 16);//转换为64位整数
	//提取高32位和低32位
	high32 = (ViUInt32)(value >> 32);
	low32 = (ViUInt32)(value & 0xFFFFFFFF);
	//printf("   switchState: high32 = %lu,low32 = %lu.\n", high32, low32);
	//设置需要闭合的继电器bit.
	set_pattern[0] = low32;
	set_pattern[1] = high32;
	free(hexStr);//释放内存

	//设置子单元所有输出状态.
	status = MtPR__setChannelPattern(vi, subUnit, set_pattern);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	/*///-------test 获取子单元所有输出状态.-------////
	//ViUInt32 get_pattern[2] = { 0, 0 };
	//status = MtPR__getChannelPattern(vi, subUnit, get_pattern);//OK
	//printf(" get_switchState: high32 = %lu,low32 = %lu.\n", get_pattern[1], get_pattern[0]);

	ViReal64 get_resistance = 0;
	status = MtPR__resGetResistance( vi,  subUnit, &get_resistance);
	////-------test 获取子单元所有输出状态.-------///*/

	return status;
}


ViStatus MtPR__resSetResistance_5170B(ViSession vi, ViUInt32 subUnit,
	ViUInt32 mode, const ViReal64 resistance)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;//MT__EAI_DEV_NOTFOUND;//
	}
	if (subUnit > 2 || subUnit < 1)
	{
		device->errorCode = MTPR__ERROR_BAD_SUB;
		return device->errorCode;
	}

	int switchState[64] = { 0 };
	ViUInt32 set_pattern[2] = { 0x0, 0x0 };//十六进制表示40个开关的闭合情况.
	//初始化初始开关状态
	//小电阻(K129~K132)默认全断开.
	for (int j = 0; j < 64; j++)
	{
		switchState[j] = 0;
	}
	device->MT5170B_param.g_switchState37 = 1;//继电器K137的状态.
	device->MT5170B_param.g_switchState38 = 0;//继电器K138的状态.

	//默认大电阻全闭合,即K119~K126,K139~K140,全闭合.
	switchState[38] = 1;
	switchState[39] = 1;
	for (int jj = 18; jj <= 25; jj++)
	{
		switchState[jj] = 1;
	}
	//精调,小电阻(K133~K136)默认全闭合.取精调最小值.
	for (int jj = 32; jj <= 35; jj++)
	{
		switchState[jj] = 1;
	}//20250328

	double q_calibrationValue[53];
	memset(q_calibrationValue, 0, sizeof(q_calibrationValue));//将一块内存区域的每个字节设置为指定的值.
	memmove(q_calibrationValue, device->MT5170B_param.g_calibrationValue_CH[subUnit - 1], 53 * sizeof(double));//将src中的Size个数据复制到dst中.

	double set_resistance = resistance;
	//程控电阻可设置的最小值、最大值.
	double minValue = q_calibrationValue[51], maxValue = q_calibrationValue[50];
	int length = sizeof(switchState) / sizeof(switchState[0]);//64

	//K138=0,K137=0,电阻断路; K138=0,K137=1,电阻接入; K138=1,K137=1/0,电阻短路; 
	if (0.0 == set_resistance)
	{//与上位机约定输入0时,表示短路即Short  K138=1,K137=1/0,电阻短路; 
		for (int j = 0; j < 64; j++)
		{
			switchState[j] = 0;
		}
		switchState[37] = 1;//K138闭合（1）.
		device->MT5170B_param.g_switchState38 = 1;//继电器K138的状态.
	}
	else if (minValue > set_resistance)
	{//K138=0,K137=0,电阻断路OPEN; 
		for (int j = 0; j < 64; j++)
		{
			switchState[j] = 0;
		}
		switchState[37] = 0;//K138断开（0）.
		device->MT5170B_param.g_switchState38 = 0;//继电器K138的状态.
		switchState[36] = 0;//K137断开（0）.
		device->MT5170B_param.g_switchState37 = 0;//继电器K137的状态.
	}
	else
	{
		////将电阻的设置分辨率限制在2毫欧. //限制数值在 [min, max] 之间，并四舍五入到小数点后3位.
		//set_resistance = clamp_and_round(set_resistance, minValue, maxValue);
		//printf("\r\n-----------clamp_and_round() set_resistance = %6.3lf--------------\n", set_resistance);

		//根据设置电阻大小,计算确定开关状态.
		status = calculate_switchState(vi, subUnit, set_resistance, switchState, length, q_calibrationValue);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	/*///----------test查看闭合了哪些开关.------------------////
	//switchState[0]~switchState[37]对应继电器的K101~K138.
	int jj = 0;
	printf("二进制0~63: ");
	for (int j = 0; j < 64; j++)
	{
	jj++;
	printf("%d", switchState[j]);
	if (0 == jj % 4 && 0 != jj)
	{
	printf(" ");
	}
	}
	////----------test查看闭合了哪些开关.------------------///*/

	char* hexStr = NULL;
	ViUInt64 value = 0;
	ViUInt32 high32 = 0;
	ViUInt32 low32 = 0;
	//hexStr = binaryArrayToHex(switchState, length);
	hexStr = binaryToHex(switchState, length);
	if (hexStr == NULL)
	{
		//printf(" binaryArrayToHex ERROR. ");
		device->errorCode = MTPR__ERROR_INV_PARAMETER;
		return device->errorCode;
	}
	//printf("   switchState-十六进制结果: %s\n", hexStr);
	value = strtoull(hexStr, NULL, 16);//转换为64位整数
	//提取高32位和低32位
	high32 = (ViUInt32)(value >> 32);
	low32 = (ViUInt32)(value & 0xFFFFFFFF);
	//printf("   switchState: high32 = %lu,low32 = %lu.\n", high32, low32);
	//设置需要闭合的继电器bit.
	set_pattern[0] = low32;
	set_pattern[1] = high32;
	free(hexStr);//释放内存

	////----------just test------------------
	////set_pattern[0] = 201068577UL;
	////set_pattern[1] = 208UL;
	//set_pattern[0] = 133959728UL;
	//set_pattern[1] = 208UL;
	////----------just test------------------

	//设置子单元所有输出状态.
	status = MtPR__setChannelPattern(vi, subUnit, set_pattern);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}

	/*///-------test 获取子单元所有输出状态.-------////
	//ViUInt32 get_pattern[2] = { 0, 0 };
	//status = MtPR__getChannelPattern(vi, subUnit, get_pattern);//OK
	//printf(" get_switchState: high32 = %lu,low32 = %lu.\n", get_pattern[1], get_pattern[0]);

	ViReal64 get_resistance = 0;
	status = MtPR__resGetResistance( vi,  subUnit, &get_resistance);
	////-------test 获取子单元所有输出状态.-------///*/
	//printf("\r\n-----------------MtPR__resSetResistance---End------------------------------------------\n\n");


	return status;
}
ViStatus MtPR__resSetResistance_5171(ViSession vi, ViUInt32 subUnit,
	ViUInt32 mode, const ViReal64 resistance)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;
	}
	if (subUnit > 6 || subUnit < 1)
	{
		device->errorCode = MTPR__ERROR_BAD_SUB;
		return device->errorCode;
	}

	int switchState[32] = { 0 };
	ViUInt32 set_pattern[2] = { 0x0, 0x0 };//十六进制表示30个开关的闭合情况.
	//初始化初始开关状态
	for (int j = 0; j < 32; j++)
	{
		switchState[j] = 1;
	}
	device->MT5171_param.g_switchState37 = 2;//K137=0,电阻断路; K137=1,电阻短路; K137=2,电阻接入; 

	double q_calibrationValue[33];
	memset(q_calibrationValue, 0, sizeof(q_calibrationValue));//将一块内存区域的每个字节设置为指定的值.
	memmove(q_calibrationValue, device->MT5171_param.g_calibrationValue_CH[subUnit-1], CALIBRATION_NUM_5171 * sizeof(double));//将src中的Size个数据复制到dst中.
	
	//printf("\r\n\n-----------------MtPR__resSetResistance---Start----------------------------------------\n");
	double set_resistance = resistance;
	//程控电阻可设置的最小值、最大值.
	double minValue = q_calibrationValue[31], maxValue = q_calibrationValue[30];
	int length = sizeof(switchState) / sizeof(switchState[0]);//64

	//如果 resistance=0.0,该怎么处理.
	//RL10=1,电阻接入;
	if (0.0 == set_resistance)
	{//与上位机约定输入0值,表示短路即Short  电阻短路;
		for (int j = 0; j < 32; j++)
		{
			switchState[j] = 0;
		}
		//短路最小值,闭合RL10、RL9、RL19、RL29;
		switchState[8] = 1;
		switchState[9] = 1;
		switchState[18] = 1;
		switchState[28] = 1;
		device->MT5171_param.g_switchState37 = 1;
	}
	else if (minValue > set_resistance)
	{//电阻断路OPEN,RL10断开;
		switchState[9] = 0;
		device->MT5171_param.g_switchState37 = 0;
	}
	else
	{
		////将电阻的设置分辨率限制在2毫欧. //限制数值在 [min, max] 之间，并四舍五入到小数点后3位.
		//set_resistance = clamp_and_round(set_resistance, minValue, maxValue);
		//printf("\r\n-----------clamp_and_round() set_resistance = %6.3lf--------------\n", set_resistance);

		//根据设置电阻大小,计算确定开关状态.
		status = calculate_switchState_5171(vi, subUnit, set_resistance, switchState, length, q_calibrationValue);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	//////----------test查看闭合了哪些开关.------------------////
	////switchState[0]~switchState[37]对应继电器的K101~K138.
	//int jj = 0;
	//printf("二进制0~63: ");
	//for (int j = 0; j < 64; j++)
	//{
	//	jj++;
	//	printf("%d", switchState[j]);
	//	if (0 == jj % 4 && 0 != jj)
	//	{
	//		printf(" ");
	//	}
	//}
	//////----------test查看闭合了哪些开关.------------------////

	char* hexStr = NULL;
	ViUInt64 value = 0;
	ViUInt32 high32 = 0;
	ViUInt32 low32 = 0;
	//hexStr = binaryArrayToHex(switchState, length);
	hexStr = binaryToHex(switchState, length);
	if (hexStr == NULL)
	{
		//printf(" binaryArrayToHex ERROR. ");
		device->errorCode = MTPR__ERROR_INV_PARAMETER;
		return device->errorCode;
	}
	//printf("   switchState-十六进制结果: %s\n", hexStr);
	//// 可选：保存到文件
	//FILE* fp = NULL;
	//int err = fopen_s(&fp, "output.txt", "w");
	//if (!err)
	//{
	//	fprintf(fp, "%s", hexStr);
	//	fclose(fp);
	//}
	value = strtoull(hexStr, NULL, 16);//转换为64位整数
	//提取高32位和低32位
	high32 = (ViUInt32)(value >> 32);
	low32 = (ViUInt32)(value & 0xFFFFFFFF);
	//printf("   switchState: high32 = %lu,low32 = %lu.\n", high32, low32);
	//设置需要闭合的继电器bit.
	set_pattern[0] = low32;
	set_pattern[1] = high32;
	free(hexStr);//释放内存

	////----------just test------------------
	////set_pattern[0] = 201068577UL;
	////set_pattern[1] = 208UL;
	//set_pattern[0] = 133959728UL;
	//set_pattern[1] = 208UL;
	////----------just test------------------

	//设置子单元所有输出状态.
	status = MtPR__setChannelPattern_5171(vi, subUnit, set_pattern);
	if (status)
	{
		return MTPR__ERROR_WRITE_FAIL;
	}


	//////-------test 获取子单元所有输出状态.-------////
	////ViUInt32 get_pattern[2] = { 0, 0 };
	////status = MtPR__getChannelPattern(vi, subUnit, get_pattern);//OK
	////printf(" get_switchState: high32 = %lu,low32 = %lu.\n", get_pattern[1], get_pattern[0]);

	//ViReal64 get_resistance = 0;
	//status = MtPR__resGetResistance_5171( vi,  subUnit, &get_resistance);
	//////-------test 获取子单元所有输出状态.-------////
	//printf("\r\n-----------------MtPR__resSetResistance---End------------------------------------------\n\n");


	return status;
}
ViStatus MtPR__resSetResistance_5172(ViSession vi, ViUInt32 subUnit,
	ViUInt32 mode, const ViReal64 resistance)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;//MT__EAI_DEV_NOTFOUND;//
	}
	if (subUnit > 12 || subUnit < 1)
	{
		device->errorCode = MTPR__ERROR_BAD_SUB;
		return device->errorCode;
	}

	int switchState[32] = { 0 };
	ViUInt32 set_pattern[2] = { 0x0, 0x0 };//十六进制表示30个开关的闭合情况.
	//初始化初始开关状态
	for (int j = 0; j < 32; j++)
	{
		switchState[j] = 1;
	}
	device->MT5172_param.g_switchState37 = 2;//K137=0,电阻断路; K137=1,电阻短路; K137=2,电阻接入; 

	double q_calibrationValue[33];
	memset(q_calibrationValue, 0, sizeof(q_calibrationValue));//将一块内存区域的每个字节设置为指定的值.

	////ViUInt32 store = 0;//0-用户数据;1-工厂数据.
	////ViUInt32 offset = 0;
	////ViUInt32 numValues = CALIBRATION_NUM_5172;//校准数据个数
	if (subUnit > 0 && subUnit <= 12)
	{
		////memset(g_calibrationValue_5172, 0, sizeof(g_calibrationValue_5172);//将一块内存区域的每个字节设置为指定的值.
		memmove(q_calibrationValue, device->MT5172_param.g_calibrationValue_CH[subUnit-1], CALIBRATION_NUM_5172 * sizeof(double));//将src中的Size个数据复制到dst中.
		////memcpy(g_calibrationValue_5172, g_calibrationValue_5172_CH1, 33 * sizeof(double));//将src中的Size个数据复制到dst中.
	}

	//printf("\r\n\n-----------------MtPR__resSetResistance---Start----------------------------------------\n");
	double set_resistance = resistance;
	//程控电阻可设置的最小值、最大值.
	double minValue = q_calibrationValue[31], maxValue = q_calibrationValue[30];
	int length = sizeof(switchState) / sizeof(switchState[0]);//64
	
	//如果 resistance=0.0,该怎么处理.
	//RL10=1,电阻接入;
	if (0.0 == set_resistance)
	{//注意:与上位机约定上位机传入0时,表示短路即Short  电阻短路;
		for (int j = 0; j < 32; j++)
		{
			switchState[j] = 0;
		}
		//短路最小值,闭合RL10、RL9、RL19、RL29;
		switchState[8] = 1;
		switchState[9] = 1;
		switchState[18] = 1;
		switchState[28] = 1;
		device->MT5172_param.g_switchState37 = 1;
	}
	else if (doubleZeroCompare(minValue - set_resistance) > 0)
	{//电阻断路OPEN,RL10断开;
		switchState[9] = 0;
		device->MT5172_param.g_switchState37 = 0;
	}
	else
	{
		////将电阻的设置分辨率限制在2毫欧. //限制数值在 [min, max] 之间，并四舍五入到小数点后3位.
		//set_resistance = clamp_and_round(set_resistance, minValue, maxValue);
		//printf("\r\n-----------clamp_and_round() set_resistance = %6.3lf--------------\n", set_resistance);

		//根据设置电阻大小,计算确定开关状态.
		status = calculate_switchState_5172(vi, subUnit, set_resistance, switchState, length, q_calibrationValue);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	//////----------test查看闭合了哪些开关.------------------////
	////switchState[0]~switchState[37]对应继电器的K101~K138.
	//int jj = 0;
	//printf("二进制0~63: ");
	//for (int j = 0; j < 64; j++)
	//{
	//	jj++;
	//	printf("%d", switchState[j]);
	//	if (0 == jj % 4 && 0 != jj)
	//	{
	//		printf(" ");
	//	}
	//}
	//////----------test查看闭合了哪些开关.------------------////

	char* hexStr = NULL;
	ViUInt64 value = 0;
	ViUInt32 high32 = 0;
	ViUInt32 low32 = 0;
	//hexStr = binaryArrayToHex(switchState, length);
	hexStr = binaryToHex(switchState, length);
	if (hexStr == NULL)
	{
		//printf(" binaryArrayToHex ERROR. ");
		device->errorCode = MTPR__ERROR_INV_PARAMETER;
		return device->errorCode;
	}
	//printf("   switchState-十六进制结果: %s\n", hexStr);
	//// 可选：保存到文件
	//FILE* fp = NULL;
	//int err = fopen_s(&fp, "output.txt", "w");
	//if (!err)
	//{
	//	fprintf(fp, "%s", hexStr);
	//	fclose(fp);
	//}
	value = strtoull(hexStr, NULL, 16);//转换为64位整数
	//提取高32位和低32位
	high32 = (ViUInt32)(value >> 32);
	low32 = (ViUInt32)(value & 0xFFFFFFFF);
	//printf("   switchState: high32 = %lu,low32 = %lu.\n", high32, low32);
	//设置需要闭合的继电器bit.
	set_pattern[0] = low32;
	set_pattern[1] = high32;
	free(hexStr);//释放内存

	////----------just test------------------
	////set_pattern[0] = 201068577UL;
	////set_pattern[1] = 208UL;
	//set_pattern[0] = 133959728UL;
	//set_pattern[1] = 208UL;
	////----------just test------------------

	//设置子单元所有输出状态.
	status = MtPR__setChannelPattern_5172(vi, subUnit, set_pattern);
	if (status)
	{
		return MTPR__ERROR_WRITE_FAIL;
	}


	//////-------test 获取子单元所有输出状态.-------////
	////ViUInt32 get_pattern[2] = { 0, 0 };
	////status = MtPR__getChannelPattern(vi, subUnit, get_pattern);//OK
	////printf(" get_switchState: high32 = %lu,low32 = %lu.\n", get_pattern[1], get_pattern[0]);

	//ViReal64 get_resistance = 0;
	//status = MtPR__resGetResistance_5172( vi,  subUnit, &get_resistance);
	//////-------test 获取子单元所有输出状态.-------////
	//printf("\r\n-----------------MtPR__resSetResistance---End------------------------------------------\n\n");


	return status;
}
//**************************************************************************
// Set resistance as close as possible to the given value
//		vi (in)				Instrument handle
//		subUnit (in)		Sub-unit number
//		mode (in)			A value indicating how the given resistance value is to be applied
//		resistance (in)		the resistance value to set
//		Return = ViStatus
//**************************************************************************
ViStatus MtPR__resSetResistance(ViSession vi, ViUInt32 subUnit,
	ViUInt32 mode, const ViReal64 resistance)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;//MT__EAI_DEV_NOTFOUND;//
	}

	//程控电阻:0x70A-->MT5170B-01; 0x70C-->MT5170B-02; 0x70B-->MT5171; 
	if (0x70A == device->mtModelName || 0x70C == device->mtModelName)
	{
		status = MtPR__resSetResistance_5170B(vi, subUnit, mode, resistance);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else if (0x70B == device->mtModelName)
	{
		status = MtPR__resSetResistance_5171(vi, subUnit, mode, resistance);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else if (0x70D == device->mtModelName)
	{
		status = MtPR__resSetResistance_5172(vi, subUnit, mode, resistance);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else
	{
		device->errorCode = MTPR__ERROR_BAD_SESSION;
		return device->errorCode;
	}

	return status;
}

////获取当前的电阻值. 方法二: 根据开关的状态计算出当前的电阻值
//ViStatus MtPR__CalResistanceBasedOnSwitchStatus(ViSession vi, ViUInt32 subUnit, 
//	ViAUInt32 pattern, ViPReal64 *resistance)
//{
//	ViStatus status = VI_SUCCESS;//checks Status of a function
//
//
//	//获取当前的电阻值. 方法二: 根据开关的状态计算出当前的电阻值
//
//	ViUInt32 get_pattern[2] = { 0 };
//	status = MtPR__getChannelPattern(vi, subUnit, get_pattern);
//	printf("getChannelPattern: high32 = %lu,low32 = %lu.\n", get_pattern[1], get_pattern[0]);
//
//	//合并两个32位十进制数为一个64位十六进制数
//	//将第一个32位整数左移32位，为第二个32位整数腾出空间
//	uint64_t merged = ((uint64_t)get_pattern[1]) << 32;
//	// 将第二个32位整数与左移后的结果进行按位或操作
//	merged |= get_pattern[0];// 输出合并后的64位十六进制数
//	printf("合并后的64位十六进制数: 0x%llx\n", merged);
//
//	int switchState0[32] = { 0 };
//	int switchState1[32] = { 0 };
//	//初始化初始开关状态
//	for (int j = 0; j < 32; j++)
//	{
//		switchState0[j] = 0;
//		switchState1[j] = 0;
//	}
//	decimalToBinary2(get_pattern[0], switchState0);//OK
//	decimalToBinary2(get_pattern[1], switchState1);//OK
//	/**///----------test查看闭合了哪些开关.------------------////
//	int jj = 0;
//	printf("二进制0~63: ");
//	for (int j = 0; j < 32; j++)
//	{
//		jj++;
//		printf("%d", switchState0[j]);
//		if (0 == jj % 4 && 0 != jj)
//		{
//			printf(" ");
//		}
//	}
//	printf("\n");
//	printf("二进制0~63: ");
//	jj = 0;
//	for (int j = 0; j < 32; j++)
//	{
//		jj++;
//		printf("%d", switchState1[j]);
//		if (0 == jj % 4 && 0 != jj)
//		{
//			printf(" ");
//		}
//	}
//	printf("\n");
//	//----------test查看闭合了哪些开关.------------------///*/
//
//
//
//	return status;
//}


//**************************************************************************
// Obtain the set resistance value
//		vi (in)				Instrument handle
//		subUnit (in)		Sub-unit number
//		resistance (out)	pointer to variable to receive the resistance value
//		Return = ViStatus
//**************************************************************************
ViStatus MtPR__resGetResistance(ViSession vi, ViUInt32 subUnit,
	ViPReal64 resistance)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;
	}

	//程控电阻:0x70A-->MT5170B-01; 0x70C-->MT5170B-02; 0x70B-->MT5171; 
	if (0x70A == device->mtModelName || 0x70C == device->mtModelName)//程控电阻:0x70A-->MT5170B-01; 0x70C-->MT5170B-02;
	{
		if (subUnit > 2 || subUnit < 1)
		{
			device->errorCode = MTPR__ERROR_BAD_SUB;
			return device->errorCode;
		}
		//K138=0,K137=0,电阻断路; K138=0,K137=1,电阻接入; K138=1,K137=1/0,电阻短路; 
		if (0 == device->MT5170B_param.g_switchState38)
		{
			if (0 == device->MT5170B_param.g_switchState37)
			{//K138=0,K137=0,电阻断路OPEN; 
				*resistance = HUGE_VAL;
			}
			else if (1 == device->MT5170B_param.g_switchState37)
			{//K138=0,K137=1,电阻接入; 
				//获取当前的电阻值. 方法一: 
				*resistance = device->MT5170B_param.g_currentSetR_CH[subUnit - 1];
			}
		}
		else if (1 == device->MT5170B_param.g_switchState38)
		{//K138=1,K137=1/0,电阻短路Short; 
			*resistance = device->MT5170B_param.g_calibrationValue_CH[subUnit - 1][0];
		}
	}
	else if (0x70B == device->mtModelName)//程控电阻:0x70B-->MT5171; 
	{
		if (subUnit > 6 || subUnit < 1)
		{
			device->errorCode = MTPR__ERROR_BAD_SUB;
			return device->errorCode;
		}
		//K137=0,电阻断路; K137=1,电阻短路; 
		if (0 == device->MT5171_param.g_switchState37)
		{//K137=0,电阻断路OPEN; 
			*resistance = HUGE_VAL;
		}
		else if (1 == device->MT5171_param.g_switchState37)
		{//K137=1,电阻短路Short; 
			*resistance = device->MT5171_param.g_calibrationValue_CH[subUnit - 1][26];
		}
		else
		{//K137=2,电阻接入; 
			*resistance = device->MT5171_param.g_currentSetR_CH[subUnit - 1];
		}
	}
	else if (0x70D == device->mtModelName)//程控电阻:0x70D-->MT5172;
	{
		if (subUnit > 12 || subUnit < 1)
		{
			device->errorCode = MTPR__ERROR_BAD_SUB;
			return device->errorCode;
		}
		//K137=0,电阻断路; K137=1,电阻短路; 
		if (0 == device->MT5172_param.g_switchState37)
		{//K137=0,电阻断路OPEN; 
			*resistance = HUGE_VAL;
		}
		else if (1 == device->MT5172_param.g_switchState37)
		{//K137=1,电阻短路Short;
			*resistance = device->MT5172_param.g_calibrationValue_CH[subUnit - 1][26];
		}
		else
		{//K137 = 2, 电阻接入;
			//获取当前的电阻值. 方法一:
			*resistance = device->MT5172_param.g_currentSetR_CH[subUnit - 1];
		}
	}
	else
	{
		device->errorCode = MTPR__ERROR_BAD_SESSION;
		return device->errorCode;
	}

	//获取当前的电阻值. 方法二: 根据开关的状态计算出当前的电阻值

	return status;
}


//**************************************************************************
// Obtain detailed resistor information  //获取电阻的详细信息
//		vi (in)			Instrument handle //仪器句柄
//		subUnit (in)	Sub-unit number //子单元号
//		minRes (out)	pointer to variable to receive the minimum resistance setting //指向接收最小电阻设置的变量的指针
//		maxRes (out)	pointer to variable to receive the maximum resistance setting //指向变量的指针，接收最大电阻设置
//		refRes (out)	pointer to variable to receive the reference resistance value //指向变量的指针以接收引用的阻值
//		precPC (out)	pointer to variable to receive percentage precision (+/- percent) //指针指向变量接收百分比精度（+/-百分比）
//		precDelta (out)	pointer to variable to receive delta precision (+/- ohms) //指针指向变量，接收增量精度（+/-欧姆）
//		int1 (out)		pointer to (currently unused) variable //指针指向（当前未使用的）变量
//		intDelta (out)	pointer to variable to receive internal precision (+/- ohms) //指针指向接收内部精度（+/-欧姆）的变量
//		capabilities (out)	pointer to variable to receive capabilities flags //指向接收能力标志的变量指针
//		Return = ViStatus
//**************************************************************************
ViStatus MtPR__resGetInfo(ViSession vi, ViUInt32 subUnit,
	ViPReal64 minRes, ViPReal64 maxRes, ViPReal64 refRes, ViPReal64 precPC,
	ViPReal64 precDelta, ViPReal64 int1, ViPReal64 intDelta, ViPUInt32 capabilities)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;
	}

	if (0x70A == device->mtModelName || 0x70C == device->mtModelName)
	{//程控电阻:0x70A-->MT5170B-01; 0x70C-->MT5170B-02;
		if (subUnit > 2 || subUnit < 1)
		{
			device->errorCode = MTPR__ERROR_BAD_SUB;
			return device->errorCode;
		}
		*minRes = device->MT5170B_param.g_calibrationValue_CH[subUnit - 1][51];
		*maxRes = device->MT5170B_param.g_calibrationValue_CH[subUnit - 1][50];
		*refRes = 0;
		*precPC = 0;
		*precDelta = 0;
		*int1 = 0;
		*intDelta = device->MT5170B_param.g_calibrationValue_CH[subUnit - 1][52];//0.001;// //1毫欧.
		*capabilities = 0;
	}
	else if (0x70B == device->mtModelName)
	{//程控电阻:0x70B-->MT5171; 
		if (subUnit > 6 || subUnit < 1)
		{
			device->errorCode = MTPR__ERROR_BAD_SUB;
			return device->errorCode;
		}
		*minRes = device->MT5171_param.g_calibrationValue_CH[subUnit - 1][31];
		*maxRes = device->MT5171_param.g_calibrationValue_CH[subUnit - 1][30];
		*refRes = 0;
		*precPC = 0;
		*precDelta = 0;
		*int1 = 0;
		*intDelta = device->MT5171_param.g_calibrationValue_CH[subUnit - 1][32];//0.001;// //1毫欧.
		*capabilities = 0;
	}
	else if (0x70D == device->mtModelName)
	{//程控电阻:0x70D-->MT5172;
		if (subUnit > 12 || subUnit < 1)
		{
			device->errorCode = MTPR__ERROR_BAD_SUB;
			return device->errorCode;
		}
		*minRes = device->MT5172_param.g_calibrationValue_CH[subUnit - 1][31];
		*maxRes = device->MT5172_param.g_calibrationValue_CH[subUnit - 1][30];
		*refRes = 0;
		*precPC = 0;
		*precDelta = 0;
		*int1 = 0;
		*intDelta = device->MT5172_param.g_calibrationValue_CH[subUnit - 1][32];//0.001;// //1毫欧.
		*capabilities = 0;
	}
	else
	{
		device->errorCode = MTPR__ERROR_BAD_SESSION;
		return device->errorCode;
	}

	return status;
}

//写入 SN number
ViStatus MtPR__writeCardSNnumber(ViSession vi, ViUInt32 subUnit,
	ViUInt32 store, ViUInt32 SN)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;
	}

	ViUInt32 addr = SPI_FLASH_CARD_SN_NUMBER;
	int datasize = 4;
	ViUInt32 dataArray[4] = { 0 };//4=1*32/8
	for (int i = 0; i < 4; i++)
	{
		uint8_t byteToWtrite = LOW_8_BITS(SN >> (8 * (3 - i)));
		dataArray[i] = byteToWtrite;
	}
	//写校准数据.多次写数据,只用首次写数据时,擦除一次（cnt=1时,起作用）.
	status = f_FlashWriteData(device, datasize, addr, dataArray, 1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}

	return status;
}

//读取 SN-number 和 FwVersion
ViStatus MtPR__readCardSNnumber(ViSession vi, ViUInt32 subUnit,
	ViUInt32 store, ViPUInt32 SN, ViPUInt64 FwVersion)
{
	ViStatus status = VI_SUCCESS;//checks Status of a function

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;
	}

	ViUInt32 addr = SPI_FLASH_CARD_SN_NUMBER;
	int datasize = 4;
	//读取数据
	ViUInt32 arraydata[4] = { 0 };
	status = FlashReadData(device, datasize, addr, arraydata);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//
	int k = -1;
	//每4个一组进行组合.
	ViUInt32 combine_result[3] = { 0 };
	for (int i = 0; i < datasize; i += 4)
	{
		ViUInt32 result = 0;
		for (size_t j = 0; j < 4; j++)
		{
			result = result | static_cast<unsigned long>(arraydata[i + j]) << ((3 - j) * 8);
		}
		k++;
		combine_result[k] = result;
	}
	*SN = combine_result[0];

	//读取硬件版本号.
	ViUInt32 yearVal = 0;
	ViUInt32 dateVal = 0;
	status = viIn32(vi, VI_PXI_BAR0_SPACE, 0x0004, &yearVal);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = viIn32(vi, VI_PXI_BAR0_SPACE, 0x0008, &dateVal);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	*FwVersion = 0;
	*FwVersion = ((ViUInt64)yearVal << 32) | dateVal;

	return status;
}

ViStatus MtPR__OnlineUpdate_U45(ViSession vi, const char* path)
{
	ViStatus  status = VI_SUCCESS;

	MT_DEVICE* device = findDevice(vi);
	if (device == NULL)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;
	}

	ViUInt8 buf[256] = { 0 };
	ViUInt32 bufTmp[256] = { 0 };
	size_t len = 0;
	size_t totalLen = 0;
	size_t tempLen = 0;
	size_t i = 0;
	ViUInt32 addr = 0x400000;
	ViUInt32 onceWriteLen = 256;
	FILE *pFile = NULL;

	pFile = fopen(path, "rb");
	if (pFile == NULL)
	{
		device->errorCode = MT__EAI_OPEN_FILE;
		return device->errorCode;
	}
	status = FlashEraseU45(device);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}

	while (!feof(pFile)) //没有到文件末尾
	{
		memset(buf, 0, sizeof(buf));
		len = fread(buf, sizeof(ViUInt8), sizeof(buf), pFile);
		totalLen += len;
		tempLen += len;
		i++;
        printf("while %lu tempLen %lu len %lu addr %x\r\n", i, tempLen, len, addr);

		status = FlashWriteEnableU45(device, MTDCPOWER_VAL_ON);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}

		for (size_t m = 0; m < onceWriteLen; m++)
		{
			bufTmp[m] = buf[m];
		}

		status = FlashWriteDataU45(device, (ViUInt32)len, addr, bufTmp);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}

		addr += (ViUInt32)len;
	}
	printf("Success\r\n");
	fclose(pFile);

	return status;
}

ViStatus MtPR__OnlineUpdateMultiple_U45(ViSession vi[], ViInt32 number, const char* path, ViStatus result[])
{
	ViStatus  status = VI_SUCCESS;
	int i = 0;
	MT_DEVICE* device[16];
	if (number > 16)
	{
		return MTPR__ERROR_CARD_INACCESSIBLE;
	}

	for (i = 0; i < number; i++)
	{
		device[i] = findDevice(vi[i]);
		if (device[i] == NULL)
		{
			return MTPR__ERROR_CARD_INACCESSIBLE;
		}
	}

	ViUInt8 buf[256] = { 0 };
	ViUInt32 bufTmp[256] = { 0 };
	size_t len = 0;
	size_t totalLen = 0;
	size_t tempLen = 0;
	size_t j = 0;
	ViUInt32 addr = 0x400000;
	ViUInt32 onceWriteLen = 256;
	FILE *pFile = NULL;

	pFile = fopen(path, "rb");
	if (pFile == NULL)
	{
		return MT__EAI_OPEN_FILE;
	}

	for (i = 0; i < number; i++)
	{
		device[i]->errorCode = 0;
		result[i] = 0;
		status = FlashEraseU45(device[i]);
		if (status)
		{
			device[i]->errorCode = mt_translate_sys_error(status);
			return device[i]->errorCode;
		}
	}
	while (!feof(pFile)) //没有到文件末尾
	{
		memset(buf, 0, sizeof(buf));
		len = fread(buf, sizeof(ViUInt8), sizeof(buf), pFile);
		totalLen += len;
		tempLen += len;
		j++;
        printf("while:%lu tempLen:%lu len:%lu addr:%x\r\n", j, tempLen, len, addr);

		for (i = 0; i < number; i++)
		{
			status = FlashWriteEnableU45(device[i], MTDCPOWER_VAL_ON);
			if (status)
			{
				device[i]->errorCode = mt_translate_sys_error(status);
				result[i] = mt_translate_sys_error(status);
				return device[i]->errorCode;
			}

			for (size_t m = 0; m < onceWriteLen; m++)
			{
				bufTmp[m] = buf[m];
			}

			status = FlashWriteDataU45(device[i], (ViUInt32)len, addr, bufTmp);
			if (status)
			{
				device[i]->errorCode = mt_translate_sys_error(status);
				result[i] = mt_translate_sys_error(status);
				return device[i]->errorCode;
			}
		}
		addr += (ViUInt32)len;
	}
	printf("Success\r\n");
	fclose(pFile);

	return 0;
}


