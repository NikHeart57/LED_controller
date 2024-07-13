/*
 * ST7789.h
 * �������� � ������ 65k - 12-bit/pixel (RGB-5-6-5-bit input), 65K-Colors, 3Ah=�05h�
 *
 *
 */ 


#ifndef ST7789_H_
#define ST7789_H_


//==============================================================================
//							���������� �����������
//==============================================================================
			
// ����������� ����� � ����� SPI
#define SS			PB4
#define MOSI		PB5
#define MISO		PB6
#define SCK			PB7
#define DDR_SPI		DDRB
#define PORT_SPI	PORTB

// ����������� ����� � ����� � ������� ��������� ����� ST7789
#define DC			PC2							// ��� DC
#define DDR_DC		DDRC						// ���� �� ������� ����� DC
#define PORT_DC     PORTC						// ���� �� ������� ����� DC
#define RES			PC3							// ��� RES
#define DDR_RES		DDRC						// ���� �� ������� ����� RES
#define PORT_RES	PORTC						// ���� �� ������� ����� RES

// ������� ������
#define SCREEN_WIDTH		240					// ������ ������ �� �����������
#define SCREEN_HEIGHT		240					// ������ ������ �� ���������

// ��������� ��������� ������
#define RED_FONT			31
#define GREEN_FONT			61
#define BLUE_FONT			31

#define RED_BACKGROUND		0
#define GREEN_BACKGROUND	0
#define BLUE_BACKGROUND		0

#define FONT_SIZE			2



//==============================================================================
//							������������ �����������
//==============================================================================

// ������� 
#define DC_HIGH     PORT_DC |=  (1 << DC);		// SendData
#define DC_LOW      PORT_DC &=~ (1 << DC);		// SendCommand
#define DATA		PORT_DC |=  (1 << DC);		// SendData
#define COMMAND     PORT_DC &=~ (1 << DC);		// SendCommand
#define RESET_HIGH  PORT_RES |=  (1 << RES);	//
#define RESET_LOW	PORT_RES &=~ (1 << RES);	//


// ���� ������ - ������� 1
#define CMD_NOP					0x00  // No operation
#define CMD_SWRESET				0x01  // Software reset
#define CMD_RDDID				0x04  // Read display ID
#define CMD_RDDST				0x09  // Read display status
#define CMD_RDDPM				0x0A  // Read display power
#define CMD_RDDMADCTL			0x0B  // Read display
#define CMD_RDDCOLMOD			0x0C  // Read display pixel
#define CMD_RDDIM				0x0D  // Read display image
#define CMD_RDDSM				0x0E  // Read display signal
#define CMD_RDDSDR				0x0F  // Read display self-diagnostic result
#define CMD_SLPIN				0x10  // Sleep in
#define CMD_SLPOUT				0x11  // Sleep out
#define CMD_PTLON				0x12  // Partial mode on
#define CMD_NORON				0x13  // Partial off (Normal)
#define CMD_INVOFF				0x20  // Display inversion off
#define CMD_INVON				0x21  // Display inversion on
#define CMD_GAMSET				0x26  // Gamma set
#define CMD_DISPOFF				0x28  // Display off
#define CMD_DISPON				0x29  // Display on
#define CMD_CASET				0x2A  // Column address set
#define CMD_RASET				0x2B  // Row address set
#define CMD_RAMWR				0x2C  // Memory write
#define CMD_RAMRD				0x2E  // Memory read
#define CMD_PTLAR				0x30  // Partial start/end address set
#define CMD_VSCRDEF				0x33  // Vertical scrolling definition
#define CMD_TEOFF				0x34  // Tearing line effect off
#define CMD_TEON				0x35  // Tearing line effect on
#define CMD_MADCTL				0x36  // Memory data access control
#define CMD_VSCRSADD			0x37  // Vertical address scrolling
#define CMD_IDMOFF				0x38  // Idle mode off
#define CMD_IDMON				0x39  // Idle mode on
#define CMD_COLMOD				0x3A  // Interface pixel format
#define CMD_RAMWRC				0x3C  // Memory write continue
#define CMD_RAMRDC				0x3E  // Memory read continue
#define CMD_TESCAN				0x44  // Set tear scanline
#define CMD_RDTESCAN			0x45  // Get scanline
#define CMD_WRDISBV				0x51  // Write display brightness
#define CMD_RDDISBV				0x52  // Read display brightness value
#define CMD_WRCTRLD				0x53  // Write CTRL display
#define CMD_RDCTRLD				0x54  // Read CTRL value display
#define CMD_WRCACE				0x55  // Write content adaptive brightness control and Color enhancemnet
#define CMD_RDCABC				0x56  // Read content adaptive brightness control
#define CMD_WRCABCMB			0x5E  // Write CABC minimum brightness
#define CMD_RDCABCMB			0x5F  // Read CABC minimum brightness
#define CMD_RDABCSDR			0x68  // Read Automatic Brightness Control Self-Diagnostic Result
#define CMD_RDID1				0xDA  // Read ID1
#define CMD_RDID2				0xDB  // Read ID2
#define CMD_RDID3				0xDC  // Read ID3

//���� ������ - ������� 2
#define CMD_RAMCTRL				0xB0  // RAM Control
#define CMD_RGBCTRL				0xB1  // RGB Control
#define CMD_PORCTRL				0xB2  // Porch control
#define CMD_FRCTRL1				0xB3  // Frame Rate Control 1
#define CMD_GCTRL				0xB7  // Gate control
#define CMD_DGMEN				0xBA  // Digital Gamma Enable
#define CMD_VCOMS				0xBB  // VCOM Setting
#define CMD_LCMCTRL				0xC0  // LCM Control
#define CMD_IDSET				0xC1  // ID Setting
#define CMD_VDVVRHEN			0xC2  // VDV and VRH Command enable
#define CMD_VRHS				0xC3  // VRH Set
#define CMD_VDVSET				0xC4  // VDV Setting
#define CMD_VCMOFSET			0xC5  // VCOM Offset Set
#define CMD_FRCTR2				0xC6  // FR Control 2
#define CMD_CABCCTRL			0xC7  // CABC Control
#define CMD_REGSEL1				0xC8  // Register value selection 1
#define CMD_REGSEL2				0xCA  // Register value selection 2
#define CMD_PWMFRSEL			0xCC  // PWM Frequency Selection
#define CMD_PWCTRL1				0xD0  // Power Control 1
#define CMD_VAPVANEN			0xD2  // Enable VAP/VAN signal output
#define CMD_CMD2EN				0xDF  // Command 2 Enable
#define CMD_PVGAMCTRL			0xE0  // Positive Voltage Gamma Control
#define CMD_NVGAMCTRL			0xE1  // Negative voltage Gamma Control
#define CMD_DGMLUTR				0xE2  // Digital Gamma Look-up Table for Red
#define CMD_DGMLUTB				0xE3  // Digital Gamma Look-up Table for Blue
#define CMD_GATECTRL			0xE4  // Gate control
#define CMD_PWCTRL2				0xE8  // Power Control 2
#define CMD_EQCTRL				0xE9  // Equalize Time Control
#define CMD_PROMCTRL			0xEC  // Program Control
#define CMD_PROMEN				0xFA  // Program Mode Enable
#define CMD_NVMSET				0xFC  // NVM Setting
#define CMD_PROMACT				0xFE  // Program Action

#define CMDLIST_END				0xFF  // End command (used for command list)



//==============================================================================
//								��������� ������
//==============================================================================

typedef struct colour 
{
	char red;
	char green;
	char blue;
} colour;
	
const colour RED	 = {31, 0,  0 };
const colour GREEN	 = {0,  61, 0 };
const colour BLUE	 = {0,  0,  31};
		 
const colour YELLOW	 = {31, 61, 0 };
const colour MAGENTA = {31, 0,  31};
const colour CYAN	 = {0,  61, 31};
		
const colour WHITE	 = {31, 61, 31};
const colour BLACK	 = {0,  0,  0 };

const colour colours_set[] = {RED, GREEN, BLUE, YELLOW, MAGENTA, CYAN, WHITE, BLACK};	

	

//==============================================================================
//				 ������� ������������� SPI � ������������� ������
//==============================================================================
		
void ST7789_InitSPI(void);
void ST7789_InitST7789(void);

//==============================================================================
//								������� � �������
//==============================================================================	

void ST7789_SetXpos(char x);	
void ST7789_SetYpos(char y);
void ST7789_SetXYpos(char x, char y);

void ST7789_GetXpos(char* x);
void ST7789_GetYpos(char* y);
void ST7789_GetXYpos(char* x, char* y);
		
//==============================================================================
//				�������� ������� ��� ������ �� ����� ���� � ���������
//==============================================================================

// ���������� �������
void ST7789_DrawRectangle(char x, char y, char hight, char lentgh, char red, char green, char blue);
void ST7789_DrawRectangle(char x, char y, char hight, char lentgh, colour paint);

// ���������� �������
void ST7789_DrawSquare(char x, char y, char size, char red, char green, char blue);
void ST7789_DrawSquare(char x, char y, char size, colour paint);


// ������ ����� ������		
void ST7789_FillScreen(char red, char green, char blue);
void ST7789_FillScreen(colour paint);
	
// �������� ������ ASCII
	// ������������� ������
	// �� ������� ���� ������� ������
void ST7789_PrintChar(char letter, char red_font, char green_font, char blue_font, char red_background, char green_background, char blue_background, char size);						// �� RGB	� ��� ���������
void ST7789_PrintChar(char letter, colour colour_font, colour colour_background, char size);																							// �� �����	� ��� ���������
void ST7789_PrintChar(char letter, char xcur, char ycur, char red_font, char green_font, char blue_font, char red_background, char green_background, char blue_background, char size);	// �� RGB	� � ������������
void ST7789_PrintChar(char letter, char xcur, char ycur, colour colour_font, colour colour_background, char size);																		// �� ����� � � ������������
	// ������������� ������
	// �� ��������� ���� ������� ������
void ST7789_PrintChar(char letter, char red_font, char green_font, char blue_font, char size);									// �� RGB	� ��� ���������
void ST7789_PrintChar(char letter, colour colour_font, char size);																// �� �����	� ��� ���������
void ST7789_PrintChar(char letter, char xcur, char ycur, char red_font, char green_font, char blue_font, char size);			// �� RGB	� � ������������
void ST7789_PrintChar(char letter, char xcur, char ycur, colour colour_font, char size);										// �� �����	� � ������������
	// ����������� �����
	// �� ��������� ���� ��������� ������
void ST7789_PrintChar(char letter, char size);									// ��� ���������
void ST7789_PrintChar(char letter);												// ��� ���������
void ST7789_PrintChar(char letter, char xcur, char ycur, char size);			// � ������������
void ST7789_PrintChar(char letter, char xcur, char ycur);						// � ������������


// �������� ������ ASCII
	// ������������� ������
	// �� ������� ���� ������� ������
void ST7789_PrintString(char* string, char red_font, char green_font, char blue_font, char red_background, char green_background, char blue_background, char size);								// �� RGB	� ��� ���������
void ST7789_PrintString(char* string, colour colour_font, colour colour_background, char size);																									// �� �����	� ��� ���������
void ST7789_PrintString(char* string, char xcur, char ycur, char red_font, char green_font, char blue_font, char red_background, char green_background, char blue_background, char size);		// �� RGB	� � ������������
void ST7789_PrintString(char* string, char xcur, char ycur, colour colour_font, colour colour_background, char size);																			// �� �����	� � ������������
	// ������������� ������
	// �� ��������� ���� ������� ������
void ST7789_PrintString(char* string, char red_font, char green_font, char blue_font, char size);							// �� RGB	� ��� ���������
void ST7789_PrintString(char* string, colour colour_font, char size);														// �� ����� � ��� ���������
void ST7789_PrintString(char* string, char xcur, char ycur, char red_font, char green_font, char blue_font, char size);		// �� RGB	� � ������������
void ST7789_PrintString(char* string, char xcur, char ycur, colour colour_font, char size);									// �� ����� � � ������������
	// ����������� �����
	// �� ��������� ���� ��������� ������
void ST7789_PrintString(char* string, char size);								// ��� ���������
void ST7789_PrintString(char* string);											// ��� ���������
void ST7789_PrintString(char* string, char xcur, char ycur, char size);			// � ������������
void ST7789_PrintString(char* string, char xcur, char ycur);					// � ������������
	
//==============================================================================
//	INLINE				������� �������� ������ �� SPI
//==============================================================================
		
inline void ST7789_Send(char data);	
inline void ST7789_SendData(char data);
inline void ST7789_SendCommand(char data);
	
	
//==============================================================================
//							   ����� monocraft
//==============================================================================
	
const char monocraft[224][6] =
{
		
	//0x00 - 0x0F
	//{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},							// ����������� �������
		
	//0x10 - 0x1F
	//{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},							// ����������� �������
		
	//0x20 - 0x2F
	{0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000},	// 0x20 - 32 - ������
	{0b00000000, 0b11111010, 0b00000000, 0b00000000, 0b00000000, 0b00000000},	// 0x21 - 33 - !
	{0b00000000, 0b01100000, 0b00000000, 0b01100000, 0b00000000, 0b00000000},	// 0x22 - 34 - "
	{0b00101000, 0b11111110, 0b00101000, 0b11111110, 0b00101000, 0b00000000},	// 0x23 - 35 - #
	{0b00100100, 0b01010100, 0b11010110, 0b01010100, 0b01001000, 0b00000000},	// 0x24 - 36 - $
	{0b11000010, 0b00001100, 0b00010000, 0b01100000, 0b10000110, 0b00000000},	// 0x25 - 37 - %
	{0b00001100, 0b01010010, 0b10111010, 0b01001100, 0b00010010, 0b00000000},	// 0x26 - 38 - &
	{0b00000000, 0b00000000, 0b01100000, 0b00000000, 0b00000000, 0b00000000},	// 0x27 - 39 - '
	{0b00000000, 0b00111000, 0b01000100, 0b10000010, 0b00000000, 0b00000000},	// 0x28 - 40 - (
	{0b00000000, 0b10000010, 0b01000100, 0b00111000, 0b00000000, 0b00000000},	// 0x29 - 41 - )
	{0b00000000, 0b01010000, 0b00100000, 0b01010000, 0b00000000, 0b00000000},	// 0x2A - 42 - *
	{0b00010000, 0b00010000, 0b01111100, 0b00010000, 0b00010000, 0b00000000},	// 0x2B - 43 - +
	{0b00000000, 0b00000001, 0b00000010, 0b00000000, 0b00000000, 0b00000000},	// 0x2C - 44 - ,
	{0b00000000, 0b00010000, 0b00010000, 0b00010000, 0b00000000, 0b00000000},	// 0x2D - 45 - -
	{0b00000000, 0b00000000, 0b00000010, 0b00000000, 0b00000000, 0b00000000},	// 0x2E - 46 - .
	{0b00000010, 0b00001100, 0b00010000, 0b01100000, 0b10000000, 0b00000000},	// 0x2F - 47 - /
		
	//0x30 - 0x3F
	{0b01111100, 0b10001010, 0b10010010, 0b10100010, 0b01111100, 0b00000000},	// 0x30 - 48 - 0
	{0b00000010, 0b01000010, 0b11111110, 0b00000010, 0b00000010, 0b00000000},	// 0x31 - 49 - 1
	{0b01000110, 0b10001010, 0b10010010, 0b10010010, 0b01100110, 0b00000000},	// 0x32 - 50 - 2
	{0b01000100, 0b10000010, 0b10010010, 0b10010010, 0b01101100, 0b00000000},	// 0x33 - 51 - 3
	{0b00011000, 0b00101000, 0b01001000, 0b10001000, 0b11111110, 0b00000000},	// 0x34 - 52 - 4
	{0b11100100, 0b10100010, 0b10100010, 0b10100010, 0b10011100, 0b00000000},	// 0x35 - 53 - 5
	{0b00111100, 0b01010010, 0b10010010, 0b10010010, 0b00001100, 0b00000000},	// 0x36 - 54 - 6
	{0b11000000, 0b10000000, 0b10001110, 0b10010000, 0b11100000, 0b00000000},	// 0x37 - 55 - 7
	{0b01101100, 0b10010010, 0b10010010, 0b10010010, 0b01101100, 0b00000000},	// 0x38 - 56 - 8
	{0b01100000, 0b10010010, 0b10010010, 0b10010100, 0b01111000, 0b00000000},	// 0x39 - 57 - 9
	{0b00000000, 0b00000000, 0b00100100, 0b00000000, 0b00000000, 0b00000000},	// 0x3A - 58 - :
	{0b00000000, 0b00000010, 0b00100100, 0b00000000, 0b00000000, 0b00000000},	// 0x3B - 59 - ;
	{0b00010000, 0b00101000, 0b01000100, 0b10000010, 0b00000000, 0b00000000},	// 0x3C - 60 - <
	{0b00100100, 0b00100100, 0b00100100, 0b00100100, 0b00000000, 0b00000000},	// 0x3D - 61 - =
	{0b10000010, 0b01000100, 0b00101000, 0b00010000, 0b00000000, 0b00000000},	// 0x3E - 62 - >
	{0b01000000, 0b10000000, 0b10001010, 0b10010000, 0b01100000, 0b00000000},	// 0x3F - 63 - ?
		
	//0x40 - 0x4F
	{0b00111100, 0b01000010, 0b01011010, 0b01001010, 0b00111010, 0b00000000},	// 0x40 - 64 - @
	{0b01111110, 0b10010000, 0b10010000, 0b10010000, 0b01111110, 0b00000000},	// 0x41 - 65 - A
	{0b11111110, 0b10010010, 0b10010010, 0b10010010, 0b01101100, 0b00000000},	// 0x42 - 66 - B
	{0b01111100, 0b10000010, 0b10000010, 0b10000010, 0b01000100, 0b00000000},	// 0x43 - 67 - C
	{0b11111110, 0b10000010, 0b10000010, 0b10000010, 0b01111100, 0b00000000},	// 0x44 - 68 - D
	{0b11111110, 0b10010010, 0b10010010, 0b10010010, 0b10000010, 0b00000000},	// 0x45 - 69 - E
	{0b11111110, 0b10010000, 0b10010000, 0b10010000, 0b10000000, 0b00000000},	// 0x46 - 70 - F
	{0b01111100, 0b10000010, 0b10000010, 0b10100010, 0b10111100, 0b00000000},	// 0x47 - 71 - G
	{0b11111110, 0b00010000, 0b00010000, 0b00010000, 0b11111110, 0b00000000},	// 0x48 - 72 - H
	{0b00000000, 0b10000010, 0b11111110, 0b10000010, 0b00000000, 0b00000000},	// 0x49 - 73 - I
	{0b00000100, 0b00000010, 0b00000010, 0b00000010, 0b11111100, 0b00000000},	// 0x4A - 74 - J
	{0b11111110, 0b00100000, 0b00100000, 0b01010000, 0b10001110, 0b00000000},	// 0x4B - 75 - K
	{0b11111110, 0b00000010, 0b00000010, 0b00000010, 0b00000000, 0b00000000},	// 0x4C - 76 - L
	{0b11111110, 0b01000000, 0b00100000, 0b01000000, 0b11111110, 0b00000000},	// 0x4D - 77 - M
	{0b11111110, 0b01000000, 0b00100000, 0b00010000, 0b11111110, 0b00000000},	// 0x4E - 78 - N
	{0b01111100, 0b10000010, 0b10000010, 0b10000010, 0b01111100, 0b00000000},	// 0x4F - 79 - O
		
	//0x50 - 0x5F
	{0b11111110, 0b10010000, 0b10010000, 0b10010000, 0b01100000, 0b00000000},	// 0x50 - 80 - P
	{0b01111100, 0b10000010, 0b10000010, 0b10000100, 0b01111010, 0b00000000},	// 0x51 - 81 - Q
	{0b11111110, 0b10010000, 0b10010000, 0b10010000, 0b01101110, 0b00000000},	// 0x52 - 82 - R
	{0b01100100, 0b10010010, 0b10010010, 0b10010010, 0b01001100, 0b00000000},	// 0x53 - 83 - S
	{0b10000000, 0b10000000, 0b11111110, 0b10000000, 0b10000000, 0b00000000},	// 0x54 - 84 - T
	{0b11111100, 0b00000010, 0b00000010, 0b00000010, 0b11111100, 0b00000000},	// 0x55 - 85 - U
	{0b11110000, 0b00001100, 0b00000010, 0b00001100, 0b11110000, 0b00000000},	// 0x56 - 86 - V
	{0b11111110, 0b00000100, 0b00001000, 0b00000100, 0b11111110, 0b00000000},	// 0x57 - 87 - W
	{0b11000110, 0b00101000, 0b00010000, 0b00101000, 0b11000110, 0b00000000},	// 0x58 - 88 - X
	{0b10000000, 0b01000000, 0b00111110, 0b01000000, 0b10000000, 0b00000000},	// 0x59 - 89 - Y
	{0b10000110, 0b10001010, 0b10010010, 0b10100010, 0b11000010, 0b00000000},	// 0x5A - 90 - Z
	{0b00000000, 0b11111110, 0b10000010, 0b10000010, 0b00000000, 0b00000000},	// 0x5B - 91 - [
	{0b10000000, 0b01100000, 0b00010000, 0b00001100, 0b00000010, 0b00000000},	// 0x5C - 92 - \	backslash
	{0b00000000, 0b10000010, 0b10000010, 0b11111110, 0b00000000, 0b00000000},	// 0x5D - 93 - ]
	{0b00100000, 0b01000000, 0b10000000, 0b01000000, 0b00100000, 0b00000000},	// 0x5E - 94 - ^
	{0b00000001, 0b00000001, 0b00000001, 0b00000001, 0b00000001, 0b00000000},	// 0x5F - 95 - _

	//0x60 - 0x6F
	{0b01000000, 0b00100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000},	// 0x60 - 96 - '
	{0b00000100, 0b00101010, 0b00101010, 0b00101010, 0b00011110, 0b00000000},	// 0x61 - 97 - a
	{0b01111110, 0b00010010, 0b00100010, 0b00100010, 0b00011100, 0b00000000},	// 0x62 - 98 - b
	{0b00011100, 0b00100010, 0b00100010, 0b00100010, 0b00010100, 0b00000000},	// 0x63 - 99 - c
	{0b00011100, 0b00100010, 0b00100010, 0b00010010, 0b01111110, 0b00000000},	// 0x64 - 100 - d
	{0b00011100, 0b00101010, 0b00101010, 0b00101010, 0b00011010, 0b00000000},	// 0x65 - 101 - e
	{0b00100000, 0b01111110, 0b10100000, 0b10100000, 0b00000000, 0b00000000},	// 0x66 - 102 - f
	{0b00011001, 0b00100101, 0b00100101, 0b00100101, 0b00111110, 0b00000000},	// 0x67 - 103 - g
	{0b11111110, 0b00010000, 0b00100000, 0b00100000, 0b00011110, 0b00000000},	// 0x68 - 104 - h
	{0b00000000, 0b00100000, 0b10111100, 0b00000010, 0b00000000, 0b00000000},	// 0x69 - 105 - i
	{0b00000100, 0b00000010, 0b00000010, 0b00000010, 0b10111100, 0b00000000},	// 0x6A - 106 - j
	{0b00000000, 0b11111110, 0b00001000, 0b00010100, 0b00100010, 0b00000000},	// 0x6B - 107 - k
	{0b00000000, 0b10000000, 0b11111100, 0b00000010, 0b00000010, 0b00000000},	// 0x6C - 108 - l
	{0b00111110, 0b00100000, 0b00011000, 0b00100000, 0b00011110, 0b00000000},	// 0x6D - 109 - m
	{0b00111110, 0b00100000, 0b00100000, 0b00100000, 0b00011110, 0b00000000},	// 0x6E - 110 - n
	{0b00011100, 0b00100010, 0b00100010, 0b00100010, 0b00011100, 0b00000000},	// 0x6F - 111 - o
		
	//0x70 - 0x7F
	{0b00111111, 0b00010100, 0b00100100, 0b00100100, 0b00011000, 0b00000000},	// 0x70 - 112 - p
	{0b00011000, 0b00100100, 0b00100100, 0b00010100, 0b00111111, 0b00000000},	// 0x71 - 113 - q
	{0b00111110, 0b00010000, 0b00100000, 0b00100000, 0b00010000, 0b00000000},	// 0x72 - 114 - r
	{0b00010010, 0b00101010, 0b00101010, 0b00101010, 0b00100100, 0b00000000},	// 0x73 - 115 - s
	{0b00000000, 0b00100000, 0b11111100, 0b00100010, 0b00000000, 0b00000000},	// 0x74 - 116 - t
	{0b00111100, 0b00000010, 0b00000010, 0b00000010, 0b00111110, 0b00000000},	// 0x75 - 117 - u
	{0b00111000, 0b00000100, 0b00000010, 0b00000100, 0b00111000, 0b00000000},	// 0x76 - 118 - v
	{0b00111100, 0b00000010, 0b00001110, 0b00000010, 0b00111110, 0b00000000},	// 0x77 - 119 - w
	{0b00100010, 0b00010100, 0b00001000, 0b00010100, 0b00100010, 0b00000000},	// 0x78 - 120 - x
	{0b00111001, 0b00000101, 0b00000101, 0b00000101, 0b00111110, 0b00000000},	// 0x79 - 121 - y
	{0b00100010, 0b00100110, 0b00101010, 0b00110010, 0b00100010, 0b00000000},	// 0x7A - 122 - z
	{0b00000000, 0b00010000, 0b01101100, 0b10000010, 0b00000000, 0b00000000},	// 0x7B - 123 - �������� ������ �����
	{0b00000000, 0b00000000, 0b11111110, 0b00000000, 0b00000000, 0b00000000},	// 0x7C - 124 - |
	{0b00000000, 0b10000010, 0b01101100, 0b00010000, 0b00000000, 0b00000000},	// 0x7D - 125 - �������� ������ ������
	{0b00011000, 0b00100000, 0b00010000, 0b00001000, 0b00110000, 0b00000000},	// 0x7E - 126 - ~
	{0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000},	// 0x7F - 127 - DEL
			
	// ������������ ������� ASCII	
	//0x80 - 0x8F
	{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},							// 
			
	//0x90 - 0x9F
	{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},							//
			
	//0xA0 - 0xAF
	{},	// 0xA0 - 160 -
	{},	// 0xA1 - 161 -
	{},	// 0xA2 - 162 -
	{},	// 0xA3 - 163 -
	{},	// 0xA4 - 164 -
	{},	// 0xA5 - 165 -
	{},	// 0xA6 - 166 -
	{},	// 0xA7 - 167 -
	{0b00111110, 0b10101010, 0b00101010, 0b10101010, 0b00100010, 0b00000000},	// 0xA8 - 168 - �
	{},	// 0xA9 - 169 -
	{},	// 0xAA - 170 -
	{},	// 0xAB - 171 -
	{},	// 0xAC - 172 -
	{},	// 0xAD - 173 -
	{},	// 0xAE - 174 -
	{},	// 0xAF - 175 -
			
	//0xB0 - 0xBF
	{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},							//
			
	//0xC0 - 0xCF
	{0b01111110, 0b10010000, 0b10010000, 0b10010000, 0b01111110, 0b00000000},	// 0xC0 - 192 - �
	{0b11111110, 0b10100010, 0b10100010, 0b10100010, 0b10011100, 0b00000000},	// 0xC1 - 193 - �
	{0b11111110, 0b10010010, 0b10010010, 0b10010010, 0b01101100, 0b00000000},	// 0xC2 - 194 - �
	{0b11111110, 0b10000000, 0b10000000, 0b10000000, 0b10000000, 0b00000000},	// 0xC3 - 195 - �
	{0b00000011, 0b00011110, 0b11100010, 0b10000010, 0b11111111, 0b00000000},	// 0xC4 - 196 - �
	{0b11111110, 0b10010010, 0b10010010, 0b10010010, 0b10000010, 0b00000000},	// 0xC5 - 197 - �
	{0b11011110, 0b00100000, 0b11111110, 0b00100000, 0b11011110, 0b00000000},	// 0xC6 - 198 - �
	{0b01000100, 0b10000010, 0b10100010, 0b10100010, 0b01011100, 0b00000000},	// 0xC7 - 199 - �
	{0b11111110, 0b00000100, 0b00001000, 0b00110000, 0b11111110, 0b00000000},	// 0xC8 - 200 - �
	{},	// 0xC9 - 201 - �
	{},	// 0xCA - 202 - �
	{},	// 0xCB - 203 - �
	{},	// 0xCC - 204 - �
	{0b11111110, 0b00010000, 0b00010000, 0b00010000, 0b11111110, 0b00000000},	// 0xCD - 205 - �
	{},	// 0xCE - 206 - �
	{},	// 0xCF - 207 - �
			
	//0xD0 - 0xDF
	{0b11111110, 0b10010000, 0b10010000, 0b10010000, 0b01100000, 0b00000000},	// 0xD0 - 208 - �
	{},	// 0xD1 - 209 - �
	{},	// 0xD2 - 210 - �
	{},	// 0xD3 - 211 - �
	{},	// 0xD4 - 212 - �
	{},	// 0xD5 - 213 - �
	{},	// 0xD6 - 214 - �
	{},	// 0xD7 - 215 - �
	{},	// 0xD8 - 216 - �
	{},	// 0xD9 - 217 - �
	{},	// 0xDA - 218 - �
	{},	// 0xDB - 219 - �
	{},	// 0xDC - 220 - �
	{},	// 0xDD - 221 - �
	{},	// 0xDE - 222 - �
	{},	// 0xDF - 223 - �
			
	//0xE0 - 0xEF
	{0b00000100, 0b00101010, 0b00101010, 0b00101010, 0b00011110, 0b00000000},	// 0xE0 - 224 - �
	{},	// 0xE1 - 225 - �
	{0b00111110, 0b00101010, 0b00101010, 0b00101010, 0b00010100, 0b00000000},	// 0xE2 - 225 - �
	{}, // 0xE3 - 226 - �
	{},	// 0xE4 - 227 - �
	{0b00011100, 0b00101010, 0b00101010, 0b00101010, 0b00011010, 0b00000000},	// 0xE5 - 228 - �
	{},	// 0xE6 - 229 - �
	{},	// 0xE7 - 230 - �
	{0b00111110, 0b00000100, 0b00001000, 0b00010000, 0b00111110, 0b00000000},	// 0xE8 - 231 - �
	{0b00111110, 0b10000100, 0b01001000, 0b10010000, 0b00111110, 0b00000000},	// 0xE9 - 232 - �
	{0b00111110, 0b00001000, 0b00001000, 0b00010100, 0b00100010, 0b00000000},	// 0xEA - 233 - �
	{},	// 0xEB - 234 - �
	{},	// 0xEC - 235 - �
	{0b00111110, 0b00001000, 0b00001000, 0b00001000, 0b00111110, 0b00000000},	// 0xED - 236 - �
	{0b00011100, 0b00100010, 0b00100010, 0b00100010, 0b00011100, 0b00000000},	// 0xEE - 237 - �
	{},	// 0xEF - 238 - �
			
	//0xF0 - 0xFF
	{0b00111111, 0b00010100, 0b00100100, 0b00100100, 0b00011000, 0b00000000},	// 0xF0 - 240 - �
	{0b00011100, 0b00100010, 0b00100010, 0b00100010, 0b00010100, 0b00000000},	// 0xF1 - 241 - �
	{0b00100000, 0b00100000, 0b00111110, 0b00100000, 0b00100000, 0b00000000},	// 0xF2 - 242 - �
	{},	// 0xF3 - 243 - �
	{},	// 0xF4 - 244 - �
	{},	// 0xF5 - 245 - �
	{},	// 0xF6 - 246 - �
	{0b00110000, 0b00001000, 0b00001000, 0b00001000, 0b00111110, 0b00000000},	// 0xF7 - 247 - �
	{},	// 0xF8 - 248 - �
	{},	// 0xF9 - 249 - �
	{},	// 0xFA - 250 - �
	{},	// 0xFB - 251 - �
	{0b00111110, 0b00001010, 0b00001010, 0b00001010, 0b00000100, 0b00000000},	// 0xFC - 252 - �
	{},	// 0xFD - 253 - �
	{},	// 0xFE - 254 - �
	{},	// 0xFF - 255 - �
};
	
	
	
		
////// 							������ INLINE							  //////
//==============================================================================
//	INLINE					������� ��������� SPI
//==============================================================================
	
inline void Send(char data)
{
	SPDR = data;
	while(!(SPSR & (1 << SPIF)));
}
				
inline void SendData(char data)
{
	DC_HIGH;
	SPDR = data;
	while(!(SPSR & (1 << SPIF)));
}

inline void SendCommand(char data)
{
	DC_LOW;
	SPDR = data;
	while(!(SPSR & (1 << SPIF)));
}
	
#endif /* ST7789_H_ */