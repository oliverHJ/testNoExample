#include "MtPRcommon.h"
#include "error.h"

#include <sys/stat.h>
#include <stdbool.h>

#if defined(_WIN32)
#include <direct.h>  // Windows需要这个头文件
#include <windows.h>
#define MKDIR(path) _mkdir(path)
#else
#include <unistd.h>   // Linux/Mac需要这个头文件
#define MKDIR(path) mkdir(path, 0755)
#endif


#pragma warning(disable:4996)


//**************************************************************************
// Function Implementation
//**************************************************************************

void save_csv(double resistance, int switchStatus[], int len)
{
	FILE *fp;
	char filename[256];
	//sprintf_s(filename, "setR_switchStatus.csv");
	//int err = fopen_s(&fp, filename, "a");
	sprintf(filename, "%s", "setR_switchStatus.csv");    
	fp = fopen(filename, "a");//"a"：以“追加”方式打开文件。"w"：以“清空后写入”方式打开文件。
	if (NULL==fp) {
		printf("fopen filename ERROR!");
		return;
	}
	fprintf(fp, "SerialNumber,status,setResistance,calibrationValue,sum\n");//写入数据

	int value = 0;
	for (int jj = 0; jj < len; jj++)
	{
		value = switchStatus[jj];
		//写入数据到CSV文件
		if (0 == jj) {
			fprintf(fp, "%d, %d, %.6lf\n", (jj + 1), value, resistance);//写入数据
		}
		else {
			fprintf(fp, "%d, %d\n", (jj + 1), value);//写入数据
		}
	}
	fclose(fp); // 关闭文件
}

//获取系统日期和时间
int f_getSystemTime(struct tm &local_time)
{
	time_t current_time;//声明time_t类型变量
	//struct tm local_time;//tm结构指针

	//local_time.tm_sec=0;     /* seconds after the minute - [0,59] */
	//local_time.tm_min=0;     /* minutes after the hour - [0,59] */
	//local_time.tm_hour=0;    /* hours since midnight - [0,23] */
	//local_time.tm_mday=1;    /* day of the month - [1,31] */
	//local_time.tm_mon=0;     /* months since January - [0,11] */
	//local_time.tm_year=0;    /* years since 1900 */
	//local_time.tm_wday=0;    /* days since Sunday - [0,6] */
	//local_time.tm_yday=0;    /* days since January 1 - [0,365] */
	//local_time.tm_isdst=0;   /* daylight savings time flag */

	time(&current_time); //获取系统日期和时间
    int err = 0;
#if defined(_WIN32)
	err = localtime_s(&local_time, &current_time);//转换为当地日期和时间
#else
	err = localtime_r(&current_time, &local_time);
#endif
	
	//printf("本地时间: %d-%02d-%02d %02d:%02d:%02d  \n\n",
	//	local_time.tm_year + 1900,  // 年份从1900开始计数
	//	local_time.tm_mon + 1,      // 月份范围0~11，需+1
	//	local_time.tm_mday,         // 日
	//	local_time.tm_hour,         // 时
	//	local_time.tm_min,          // 分
	//	local_time.tm_sec);         // 秒

	return err;
}

// 检查目录是否存在
bool dir_exists(const char* path)
{
	struct stat info;
	if (stat(path, &info) != 0) {
		return false;  // 无法访问
	}
	return (info.st_mode & S_IFDIR) != 0;  // 是目录
}
// 创建目录
bool create_dir(const char* path)
{
	return MKDIR(path) == 0;
}
// 判断文件夹是否存在并创建
int check_and_create_dir(const char* dir_path)
{
	//const char* dir_path = "./my_folder";

	if (dir_exists(dir_path)) {
		printf("目录已存在: %s\n", dir_path);
	}
	else {
		printf("目录不存在，正在创建: %s\n", dir_path);
		if (create_dir(dir_path)) {
			printf("目录创建成功\n");
		}
		else {
			perror("创建目录失败");
			return -1;
		}
	}

	return 0;
}

void safe_strcpy(char *dest, const char *src, size_t size) {
	if (dest == NULL || src == NULL || size == 0) {
		return;
	}
	// 拷贝字符串，最多拷贝 size - 1 个字符
	strncpy(dest, src, size - 1);
	// 确保字符串以 '\0' 结尾
	dest[size - 1] = '\0';
}

void safe_strcat(char *dest, const char *src, size_t size) {
	if (dest == NULL || src == NULL || size == 0) {
		return;
	}
	size_t cur_len = strlen(dest);
	if (cur_len < size - 1) {
		strncat(dest, src, size - cur_len - 1);
	}
	dest[size - 1] = '\0';
}

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
int find_first_less(double array[], int low, int high, int direct, double target)
{
	int length = high - low + 1;
	int left = 0;
	int right = length - 1;
	int res = -1; //用于记录找到的位置，初始为-1表示未找到

	if (1 == direct)//array[] 升序排列
	{
		while (left <= right)
		{
			int mid = left + (right - left) / 2; //防止溢出
			if (array[mid + low] <= target)
			{
				res = mid; //记录当前满足条件的位置
				left = mid + 1; //继续向右查找
			}
			else
			{
				right = mid - 1; //向左调整
			}
		}
		//printf("array[%d] = %f\n", (res + low), array[res + low]);
		//return (res != -1) ? array[res + low] : -1; // 返回结果或-1
		return (res != -1) ? (res + low) : -1; // 返回结果或-1
	}
	else
	{
		while (left <= right)
		{
			int mid = left + (right - left) / 2; // 防止溢出
			if (array[mid + low] <= target)
			{
				res = mid;       //当前元素小于b，记录位置
				right = mid - 1; //继续往左寻找更大的可能值
			}
			else
			{
				left = mid + 1;//当前元素大于等于b，需往右查找更小的元素
			}
		}
		//printf("array[%d] = %f\n", (res + low), array[res + low]);
		//return (res != -1) ? array[res + low] : -1; // 返回结果或-1
		return (res != -1) ? (res + low) : -1; // 返回结果或-1
	}

	return -1; //表示没有找到
}

//二分法查找
int binary_search(double array[], int left, int right, double target)
{
	while (left <= right) {
		int mid = left + (right - left) / 2; //防止整数溢出

		if (array[mid] == target) {
			return mid; // 找到目标，返回索引
		}
		else if (array[mid] < target) {
			left = mid + 1; // 目标在右半部分
		}
		else {
			right = mid - 1; // 目标在左半部分
		}
	}

	return -1; //未找到目标
}

//找出与目标数,误差最小的数.
int findClosest(double A[], int len, double b) 
{
	if (len <= 0) {
		// 处理数组为空的情况，这里返回0.0，可根据需求调整
		return 0;
	}

	int index = 0;
	double closest = A[0];
	double min_error = fabs(A[0] - b);

	for (int i = 1; i < len; i++)
	{
		double current_error = fabs(A[i] - b);
		if (current_error < min_error) 
		{
			min_error = current_error;
			closest = A[i];
			index = i;
		}
	}

	return index;
}

bool hasOne(int arr[], int size) 
{
	for (int i = 0; i < size; i++)
	{
		if (arr[i] == 1) {
			return true;
		}
	}
	return false;
}

//两数据交换
void swap(double &a, double &b)
{
	double tmp = a;
	a = b;
	b = tmp;
}

//冒泡排序,降序排列. flag=1降序排列, flag=0升序排列.
void bubble_sort(double arr[], size_t len, int flag)
{
	size_t i = 0, j = 0;
	for (i = 0; i < len; i++)
	{
		bool hasSwap = false; //优化，判断数组是否已经有序，如果有序可以提前退出循环
		for (j = 1; j<len - i; j++)
		{
			if (1 == flag)
			{
				//降序排列
				if (arr[j - 1]<arr[j])
				{
					swap(arr[j - 1], arr[j]);
					hasSwap = true;
				}
			}
			else
			{
				//升序排列
				if (arr[j - 1] > arr[j])
				{
					double temp = arr[j - 1];
					arr[j - 1] = arr[j];
					arr[j] = temp;

					//swap(arr[j - 1], arr[j]);

					hasSwap = true;
				}
			}
		}
		if (!hasSwap)
		{
			break;
		}
	}
}


int doubleZeroCompare(double value)
{
	if (fabs(value) < 1e-12) // =0
	{
		return 0;
	}
	else if (value > 1e-12) // >0
	{
		return 1;
	}
	else //if (value < -(1e-12)) // <0
	{
		return -1;
	}
}

//uint64_t类型整数代表的 IEEE 754 双精度数的位表示。
//函数通过指针类型转换将该整数转换为 double 类型。
//============================================================
//函数名称：ieee754_to_double
//函数功能：函数通过指针类型转换将该整数转换为 double 类型。
//变   量：uint64_t类型整数代表的 IEEE 754 双精度数的位表示。
//      
//返 回 值：转换后的 double 类型.
//============================================================
double ieee754_to_double(uint64_t ieee754)
{//示例：uint64_t ieee754 = 0x3fc00016c7634c9d;//即：0.1250027154515906
	return *(double *)&ieee754;
}

/** 
* @双精度浮点数转为IEEE754格式的数据.
* @brief 将double数据转为IEEE754格式的数据
* @param converData IEEE754数据结构体，传出参数
* @param data 传入的数据
*/
void doubledata2IEEE754(Data2IEEE754* converData, double data)
{
	int s = 0;//正负 
	int E = 0;//指数
	int i = 0;//用于表征E
	int flag = 1;//循环标志位
	double M = 0.0;//尾数
	double buf = data;//对data数据进行暂存

	//处理 0 的特殊情况
	if (data == 0.0)
	{
		converData->double2IEEEHight = (data < 0) ? (1 << 31) : 0;//符号位
		converData->double2IEEELower = 0; //尾数全 0
		return;
	}

	if (data >= 0)
	{//为正数
		s = 0;
	}
	else if (data < 0)
	{//为负数
		s = 1;
		data = -data;//转为正数
		buf = data;
	}

	while (flag)
	{
		if (data >= 1.0 && data < 2.0)
		{
			flag = 0;
			break;
		}
		else if (data >= 2.0)
		{
			data *= 0.5; //等价于 pow(2.0, -1)
			i++;
		}
		else if (data < 1.0)
		{
			data *= 2.0; //等价于 pow(2.0, 1)
			i++;
		}
	}

	if (buf >= 1.0)
	{
		E = i + 1023; //IEEE 754 指数偏移量
		M = buf * pow(2.0, -i) - 1.0;
	}
	else
	{
		E = 1023 - i;//注意：i 是负数或 0
		M = buf * pow(2.0, i) - 1.0;
	}
	// 52 - 20 = 32,将M乘上2^20次方
	double doubleDataM = M * pow(2.0, 20.0);
	int Mint = (int)(floor(doubleDataM));//获取整数部分
	double doubleDataM2 = doubleDataM - Mint;//获取小数部分
	//printf(" doubleDataM -- %.15lf\n", doubleDataM);
	//printf("doubleDataM2 -- %.15lf\n", doubleDataM2);

	converData->double2IEEEHight = (int)(s * pow(2.0, 31.0)) + (int)(E * pow(2.0, 20.0)) + Mint;
	converData->double2IEEELower = (unsigned long)(doubleDataM2 * pow(2.0, 32.0));
	//printf("double_Hight -- %x \n", converData->double2IEEEHight);
	//printf("double_Lower -- %x \n", converData->double2IEEELower);
	//printf("        data -- %.9lf \n\n", buf);
}

/**
* @brief 将double数据转为IEEE754格式的数据
* @param binary：double数据的二进制形式; dst：double数据的十六进制形式.
* @param data 传入的数据
*/
void doubledata2Hex(int *binary, double data, ViUInt8 *Hex)
{
	int s = 0;//正负 
	int E = 0;//指数
	int i = 0;//用于表征E
	int flag = 1;//循环标志位
	double M = 0.0;//尾数
	double buf = data;//对data数据进行暂存
	unsigned char dst[16] = { 0 };
	//memset(dst, 0, sizeof(dst));

	//处理 0 的特殊情况
	if (data == 0.0)
	{
		for (i = 0; i < 64; i++)
		{
			binary[i] = 0;
		}
		for (i = 0; i < 16; i++)
		{
			dst[i] = 0;
		}
		return;
	}

	if (data >= 0)//为正数
	{
		s = 0;
	}
	else if (data < 0)//为负数
	{
		s = 1;
		data = -data;//转为正数
		buf = data;
	}

	while (flag)
	{
		if (data >= 1 && data < 2)
		{
			flag = 0;
			break;
		}
		else if (data >= 2)
		{
			data = data * pow(2.0, -1);
			i++;
		}
		else if (data < 1)
		{
			data = data * pow(2.0, 1);
			i++;
		}
	}

	if (buf >= 1)
	{
		E = i + 1023;
		M = buf * pow(2.0, -i) - 1;
	}
	else
	{
		E = 1023 - i;
		M = buf * pow(2.0, i) - 1;
	}
	binary[0] = s;
	double doubleDataM = M;
	//floor(x)向下取整”，或者说“向下舍入”、“向零取舍”，即取不大于x的最大整数.
	int Mint = (int)floor(doubleDataM);//获取整数部分
	double doubleDataM2 = doubleDataM - Mint;//获取小数部分
	//printf("integer -- %.15lf\n", Mint);
	//printf("decimals -- %.15lf\n", doubleDataM);
	//printf("fractional part -- %.15lf\n", doubleDataM2);

	long long M_integer = (int)(s * pow(2.0, 11)) + (int)E + (int)Mint;
	for (i = 1; i < 12; i++)
	{
		binary[12 - i] = M_integer % 2;
		M_integer = M_integer / 2;
	}
	for (i = 12; i < 64; i++)
	{
		doubleDataM2 = doubleDataM2 * 2;
		if (doubleDataM2 >= 1)
		{
			binary[i] = 1;
			doubleDataM2 = doubleDataM2 - 1;
		}
		else
		{
			binary[i] = 0;
		}
	}
	unsigned long long count = 0;
	int x = 0, temp = 0;
	int savebit = 8;//savebit=8;按8位存储
	for (int i = 0; i < 64; i += savebit)
	{
		for (int j = 0; j < savebit; j++)
		{
			count += (unsigned long long)( binary[i + j] * pow(2.0, (savebit - 1 - j)) );
		}
		while ((x - temp) != savebit / 4)
		{
			dst[x] = count % 16;
			count = count / 16;
			if (dst[x] > 9)
			{
				dst[x] = 'A' + (dst[x] - 10);
			}
			else
			{
				dst[x] = '0' + dst[x];
			}
			//printf("%C ", dst[x]);
			x++;
		}
		temp = x;
	}
	//printf("\n");
	for (int i = 0; i < 16; i++)
	{
		//Hex[i] = dst[i];
		Hex[i] = dst[15 - i];//逆序存
		//printf("%C ", Hex[i]);
	}
	//printf("\n");
}

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
uint64_t merge_reverse_bytes(uint32_t a1, uint32_t b1) 
{
	// 反转 A1 的字节顺序
	uint32_t a1_rev = ((a1 >> 24) & 0xFF) |        // 字节 0 -> 字节 3
		((a1 >> 8) & 0xFF00) |       // 字节 1 -> 字节 2
		((a1 << 8) & 0xFF0000) |     // 字节 2 -> 字节 1
		((a1 << 24) & 0xFF000000);   // 字节 3 -> 字节 0

	// 反转 B1 的字节顺序
	uint32_t b1_rev = ((b1 >> 24) & 0xFF) |
		((b1 >> 8) & 0xFF00) |
		((b1 << 8) & 0xFF0000) |
		((b1 << 24) & 0xFF000000);

	// 合并成一个 64 位整数
	uint64_t result = ((uint64_t)a1_rev << 32) | b1_rev;

	return result;
}


//将二进制数组转换为十六进制字符串（高位在前）.
char* binaryArrayToHex(int array[], int length)
{
	char* paddedBinary = nullptr;
	if (array == NULL || length <= 0)
		return NULL;

	// 1. 计算需要补零的位数
	int padding = (4 - (length % 4)) % 4;
	int totalBits = length + padding;

	// 2. 创建补零后的二进制字符串（高位在前，左侧补零）
	paddedBinary = (char*)malloc(totalBits + 1);
	int index = 0;
	for (int i = 0; i < padding; i++)
	{
		paddedBinary[index++] = '0';//补零
	}
	for (int i = 0; i < length; i++)
	{
		paddedBinary[index++] = array[i] ? '1' : '0';
	}
	paddedBinary[index] = '\0';

	// 3. 计算十六进制字符串长度，并分配内存
	int hexLen = totalBits / 4;
	char* hexStr = (char*)malloc(hexLen + 1); // +1 用于终止符

	// 4. 每4位转换为十六进制字符
	for (int i = 0; i < hexLen; i++)
	{
		int binIndex = i * 4;
		int val = 0;
		for (int j = 0; j < 4; j++)
		{
			val = (val << 1) | (paddedBinary[binIndex + j] - '0');
		}
		hexStr[i] = (val < 10) ? ('0' + val) : ('A' + val - 10);
		//printf("十六进制结果: %c\n", hexStr[i]);//1011输出B
	}
	hexStr[hexLen] = '\0';

	// 5. 释放临时内存
	free(paddedBinary);

	return hexStr;
}

//将二进制数组转换为十六进制字符串（低位在前）.
char* binaryToHex(int* array, int length)
{//示例：array[0] 是最低位
	// 参数合法性检查
	if (array == NULL || length <= 0) {
		return NULL;
	}

	//计算需要的十六进制字符数
	int hex_len = (length + 3) / 4;
	char* hexStr = (char*)malloc(hex_len + 1);// +1 用于存放结尾的 '\0'
	if (hexStr == NULL)
	{
		return NULL;//内存分配失败
	}

	int pos = 0;//当前写入 hexStr 的位置
	//从高位到低位遍历每个四位组
	for (int i = hex_len - 1; i >= 0; i--)
	{
		int start = i * 4;//当前四位组的起始索引
		int val = 0;

		//计算四位组的值（0~15）
		for (int j = 0; j < 4; j++)
		{
			int bitIndex = start + j;
			int bit = (bitIndex < length) ? array[bitIndex] : 0;
			val += bit * (1 << j);//权值 1, 2, 4, 8
		}

		//转换为十六进制字符
		hexStr[pos++] = (val < 10) ? ('0' + val) : ('A' + val - 10);
	}
	hexStr[pos] = '\0';//添加字符串结束符
	return hexStr;
}

//函数：将单个十六进制字符转换为对应的二进制字符串.
char* hexCharToBinary(char hexChar)
{
	char* binary = (char*)malloc(5 * sizeof(char)); // 分配内存，4位二进制+1个字符串结束符
	switch (hexChar)
	{
	case '0': mt_strcpy(binary, 4, "0000"); break;
	case '1': mt_strcpy(binary, 4, "0001"); break;
	case '2': mt_strcpy(binary, 4, "0010"); break;
	case '3': mt_strcpy(binary, 4, "0011"); break;
	case '4': mt_strcpy(binary, 4, "0100"); break;
	case '5': mt_strcpy(binary, 4, "0101"); break;
	case '6': mt_strcpy(binary, 4, "0110"); break;
	case '7': mt_strcpy(binary, 4, "0111"); break;
	case '8': mt_strcpy(binary, 4, "1000"); break;
	case '9': mt_strcpy(binary, 4, "1001"); break;
	case 'A': mt_strcpy(binary, 4, "1010"); break;
	case 'B': mt_strcpy(binary, 4, "1011"); break;
	case 'C': mt_strcpy(binary, 4, "1100"); break;
	case 'D': mt_strcpy(binary, 4, "1101"); break;
	case 'E': mt_strcpy(binary, 4, "1110"); break;
	case 'F': mt_strcpy(binary, 4, "1111"); break;
	case 'a': mt_strcpy(binary, 4, "1010"); break;
	case 'b': mt_strcpy(binary, 4, "1011"); break;
	case 'c': mt_strcpy(binary, 4, "1100"); break;
	case 'd': mt_strcpy(binary, 4, "1101"); break;
	case 'e': mt_strcpy(binary, 4, "1110"); break;
	case 'f': mt_strcpy(binary, 4, "1111"); break;
	default:  mt_strcpy(binary, 4, "0000"); break; // 非法字符默认为0000
	}
	return binary;
}

//函数：将十进制数转换为二进制字符串.
void decimalToBinary(int decimal, char* binary) {
	int index = 0; // 用于记录二进制字符串的位置
	while (decimal > 0) {
		int remainder = decimal % 2; // 获取余数
		binary[index++] = '0' + remainder; // 将余数转换为字符并存储
		decimal /= 2; // 更新十进制数
	}
	binary[index] = '\0'; // 添加字符串结束符

	// 反转字符串，因为目前的二进制是反向存储的
	int start = 0;
	int end = index - 1;
	while (start < end) {
		char temp = binary[start];
		binary[start] = binary[end];
		binary[end] = temp;
		start++;
		end--;
	}
}

//函数：将十进制数转换为二进制整型数组.
void decimalToBinary2(int num, int binary[])
{
	// 处理十进制数为 0 的情况
	if (num == 0) {
		//printf("0\n");
		return;
	}

	int i = 0;
	// 计算二进制位
	while (num > 0) {
		binary[i] = num % 2;
		num = num / 2;
		i++;
	}
	//// 逆序输出二进制位
	//printf("\n 逆序输出二进制位:");
	//for (int j = i - 1; j >= 0; j--) {
	//	printf("%d", binary[j]);
	//}
	//printf("\n");
}

//函数：将64位无符号长整型数转换为64位二进制整型数组.
void convert_to_binary_array(uint64_t num, int binary_array[64]) 
{
	//方法一：逐位提取法. 
	for (int i = 0; i < 64; i++) 
	{
		//数组索引0对应最高有效位(MSB),索引63对应最低有效位(LSB).
		//binary_array[i] = (num >> (63 - i)) & 1;//从最高位开始提取每一位.	
		
		//数组索引0对应最低有效位(LSB),索引63对应最高有效位(MSB).
		binary_array[i] = (num >> (i)) & 1;//从最低位开始提取每一位.
	}

	//int binary_array2[64] = { 0 };
	////方法二：掩码法.
	//for (int i = 63; i >= 0; i--) 
	//{
	//	////数组索引0对应最高有效位(MSB),索引63对应最低有效位(LSB).
	//	//binary_array2[63 - i] = (num & (1ULL << i)) ? 1 : 0;//从最高位开始提取每一位.
	
	//	//数组索引0对应最低有效位(LSB),索引63对应最高有效位(MSB).
	//	binary_array2[i] = (num & (1ULL << i)) ? 1 : 0;//从最低位开始提取每一位.
	//}

	/*///----------test查看闭合了哪些开关.------------------////
	int jj = 0;
	printf("二进制0~63: ");
	for (int j = 0; j < 64; j++)
	{
		jj++;
		printf("%d", binary_array[j]);
		if (0 == jj % 4 && 0 != jj)
		{
			printf(" ");
		}
	}
	printf("\n");
	for (int j = 0; j < 64; j++)
	{
		jj++;
		printf("%d", binary_array2[j]);
		if (0 == jj % 4 && 0 != jj)
		{
			printf(" ");
		}
	}
	////----------test查看闭合了哪些开关.------------------///*/
}

//限制数值在 [min, max] 之间，并四舍五入到小数点后3位.
double clamp_and_round(double value, double min, double max) 
{
	if (0.0 == value)
	{
		return value;
	}
	// 1. 范围限制（Clamping）
	if (value < min) 
	{
		value = min;
	}
	else if (value > max) 
	{
		value = max;
	}

	// 2. 四舍五入到小数点后3位
	value = round(value * 1000.0+0.5) / 1000.0;

	return value;
}


//写校准数据.多次写数据,只用首次写数据时,擦除一次（cnt=1时,起作用）.
ViStatus f_FlashWriteData(MT_DEVICE* device, int datasize, ViUInt32 addr, ViUInt32 dataArray[], int cnt)
{
	ViStatus status = VI_SUCCESS;

	if (1 == cnt)
	{//多次写数据,只用首次写数据时,擦除一次.
		//写使能打开
		status = FlashWriteEnable(device, MTDCPOWER_VAL_ON);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		//写擦除
		status = FlashWriteErase(device, addr);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	//写使能打开
	status = FlashWriteEnable(device, MTDCPOWER_VAL_ON);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//写数据
	status = FlashWriteData(device, datasize, addr, dataArray);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//写使能关闭
	status = FlashWriteEnable(device, MTDCPOWER_VAL_OFF);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}

	return status;
}

ViStatus FlashWriteEnable(MT_DEVICE* device, bool enable)
{
	ViStatus status = VI_SUCCESS;

	//复位
	//LOG("MTDCPOWER_ATTR_WRST_FIFO_FPGA:%x %x", MTDCPOWER_ATTR_WRST_FIFO_FPGA, 1);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRST_FIFO_FPGA, 1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//LOG("MTDCPOWER_ATTR_WRST_FIFO_FPGA:%x %x", MTDCPOWER_ATTR_WRST_FIFO_FPGA, 0);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRST_FIFO_FPGA, 0);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//配置长度
	//LOG("MTDCPOWER_ATTR_INSTRUCTION_LEN_FPGA:%x %x", MTDCPOWER_ATTR_INSTRUCTION_LEN_FPGA, 1);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_INSTRUCTION_LEN_FPGA, 1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//LOG("MTDCPOWER_ATTR_ADDRESS_LEN_FPGA:%x %x", MTDCPOWER_ATTR_ADDRESS_LEN_FPGA, 0);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_ADDRESS_LEN_FPGA, 0);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//LOG("MTDCPOWER_ATTR_DUMMY_LEN_FPGA:%x %x", MTDCPOWER_ATTR_DUMMY_LEN_FPGA, 0);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_DUMMY_LEN_FPGA, 0);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//LOG("MTDCPOWER_ATTR_DATA_LEN_FPGA:%x %x", MTDCPOWER_ATTR_DATA_LEN_FPGA, 0);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_DATA_LEN_FPGA, 0);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	if (enable)
	{
		//写FIFO使能
		//LOG("MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA:%x %x", MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, 0x06);
		status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, 0x06);//写使能打开
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else
	{
		//写FIFO使能
		//LOG("MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA:%x %x", MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, 0x04);
		status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, 0x04);//写使能关闭
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	//SPI开始使能
	//LOG("MTDCPOWER_ATTR_WRST_FIFO_FPGA:%x %x", MTDCPOWER_ATTR_START_SPI_FPGA, 1);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_START_SPI_FPGA, 1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}

	mt_Sleep(10);
	return status;
}
ViStatus FlashWriteErase(MT_DEVICE* device, ViInt32 addr)
{
	ViStatus status = VI_SUCCESS;

	ViUInt8 addr_H = (addr >> 16) & 0xFF;
	ViUInt8 addr_M = (addr >> 8) & 0xFF;
	ViUInt8 addr_L = addr & 0xFF;
	//复位
	//LOG("MTDCPOWER_ATTR_WRST_FIFO_FPGA:%x %x", MTDCPOWER_ATTR_WRST_FIFO_FPGA, 1);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRST_FIFO_FPGA, 1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//LOG("MTDCPOWER_ATTR_WRST_FIFO_FPGA:%x %x", MTDCPOWER_ATTR_WRST_FIFO_FPGA, 0);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRST_FIFO_FPGA, 0);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//配置长度
	//LOG("MTDCPOWER_ATTR_INSTRUCTION_LEN_FPGA:%x %x", MTDCPOWER_ATTR_INSTRUCTION_LEN_FPGA, 1);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_INSTRUCTION_LEN_FPGA, 1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//LOG("MTDCPOWER_ATTR_ADDRESS_LEN_FPGA:%x %x", MTDCPOWER_ATTR_ADDRESS_LEN_FPGA, 3);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_ADDRESS_LEN_FPGA, 3);//地址长度为3或者4 取决于flash的深度
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//LOG("MTDCPOWER_ATTR_DUMMY_LEN_FPGA:%x %x", MTDCPOWER_ATTR_DUMMY_LEN_FPGA, 0);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_DUMMY_LEN_FPGA, 0);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//LOG("MTDCPOWER_ATTR_DATA_LEN_FPGA:%x %x", MTDCPOWER_ATTR_DATA_LEN_FPGA, 0);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_DATA_LEN_FPGA, 0);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//写FIFO 擦除
	//LOG("MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA:%x %x", MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, MTDCPOWER_VAL_ERASE_SECTOR);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, MTDCPOWER_VAL_ERASE_SECTOR);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//LOG("MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA:%x %x", MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, addr_H);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, addr_H);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//LOG("MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA:%x %x", MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, addr_M);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, addr_M);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//LOG("MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA:%x %x", MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, addr_L);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, addr_L);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//SPI开始使能
	//LOG("MTDCPOWER_ATTR_START_SPI_FPGA:%x %x", MTDCPOWER_ATTR_START_SPI_FPGA, 1);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_START_SPI_FPGA, 1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}

	mt_Sleep(1500);
	return status;
}
ViStatus FlashWriteData(MT_DEVICE* device, int datasize, ViInt32 addr, ViUInt32 dataArray[])
{
	ViStatus status = VI_SUCCESS;

	ViUInt8 addr_H = (addr >> 16) & 0xFF;
	ViUInt8 addr_M = (addr >> 8) & 0xFF;
	ViUInt8 addr_L = addr & 0xFF;
	//复位
	//printf("MTDCPOWER_ATTR_WRST_FIFO_FPGA:%x %x \n", MTDCPOWER_ATTR_WRST_FIFO_FPGA, 1);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRST_FIFO_FPGA, 1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//printf("MTDCPOWER_ATTR_WRST_FIFO_FPGA:%x %x \n", MTDCPOWER_ATTR_WRST_FIFO_FPGA, 0);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRST_FIFO_FPGA, 0);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//配置长度
	//printf("MTDCPOWER_ATTR_INSTRUCTION_LEN_FPGA:%x %x \n", MTDCPOWER_ATTR_INSTRUCTION_LEN_FPGA, 1);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_INSTRUCTION_LEN_FPGA, 1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//printf("MTDCPOWER_ATTR_ADDRESS_LEN_FPGA:%x %x \n", MTDCPOWER_ATTR_ADDRESS_LEN_FPGA, 3);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_ADDRESS_LEN_FPGA, 3);//地址长度为3或者4 取决于flash的深度
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//printf("MTDCPOWER_ATTR_DUMMY_LEN_FPGA:%x %x \n", MTDCPOWER_ATTR_DUMMY_LEN_FPGA, 0);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_DUMMY_LEN_FPGA, 0);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//printf("MTDCPOWER_ATTR_DATA_LEN_FPGA:%x %x \n", MTDCPOWER_ATTR_DATA_LEN_FPGA, datasize);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_DATA_LEN_FPGA, datasize);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//写FIFO 写数据
	//printf("MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA:%x %x \n", MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, MTDCPOWER_VAL_PAGE_PROGRAM_WRITE);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, MTDCPOWER_VAL_PAGE_PROGRAM_WRITE);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//printf("MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA:%x %x \n", MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, addr_H);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, addr_H);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//printf("MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA:%x %x \n", MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, addr_M);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, addr_M);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//printf("MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA:%x %x \n", MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, addr_L);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, addr_L);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}

	for (int i = 0; i < datasize; i++)
	{
		//printf(" \r i = %d;  MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA:%x %x \n", i, MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, dataArray[i]);
		viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, dataArray[i]);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	//读写flag
	//printf("MTDCPOWER_ATTR_WR_RD_FLAG_FPGA:%x %x \n", MTDCPOWER_ATTR_WR_RD_FLAG_FPGA, 1);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WR_RD_FLAG_FPGA, 1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//SPI开始使能
	//printf("MTDCPOWER_ATTR_START_SPI_FPGA:%x %x \n", MTDCPOWER_ATTR_START_SPI_FPGA, 1);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_START_SPI_FPGA, 1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}

	mt_Sleep(50);
	return status;
}
//读取数据.
ViStatus FlashReadData(MT_DEVICE* device, int datasize, ViInt32 addr, ViUInt32* dataArray)
{
	ViStatus status = VI_SUCCESS;

	//1.参数验证
	if (datasize <= 0 || !dataArray) {
		device->errorCode = MTPR__ERROR_INV_PARAMETER;
		return device->errorCode;
	}
	//2.地址分解
	ViUInt8 addr_H = (addr >> 16) & 0xFF;
	ViUInt8 addr_M = (addr >> 8) & 0xFF;
	ViUInt8 addr_L = addr & 0xFF;
	//3.复位
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRST_FIFO_FPGA, 1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//printf("MTDCPOWER_ATTR_WRST_FIFO_FPGA:%x %x \n", MTDCPOWER_ATTR_WRST_FIFO_FPGA, 0);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRST_FIFO_FPGA, 0);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//4.配置长度
	//printf("MTDCPOWER_ATTR_INSTRUCTION_LEN_FPGA:%x %x \n", MTDCPOWER_ATTR_INSTRUCTION_LEN_FPGA, 1);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_INSTRUCTION_LEN_FPGA, 1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//printf("MTDCPOWER_ATTR_ADDRESS_LEN_FPGA:%x %x \n", MTDCPOWER_ATTR_ADDRESS_LEN_FPGA, 3);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_ADDRESS_LEN_FPGA, 3);//地址长度为3或者4 取决于flash的深度
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//printf("MTDCPOWER_ATTR_DUMMY_LEN_FPGA:%x %x \n", MTDCPOWER_ATTR_DUMMY_LEN_FPGA, 0);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_DUMMY_LEN_FPGA, 0);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//printf("MTDCPOWER_ATTR_DATA_LEN_FPGA:%x %x \n", MTDCPOWER_ATTR_DATA_LEN_FPGA, datasize);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_DATA_LEN_FPGA, datasize);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//5.写FIFO 写数据
	//printf("MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA:%x %x \n", MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, MTDCPOWER_VAL_READ_DATA);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, MTDCPOWER_VAL_READ_DATA);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//printf("MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA:%x %x \n", MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, addr_H);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, addr_H);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//printf("MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA:%x %x \n", MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, addr_M);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, addr_M);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//printf("MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA:%x %x \n", MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, addr_L);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, addr_L);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}

	//6.读写标志和SPI使能
	//printf("MTDCPOWER_ATTR_WR_RD_FLAG_FPGA:%x %x \n", MTDCPOWER_ATTR_WR_RD_FLAG_FPGA, 0);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WR_RD_FLAG_FPGA, 0);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//SPI开始使能
	//printf("MTDCPOWER_ATTR_START_SPI_FPGA:%x %x \n", MTDCPOWER_ATTR_START_SPI_FPGA, 1);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_START_SPI_FPGA, 1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//7.读取数据
	ViInt32* arraydata = (ViInt32*)malloc(sizeof(ViInt32) * datasize);
	if (arraydata == NULL) {
		device->errorCode = MTPR__ERROR_OUT_OF_MEMORY_HEAP;
		return device->errorCode;
	}
	for (int i = 0; i < datasize; i++)
	{
		status = viIn32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_READ_DATA_FIFO_FPGA, (ViUInt32*)(&arraydata[i]));
		//printf("\r i=%d; Read:MTDCPOWER_ATTR_READ_DATA_FIFO_FPGA:%x %x \n", i, MTDCPOWER_ATTR_READ_DATA_FIFO_FPGA, arraydata[i]);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		dataArray[i] = arraydata[i];
	}
	free(arraydata);
	
	////7.优化后的批量读取：
	//// 使用 viMoveIn32 批量读取
	//status = viMoveIn32(device->handle,     // 会话句柄
	//	VI_PXI_BAR0_SPACE,            // 地址空间
	//	MTDCPOWER_ATTR_READ_DATA_FIFO_FPGA,  // 偏移地址
	//	datasize,                     // 要读取的数据数量
	//	dataArray);                   // 存储数据的缓冲区
	//if (status != VI_SUCCESS) {
	//	device->errorCode = mt_translate_sys_error(status);
	//	return device->errorCode;
	//}

	mt_Sleep(1000);
	return status;
}
//批量读取数据.
ViStatus FlashReadData_BatchFetch(MT_DEVICE* device, int datasize, ViInt32 addr, ViUInt32* dataArray)
{
	ViStatus status = VI_SUCCESS;

	//1.参数验证
	if (datasize <= 0 || !dataArray) {
		device->errorCode = MTPR__ERROR_INV_PARAMETER;
		return device->errorCode;
	}
	//2.地址分解
	ViUInt8 addr_H = (addr >> 16) & 0xFF;
	ViUInt8 addr_M = (addr >> 8) & 0xFF;
	ViUInt8 addr_L = addr & 0xFF;
	//3.复位
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRST_FIFO_FPGA, 1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//printf("MTDCPOWER_ATTR_WRST_FIFO_FPGA:%x %x \n", MTDCPOWER_ATTR_WRST_FIFO_FPGA, 0);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRST_FIFO_FPGA, 0);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//4.配置长度
	//printf("MTDCPOWER_ATTR_INSTRUCTION_LEN_FPGA:%x %x \n", MTDCPOWER_ATTR_INSTRUCTION_LEN_FPGA, 1);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_INSTRUCTION_LEN_FPGA, 1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//printf("MTDCPOWER_ATTR_ADDRESS_LEN_FPGA:%x %x \n", MTDCPOWER_ATTR_ADDRESS_LEN_FPGA, 3);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_ADDRESS_LEN_FPGA, 3);//地址长度为3或者4 取决于flash的深度
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//printf("MTDCPOWER_ATTR_DUMMY_LEN_FPGA:%x %x \n", MTDCPOWER_ATTR_DUMMY_LEN_FPGA, 0);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_DUMMY_LEN_FPGA, 0);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//printf("MTDCPOWER_ATTR_DATA_LEN_FPGA:%x %x \n", MTDCPOWER_ATTR_DATA_LEN_FPGA, datasize);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_DATA_LEN_FPGA, datasize);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//5.写FIFO 写数据
	//printf("MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA:%x %x \n", MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, MTDCPOWER_VAL_READ_DATA);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, MTDCPOWER_VAL_READ_DATA);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//printf("MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA:%x %x \n", MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, addr_H);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, addr_H);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//printf("MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA:%x %x \n", MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, addr_M);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, addr_M);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//printf("MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA:%x %x \n", MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, addr_L);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, addr_L);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}

	//6.读写标志和SPI使能
	//printf("MTDCPOWER_ATTR_WR_RD_FLAG_FPGA:%x %x \n", MTDCPOWER_ATTR_WR_RD_FLAG_FPGA, 0);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WR_RD_FLAG_FPGA, 0);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//SPI开始使能
	//printf("MTDCPOWER_ATTR_START_SPI_FPGA:%x %x \n", MTDCPOWER_ATTR_START_SPI_FPGA, 1);
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_START_SPI_FPGA, 1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	////7.读取数据
	//ViInt32* arraydata = (ViInt32*)malloc(sizeof(ViInt32) * datasize);
	//if (arraydata == NULL) {
	//	device->errorCode = MTPR__ERROR_OUT_OF_MEMORY_HEAP;
	//	return device->errorCode;
	//}
	//for (int i = 0; i < datasize; i++)
	//{
	//  //viIn32:Reads in an 32-bit value from the specified memory space and offset.
	//	status = viIn32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_READ_DATA_FIFO_FPGA, (ViUInt32*)(&arraydata[i]));
	//	//printf("\r i=%d; Read:MTDCPOWER_ATTR_READ_DATA_FIFO_FPGA:%x %x \n", i, MTDCPOWER_ATTR_READ_DATA_FIFO_FPGA, arraydata[i]);
	//	if (status) {
	//		device->errorCode = mt_translate_sys_error(status);
	//		return device->errorCode;
	//	}
	//	dataArray[i] = arraydata[i];
	//}
	//free(arraydata);

	//7.优化后的批量读取：
	// 使用 viMoveIn32 批量读取
	//viMoveIn32:Moves a block of data from the specified address space and offset to local memory.
	status = viMoveIn32(device->handle,     // 会话句柄
		VI_PXI_BAR0_SPACE,            // 地址空间
		MTDCPOWER_ATTR_POWERON_READ_FACTOR,  // 偏移地址
		datasize,                     // 要读取的数据数量
		dataArray);                   // 存储数据的缓冲区
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}

	mt_Sleep(1000);
	return status;
}

ViStatus FlashEraseU45(MT_DEVICE* device)
{
	ViStatus  status = VI_SUCCESS;

	ViUInt32 addr = 0x400000;
	for (int i = 0; i < 64; i++)
	{
		status = FlashWriteEnableU45(device, 1);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
		status = FlashWriteEraseU45(device, addr);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}

		addr = addr + 64 * 1024;
	}

	return status;
}
ViStatus FlashWriteEnableU45(MT_DEVICE* device, bool enable)
{
	ViStatus status = VI_SUCCESS;

	//复位
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRST_FIFO_FPGA, 1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRST_FIFO_FPGA, 0);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}

	//配置长度
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_INSTRUCTION_LEN_FPGA, 1);//指令字节长度
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//地址长度为3或者4,取决于flash的深度
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_ADDRESS_LEN_FPGA, 0);//地址字节长度
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_DUMMY_LEN_FPGA, 0);//虚假字节长度
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_DATA_LEN_FPGA, 0);//数据长度
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}

	if (enable)
	{
		//写FIFO使能
		status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, 0x06);//写使能打开
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	else
	{
		//写FIFO使能
		status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, 0x04);//写使能关闭
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}
	//SPI开始使能
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_START_SPI_FPGA, 1);//SPI工作开始信号
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}

	mt_Sleep(10);
	return status;
}
ViStatus FlashWriteEraseU45(MT_DEVICE* device, ViUInt32 addr)
{
	ViStatus  status = VI_SUCCESS;

	ViUInt8 addr_H = (addr >> 16) & 0xFF;
	ViUInt8 addr_M = (addr >> 8) & 0xFF;
	ViUInt8 addr_L = addr & 0xFF;
	//printf("addr=%x : %x-%x-%x\r\n", addr, addr_H, addr_M, addr_L);
	//复位
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRST_FIFO_FPGA, 1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRST_FIFO_FPGA, 0);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}

	//配置长度
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_INSTRUCTION_LEN_FPGA, 1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_ADDRESS_LEN_FPGA, 3);//地址长度为3或者4 取决于flash的深度
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_DUMMY_LEN_FPGA, 0);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_DATA_LEN_FPGA, 0);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}

	//写FIFO 擦除
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, MTDCPOWER_VAL_ERASE_BLOCK);//64k
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, addr_H);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, addr_M);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, addr_L);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}

	//SPI开始使能
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_START_SPI_FPGA, 1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}

	mt_Sleep(1500);
	return status;
}
ViStatus FlashWriteDataU45(MT_DEVICE* device, int datasize, ViUInt32 addr, ViUInt32 dataArray[])
{
	ViStatus status = VI_SUCCESS;

	ViUInt8 addr_H = (addr >> 16) & 0xFF;
	ViUInt8 addr_M = (addr >> 8) & 0xFF;
	ViUInt8 addr_L = addr & 0xFF;
	printf("\n addr=%x : %x-%x-%x\r\n", addr, addr_H, addr_M, addr_L);

	//复位
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRST_FIFO_FPGA, 1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRST_FIFO_FPGA, 0);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}

	//配置长度
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_INSTRUCTION_LEN_FPGA, 1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_ADDRESS_LEN_FPGA, 3);//地址长度为3或者4 取决于flash的深度
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_DUMMY_LEN_FPGA, 0);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_DATA_LEN_FPGA, datasize);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}

	//写FIFO 写数据
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, MTDCPOWER_VAL_PAGE_PROGRAM_WRITE);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, addr_H);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, addr_M);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, addr_L);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}

	for (int i = 0; i < datasize; i++)
	{
		//printf("MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA:%x %x", MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, dataArray[i]);
		status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WRITE_DATA_FIFO_FPGA, dataArray[i]);
		if (status) {
			device->errorCode = mt_translate_sys_error(status);
			return device->errorCode;
		}
	}

	//读写flag
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_WR_RD_FLAG_FPGA, 1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}
	//SPI开始使能
	status = viOut32(device->handle, VI_PXI_BAR0_SPACE, MTDCPOWER_ATTR_START_SPI_FPGA, 1);
	if (status) {
		device->errorCode = mt_translate_sys_error(status);
		return device->errorCode;
	}

	mt_Sleep(10);
	return status;
}



void* mt_malloc(size_t size) 
{
	if (size > 0)
		return mt_allocator.local_malloc(size);
	return NULL;
}

char* mt_strdup(const char* s) 
{
	size_t len = strlen(s) + 1;
	char* m = (char*)mt_malloc(len);
	if (m == NULL)
		return NULL;
	return (char*)memcpy(m, s, len);
}

const char* mt_unknown_err_code(int err) 
{
	char buf[32];
	char* copy;

#if defined(_WIN32)
	_snprintf_s(buf, sizeof(buf) - 1, "Unknown system error %d", err);
#else
	snprintf(buf, sizeof(buf), "Unknown system error %d", err);
#endif
	copy = mt_strdup(buf);

	return copy != NULL ? copy : "Unknown system error";
}

int mt_strcpy(char *dest, size_t size, const char *src)
{
#if defined(_WIN32)
	return strcpy_s(dest, size, src);
#else
	return snprintf(dest, size, "%s", src);
#endif
}

//单位:微秒
void mt_Sleep(ViUInt64 dwMilliseconds)
{
#if defined(_WIN32)
	Sleep(dwMilliseconds);
#else
	usleep(1000 * dwMilliseconds);
#endif
}


