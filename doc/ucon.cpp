/*
  ucon.cpp
  -
  uCON unix Source Code - splice (splice@parodius.com)
                          Last Updated: July 27, 1999

  uCON DOS Version      - 1995 (Chicken & chp)
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <fcntl.h>
#include <math.h>

// #define DEBUG
char commands[1];
char comname1[64];
char comname2[64];
char comname3[64];

int     comnumber;
int     k=0, z=0, hirom=0, h=-1, kk=0, ramsize, romindex;
int     console=0;
char    uconver[6], readchunk[8192], readchunk2[8192];
char    readchunk3[32768];
char    seganame[15];

void calcandwriteheader(FILE *fileptrtodo, char *newheader)
{
 unsigned long  truesize;

 fseek(fileptrtodo, 0, SEEK_END);
 truesize=ftell(fileptrtodo);
 fseek(fileptrtodo, 0, SEEK_SET);
 truesize-=ftell(fileptrtodo);
 truesize=(truesize-512)/8192;
 newheader[1]=truesize/256;
 newheader[0]=(truesize)-(newheader[1]*256);
 fseek(fileptrtodo, 0, SEEK_SET);
 fwrite(newheader, sizeof(newheader), 1, fileptrtodo);
}

void chkiffileexist(FILE *fileptrtochk, char *filenametochk)
{
 if (fileptrtochk==NULL)
	{
        printf("\nuCON Error:  Cannot open input file \"%s.\"\n\n", filenametochk);
	exit(1);
	}
}

void id_error()
{
 printf("\nuCON Error:  Image cannot be identified.  Check to make sure it is actually\n");
 printf("             an image, and that it is the first multi file.\n\n");
 printf(" Use a system switch (/sg or /sn) last to override the error checking.\n");
 exit(255);
}

void commanderror(char badcommand, int badsystem)
{
 char   systemname[2][13];

 strcpy(systemname[0], "Super NES");
 strcpy(systemname[1], "Sega Genesis");
 systemname[0][sizeof(systemname)]='\0';
 systemname[1][sizeof(systemname)]='\0';
printf("\nuCON Error:  The %c command only applies to %s images.\n\n", badcommand, systemname[badsystem]);
 exit(255);
}

void diskspaceerror()
{
 printf("\nuCON Error:  Error while writing file.  Check free space.\n");
 exit(255);
}

void checkminsize (unsigned long testfilesize)
{
 if (testfilesize<32768)
	{
	printf("\nuCON Error:  Image specified is less than .25 Mbit in length.\n\n");
	exit(255);
	}
}

void getcinfo()
{
// unsigned long ROMChecksum, ROMComplementChecksum;

  char  cartname[23];
  char  cartinfo[256][40];
  int   cartsize;
  char  makerid[256][33];
  char  countryid[256][40];
  char  format[8][32];
  int   version;
  float filesize;
  unsigned long file_size;
  FILE  *file_ptr;
  char  inchar[1], tempchar[1];
  int   makerindex, countryindex, j;
  float subsize=512.0;
  signed long offset=0;
  int   power=0;
  int   formattemp;
  int checksum1, checksum2, comchecksum1, comchecksum2;
  unsigned int checksum, comchecksum;
  int   error=0, boxercheck=0;
  float introsize;

 cartname[22]='\0';
 for (j=0; j<255; j++)
	{
	strcpy(cartinfo[j], "<<Unknown>>");
	strcpy(countryid[j], "<<Unknown>>");
	}

 strcpy(makerid[0], "<<INVALID>>");
 strcpy(makerid[1], "Nintendo");
 strcpy(makerid[2], "<<Unknown 2>>");
 strcpy(makerid[3], "Imagineer-Zoom");
 strcpy(makerid[4], "<<Unknown 4>>");
 strcpy(makerid[5], "Zamuse");
 strcpy(makerid[6], "Falcom");
 strcpy(makerid[7], "<<Unknown 7>>");
 strcpy(makerid[8], "Capcom");
 strcpy(makerid[9], "HOT-B");
 strcpy(makerid[10], "Jaleco");
 strcpy(makerid[11], "Coconuts");
 strcpy(makerid[0x0c], "Coconuts");
 strcpy(makerid[13], "<<Unknown 13>>");
 strcpy(makerid[14], "Technos");
 strcpy(makerid[15], "Pack-In-Video");
 strcpy(makerid[16], "<<Unknown 16>>");
 strcpy(makerid[0x11], "Starfish");
 strcpy(makerid[18], "Gremlin Graphics");
 strcpy(makerid[19], "Electronic Arts");
 strcpy(makerid[20], "<<Unknown 20>>");
 strcpy(makerid[21], "Cobra Team");
 strcpy(makerid[22], "Human");
 strcpy(makerid[23], "KOEI");
 strcpy(makerid[24], "Hudson Soft");
 strcpy(makerid[25], "Yanoman");
 strcpy(makerid[26], "Yanoman");
 strcpy(makerid[27], "<<Unknown 27>>");
 strcpy(makerid[28], "Tecmo Products");
 strcpy(makerid[29], "<<Unknown 29>>");
 strcpy(makerid[30], "Open System");
 strcpy(makerid[31], "Virgin Games");
 strcpy(makerid[32], "KSS");
 strcpy(makerid[33], "Sunsoft");
 strcpy(makerid[34], "Pow");
 strcpy(makerid[35], "Micro World");
 strcpy(makerid[36], "<<Unknown 36>>");
 strcpy(makerid[37], "<<Unknown 37>>");
 strcpy(makerid[38], "Enix");
 strcpy(makerid[39], "Loriciel/Electro Brain");
 strcpy(makerid[40], "Kemco");
 strcpy(makerid[41], "Seta");
 strcpy(makerid[42], "<<Unknown 42>>");
 strcpy(makerid[43], "<<Unknown 43>>");
 strcpy(makerid[44], "<<Unknown 44>>");
 strcpy(makerid[45], "Visit");
 strcpy(makerid[46], "<<Unknown 46>>");
 strcpy(makerid[47], "<<Unknown 47>>");
 strcpy(makerid[48], "Viacom New Media");
// lmk-sio.lha
 strcpy(makerid[49], "Carrozzeria");
 strcpy(makerid[50], "Dynamic");
// strcpy(makerid[0x33], "Rumic Soft");
// Media Rings Corporation ?
// strcpy(makerid[0x33], "Saurus");
 strcpy(makerid[0x33], "<<UNDEFINED>>");
 strcpy(makerid[52], "Magifact");
// strcpy(makerid[53], "DTMC");
 strcpy(makerid[53], "Hect");
 strcpy(makerid[54], "<<Unknown 54>>");
 strcpy(makerid[55], "<<Unknown 55>>");
 strcpy(makerid[56], "<<Unknown 56>>");
 strcpy(makerid[57], "<<Unknown 57>>");
 strcpy(makerid[58], "<<Unknown 58>>");
 strcpy(makerid[59], "Arcade Zone");
 strcpy(makerid[60], "Empire Software");
// strcpy(makerid[61], "Loriciel/Electro Brain");
 strcpy(makerid[61], "Loriciel");
 strcpy(makerid[62], "<<Unknown 62>>");
 strcpy(makerid[63], "<<Unknown 63>>");
// strcpy(makerid[64], "Seika Corp.");
 strcpy(makerid[64], "Seika");
 strcpy(makerid[65], "Ubi Soft");
 strcpy(makerid[66], "<<Unknown 66>>");
 strcpy(makerid[67], "<<Unknown 67>>");
 strcpy(makerid[68], "<<Unknown 68>>");
 strcpy(makerid[69], "<<Unknown 69>>");
// PAL 246
 strcpy(makerid[70], "System 3");
 strcpy(makerid[71], "Spectrum HoloByte");
 strcpy(makerid[72], "<<Unknown 72>>");
 strcpy(makerid[73], "Irem");
 strcpy(makerid[74], "<<Unknown 74>>");
// strcpy(makerid[75], "Raya Systems/Sculptured Software");
 strcpy(makerid[75], "Raya Systems");
 strcpy(makerid[76], "Renovation Products*");
 strcpy(makerid[77], "Malibu Games");
 strcpy(makerid[78], "<<Unknown 78>>");
 strcpy(makerid[79], "U.S. Gold");
// strcpy(makerid[80], "Absolute Entertainment");
 strcpy(makerid[80], "Absolute");
 strcpy(makerid[81], "Acclaim");
 strcpy(makerid[82], "Activision");
 strcpy(makerid[83], "American Sammy");
// strcpy(makerid[84], "GameTek");
 strcpy(makerid[84], "Gametek");
// strcpy(makerid[85], "Hi-Tech Expressions Inc.");
// strcpy(makerid[85], "Hi-Tech Expressions");
// strcpy(makerid[85], "Hi Tech Expressions");
 strcpy(makerid[85], "Hi Tech Entertainment");
 strcpy(makerid[86], "LJN");
 strcpy(makerid[87], "<<Unknown 87>>");
 strcpy(makerid[88], "<<Unknown 88>>");
 strcpy(makerid[89], "<<Unknown 89>>");
 strcpy(makerid[90], "Mindscape");
 strcpy(makerid[91], "<<Unknown 91>>");
 strcpy(makerid[92], "<<Unknown 92>>");
 strcpy(makerid[93], "Tradewest");
 strcpy(makerid[94], "<<Unknown 94>>");
// strcpy(makerid[95], "American Softworks Corp.");
 strcpy(makerid[95], "American Softworks");
 strcpy(makerid[96], "Titus");
// strcpy(makerid[97], "Virgin Games");
 strcpy(makerid[97], "Virgin Interactive Entertainment");
 strcpy(makerid[98], "Maxis*");
 strcpy(makerid[99], "Origin/FCI/Pony Canyon");
 strcpy(makerid[100], "<<Unknown 100>>");
 strcpy(makerid[101], "<<Unknown 101>>");
 strcpy(makerid[102], "<<Unknown 102>>");
 strcpy(makerid[103], "Ocean");
 strcpy(makerid[104], "<<Unknown 104>>");
 strcpy(makerid[105], "Electronic Arts");
 strcpy(makerid[106], "<<Unknown 106>>");
 strcpy(makerid[107], "Laserbeam");
 strcpy(makerid[108], "<<Unknown 108>>");
 strcpy(makerid[109], "<<Unknown 109>>");
 strcpy(makerid[110], "Elite");
 strcpy(makerid[111], "Electro Brain");
 strcpy(makerid[112], "Infogrames");
 strcpy(makerid[113], "Interplay");
 strcpy(makerid[114], "LucasArts");
// strcpy(makerid[115], "Parker Bros.");
// strcpy(makerid[115], "Waddington Games");
 strcpy(makerid[115], "Parker Brothers");
 strcpy(makerid[116], "Konami");
 strcpy(makerid[117], "STORM*");
 strcpy(makerid[118], "<<Unknown 118>>");
 strcpy(makerid[119], "<<Unknown 119>>");
// strcpy(makerid[120], "T*HQ Software");
 strcpy(makerid[120], "T*HQ");
// strcpy(makerid[121], "Accolade Inc.");
 strcpy(makerid[121], "Accolade");
 strcpy(makerid[122], "Triffix");
 strcpy(makerid[123], "<<Unknown 123>>");
 strcpy(makerid[124], "Microprose");
 strcpy(makerid[125], "<<Unknown 125>>");
 strcpy(makerid[126], "<<Unknown 126>>");
 strcpy(makerid[127], "Kemco");
// strcpy(makerid[128], "Misawa Entertainment Co.,Ltd.");
 strcpy(makerid[128], "Misawa");
// strcpy(makerid[129], "Teichio Co. Ltd.");
 strcpy(makerid[129], "Teichio");
 strcpy(makerid[130], "Namco");
 strcpy(makerid[131], "Lozc");
// strcpy(makerid[132], "KOEI/KOEI!");
 strcpy(makerid[132], "KOEI");
 strcpy(makerid[133], "<<Unknown 133>>");
 strcpy(makerid[134], "T.I.M.");
 strcpy(makerid[135], "<<Unknown 135>>");
// strcpy(makerid[136], "DATAM-Polystar");
 strcpy(makerid[136], "Datam Polystar");
 strcpy(makerid[137], "<<Unknown 137>>");
 strcpy(makerid[138], "<<Unknown 138>>");
 strcpy(makerid[139], "Bullet-Proof Software");
 strcpy(makerid[140], "Vic Tokai");
 strcpy(makerid[141], "<<Unknown 141>>");
 strcpy(makerid[142], "Character Soft");
 strcpy(makerid[143], "I'Max");
 strcpy(makerid[144], "Takara");
 strcpy(makerid[145], "Chun Soft");
// strcpy(makerid[146], "Video System Co., Ltd.");
 strcpy(makerid[146], "Video System");
// strcpy(makerid[147], "Interbec");
 strcpy(makerid[147], "Bec");
 strcpy(makerid[148], "<<Unknown 148>>");
 strcpy(makerid[149], "Varie");
 strcpy(makerid[150], "<<Unknown 150>>");
 strcpy(makerid[151], "Kaneco");
 strcpy(makerid[152], "<<Unknown 152>>");
 strcpy(makerid[153], "Pack-In-Video");
 strcpy(makerid[154], "Nichibutsu");
 strcpy(makerid[155], "Tecmo");
 strcpy(makerid[156], "Imagineer");
 strcpy(makerid[157], "<<Unknown 157>>");
 strcpy(makerid[158], "<<Unknown 158>>");
 strcpy(makerid[159], "<<Unknown 159>>");
// Laser Soft, Telenet?
// strcpy(makerid[160], "Wolf Team*");
 strcpy(makerid[160], "Telenet");
 strcpy(makerid[161], "Hori");
 strcpy(makerid[162], "<<Unknown 162>>");
 strcpy(makerid[163], "<<Unknown 163>>");
 strcpy(makerid[164], "Konami");
 strcpy(makerid[165], "K. Amusement Leasing Co.");
 strcpy(makerid[166], "<<Unknown 166>>");
 strcpy(makerid[167], "Takara");
 strcpy(makerid[168], "<<Unknown 168>>");
 strcpy(makerid[169], "Technos");
// strcpy(makerid[170], "Victor Entertainment, Inc.");
 strcpy(makerid[170], "JVC");
 strcpy(makerid[171], "<<Unknown 171>>");
 strcpy(makerid[172], "TOEI Animation");
 strcpy(makerid[173], "Toho");
 strcpy(makerid[174], "<<Unknown 174>>");
// strcpy(makerid[175], "Namco Ltd.");
// strcpy(makerid[175], "Namcot");
 strcpy(makerid[175], "Namco");
 strcpy(makerid[176], "<<Unknown 176>>");
 strcpy(makerid[177], "ASCII");
// strcpy(makerid[178], "BanDai America");
 strcpy(makerid[178], "Bandai");
 strcpy(makerid[179], "<<Unknown 179>>");
 strcpy(makerid[180], "Enix");
 strcpy(makerid[181], "<<Unknown 181>>");
 strcpy(makerid[182], "Halken");
 strcpy(makerid[183], "<<Unknown 183>>");
 strcpy(makerid[184], "<<Unknown 184>>");
 strcpy(makerid[185], "<<Unknown 185>>");
 strcpy(makerid[186], "Culture Brain");
 strcpy(makerid[187], "Sunsoft");
 strcpy(makerid[188], "Toshiba EMI");
 strcpy(makerid[189], "Sony Imagesoft");
 strcpy(makerid[190], "<<Unknown 190>>");
 strcpy(makerid[191], "Sammy");
 strcpy(makerid[192], "Taito");
 strcpy(makerid[193], "<<Unknown 193>>");
 strcpy(makerid[194], "Kemco");
 strcpy(makerid[195], "Square");
 strcpy(makerid[196], "Tokuma Soft");
 strcpy(makerid[197], "Data East");
 strcpy(makerid[198], "Tonkinhouse");
 strcpy(makerid[199], "<<Unknown 199>>");
 strcpy(makerid[200], "KOEI");
 strcpy(makerid[201], "<<Unknown 201>>");
 strcpy(makerid[202], "Konami");
 strcpy(makerid[203], "NTVIC");
 strcpy(makerid[204], "<<Unknown 204>>");
// strcpy(makerid[205], "meldac/KAZe");
 strcpy(makerid[205], "Meldac");
 strcpy(makerid[206], "Pony Canyon");
 strcpy(makerid[207], "Sotsu Agency/Sunrise");
 strcpy(makerid[208], "Disco/Taito");
 strcpy(makerid[209], "Sofel");
// strcpy(makerid[210], "Quest Corp.");
 strcpy(makerid[210], "Quest");
 strcpy(makerid[211], "Sigma*");
 strcpy(makerid[212], "<<Unknown 212>>");
 strcpy(makerid[213], "<<Unknown 213>>");
// Naxat Soft
 strcpy(makerid[214], "Naxat");
 strcpy(makerid[215], "<<Unknown 215>>");
// strcpy(makerid[216], "Capcom Co., Ltd.");
 strcpy(makerid[216], "Capcom");
 strcpy(makerid[217], "Banpresto");
 strcpy(makerid[218], "Tomy");
// strcpy(makerid[219], "BITS/Argonaut");
 strcpy(makerid[219], "Acclaim");
 strcpy(makerid[220], "<<Unknown 220>>");
// Masaya?
 strcpy(makerid[221], "NCS");
// strcpy(makerid[222], "Human Entertainment");
 strcpy(makerid[222], "Human");
// strcpy(makerid[223], "Ringler Studios*");
 strcpy(makerid[223], "Altron");
// strcpy(makerid[224], "Jaleco Ltd.");
 strcpy(makerid[224], "Jaleco");
 strcpy(makerid[225], "<<Unknown 225>>");
 strcpy(makerid[226], "Yutaka");
 strcpy(makerid[227], "<<Unknown 227>>");
 strcpy(makerid[228], "T&Esoft");
// strcpy(makerid[229], "Epoch Co.,Ltd.");
 strcpy(makerid[229], "Epoch");
 strcpy(makerid[230], "<<Unknown 230>>");
 strcpy(makerid[231], "Athena");
 strcpy(makerid[232], "Asmik");
 strcpy(makerid[233], "Natsume");
 strcpy(makerid[234], "King Records");
 strcpy(makerid[235], "Atlus");
// strcpy(makerid[236], "Sony Music Entertainment (Japan) Inc.");
// strcpy(makerid[236], "Sony Music Entertainment (Japan)");
 strcpy(makerid[236], "Sony Music Entertainment");
 strcpy(makerid[237], "<<Unknown 237>>");
 strcpy(makerid[238], "IGS");
 strcpy(makerid[239], "<<Unknown 239>>");
 strcpy(makerid[240], "<<Unknown 240>>");
 strcpy(makerid[241], "Motown Software");
 strcpy(makerid[242], "Left Field Entertainment");
 strcpy(makerid[243], "Beam Software/Extreme");
 strcpy(makerid[244], "TecMagik");
 strcpy(makerid[245], "<<Unknown 245>>");
 strcpy(makerid[246], "<<Unknown 246>>");
 strcpy(makerid[247], "<<Unknown 247>>");
 strcpy(makerid[248], "<<Unknown 248>>");
 strcpy(makerid[249], "Cybersoft");
 strcpy(makerid[250], "<<Unknown 250>>");
 strcpy(makerid[251], "<<Unknown 251>>");
 strcpy(makerid[252], "<<Unknown 252>>");
 strcpy(makerid[253], "<<Unknown 253>>");
 strcpy(makerid[254], "Davidson");
 strcpy(makerid[255], "<<INVALID>>");

 strcpy(cartinfo[0], "ROM only");
 strcpy(cartinfo[1], "ROM and RAM");
 strcpy(cartinfo[2], "ROM and Save RAM");
 strcpy(cartinfo[3], "ROM and DSP1 chip");
 strcpy(cartinfo[4], "ROM, DSP1 micro chip and RAM");
 strcpy(cartinfo[5], "ROM, DSP1 micro chip and Save RAM");
 strcpy(cartinfo[19], "ROM and Super FX chip");
 strcpy(cartinfo[20], "ROM, RAM, and Super FX chip");
 strcpy(cartinfo[21], "ROM, RAM, Super FX chip, and Save RAM");
 strcpy(cartinfo[53], "ROM, RAM, SA-1 chip, and Save RAM");
 strcpy(cartinfo[54], "ROM and SA-1 chip");
 strcpy(cartinfo[67], "ROM and S-DD1 chip"); 
 strcpy(cartinfo[0xe3], "ROM, RAM, and GameBoy Guts");
 strcpy(cartinfo[0xf6], "ROM and DSP2 chip");
 strcpy(cartinfo[255], "<<INVALID>>");

 strcpy(countryid[0], "Japan");
 strcpy(countryid[1], "U.S.A.");
 strcpy(countryid[2], "Europe, Australia, Oceania and Asia");
 strcpy(countryid[3], "Sweden");
 strcpy(countryid[4], "Finland");
 strcpy(countryid[5], "Denmark");
 strcpy(countryid[6], "France");
 strcpy(countryid[7], "Holland");
 strcpy(countryid[8], "Spain");
 strcpy(countryid[9], "Germany, Austria and Switzerland");
 strcpy(countryid[10], "Italy");
 strcpy(countryid[11], "Hong Kong and China");
 strcpy(countryid[12], "Indonesia");
 strcpy(countryid[13], "Korea");
 strcpy(countryid[255], "<<INVALID>>");

 strcpy(format[0], "Super Magicom (SMC)");
 strcpy(format[1], "Multi Game Doctor II");
 strcpy(format[2], "Super Wild Card (SWC)");
 strcpy(format[3], "Multi Game Doctor II/Game Boxer");
 strcpy(format[4], "Pro Fighter (FIG)");
 strcpy(format[6], "Super Magicom IC2");
 strcpy(format[7], "Game Doctor SF III");

 file_ptr=fopen(comname1, "rb");
 chkiffileexist(file_ptr, comname1);

 fseek(file_ptr, 0, SEEK_END);
 file_size=ftell(file_ptr);
 fseek(file_ptr, 0, SEEK_SET);
 file_size-=ftell(file_ptr);
 checkminsize(file_size);

 if ((file_size%1024)==0)
	{
	subsize=0.0;
	fseek(file_ptr, 65500, SEEK_SET);
	fread(inchar, sizeof(inchar), 1, file_ptr);
	if ((checksum1=inchar[0])<0)
		checksum1=checksum1+256;
	fread(inchar, sizeof(inchar), 1, file_ptr);
	if ((checksum2=inchar[0])<0)
		checksum2=checksum2+256;
	fread(inchar, sizeof(inchar), 1, file_ptr);
	if ((comchecksum1=inchar[0])<0)
		comchecksum1=comchecksum1+256;
	fread(inchar, sizeof(inchar), 1, file_ptr);
	if ((comchecksum2=inchar[0])<0)
		comchecksum2=comchecksum2+256;
	checksum=checksum1+checksum2*256;
	comchecksum=comchecksum1+comchecksum2*256;
	if ((checksum+comchecksum)==65535)
		{
		h=1;
		k=3;
		offset=32256;
		}
	else
		{
		h=0;
		k=1;
		offset=-512;
		}
	}

if (k==0)
	{
	subsize=512.0;
	fseek(file_ptr, 66012, SEEK_SET);
	fread(inchar, sizeof(inchar), 1, file_ptr);
	if ((checksum1=inchar[0])<0)
		checksum1=checksum1+256;
	fread(inchar, sizeof(inchar), 1, file_ptr);
	if ((checksum2=inchar[0])<0)
		checksum2=checksum2+256;
	fread(inchar, sizeof(inchar), 1, file_ptr);
	if ((comchecksum1=inchar[0])<0)
		comchecksum1=comchecksum1+256;
	fread(inchar, sizeof(inchar), 1, file_ptr);
	if ((comchecksum2=inchar[0])<0)
		comchecksum2=comchecksum2+256;
	checksum=checksum1+checksum2*256;
	comchecksum=comchecksum1+comchecksum2*256;
	if ((checksum+comchecksum)==65535)
		{
		offset=32768;
		h=1;
		k=2;
		}
	else
		{
		offset=0;
		h=0;
		k=0;
		}

	}

 if (h==-1)
	id_error();

 if ((strcmp(comname2, "/snl")==0) || (strcmp(comname3, "/snl")==0))
	{
	h=0;
	offset=0;
	}

 if ((strcmp(comname2, "/snh")==0) || (strcmp(comname3, "/snh")==0))
	{
	h=1;
	offset=32768;
	}

 if (k==2)
	{
	fseek(file_ptr, 4, SEEK_SET);
	fread(tempchar, sizeof(tempchar), 1, file_ptr);
	if ((formattemp=tempchar[0])<0) formattemp=tempchar[0]+256;
	if (formattemp!=char(0))
		k=4;
	}

 if (k==0)
	{
	fseek(file_ptr, 4, SEEK_SET);
	fread(tempchar, sizeof(tempchar), 1, file_ptr);
	if ((formattemp=tempchar[0])<0)
		formattemp=tempchar[0]+256;
	if (formattemp!=0)
		kk=4;
	if (formattemp==0xcc)
		kk=6;
	fseek(file_ptr, 5, SEEK_SET);
	fread(tempchar, sizeof(tempchar), 1, file_ptr);
	if ((formattemp=tempchar[0])<0)
		formattemp=tempchar[0]+256;
	if (formattemp!=0)
		kk=4;
	fseek(file_ptr, 2, SEEK_SET);
	fread(tempchar, sizeof(tempchar), 1, file_ptr);
	if ((formattemp=tempchar[0])<0)
		formattemp=tempchar[0]+256;
	if ((formattemp>0) && (formattemp!=0x40))
		kk=2;
	}

 filesize=(file_size-subsize)/131072.0;
 fseek(file_ptr, 33216+offset, SEEK_SET);
 fread(cartname, (sizeof(cartname)-1), 1, file_ptr);
 fseek(file_ptr, 33238+offset, SEEK_SET);
 fread(inchar, sizeof(char), 1, file_ptr);
 if ((romindex=inchar[0])<0) romindex=romindex+256;
 if (romindex>255) error++;

 fseek(file_ptr, 33240+offset, SEEK_SET);
 fread(inchar, sizeof(char), 1, file_ptr);
 if ((ramsize=inchar[0])<0) ramsize=ramsize+256;
 if (ramsize>21)
	{
	error++;
	ramsize=0;
	}
 if (ramsize>0)
	ramsize=int(pow(2, ramsize)*8);
 fread(inchar, sizeof(char), 1, file_ptr);
 if ((countryindex=inchar[0])<0) countryindex=countryindex+256;

 fread(inchar, sizeof(char), 1, file_ptr);
 if ((makerindex=inchar[0])<0) makerindex=makerindex+256;
 if (makerindex>254)
	{
	error++;
	makerindex=255;
	}
 if (makerindex==0x33)
	{
	makerindex=0;
	fseek(file_ptr, 0x81b0+offset, SEEK_SET);
	fread(inchar, sizeof(char), 1, file_ptr);
	if (inchar[0]<0x3a)
		makerindex=(inchar[0]-0x30)*16;
	else
		makerindex=(inchar[0]-0x37)*16;
	fread(inchar, sizeof(char), 1, file_ptr);
	if (inchar[0]<0x3a)
		makerindex+=inchar[0]-0x30;
	else
		makerindex+=inchar[0]-0x37;
	fseek(file_ptr, 0x81db+offset, SEEK_SET);
	}

 fread(inchar, sizeof(char), 1, file_ptr);
 if ((version=inchar[0])<0) version=version+256;
 if (version>128)
	{
	error++;
	version=0;
	}

 fseek(file_ptr, 33239+offset, SEEK_SET);
 fread(inchar, sizeof(char), 1, file_ptr);
 if ((power=inchar[0])<0) power=power+256;
 if (power>22)
	{
	error++;
	power=0;
	}

 if ((error>1) && ((k==0) || (k==2) || (k==4)))
	{
	if (h==1)
		{
		h=0;
		k=0;
		offset=0;
		}
	else
		{
		h=1;
		k=2;
		offset=32768;
		}

	if (strcmp(comname2, "/snl")==0)
		{
		h=0;
		k=0;
		offset=0;
		}

	if (strcmp(comname2, "/snh")==0)
		{
		h=1;
		k=2;
		offset=32768;
		}

	fseek(file_ptr, 33216+offset, SEEK_SET);
	fread(cartname, (sizeof(cartname)-1), 1, file_ptr);
	fseek(file_ptr, 33240+offset, SEEK_SET);
	fread(inchar, sizeof(char), 1, file_ptr);
	if ((ramsize=inchar[0])<0) ramsize=ramsize+256;
	if (ramsize>21)
		ramsize=0;
	if (ramsize>0)
		ramsize=int(pow(2, ramsize)*8);
	fread(inchar, sizeof(char), 1, file_ptr);
	if ((countryindex=inchar[0])<0)
		countryindex=countryindex+256;

	fread(inchar, sizeof(char), 1, file_ptr);
	if ((makerindex=inchar[0])<0)
		makerindex=makerindex+256;
	if (makerindex>254)
		makerindex=255;
	if (makerindex==0x33)
		{
		makerindex=0;
		fseek(file_ptr, 0x81b0+offset, SEEK_SET);
		fread(inchar, sizeof(char), 1, file_ptr);
		if (inchar[0]<0x3a)
			makerindex=(inchar[0]-0x30)*16;
		else
			makerindex=(inchar[0]-0x37)*16;
		fread(inchar, sizeof(char), 1, file_ptr);
		if (inchar[0]<0x3a)
			makerindex+=inchar[0]-0x30;
		else
			makerindex+=inchar[0]-0x37;
		fseek(file_ptr, 0x81db+offset, SEEK_SET);
		}

	fread(inchar, sizeof(char), 1, file_ptr);
	if ((version=inchar[0])<0)
		version=version+256;
	if (version>255)
		version=0;

	fseek(file_ptr, 33239+offset, SEEK_SET);
	fread(inchar, sizeof(char), 1, file_ptr);
	if ((power=inchar[0])<0)
		power=power+256;
	if (power>22)
		power=0;
	}

 if ((error>1) && ((k==1) || (k==3)))
	{
	if (h==1)
		{
		h=0;
		k=1;
		offset=-512;
		}
	else
		{
		h=1;
		k=3;
		offset=32256;
		}
	fseek(file_ptr, 33216+offset, SEEK_SET);
	fread(cartname, (sizeof(cartname)-1), 1, file_ptr);
	fseek(file_ptr, 33240+offset, SEEK_SET);
	fread(inchar, sizeof(char), 1, file_ptr);
	if ((ramsize=inchar[0])<0)
		ramsize=ramsize+256;
	if (ramsize>21)
		ramsize=0;
	if (ramsize>0)
		ramsize=int(pow(2, ramsize)*8);
	fread(inchar, sizeof(char), 1, file_ptr);
	if ((countryindex=inchar[0])<0)
		countryindex=countryindex+256;

	fread(inchar, sizeof(char), 1, file_ptr);
	if ((makerindex=inchar[0])<0)
		makerindex=makerindex+256;
	if (makerindex>254)
		makerindex=255;
	if (makerindex==0x33)
		{
		makerindex=0;
		fseek(file_ptr, 0x81b0+offset, SEEK_SET);
		fread(inchar, sizeof(char), 1, file_ptr);
		if (inchar[0]<0x3a)
			makerindex=(inchar[0]-0x30)*16;
		else
			makerindex=(inchar[0]-0x37)*16;
		fread(inchar, sizeof(char), 1, file_ptr);
		if (inchar[0]<0x3a)
			makerindex+=inchar[0]-0x30;
		else
			makerindex+=inchar[0]-0x37;
		fseek(file_ptr, 0x81db+offset, SEEK_SET);
		}

	fread(inchar, sizeof(char), 1, file_ptr);
	if ((version=inchar[0])<0)
		version=version+256;
	if (version>255)
		version=0;

	fseek(file_ptr, 33239+offset, SEEK_SET);
	fread(inchar, sizeof(char), 1, file_ptr);
	if ((power=inchar[0])<0) power=power+256;
	if (power>22)
		power=0;
	}

 cartsize=int((pow(2, power)*1024.0)/131072.0);
 if (k==1)
	{
	fseek(file_ptr, 32725, SEEK_SET);
	fread(inchar, sizeof(inchar), 1, file_ptr);
	if ((boxercheck=inchar[0])<0) boxercheck=boxercheck+256;
	if (boxercheck==0x92)
		{
		h=1;
		k=3;
		}
	}

 if (k==0)
	{
	fseek(file_ptr, 33237, SEEK_SET);
	fread(inchar, sizeof(inchar), 1, file_ptr);
	if ((boxercheck=inchar[0])<0)
		boxercheck=boxercheck+256;
	if (boxercheck==0xfd)
		kk=6;
	}

 if (k==0)
	{
	fseek(file_ptr, 15, SEEK_SET);
	fread(inchar, sizeof(inchar), 1, file_ptr);
	if ((boxercheck=inchar[0])<0)
		boxercheck=boxercheck+256;
	if (boxercheck==0x33)
		{
		k=7;
		kk=7;
		}
	}

 if ((kk==6) || (kk==7))
	h=1;

 cartname[22]='\0';

// splitch
// ROMChecksum = file_ptr [0xffde] + (file_ptr [0xffdf] << 8);
// ROMComplementChecksum = file_ptr [0xffdc] + (file_ptr [0xffdd] << 8);

 printf("uCON %s", uconver);
 printf("\t\"What Every Console User Needs\" - By Chicken & chp\n");
 printf("\t\t\t\t\t\t\t Unix port: splice\n");
 printf("Filename     : %s\n", comname1);
 printf("Game Pak Name: %s\n", cartname);
 printf("Game Pak Type: %s\n", cartinfo[romindex]);
 printf("ROM Size     : %d Mbit\n", cartsize);
 printf("RAM Size     : %d Kbit\n", ramsize);

#ifdef DEBUG
 printf("- DEBUG -\n");
 printf("ROMindex     : %d\n", romindex);
#endif

// Virgin Games, McO'RIVER
 if (countryindex==4)
	if (makerindex==81)
		makerindex=0;

// McO'RIVER
 if (makerindex==146)
	if (countryindex==1)
		strcpy(makerid[146], "McO'RIVER");

// FCI
 if (makerindex==0xce)
	if (countryindex==1)
		strcpy(makerid[0xce], "FCI");

// to show unknown maker
 if ((makerindex<0) || (makerindex>255))
	makerindex=2;

 printf("Licensee     : %s\n", makerid[makerindex]);
 printf("For Use In   : %s", countryid[countryindex]);

 if ((countryindex==0) || (countryindex==1))
        printf(" (NTSC)\n");
 else if ((countryindex>1) && (countryindex<14))
        printf(" (PAL)\n");
 else
  printf("\n");

 printf("Revision     : 1.%d\n", version);
 printf("File Size    : %4.2f Mbit\n", filesize);
 printf("Image Format : %s", format[k]);

 if ((kk==4) && (h==0))
	if (ramsize==0)
                printf(" [FIG SRAM OFF]");
	else
                printf(" [FIG SRAM SET]");
  else
 if (kk>0)
        printf("%s", format[kk]);

 if ((k==3) || (kk==6) || (k==7))
        printf(" [Multi File]");

 if ((k==1) && ((filesize*131072)<1049089))
	if ((k==1) && ((cartsize*131072-(filesize*131072))>131072))
                printf(" [Multi File]");

 if (((k==0) || (k==2) || (k==4)) && ((filesize*131072)<1049089))
	if (((k==0) || (k==2) || (k==4)) && ((cartsize*131072-(filesize*131072))>131072))
                printf(" [Multi File]");

 printf("\nHiROM Game   : ");
 if (h==0)
        printf("No\n");
 else if (h==1)
        printf("Yes\n");
 else
        printf("Unknown\n");

 if ((filesize>int(filesize)) || ((filesize>cartsize) && (cartsize!=0)))
	{
	introsize=(filesize*131072)-(int(filesize)*131072);
	if (introsize==0)
		introsize=(filesize*131072)-(cartsize*131072);
         printf("Intro/Trainer: Yes, %4.0f bytes\n", introsize);
	}
 else
	{
        printf("Intro/Trainer: No\n");
	}

  if ((k==2) || (k==4)) k=0;
  if (k==3) k=1;

 printf("\n");
}

void mghftb()
{
 unsigned long   i;
 FILE   *file_ptr;
 FILE   *file_ptr2;
 FILE   *file_ptr3;
 char   header[512];
 char   head[1];
 char   *name;
 unsigned long file_size;
 unsigned long file_size2=0;
 int    meter;
 int    metersize;
 int    j,checklen=0,o;
 int    errorchk, boxflag=0, gdsf3flag=0;

 memset(header, 0, sizeof(header));
 header[0]='G';
 header[1]='A';
 header[2]='M';
 header[3]='E';
 header[4]=' ';
 header[5]='D';
 header[6]='O';
 header[7]='C';
 header[8]='T';
 header[9]='O';
 header[10]='R';
 header[11]=' ';
 header[12]='S';
 header[13]='F';
 header[14]=' ';
 header[15]='3';
 header[16]=0x80;
 header[17]=0x20;
 header[18]=0x21;
 header[19]=0x22;
// header[20]=0x00;
 header[21]=0x20;
 header[22]=0x21;
 header[23]=0x22;
// header[24]=0x00;
 header[25]=0x20;
 header[26]=0x21;
 header[27]=0x22;
// header[28]=0x00;
 header[29]=0x20;
 header[30]=0x21;
 header[31]=0x22;
// header[32]=0x00;
 header[33]=0x24;
 header[34]=0x25;
 header[35]=0x23;
// header[36]=0x00;
 header[37]=0x24;
 header[38]=0x25;
 header[39]=0x23;

 if (k==1)
	{
	printf("\nuCON Error:  Image is not in FIG/SWC format.\n");
	exit(8);
	}

 if (comnumber==4)
	{
	if (strcmp(comname2, "/!")==0)
		{
		boxflag=1;
		comnumber--;
		}
	if (strcmp(comname2, "/3")==0)
		{
		gdsf3flag=1;
		comnumber--;
		}
	if (strcmp(comname2, "/#")==0)
		{
		boxflag=1;
		gdsf3flag=1;
		comnumber--;
		}
	}

 if (comnumber==3)
	{
	strcpy(comname2, comname1);

	if((name=strchr(comname2, '.'))!=NULL)
		{
		*name='_';
		*name++='_';
		*name++='_';
		*name++='_';
		*name++='\0';
		}
	if ((checklen=strlen(comname2))<7)
		for (j=checklen; j<7; j++)
			comname2[j]='_';

	for (j=6; j>1; j=j-1)
		comname2[j]=comname2[j-2];

	comname2[0]='s';
	comname2[1]='f';
	comname2[7]='a';
	comname2[8]='\0';
	strcpy(comname3, comname2);
	comname3[7]='b';
	}

 if (comnumber==4)
	{
	strcpy(comname3, comname2);
	if ((checklen=strlen(comname3))<7)
		{
		for (j=checklen; j<7; j++)
			{
			comname2[j]='_';
			comname3[j]='_';
			}
		}
	comname3[7]='b';
	comname2[7]='a';
	comname3[8]='\0';
	comname2[8]='\0';
	}

 file_ptr=fopen(comname1, "rb");
 chkiffileexist(file_ptr, comname1);

 file_ptr2=fopen(comname2, "w+b");
 file_ptr3=fopen(comname3, "w+b");
 fseek(file_ptr, 0, SEEK_END);
 file_size=ftell(file_ptr);
 fseek(file_ptr, 0, SEEK_SET);
 file_size-=ftell(file_ptr);

 if (((file_size-512)%131072)!=0)
	{
	printf("uCON Error:  Image file size is not a multiple of 1 Mbit.\n");
	printf("             It must be padded before converting.\n\n");
	remove(comname2);
	remove(comname3);
	exit(8);
	}

 if (file_size>2097664)
	{
	file_size2=file_size-2097664;
	file_size=2097664;
	fclose(file_ptr3);
	comname3[7]='c';
	file_ptr3=fopen(comname3, "w+b");
	if (file_size2==2097152)
		{
		header[20]=0x23;
		header[24]=0x23;
		header[28]=0x23;
		header[32]=0x23;
		header[35]=0x26;
		header[36]=0x27;
		header[39]=0x26;
		header[40]=0x27;
		}
	}

 if (gdsf3flag>0)
	fwrite(header, sizeof(header), 1, file_ptr2);

 meter=((file_size-512)/131072);
 metersize=64/meter;
 file_size=(file_size-512)/32768;
 printf("FIG->Boxer: File #1=> %s\n", comname2);
 o=0;
 for (i=1; i<file_size; i=i+2)
	{
	o++;
	fseek(file_ptr, i*32768+512, SEEK_SET);
	fread(readchunk3, sizeof(readchunk3), 1, file_ptr);
	errorchk=fwrite(readchunk3, sizeof(readchunk3), 1, file_ptr2);
	if (errorchk==0)
		diskspaceerror();
	}
 printf("FIG->Boxer: File #2=> %s\n", comname3);
 o=0;
 for (i=0; i<file_size; i=i+2)
	{
	o++;
	fseek(file_ptr, i*32768+512, SEEK_SET);
	fread(readchunk3, sizeof(readchunk3), 1, file_ptr);
	errorchk=fwrite(readchunk3, sizeof(readchunk3), 1, file_ptr3);
	if (errorchk==0)
		diskspaceerror();
	}

 if (boxflag==0)
	{
	head[0]=0x92;
	if (gdsf3flag==1)
		fseek(file_ptr2, 0x81d5, SEEK_SET);
	else
		fseek(file_ptr2, 0x7fd5, SEEK_SET);
	fwrite(head, sizeof(head), 1, file_ptr2);
	}

 if (file_size2>0)
	{
	fclose(file_ptr2);
	fclose(file_ptr3);
	file_size=2097664+file_size2;
	comname2[7]='b';
	comname3[7]='d';
	file_ptr2=fopen(comname2, "w+b");
	file_ptr3=fopen(comname3, "w+b");

	file_size=(file_size-512)/32768;
        printf("FIG->Boxer: File #3=> %s\n", comname2);

	j=0;
	if ((gdsf3flag>0) && (file_size2==524288))
		j=1;
	while (j>-1)
		{
		for (i=65; i<file_size; i=i+2)
			{
			fseek(file_ptr, i*32768+512, SEEK_SET);
			fread(readchunk3, sizeof(readchunk3), 1, file_ptr);
			errorchk=fwrite(readchunk3, sizeof(readchunk3), 1, file_ptr2);
			if (errorchk==0)
				diskspaceerror();
			}
		 j--;
		}

	if ((gdsf3flag==0) || ((gdsf3flag>0) && (file_size2==2097152)))
                printf("FIG->Boxer: File #4=> %s\n", comname3);

	if ((gdsf3flag>0) && (file_size2!=2097152))
		{
		fclose(file_ptr3);
		remove(comname3);
		fclose(file_ptr2);
		file_ptr3=fopen(comname2, "r+b");
		fseek(file_ptr3, 0, SEEK_END);
		}

	j=0;
	if ((gdsf3flag>0) && (file_size2==524288))
		j=1;
	while (j>-1)
		{
		for (i=64; i<file_size; i=i+2)
			{
			fseek(file_ptr, i*32768+512, SEEK_SET);
			fread(readchunk3, sizeof(readchunk3), 1, file_ptr);
			errorchk=fwrite(readchunk3, sizeof(readchunk3), 1, file_ptr3);
			if (errorchk==0)
				diskspaceerror();
			}
		j--;
		}
	}

 printf("FIG->Boxer Completed.\n");
}

void mghpatch()
{
 int    i;
 FILE   *file_ptr;
 FILE   *file_ptr2;
 char   inchar[512];
 char   onechar[1];
 char   header[2];
 int    size;
 int    last;
 int    first;
 int    olast;
 int    ofirst;
 unsigned long file_size;
 unsigned long add_size;

 if (comnumber==4)
	 file_ptr2=fopen(comname2, "w+b");

 file_ptr=fopen(comname1, "a+b");
 chkiffileexist(file_ptr, comname1);
 
 memset(inchar, char(255), sizeof(inchar));
 onechar[0]=char(255);
 
 fseek(file_ptr, 0, SEEK_END);
 file_size=ftell(file_ptr);
 fseek(file_ptr, 0, SEEK_SET);
 file_size-=ftell(file_ptr);

 printf("Padding File...\n");
 size=int((file_size-512)/131072);
 add_size=(131072*(size+1)+512)-file_size;

 if ((add_size-131072)!=0 && comnumber==3) {
	fclose(file_ptr);
	file_ptr=fopen(comname1, "r+b");

	header[1]=((size+1)*16)/256;
	header[0]=(((size+1)*16)-(header[1]*256));

	fseek(file_ptr, 0, SEEK_SET);
	fwrite(header, sizeof(header), 1, file_ptr);
	fclose(file_ptr);
	file_ptr=fopen(comname1, "a+b");
	fseek(file_ptr, 0, SEEK_SET);

     printf("[to %d Mbit] ", size+1);

     last=add_size%512;
     first=int(add_size/512);
     for (i=0; i<first; i++) fwrite(inchar, sizeof(inchar), 1, file_ptr);
     
     for (i=0; i<last; i++)
	fwrite(onechar, sizeof(onechar), 1, file_ptr);
 }
 else if ((add_size-131072)!=0 && comnumber==4) {
     last=add_size%512;
     first=int(add_size/512);
     olast=file_size%512;
     ofirst=int(file_size/512);
     printf("[to %d Mbit] ", size+1);

     for (i=0; i<ofirst; i++) 
     {
		fread(inchar, sizeof(inchar), 1, file_ptr);
		fwrite(inchar, sizeof(inchar), 1, file_ptr2);
     }

     for (i=0; i<olast; i++) 
     {
		fread(onechar, sizeof(onechar), 1, file_ptr);
		fwrite(onechar, sizeof(onechar), 1, file_ptr2);
     }

     memset(inchar, 255, sizeof(inchar));
     onechar[0]=255;

     for (i=0; i<first; i++)
     {
		fwrite(inchar, sizeof(inchar), 1, file_ptr2);
     }

     for (i=0; i<last; i++)
     {
		fwrite(onechar, sizeof(onechar), 1, file_ptr2);
     }

	header[1]=((size+1)*16)/256;
	header[0]=(((size+1)*16)-(header[1]*256));

	fseek(file_ptr2, 0, SEEK_SET);
	fwrite(header, sizeof(header), 1, file_ptr2);

 }
 printf("Image Padded.\n");
}

void mghbtf()
{
 long   i;
 FILE   *file_ptr;
 FILE   *file_ptr2;
 FILE   *file_ptr3;
 char   head[1];
 char   header[512];
 unsigned long file_size;
 int    lfile_size;
 int    meter, metersize, o, errorchk, gdsf3flag=0, mghflag=0;

 if ((k!=1) && (k!=7))
	{
	 printf("Image is not in MGD2/Game Boxer format.\n");
	 exit(8);
	}


 if (comnumber==3)
	{
	strcpy(comname3, comname1);
	comname3[8]='.';
	comname3[9]='f';
	comname3[10]='i';
	comname3[11]='g';
	comname3[12]='\0';
	}


 strcpy(comname2, comname1);


 comname2[7]='d';
 file_ptr2=fopen(comname2, "rb");

 if (NULL!=file_ptr2)
	{
	mghflag=1;

	fseek(file_ptr2, 0, SEEK_END);
	file_size=ftell(file_ptr2);
	fseek(file_ptr2, 0, SEEK_SET);
	file_size-=ftell(file_ptr2);
	if (file_size==1048576)
		gdsf3flag=1;
         fclose(file_ptr2);
	}

 if (mghflag==0)
	{
	comname2[7]='c';
	file_ptr2=fopen(comname2, "rb");
	if (NULL!=file_ptr2)
         {
		gdsf3flag=1;
                fclose(file_ptr2);
         }
	}



 if (k==7)
	gdsf3flag=1;

 if ((mghflag==1) || (gdsf3flag==1))
	comname2[7]='c';
 else
	comname2[7]='b';


 file_ptr=fopen(comname1, "rb");
 chkiffileexist(file_ptr, comname1);

 file_ptr2=fopen(comname2, "rb");
 chkiffileexist(file_ptr2, comname2);

 file_ptr3=fopen(comname3, "w+b");
 fclose(file_ptr3);
 file_ptr3=fopen(comname3, "r+b");

 fseek(file_ptr, 0, SEEK_END);
 file_size=ftell(file_ptr);
 fseek(file_ptr, 0, SEEK_SET);
 file_size-=ftell(file_ptr);

 if (gdsf3flag==1)
	{
	file_size=file_size-512;
	fseek(file_ptr, 512, SEEK_SET);
	}
 meter=file_size/131072;
 metersize=64/meter;
 lfile_size=(file_size)/32768;
 memset(header, 0, sizeof(header));

 header[1]=(((file_size*2)/8192)/256);
 header[0]=(((file_size*2)/8192)-(header[1]*256));

 header[3]=0x80;
 header[8]=0xaa;
 header[9]=0xbb;
 header[10]=0x04;
 if (ramsize==0)
	{
	header[4]=0x77;
	header[5]=0x83;
	header[8]=0x00;
	header[9]=0x00;
	header[10]=0x00;
	}
 fwrite(header, sizeof(header), 1, file_ptr3);

 printf("Boxer->FIG Working...\n");
 o=0;
 for (i=0; i<lfile_size; i++)
	{
	o++;
	fread(readchunk3, sizeof(readchunk3), 1, file_ptr2);
	fwrite(readchunk3, sizeof(readchunk3), 1, file_ptr3);
	fread(readchunk3, sizeof(readchunk3), 1, file_ptr);
	errorchk=fwrite(readchunk3, sizeof(readchunk3), 1, file_ptr3);
	if (errorchk==0)
		diskspaceerror();
	}

 fclose(file_ptr);
 fclose(file_ptr2);

 if (mghflag==1)
	{
	gdsf3flag=0;

	comname1[7]='b';
	comname2[7]='d';
	file_ptr=fopen(comname1, "rb");
	file_ptr2=fopen(comname2, "rb");

	fseek(file_ptr, 0, SEEK_END);
	file_size=ftell(file_ptr);
	fseek(file_ptr, 0, SEEK_SET);
	file_size-=ftell(file_ptr);

	meter=file_size/131072;
	metersize=64/meter;
	lfile_size=(file_size)/32768;

        printf("Boxer->FIG Working...\n");
	o=0;
	for (i=0; i<lfile_size; i++)
		{
		o++;
		fread(readchunk3, sizeof(readchunk3), 1, file_ptr2);
		fwrite(readchunk3, sizeof(readchunk3), 1, file_ptr3);
		fread(readchunk3, sizeof(readchunk3), 1, file_ptr);
		errorchk=fwrite(readchunk3, sizeof(readchunk3), 1, file_ptr3);
		if (errorchk==0)
			diskspaceerror();
		}

	head[0]=(file_size*2)/8192;
	fseek(file_ptr3, 0, SEEK_SET);
	fwrite(head, sizeof(head), 1, file_ptr3);
	}

 if (gdsf3flag==1)
	{
	comname1[7]='b';
	file_ptr=fopen(comname1, "rb");

	fseek(file_ptr, 0, SEEK_END);
	file_size=ftell(file_ptr);
	fseek(file_ptr, 0, SEEK_SET);
	file_size-=ftell(file_ptr);
	meter=file_size/131072;
	metersize=64/meter;
	lfile_size=(file_size)/32768;

        printf("Boxer->FIG Working");
	o=0;
	for (i=0; i<(lfile_size/2); i++)
		{
		o++;

		fseek(file_ptr, (((lfile_size/2)+i)*32768), SEEK_SET);
		fread(readchunk3, sizeof(readchunk3), 1, file_ptr);
		fwrite(readchunk3, sizeof(readchunk3), 1, file_ptr3);
		fseek(file_ptr, (i*32768), SEEK_SET);
		fread(readchunk3, sizeof(readchunk3), 1, file_ptr);
		errorchk=fwrite(readchunk3, sizeof(readchunk3), 1, file_ptr3);
		if (errorchk==0)
			diskspaceerror();
		}

	head[0]=(file_size)/8192;
	fseek(file_ptr3, 0, SEEK_SET);
	fwrite(head, sizeof(head), 1, file_ptr3);
	}

 file_ptr3=fopen(comname3, "r+b");
 fseek(file_ptr3, 66005, SEEK_SET);
 fread(head, sizeof(head), 1, file_ptr3);
 if ((errorchk=head[0])<0)
	errorchk=errorchk+256;
 if (errorchk==0x92)
	{
	head[0]=char(0x31);
	fseek(file_ptr3, 66005, SEEK_SET);
	fwrite(head, sizeof(head), 1, file_ptr3);
	}

 printf("Boxer->FIG Completed.\n");
}

void mghheader()
{
 FILE   *file_ptr;
 char   header[512];
 unsigned long file_size;
 int    hi=0, split=0, batt=0;

 if (k==1)
	{
	printf("Image is not in SMC/FIG/SWC Format.\n");
	exit(8);
	}

 if (comnumber>3)
	{
	if (strchr(comname2, '/')!=NULL)
		{
		if (strchr(comname2, 'h')!=NULL)
			hi=1;
		if (strchr(comname2, 'l')!=NULL)
			hi=-1;
		if (strchr(comname2, 'm')!=NULL)
			split=1;
		if (strchr(comname2, 's')!=NULL)
			batt=1;
		if (strchr(comname2, 'h')!=NULL)
			hi=1;
		}
	}

 file_ptr=fopen(comname1, "r+b");
 chkiffileexist(file_ptr, comname1);

 fseek(file_ptr, 0, SEEK_END);
 file_size=ftell(file_ptr);
 fseek(file_ptr, 0, SEEK_SET);
 file_size-=ftell(file_ptr);
 memset(header, 0, sizeof(header));

 if (split==1)
	header[2]=char(64);

 header[1]=(((file_size-512)/8192)/256);
 header[0]=(((file_size-512)/8192)-(header[1]*256));

 header[8]=0xaa;
 header[9]=0xbb;
 header[10]=0x04;

 if ((hi!=-1) && ((hi==1) || (h==1)))
	{
	header[3]=char(0x80);
	header[8]=0x00;
	header[9]=0x00;
	header[10]=0x00;
	}

 if ((batt==0) && (ramsize==0))
	{
	header[4]=char(0x77);
	header[5]=char(0x83);
	header[8]=0x00;
	header[9]=0x00;
	header[10]=0x00;
	}

 if ((romindex==3) || (romindex==4) || (romindex==5))
	{
	if (ramsize==0)
		{
		header[4]=0x47;
		header[5]=0x83;
		header[8]=0x00;
		header[9]=0x00;
		header[10]=0x00;
		}
	if (ramsize>0)
		{
		header[4]=0xfd;
		header[5]=0x82;
		header[8]=0x00;
		header[9]=0x00;
		header[10]=0x00;
		}
	}

 printf("Rewriting Header...\n");
 rewind(file_ptr);
 fseek(file_ptr, 0, SEEK_SET);
 fwrite(header, sizeof(header), 1, file_ptr);
 printf("Header Rewritten.\n");
}

void mghconvert()
{
 int    i;
 FILE   *file_ptr;
 FILE   *file_ptr2;
 char   inchar[512];
 char   onechar[1];
 char   header[512];
 int    last;
 int    first;
 unsigned long file_size;
 char   *name;
 int    checklen=0;
 int    meter, metersize, o;

 if ((comnumber==3) && (k==0)){
	 strcpy(comname2, comname1);
	 for (i=7; i>1; i=i-1)
		comname2[i]=comname1[i-2];
	 comname2[0]='s';
	 comname2[1]='f';
	 if ((comname2[7]=='a')||(comname2[7]=='b'))
		comname2[7]='-';
	 if ((name=strchr(comname2, '.'))==NULL) {
		checklen=strlen(comname2);
		comname2[checklen]='\0';
	 }
	 else if((name=strchr(comname2, '.'))!=NULL) {
		name++;
		*name++='\0';
	 }
 }
 if ((comnumber==3) && (k==1)){
	 strcpy(comname2, comname1);
	 if ((name=strchr(comname2, '.'))==NULL) {
		checklen=strlen(comname2);
		comname2[checklen]='.';
		comname2[checklen+1]='s';
		comname2[checklen+2]='m';
		comname2[checklen+3]='c';
		comname2[checklen+4]='\0';
	 }
	 else if((name=strchr(comname2, '.'))!=NULL) {
		name++;
		*name++='s';
		*name++='m';
		*name++='c';
		*name++='\0';
	 }
 }
 file_ptr=fopen(comname1, "rb");
 chkiffileexist(file_ptr, comname1);

 file_ptr2=fopen(comname2, "w+b");
 fseek(file_ptr, 0, SEEK_END);
 file_size=ftell(file_ptr);
 fseek(file_ptr, 0, SEEK_SET);
 file_size-=ftell(file_ptr);
 if (k==0){
	 meter=(file_size-512)/131072;
	 metersize=64/meter;
	 fread(header, sizeof(header), 1, file_ptr);
         printf("Converting SMC->MGD2 Format...\n");

	 first=int((file_size-512)/512);
	 last=((file_size-512)%512);
	 o=0;
	 for (i=0; i<first; i++) {
		o++;
		fread(inchar, sizeof(inchar), 1, file_ptr);
		fwrite(inchar, sizeof(inchar), 1, file_ptr2);
	 }
	 for (i=0; i<last; i++) {
		fread(onechar, sizeof(onechar), 1, file_ptr);
		fwrite(onechar, sizeof(onechar), 1, file_ptr2);
	 }
 }
 else if (k==1){
	 meter=file_size/131072;
	 metersize=64/meter;
	 memset(header, 0, sizeof(header));

	 header[1]=(((file_size)/8192)/256);
	 header[0]=(((file_size)/8192)-(header[1]*256));

	 if (h==1)
		header[3]=0x80;

	 header[8]=0xaa;
	 header[9]=0xbb;
	 header[10]=0x04;
	 fwrite(header, sizeof(header), 1, file_ptr2);
         printf("Converting MGD2->SMC Format...\n");
	 first=int((file_size)/512);
	 last=((file_size)%512);
	 o=0;
	 for (i=0; i<first; i++) {
		o++;
		fread(inchar, sizeof(inchar), 1, file_ptr);
		fwrite(inchar, sizeof(inchar), 1, file_ptr2);
	 }
	 for (i=0; i<last; i++) {
		fread(onechar, sizeof(onechar), 1, file_ptr);
		fwrite(onechar, sizeof(onechar), 1, file_ptr2);
	 }
 }

 printf("Image Converted.\n");
}

void mghsnesj()
{
 unsigned long i;
 FILE   *file_ptr;
 FILE   *file_ptr2;
 char   header[512];
 unsigned long file_size;
 int    j, checklen=0;
 char   *name;
 unsigned long  size=0;
 int    nextfile=50;
 char   nextcheck[1];
 int    filenumber=1;
 int    next, errorchk=0;
 int    meter, metersize, o;
 char   tempchar[1];
 int    mgdflag=0, mgdcheck=0;

 if ((comnumber==3) && (h==0) && (k!=1))
	{
	strcpy(comname2, comname1);
	if ((name=strchr(comname2, '.'))==NULL)
		{
		checklen=strlen(comname2);
		comname2[checklen]='.';
		comname2[checklen+1]='s';
		comname2[checklen+2]='m';
		comname2[checklen+3]='c';
		comname2[checklen+4]='\0';
		}
	else if((name=strchr(comname2, '.'))!=NULL)
		{
		name++;
		*name++='s';
		*name++='m';
		*name++='c';
		*name++='\0';
		}
	}

 if ((comnumber==3) && (h==0) && (k==1))
	{
	strcpy(comname2, comname1);
	if ((name=strchr(comname2, '.'))==NULL)
		{
		checklen=strlen(comname2);
		comname2[checklen-1]='-';
		comname2[checklen+1]='0';
		comname2[checklen+2]='7';
		comname2[checklen+3]='8';
		comname2[checklen+4]='\0';
		}
	else if((name=strchr(comname2, '.'))!=NULL)
		{
		*name--='-';
		name++;
		*name++='0';
		*name++='7';
		*name++='8';
		*name++='\0';
		}
	}

 if ((comnumber==3) && (h==1))
	{
	strcpy(comname2, comname1);
	if ((name=strchr(comname2, '.'))==NULL)
		{
		checklen=strlen(comname2);
		comname2[checklen]='.';
		comname2[checklen+1]='f';
		comname2[checklen+2]='i';
		comname2[checklen+3]='g';
		comname2[checklen+4]='\0';
		}
	else if((name=strchr(comname2, '.'))!=NULL)
		{
		name++;
		*name++='f';
		*name++='i';
		*name++='g';
		*name++='\0';
		}
	}

 memset(header, 0, sizeof(header));
 header[8]=0xaa;
 header[9]=0xbb;
 header[10]=0x04;

 checklen=strlen(comname2);
 for (j=0; j<checklen; j++)
	if (comname1[j]!=comname2[j])
		errorchk=-1;

 if (errorchk==0)
	{
	printf("uCON Error:  The input and output filenames are the same!\n");
	exit(8);
	}

 file_ptr=fopen(comname1, "rb");
 chkiffileexist(file_ptr, comname1);

 file_ptr2=fopen(comname2, "w+b");
 fseek(file_ptr, 3, SEEK_SET);
 fread(tempchar, sizeof(tempchar), 1, file_ptr);

 if (h==1)
	header[3]=0x80;

 if (ramsize==0)
	{
	header[4]=0x77;
	header[5]=0x83;
	header[8]=0x00;
	header[9]=0x00;
	header[10]=0x00;
	}

 fclose(file_ptr);
 
 if ((k!=1) && (k!=3) && (k!=7))
	fwrite(header, sizeof(header), 1, file_ptr2);
 
 next=1;

 while (next!=0)
	{
	file_ptr=fopen(comname1, "rb");
	if (file_ptr==NULL)
		{
		file_ptr=file_ptr2;
                printf("\n\nuCON Error:  Cannot open input file #%d, \"%s.\"\n\n", filenumber, comname1);
		printf("             (Image might not be completely joined!)\n");
		rewind(file_ptr2);

		header[1]=((size/8192)/256);
		header[0]=(((size/8192)-(header[1]*256)));

		fseek(file_ptr, 0, SEEK_SET);
		fwrite(header, sizeof(header), 1, file_ptr2);
                // fcloseall();
		exit(8);
		}

 if ((k==0) && (h==0))
     printf("Joining SMC File #%d=> %s\n", filenumber, comname1);
 else if ((k==0) && (h==1))
     printf("Joining FIG File #%d=> %s\n", filenumber, comname1);
 else if (k==7)
        printf("Joining GDSF3 File #%d=> %s\n", filenumber, comname1);
 else
     printf("Joining MGD2 File #%d=> %s\n", filenumber, comname1);
 filenumber++;
 fseek(file_ptr, 0, SEEK_END);
 file_size=ftell(file_ptr);
 fseek(file_ptr, 0, SEEK_SET);
 file_size-=ftell(file_ptr);
 if (k==0)
     meter=(file_size-512)/131072;
 else
     meter=file_size/131072;
 metersize=64/meter;
 if (k==0)
	{
	size=size+(file_size-512);
	file_size=(file_size-512)/8192;
	fseek(file_ptr, 512, SEEK_SET);
	}
 else if ((k==7) && (mgdflag==0))
	{
	size=size+(file_size-512);
	file_size=(file_size-512)/8192;
	fseek(file_ptr, 0, SEEK_SET);
	fread(header, sizeof(header), 1, file_ptr);
	fwrite(header, sizeof(header), 1, file_ptr2);
	fseek(file_ptr, 512, SEEK_SET);
	}
 else
	{
	size=size+file_size;
	file_size=file_size/8192;
	fseek(file_ptr, 0, SEEK_SET);
	}

 o=0;
 for (i=0; i<file_size; i++)
	{
	o++;
	fread(readchunk, sizeof(readchunk), 1, file_ptr);
	errorchk=fwrite(readchunk, sizeof(readchunk), 1, file_ptr2);

	if (errorchk==0)
		diskspaceerror();
	}

 if (k==0)
	{
	if ((name=strchr(comname1, '.'))!=NULL)
		{
		name++;
		*name++=char(nextfile++);
		*name++='\0';
		}
	fseek(file_ptr, 2, SEEK_SET);
	fread(nextcheck, sizeof(nextcheck), 1, file_ptr);
	if ((next=nextcheck[0])<0)
		next=next+256;

	next=(next&64);
	if (next!=64)
		next=0;

	fclose(file_ptr);
	}

 if ((k==1) || (k==7))
	{
	if ((name=strchr(comname1, '.'))!=NULL)
		{
		name--;
		*name=char(98+mgdflag);
		}
	else
		{
		mgdcheck=strlen(comname1);
		comname1[mgdcheck-1]=char(98+mgdflag);
		}

	next=0;
	mgdflag++;
	if (mgdflag==1)
		next=1;
	if ((k==7) && (mgdflag==2))
		next=1;
	}
 }

 file_ptr2=fopen(comname2, "r+b");

 if ((k!=1) && (k!=3) && (k!=7))
	calcandwriteheader(file_ptr2, header);

 printf("Files Joined.\n");
}

void mghchop()
{
 unsigned long   i;
 FILE   *file_ptr;
 FILE   *file_ptr2;
 FILE   *file_ptr3;
 char   header[512];
 unsigned long file_size;
 int    checklen=0;
 char   *name;
 int    noheader=0;
 int    meter, metersize, o;
 int    errorchk;

 if (k==1)
	{
	comnumber++;
	strcpy(comname2, comname1);
	if ((name=strchr(comname2, '.'))==NULL)
		{
		checklen=strlen(comname2);
		if (checklen<9)
			for (i=checklen; i<14; i++)
				comname2[i]='_';
		comname2[7]='a';
		comname2[8]='\0';
		}
	else if((name=strchr(comname2, '.'))!=NULL)
		{
		*name='_';
		*name++='_';
		*name++='_';
		*name++='_';
		*name++='\0';
		checklen=strlen(comname2);
		if (checklen<9)
			for (i=checklen; i<14; i++)
				comname2[i]='_';
		comname2[7]='a';
		comname2[8]='\0';
		}
	strcpy(comname3, comname2);
	comname3[7]='b';
	noheader=1;
	}

 if (comnumber==3)
	{
	strcpy(comname2, comname1);
	strcpy(comname3, comname1);
	if ((name=strchr(comname2, '.'))==NULL)
		{
		checklen=strlen(comname2);
		comname2[checklen]='.';
		comname2[checklen+1]='1';
		comname2[checklen+2]='\0';
		}
	else if((name=strchr(comname2, '.'))!=NULL)
		{
		name++;
		*name++='1';
		*name++='\0';
		}
	if ((name=strchr(comname3, '.'))==NULL)
		{
		checklen=strlen(comname3);
		comname3[checklen]='.';
		comname3[checklen+1]='2';
		comname3[checklen+2]='\0';
		}
	else if((name=strchr(comname3, '.'))!=NULL)
		{
		name++;
		*name++='2';
		*name++='\0';
		}
	}

 if ((comnumber==4))
	{
	if (((strlen(comname2))<8) && ((name=strchr(comname2, '1'))==NULL))
		{
		strcpy(comname2, comname1);
		strcpy(comname3, comname1);
		if ((name=strchr(comname2, '.'))==NULL)
			{
			checklen=strlen(comname2);
			comname2[checklen]='.';
			comname2[checklen+1]='1';
			comname2[checklen+2]='\0';
			}
		else if((name=strchr(comname2, '.'))!=NULL)
			{
			name++;
			*name++='1';
			*name++='\0';
			}
		if ((name=strchr(comname3, '.'))==NULL)
			{
			checklen=strlen(comname3);
			comname3[checklen]='.';
			comname3[checklen+1]='2';
			comname3[checklen+2]='\0';
			}
		else if((name=strchr(comname3, '.'))!=NULL)
			{
			name++;
			*name++='2';
			*name++='\0';
			}
		}
	else
		{
		strcpy(comname3, comname2);
		if ((name=strchr(comname3, '.'))!=NULL)
			{
			name++;
			*name++='2';
			*name++='\0';
			}
		if ((comname3[7]=='a') && ((name=strchr(comname2, '1'))==NULL))
			{
			comname3[7]='b';
			noheader=1;
			}
		}
	}

 file_ptr=fopen(comname1, "rb");
 chkiffileexist(file_ptr, comname1);

 fseek(file_ptr, 0, SEEK_END);
 file_size=ftell(file_ptr);
 fseek(file_ptr, 0, SEEK_SET);
 file_size-=ftell(file_ptr);

 if (k==1)
	file_size=file_size+512;

 meter=(file_size-512)/130172;
 metersize=64/meter;
 if (((file_size-512)%131072)!=0)
	{
	printf("uCON Error:  Image file size is not a multiple of 1 Mbit.\n"); 
	printf("It must be padded before splitting.\n\n");
	exit(8);
	}

 file_ptr2=fopen(comname2, "w+b");
 file_ptr3=fopen(comname3, "w+b");
 printf("Splitting File: File #1=> %s\n", comname2);
 memset(header, 0, sizeof(header));

 header[1]=(((file_size/2)/8192)/256);
 header[0]=(((file_size/2)/8192)-(header[1]*256));

 header[2]=0x40;
 header[8]=0xaa;
 header[9]=0xbb;
 header[10]=0x04;
 if (h==1)
	header[3]=0x80;

 if (ramsize==0)
	{
	header[4]=0x77;
	header[5]=0x83;
	header[8]=0x00;
	header[9]=0x00;
	header[10]=0x00;
	}

 if (noheader==0)
	 fwrite(header, sizeof(header), 1, file_ptr2);
 file_size=file_size/8192;
 if (k!=1)
	fseek(file_ptr, 512, SEEK_SET);
 o=0;
 for (i=0; i<file_size/2; i++){
	o++;

	fread(readchunk, sizeof(readchunk), 1, file_ptr);
	fwrite(readchunk, sizeof(readchunk), 1, file_ptr2);
 }
 header[2]=char(0);
 if (noheader==0)
	 fwrite(header, sizeof(header), 1, file_ptr3);
 fseek(file_ptr, ((file_size*8192)/2)+512, SEEK_SET);
 if (k==1)
	fseek(file_ptr, ((file_size*8192)/2), SEEK_SET);
 printf("Splitting File: File #2=> %s\n", comname3);

 o=0;
 for (i=0; i<file_size/2; i++){
	o++;
	fread(readchunk, sizeof(readchunk), 1, file_ptr);
	errorchk=fwrite(readchunk, sizeof(readchunk), 1, file_ptr3);
	if (errorchk==0)
		diskspaceerror();
 }

 printf("Image Split");
 printf("\n");
}

void mghslowrom()
{
 unsigned long   i,r;
 FILE   *file_ptr;
 FILE   *file_ptr2;
 char   header[512];
 unsigned long file_size, file_overflow=0;
 int    checklen=0;
 char   *name;
 int    meter, metersize, o;
 int    fixflag=0;

 if (comnumber==3) {
	 strcpy(comname2, comname1);
	 if ((name=strchr(comname2, '.'))==NULL) {
		checklen=strlen(comname2);
		comname2[checklen]='.';
		comname2[checklen+1]='s';
		comname2[checklen+2]='l';
		comname2[checklen+3]='o';
		comname2[checklen+4]='\0';
	 }
	 else if((name=strchr(comname2, '.'))!=NULL) {
		name++;
		*name++='s';
		*name++='l';
		*name++='o';
		*name++='\0';
	 }
 }
 file_ptr=fopen(comname1, "rb");
 chkiffileexist(file_ptr, comname1);

 file_ptr2=fopen(comname2, "w+b");
 fseek(file_ptr, 0, SEEK_END);
 file_size=ftell(file_ptr);
 fseek(file_ptr, 0, SEEK_SET);
 file_size-=ftell(file_ptr);
 if (k==0)
	 meter=(file_size-512)/130172;
 else meter=(file_size)/131072;
 metersize=64/meter;
 printf("Attempting slowROM fix...\n");
 if (k==0)
	{
	file_size=file_size-512;
	fread(header, sizeof(header), 1, file_ptr);
	fwrite(header, sizeof(header), 1, file_ptr2);
	}

 file_overflow=(file_size-((file_size/32768)*32768));

 file_size=file_size/32768;
o=0;
for (r=0; r<file_size; r++){
	o++;
	fread(readchunk3, sizeof(readchunk3), 1, file_ptr);

// A9/A2 01 8D/8E 0D 42
 i=0;
 while (i<32768)
	{
	if ((readchunk3[i]==char(0xa9)) || (readchunk3[i]==char(0xa2)))
		{
		i++;
		if(readchunk3[i]==char(0x01))
			{
			i++;
			if ((readchunk3[i]==char(0x8d)) || (readchunk3[i]==char(0x8e)))
				{
				i++;
				if (readchunk3[i]==char(0x0d))
					{
					i++;
					if (readchunk3[i]==char(0x42))
						{
						readchunk3[i-3]=char(0x00);
						fixflag=1;
						i++;
						}
					else
						i++;
					}
				else
					i++;
				}
			else
				i++;
			}
		else
			i++;
		}
	else
		i++;
	}

 /* A9 01 00 8D 0D 42 */
 i=0;
 while (i<32768){
	if (readchunk3[i]==char(169)){
		i++;
		if(readchunk3[i]==char(1)){
			i++;
			if (readchunk3[i]==char(0)){
				i++;
				if (readchunk3[i]==char(141)){
					i++;
					if (readchunk3[i]==char(13)){
						i++;
						if (readchunk3[i]==char(66)){
							readchunk3[i-4]=char(0);
							fixflag=1;
							i++;
						}
						else {
							i++;}
					}
					else {
						i++;}
				}
				else {
					i++;}
			}
			else {
				i++;}
		}
		else {
			i++;}
	}
	else {
		i++;
	}
    }

 /* A9 01 8F 0D 42 00 */
 i=0;
 while (i<32768){
	if (readchunk3[i]==char(169)){
		i++;
		if(readchunk3[i]==char(1)){
			i++;
			if (readchunk3[i]==char(143)){
				i++;
				if (readchunk3[i]==char(13)){
					i++;
					if (readchunk3[i]==char(66)){
						i++;
						if (readchunk3[i]==char(0)){
							readchunk3[i-4]=char(0);
							fixflag=1;
							i++;
						}
						else {
							i++;}
					}
					else {
						i++;}
				}
				else {
					i++;}
			}
			else {
				i++;}
		}
		else {
			i++;}
	}
	else {
		i++;
	}
    }

    fwrite(readchunk3, sizeof(readchunk3), 1, file_ptr2);
 }

 if (file_overflow>0)
	{
	fread(readchunk3, file_overflow, 1, file_ptr);
	fwrite(readchunk3, file_overflow, 1, file_ptr2);
	}

 if (fixflag==1)
        printf("slowROM Fix Successful.");
 else
	{
        printf("slowROM Fix Failed.");
	remove(comname2);
	}
 printf("\n");
}


void mghname()
{
FILE    *file_ptr;
FILE    *file_ptr2;
char    cartname[15];
char    *name;
char    mgh[32];
int     i;
int     checklen;
signed long offset=0;

 if (comnumber==3)
	{
	strcpy(comname2, comname1);
	if ((name=strchr(comname2, '.'))==NULL)
		{
		checklen=strlen(comname2);
		comname2[checklen]='.';
		comname2[checklen+1]='m';
		comname2[checklen+2]='g';
		comname2[checklen+3]='h';
		comname2[checklen+4]='\0';
		}
	 else if((name=strchr(comname2, '.'))!=NULL)
		{
		name++;
		*name++='m';
		*name++='g';
		*name++='h';
		*name++='\0';
		}
	}

 if (comnumber==4)
	{
	strcpy(cartname, comname2);
	for (i=13; i<15; i++)
		cartname[i]=' ';
	if (strlen(cartname)<16)
		for (i=strlen(cartname); i<16; i++)
			cartname[i]=' ';

	strcpy(comname2, comname1);
	if ((name=strchr(comname2, '.'))==NULL)
		{
		checklen=strlen(comname2);
		comname2[checklen]='.';
		comname2[checklen+1]='m';
		comname2[checklen+2]='g';
		comname2[checklen+3]='h';
		comname2[checklen+4]='\0';
		}
	else if((name=strchr(comname2, '.'))!=NULL)
		{
		name++;
		*name++='m';
		*name++='g';
		*name++='h';
		*name++='\0';
		}
	}

 file_ptr=fopen(comname1, "rb");
 chkiffileexist(file_ptr, comname1);

 file_ptr2=fopen(comname2, "w+b");
 printf("Creating MGH Name File...\n");

 if (comnumber==3)
	{
	if (k==1)
		offset=-512;

	if (k==0)
		if (h==1)
			offset=32768;

	fseek(file_ptr, 33216+offset, SEEK_SET);
	fread(cartname, sizeof(cartname), 1, file_ptr);
	}

 mgh[0]='M';
 mgh[1]='G';
 mgh[2]='H';
 mgh[3]=char(26);
 for (i=4; i<31; i++)
	mgh[i]=char(0);
 mgh[31]=char(255);
 for (i=16; i<31; i++)
	mgh[i]=cartname[i-16];
 fwrite(mgh, sizeof(mgh), 1, file_ptr2);
 printf("MGH Name File Created.\n");
}

void mghpalfix()
{
 unsigned long   i,r;
 FILE   *file_ptr;
 FILE   *file_ptr2;
 char   header[512];
 unsigned long file_size, file_overflow=0;
 int    checklen=0;
 char   *name;
 int    meter, metersize, o;
 int    fixflag=0;

 if (comnumber==3)
	{
	strcpy(comname2, comname1);
	if ((name=strchr(comname2, '.'))==NULL)
		{
		checklen=strlen(comname2);
		comname2[checklen]='.';
		comname2[checklen+1]='f';
		comname2[checklen+2]='i';
		comname2[checklen+3]='x';
		comname2[checklen+4]='\0';
		}
	 else if((name=strchr(comname2, '.'))!=NULL)
		{
		name++;
		*name++='f';
		*name++='i';
		*name++='x';
		*name++='\0';
		}
	}



 file_ptr=fopen(comname1, "rb");
 chkiffileexist(file_ptr, comname1);

 file_ptr2=fopen(comname2, "w+b");
 fseek(file_ptr, 0, SEEK_END);
 file_size=ftell(file_ptr);
 fseek(file_ptr, 0, SEEK_SET);
 file_size-=ftell(file_ptr);
 if (k==0)
	 meter=(file_size-512)/130172;
 else meter=(file_size)/131072;
 metersize=64/meter;
 printf("Attempting NTSC/PAL Fix...\n");

 if (k==0)
	{
	file_size=file_size-512;
	fread(header, sizeof(header), 1, file_ptr);
	fwrite(header, sizeof(header), 1, file_ptr2);
	}

 file_overflow=(file_size-((file_size/32768)*32768));

 file_size=file_size/32768;
 o=0;
 for (r=0; r<file_size; r++)
	{
	o++;
	fread(readchunk3, sizeof(readchunk3), 1, file_ptr);

// 3F 21 29/89 10 C9 10 F0
 i=0;
 while (i<32768)
	{
	if (readchunk3[i]==char(0x3f))
		{
		i++;
		if(readchunk3[i]==char(0x21))
			{
			i++;
			if ((readchunk3[i]==char(0x29)) || (readchunk3[i]==char(0x89)))
				{
				i++;
				if (readchunk3[i]==char(0x10))
					{
					i++;
					if (readchunk3[i]==char(0xc9))
						{
						i++;
						if (readchunk3[i]==char(0x10))
							{
							i++;
							if (readchunk3[i]==char(0xf0))
								{
								readchunk3[i]=char(0x80);
								fixflag=1;
								i++;
								}
							else
								i++;
							}
						else
							i++;
						}
					else
						i++;
					}
				else
					i++;
				}
			else
				i++;
			}
		else
			i++;
		}
	else
		i++;
	}

/* 3F 21 89 10 F0 */
 i=0;
 while (i<32768){
	if (readchunk3[i]=='?'){
		i++;
		if(readchunk3[i]=='!'){
			i++;
			if (readchunk3[i]=='‰') {
				i++;
				if (readchunk3[i]==char(16)){
					i++;
					if (readchunk3[i]=='ð'){
						readchunk3[i]=char(128);
						fixflag=1;
						i++;
					}
					else {
						i++;}
				}
				else {
					i++;}
			}
			else {
				i++;}
		}
		else {
			i++;}
	}
	else {
		i++;
	}
 }

// 3F 21 00 89/29 10 F0
 i=0;
 while (i<32768)
	{
	if (readchunk3[i]==char(0x3f))
		{
		i++;
		if(readchunk3[i]==char(0x21))
			{
			i++;
			if (readchunk3[i]==char(0x00))
				{
				i++;
				if ((readchunk3[i]==char(0x89)) || (readchunk3[i]==char(0x29)))
					{
					i++;
					if (readchunk3[i]==char(0x10))
						{
						i++;
						if (readchunk3[i]==char(0xf0))
							{
							readchunk3[i]=char(0x80);
							fixflag=1;
							i++;
							}
						else
							i++;
						}
					else
						i++;
					}
				else
					i++;
				}
			else
				i++;
			}
		else
			i++;
		}
	else
		i++;
	}

// 3F 21 89/29 10 00 F0
 i=0;
 while (i<32768)
	{
	if (readchunk3[i]==char(0x3f))
		{
		i++;
		if(readchunk3[i]==char(0x21))
			{
			i++;
			if ((readchunk3[i]==char(0x89)) || (readchunk3[i]==char(0x29)))
				{
				i++;
				if (readchunk3[i]==char(0x10))
					{
					i++;
					if (readchunk3[i]==char(0x00))
						{
						i++;
						if (readchunk3[i]==char(0xf0))
							{
							readchunk3[i]=char(0x80);
							fixflag=1;
							i++;
							}
						else
							i++;
						}
					else
						i++;
					}
				else
					i++;
				}
			else
				i++;
			}
		else
			i++;
		}
	else
		i++;
	}

// AD 3F 21 29 10 00 D0 NTSC
 i=0;
 while (i<32768){
	if (readchunk3[i]==char(173)){
		i++;
		if(readchunk3[i]==char(63)){
			i++;
			if (readchunk3[i]==char(33)) {
				i++;
				if (readchunk3[i]==char(41)) {
					i++;
					if (readchunk3[i]==char(16)){
						i++;
						if (readchunk3[i]==char(0)){
							i++;
							if (readchunk3[i]==char(208)){
								readchunk3[i]=char(128);
								fixflag=2;
								i++;
							}
							else {
								i++;}
						}
						else {
							i++;}
					}
					else {
						i++;}
				}
				else {
					i++;}
			}
			else {
				i++;}
		}
		else {
			i++;}
	}
	else {
		i++;
	}
 }
 
// Eric Cantona Football Challenge
/* AD 3F 21 89 10 00 D0 NTSC */
 i=0;
 while (i<32768)
	{
	if (readchunk3[i]==char(173))
		{
		i++;
		if(readchunk3[i]==char(63))
			{
			i++;
			if (readchunk3[i]==char(33))
				{
				i++;
				if (readchunk3[i]==char(137))
					{
					i++;
					if (readchunk3[i]==char(16))
						{
						i++;
						if (readchunk3[i]==char(0))
							{
							i++;
							if (readchunk3[i]==char(208))
								{
								readchunk3[i-6]=169;
								readchunk3[i-5]=16;
								readchunk3[i-4]=0;
								fixflag=2;
								i++;
								}
							else
								i++;
							}
						else
							i++;
						}
					else
						i++;
					}
				else
					i++;
				}
			else
				i++;
			}
		else
			i++;
		}
	else
		i++;
	}

/* AF 3F 21 00 29 10 D0 NTSC */
 i=0;
 while (i<32768){
	if (readchunk3[i]==char(175)){
		i++;
		if(readchunk3[i]==char(63)){
			i++;
			if (readchunk3[i]==char(33)) {
				i++;
				if (readchunk3[i]==char(0)) {
					i++;
					if (readchunk3[i]==char(41)){
						i++;
						if (readchunk3[i]==char(16)){
							i++;
							if (readchunk3[i]==char(208)){
								readchunk3[i]=char(128);
								fixflag=2;
								i++;
							}
							else {
								i++;}
						}
						else {
							i++;}
					}
					else {
						i++;}
				}
				else {
					i++;}
			}
			else {
				i++;}
		}
		else {
			i++;}
	}
	else {
		i++;
	}
 }
 
/* AF 3F 21 00 29 10 00 D0 NTSC */
 i=0;
 while (i<32768){
	if (readchunk3[i]==char(175)){
		i++;
		if(readchunk3[i]==char(63)){
			i++;
			if (readchunk3[i]==char(33)) {
				i++;
				if (readchunk3[i]==char(0)) {
					i++;
					if (readchunk3[i]==char(41)){
						i++;
						if (readchunk3[i]==char(16)){
							i++;
							if (readchunk3[i]==char(0)){
								i++;
								if (readchunk3[i]==char(208)){
									readchunk3[i]=char(234);
									readchunk3[i+1]=char(234);
									fixflag=2;
									i++;
								}
								else {
									i++;}
							}
							else {
								i++;}
						}
						else {
							i++;}
					}
					else {
						i++;}
				}
				else {
					i++;}
			}
			else {
				i++;}
		}
		else {
			i++;}
	}
	else {
		i++;
	}
 }


/* AD 3F 21 29 10 D0 PAL */
 i=0;
 while (i<32768){
	if (readchunk3[i]==char(173)){
		i++;
		if(readchunk3[i]==char(63)){
			i++;
			if (readchunk3[i]==char(33)) {
				i++;
				if (readchunk3[i]==char(41)) {
					i++;
					if (readchunk3[i]==char(16)){
						i++;
						if (readchunk3[i]==char(208)){
							readchunk3[i]=char(234);
							readchunk3[i+1]=char(234);
							fixflag=1;
							i++;
						}
						else {
							i++;}
					}
					else {
						i++;}
				}
				else {
				i++;}
			}
			else {
				i++;}
		}
		else {
			i++;}
	}
	else {
		i++;
	}
 }

/* AD 3F 21 29 10 F0 PAL */
 i=0;
 while (i<32768)
	{
	if (readchunk3[i]==char(0xad))
		{
		i++;
		if(readchunk3[i]==char(0x3f))
			{
			i++;
			if (readchunk3[i]==char(0x21))
				{
				i++;
				if (readchunk3[i]==char(0x29))
					{
					i++;
					if (readchunk3[i]==char(0x10))
						{
						i++;
						if (readchunk3[i]==char(0xf0))
							{
							readchunk3[i]=char(0x80);
							fixflag=1;
							i++;
							}
						else
							i++;
						}
					else
						i++;
					}
				else
					i++;
				}
			else
				i++;
			}
		else
			i++;
		}
	else
		i++;
	}

/* AD 3F 21 89 10 D0 PAL */
 i=0;
 while (i<32768){
	if (readchunk3[i]==char(173)){
		i++;
		if(readchunk3[i]==char(63)){
			i++;
			if (readchunk3[i]==char(33)) {
				i++;
				if (readchunk3[i]==char(137)) {
					i++;
					if (readchunk3[i]==char(16)){
						i++;
						if (readchunk3[i]==char(208)){
							readchunk3[i]=char(128);
							fixflag=1;
							i++;
						}
						else {
							i++;}
					}
					else {
						i++;}
				}
				else {
					i++;}
			}
			else {
				i++;}
		}
		else {
			i++;}
	}
	else {
		i++;
	}
 }

/* AD 3F 21 29 10 C9 00 F0 PAL */
 i=0;
 while (i<32768){
	if (readchunk3[i]==char(173)){
		i++;
		if(readchunk3[i]==char(63)){
			i++;
			if (readchunk3[i]==char(33)) {
				i++;
				if (readchunk3[i]==char(41)) {
					i++;
					if (readchunk3[i]==char(16)){
						i++;
						if (readchunk3[i]==char(201)){
							i++;
							if (readchunk3[i]==char(0)){
								i++;
								if (readchunk3[i]==char(240)){
									readchunk3[i]=char(128);
									fixflag=1;
									i++;
								}
								else {
									i++;}
							}
							else {
								i++;}
						}
						else {
							i++;}
					}
					else {
						i++;}
				}
				else {
					i++;}
			}
			else {
				i++;}
		}
		else {
			i++;}
	}
	else {
		i++;
	}
 }

// AF 3F 21 00 29/89 10 00 F0 PAL
 i=0;
 while (i<32768)
	{
	if (readchunk3[i]==char(0xaf))
		{
		i++;
		if(readchunk3[i]==char(0x3f))
			{
			i++;
			if (readchunk3[i]==char(0x21))
				{
				i++;
				if (readchunk3[i]==char(0x00))
					{
					i++;
					if (((readchunk3[i]==char(0x29)) || (readchunk3[i]==char(0x89))))
						{
						i++;
						if (readchunk3[i]==char(0x10))
							{
							i++;
							if (readchunk3[i]==char(0x00))
								{
								i++;
								if (readchunk3[i]==char(0xf0))
									{
									readchunk3[i]=char(0x80);
									fixflag=1;
									i++;
									}
								else
									i++;
								}
							else
								i++;
							}
						else
							i++;
						}
					else
						i++;
					}
				else
					i++;
				}
			else
				i++;
			}
		else
			i++;
		}
	else
		i++;
	}

// AD 3F 21 29 10 CF BD FF 80 F0 PAL Konami Ganbare Goemon 2
 i=0;
 while (i<32768){
	if (readchunk3[i]==char(173)){
		i++;
		if(readchunk3[i]==char(63)){
			i++;
			if (readchunk3[i]==char(33)){
				i++;
				if (readchunk3[i]==char(41)){
					i++;
					if (readchunk3[i]==char(16)){
						i++;
						if (readchunk3[i]==char(207)){
							i++;
							if (readchunk3[i]==char(189)){
								i++;
								if (readchunk3[i]==char(255)){
									i++;
									if (readchunk3[i]==char(128)){
										i++;
										if (readchunk3[i]==char(240)){
											readchunk3[i]=char(128);
											fixflag=1;
											i++;
										}
										else {
											i++;}
									}
									else{
										i++;}
								}
								else {
									i++;}
							}
							else {
								i++;}
						}
						else {
							i++;}
					}
					else{
						i++;}
				}
				else {
					i++;}
			}
			else {
				i++;}
		}
		else {
			i++;}
	}
	else {
		i++;
	}
 }

// AF 3F 21 EA 89 10 00 D0 PAL--Super Famista 3
 i=0;
 while (i<32768)
	{
	if (readchunk3[i]==char(0xaf))
		{
		i++;
		if(readchunk3[i]==char(0x3f))
			{
			i++;
			if (readchunk3[i]==char(0x21))
				{
				i++;
				if (readchunk3[i]==char(0xea))
					{
					i++;
					if (readchunk3[i]==char(0x89))
						{
						i++;
						if (readchunk3[i]==char(0x10))
							{
							i++;
							if (readchunk3[i]==char(0))
								{
								i++;
								if (readchunk3[i]==char(0xd0))
									{
									readchunk3[i-7]=char(0xa9);
									readchunk3[i-6]=char(0);
									readchunk3[i-5]=char(0);
									fixflag=1;
									i++;
									}
								else
									i++;
								}
							else
								i++;
							}
						else
							i++;
						}
					else
						i++;
					}
				else
					i++;
				}
			else
				i++;
			}
		else
			i++;
		}
	else
		i++;
	}

// AD 3F 21 8D xx xx 29 10 8D PAL DragonBallZ 2
 i=0;
 while (i<32768)
	{
	if (readchunk3[i]==char(0xad))
		{
		i++;
		if(readchunk3[i]==char(0x3f))
			{
			i++;
			if (readchunk3[i]==char(0x21))
				{
				i++;
				if (readchunk3[i]==char(0x8d))
					{
					i++;
					i++;
					i++;
					if (readchunk3[i]==char(0x29))
						{
						i++;
						if (readchunk3[i]==char(0x10))
							{
							i++;
							if (readchunk3[i]==char(0x8d))
								{
								readchunk3[i-1]=char(0x00);
								fixflag=1;
								i++;
								}
							else
								i++;
							}
						else
							i++;
						}
					else
						i++;
					}
				else
					i++;
				}
			else
				i++;
			}
		else
			i++;
		}
	else
		i++;
	}

// AF 3F 21 00 xx xx 29 10 00 D0 PAL Fatal Fury Special [Jap]
 i=0;
 while (i<32768)
	{
	if (readchunk3[i]==char(0xaf))
		{
		i++;
		if(readchunk3[i]==char(0x3f))
			{
			i++;
			if (readchunk3[i]==char(0x21))
				{
				i++;
				if (readchunk3[i]==char(0x00))
					{
					i++;
					i++;
					i++;
					if (readchunk3[i]==char(0x29))
						{
						i++;
						if (readchunk3[i]==char(0x10))
							{
							i++;
							if (readchunk3[i]==char(0x00))
								{
								i++;
								if (readchunk3[i]==char(0xd0))
									{
									readchunk3[i]=char(0xea);
									readchunk3[i+1]=char(0xea);
									fixflag=1;
									i++;
									}
								else
									i++;
								}
							else
								i++;
							}
						else
							i++;
						}
					else
						i++;
					}
				else
					i++;
				}
			else
				i++;
			}
		else
			i++;
		}
	else
		i++;
	}

// AD 3F 21 29 10 CF BD FF 00 F0  NTSC Tiny Toons Wacky Sports
 i=0;
 while (i<32768)
	{
	if (readchunk3[i]==char(0xad))
		{
		i++;
		if(readchunk3[i]==char(0x3f))
			{
			i++;
			if (readchunk3[i]==char(0x21))
				{
				i++;
				if (readchunk3[i]==char(0x29))
					{
					i++;
					if (readchunk3[i]==char(0x10))
						{
						i++;
						if (readchunk3[i]==char(0xcf))
							{
							i++;
							if (readchunk3[i]==char(0xbd))
								{
								i++;
								if (readchunk3[i]==char(0xff))
									{
									i++;
									if (readchunk3[i]==char(0x00))
										{
										i++;
										if (readchunk3[i]==char(0xf0))
											{
											readchunk3[i]=char(0x80);
											fixflag=2;
											i++;
											}
										else
											i++;
										}
									else
										i++;
									}
								else
									i++;
								}
							else
								i++;
							}
						else
							i++;
						}
					else
						i++;
					}
				else
					i++;
				}
			else
				i++;
			}
		else
			i++;
		}
	else
		i++;
	}

 fwrite(readchunk3, sizeof(readchunk3), 1, file_ptr2);
 }

 if (file_overflow>0)
	{
	fread(readchunk3, file_overflow, 1, file_ptr);
	fwrite(readchunk3, file_overflow, 1, file_ptr2);
	}

 if (fixflag==2)
        printf("NTSC Fix Successful.");
 else
 if (fixflag==1)
        printf("PAL Fix Successful.");
 else
	{
        printf("NTSC/PAL Fix Unsuccessful.");
	remove(comname2);
	}
 printf("\n");
}


int whichsystem()
{
 FILE   *file_ptr;
 char   inchar[1];
 int checksum1, checksum2, comchecksum1, comchecksum2;
 unsigned int checksum, comchecksum;
 int    flag=-1;
 unsigned long  file_size;

 file_ptr=fopen(comname1, "rb");
 chkiffileexist(file_ptr, comname1);

 fseek(file_ptr, 0, SEEK_END);
 file_size=ftell(file_ptr);
 fseek(file_ptr, 0, SEEK_SET);
 file_size-=ftell(file_ptr);
 checkminsize(file_size);

 fseek(file_ptr, 640, SEEK_SET);
 fread(inchar, sizeof(inchar), 1, file_ptr);
 if (inchar[0]=='E')
	{
	fseek(file_ptr, 641, SEEK_SET);
	fread(inchar, sizeof(inchar), 1, file_ptr);
	if (inchar[0]=='A')
		flag=1;
	}
 if (flag==-1)
	{
	fseek(file_ptr, 128, SEEK_SET);
	fread(inchar, sizeof(inchar), 1, file_ptr);
	if (inchar[0]=='E')
		{
		fseek(file_ptr, 129, SEEK_SET);
		fread(inchar, sizeof(inchar), 1, file_ptr);
		if (inchar[0]=='A')
			flag=1;
		}
	}
 if (flag==-1)
	{
	fseek(file_ptr, 33244, SEEK_SET);
	fread(inchar, sizeof(inchar), 1, file_ptr);
	if ((checksum1=inchar[0])<0) checksum1=checksum1+256;
	fread(inchar, sizeof(inchar), 1, file_ptr);
	if ((checksum2=inchar[0])<0) checksum2=checksum2+256;
	fread(inchar, sizeof(inchar), 1, file_ptr);
	if ((comchecksum1=inchar[0])<0) comchecksum1=comchecksum1+256;
	fread(inchar, sizeof(inchar), 1, file_ptr);
	if ((comchecksum2=inchar[0])<0) comchecksum2=comchecksum2+256;
	checksum=checksum1+checksum2*256;
	comchecksum=comchecksum1+comchecksum2*256;
	if ((checksum+comchecksum)==65535)
		flag=0;
	}
 if (flag==-1)
	{
	fseek(file_ptr, 66012, SEEK_SET);
	fread(inchar, sizeof(inchar), 1, file_ptr);
	if ((checksum1=inchar[0])<0) checksum1=checksum1+256;
	fread(inchar, sizeof(inchar), 1, file_ptr);
	if ((checksum2=inchar[0])<0) checksum2=checksum2+256;
	fread(inchar, sizeof(inchar), 1, file_ptr);
	if ((comchecksum1=inchar[0])<0) comchecksum1=comchecksum1+256;
	fread(inchar, sizeof(inchar), 1, file_ptr);
	if ((comchecksum2=inchar[0])<0) comchecksum2=comchecksum2+256;
	checksum=checksum1+checksum2*256;
	comchecksum=comchecksum1+comchecksum2*256;
	if ((checksum+comchecksum)==65535)
		flag=0;
	}
 if (flag==-1)
	{
	fseek(file_ptr, 32732, SEEK_SET);
	fread(inchar, sizeof(inchar), 1, file_ptr);
	if ((checksum1=inchar[0])<0) checksum1=checksum1+256;
	fread(inchar, sizeof(inchar), 1, file_ptr);
	if ((checksum2=inchar[0])<0) checksum2=checksum2+256;
	fread(inchar, sizeof(inchar), 1, file_ptr);
	if ((comchecksum1=inchar[0])<0) comchecksum1=comchecksum1+256;
	fread(inchar, sizeof(inchar), 1, file_ptr);
	if ((comchecksum2=inchar[0])<0) comchecksum2=comchecksum2+256;
	checksum=checksum1+checksum2*256;
	comchecksum=comchecksum1+comchecksum2*256;
	if ((checksum+comchecksum)==65535)
		flag=0;
	}
 if (flag==-1)
	{
	fseek(file_ptr, 65500, SEEK_SET);
	fread(inchar, sizeof(inchar), 1, file_ptr);
	if ((checksum1=inchar[0])<0) checksum1=checksum1+256;
	fread(inchar, sizeof(inchar), 1, file_ptr);
	if ((checksum2=inchar[0])<0) checksum2=checksum2+256;
	fread(inchar, sizeof(inchar), 1, file_ptr);
	if ((comchecksum1=inchar[0])<0) comchecksum1=comchecksum1+256;
	fread(inchar, sizeof(inchar), 1, file_ptr);
	if ((comchecksum2=inchar[0])<0) comchecksum2=comchecksum2+256;
	checksum=checksum1+checksum2*256;
	comchecksum=comchecksum1+comchecksum2*256;
	if ((checksum+comchecksum)==65535)
		flag=0;
	}
 return(flag);
}

void mghsegaj()
{
 unsigned long   i;
 FILE   *file_ptr;
 FILE   *file_ptr2;
 FILE   *file_ptr3;
 char   header[512];
 unsigned long file_size;
 int    j, checklen=0;
 char   *name;
 unsigned long  size=0;
 int    nextfile=50;
 char   nextcheck[1];
 int    filenumber=1;
 int    next;
 int    meter, metersize, o;
 int    errorchk=0;

 if (k==0)
	{
	if (comnumber==3)
		{
		strcpy(comname2, comname1);
		if ((name=strchr(comname2, '.'))==NULL)
			{
			checklen=strlen(comname2);
			comname2[checklen]='.';
			comname2[checklen+1]='s';
			comname2[checklen+2]='m';
			comname2[checklen+3]='d';
			comname2[checklen+4]='\0';
			}
		else if((name=strchr(comname2, '.'))!=NULL)
			{
			name++;
			*name++='s';
			*name++='m';
			*name++='d';
			*name++='\0';
			}
		}

	checklen=strlen(comname2);
	for (j=0; j<checklen; j++)
		if (comname1[j]!=comname2[j])
			errorchk=-1;
	if (errorchk==0)
		{
		printf("uCON Error:  The input and output filenames are the same!\n");
		exit(8);
		}

	file_ptr=fopen(comname1, "rb");
	chkiffileexist(file_ptr, comname1);

	file_ptr2=fopen(comname2, "w+b");
	memset(header, 0, sizeof(header));
	header[1]=0x03;
	header[8]=0xaa;
	header[9]=0xbb;
	header[10]=0x06;
	fwrite(header, sizeof(header), 1, file_ptr2);
	next=1;
	while (next!=0)
		{
		file_ptr=fopen(comname1, "rb");

		if (NULL==file_ptr)
			{
                        printf("uCON Error:  Cannot open input file #%d, \"%s.\"\n\n", filenumber, comname1);
			exit(8);
			}

                printf("Joining SMD Files: File #%d=> %s\n", filenumber, comname1);
		filenumber++;
		fseek(file_ptr, 0, SEEK_END);
		file_size=ftell(file_ptr);
		fseek(file_ptr, 0, SEEK_SET);
		file_size-=ftell(file_ptr);
		meter=(file_size-512)/131072;
		metersize=64/meter;
		size=size+(file_size-512);
		file_size=file_size/8192;
		fseek(file_ptr, 512, SEEK_SET);
		o=0;
		for (i=0; i<file_size; i++)
			{
			o++;
			fread(readchunk, sizeof(readchunk), 1, file_ptr);

			errorchk=fwrite(readchunk, sizeof(readchunk), 1, file_ptr2);

			if (errorchk==0)
				diskspaceerror();
			}
		if ((name=strchr(comname1, '.'))!=NULL)
			{
			name++;
			*name++=char(nextfile++);
			*name++='\0';
			}
		fseek(file_ptr, 2, SEEK_SET);
		fread(nextcheck, sizeof(nextcheck), 1, file_ptr);
		if ((next=nextcheck[0])<0) next=nextcheck[0]+256;
		fclose(file_ptr);
	}
}

 if (k==1)
	{
	if (comnumber==3)
		{
		strcpy(comname2, comname1);
		if ((name=strchr(comname2, '.'))==NULL)
			{
			checklen=strlen(comname2);
			comname2[checklen-1]='b';
			comname2[checklen]='.';
			comname2[checklen+1]='0';
			}
		else if((name=strchr(comname2, '.'))!=NULL)
			{
			name--;
			*name='b';
			name++;
			*name++='.';
			*name++='0';
			}
		strcpy(comname3, comname1);
		if ((name=strchr(comname3, '.'))==NULL)
			{
			checklen=strlen(comname3);
			comname3[checklen-1]='-';
			comname3[checklen]='.';
			comname3[checklen+1]='0';
			comname3[checklen+2]='7';
			comname3[checklen+3]='8';
			comname3[checklen+4]='\0';
			}
		else if((name=strchr(comname3, '.'))!=NULL)
			{
			name--;
			*name='-';
			name++;
			*name++='.';
			*name++='0';
			*name++='7';
			*name++='8';
			*name++='\0';
			}
		}
	file_ptr=fopen(comname1, "rb");
	chkiffileexist(file_ptr, comname1);
	file_ptr2=fopen(comname2, "rb");
	chkiffileexist(file_ptr2, comname2);
	file_ptr3=fopen(comname3, "w+b");
        printf("Joining MGD2 Files: %s\n", comname1);
	fseek(file_ptr, 0, SEEK_END);
	file_size=ftell(file_ptr);
	fseek(file_ptr, 0, SEEK_SET);
	file_size-=ftell(file_ptr);
	file_size=file_size*2;
	size=file_size;
	meter=(file_size)/131072;
	metersize=64/meter;
	file_size=file_size/8192;
	fseek(file_ptr, 0, SEEK_SET);
	o=0;
	for (i=0; i<file_size/4; i++)
		{
		o++;
		fread(readchunk, sizeof(readchunk), 1, file_ptr);

		errorchk=fwrite(readchunk, sizeof(readchunk), 1, file_ptr3);

		if (errorchk==0)
			diskspaceerror();
		}
	 fseek(file_ptr2, 0, SEEK_SET);
	 o=0;
	 for (i=0; i<file_size/4; i++)
		{
		o++;
		if ((o%(file_size/meter))==0)
			for (j=0; j<metersize; j++)
                                printf("Û");
		fread(readchunk, sizeof(readchunk), 1, file_ptr2);
		errorchk=fwrite(readchunk, sizeof(readchunk), 1, file_ptr3);

		if (errorchk==0)
			diskspaceerror();

		}
         printf("Joining MGD2 Files: %s\n", comname2);
	 fseek(file_ptr, ((file_size/2)*8192)/2, SEEK_SET);
	 o=0;
	 for (i=0; i<file_size/4; i++)
		{
		o++;
		fread(readchunk, sizeof(readchunk), 1, file_ptr);
		errorchk=fwrite(readchunk, sizeof(readchunk), 1, file_ptr3);

		if (errorchk==0)
			diskspaceerror();

		}
	 fseek(file_ptr2, ((file_size/2)*8192)/2, SEEK_SET);
	 o=0;
	 for (i=0; i<file_size/4; i++)
		{
		o++;
		fread(readchunk, sizeof(readchunk), 1, file_ptr2);
		errorchk=fwrite(readchunk, sizeof(readchunk), 1, file_ptr3);

		if (errorchk==0)
			diskspaceerror();

		}
	}

 if (k==0)
	{
	file_ptr2=fopen(comname2, "r+b");
	if ((size/8192)>255)
		{
		header[1]=char(3);
		header[0]=char((size/8192)/2);
		}
	else
		{
		header[0]=char((size/8192)/2);
		header[1]=char(3);
		}
	fseek(file_ptr, 0, SEEK_SET);
	fwrite(header, sizeof(header), 1, file_ptr2);
	}

 printf("Files Joined.\n");
}

void mghssplit()
{
 unsigned long   i;
 FILE   *file_ptr;
 FILE   *file_ptr2;
 FILE   *file_ptr3;
 char   header[512];
 unsigned long file_size;
 int    j, checklen=0;
 char   *name;
 int    meter, metersize, o;
 int    noheader=0;

 if (k==1)  {
	noheader=1;
 }
 if (comnumber==3) {
	 strcpy(comname2, comname1);
	 strcpy(comname3, comname1);
	 if ((name=strchr(comname2, '.'))==NULL) {
		checklen=strlen(comname2);
		comname2[checklen]='.';
		comname2[checklen+1]='1';
		comname2[checklen+2]='\0';
	 }
	 else if((name=strchr(comname2, '.'))!=NULL) {
		name++;
		*name++='1';
		*name++='\0';
	 }
	 if ((name=strchr(comname3, '.'))==NULL) {
		checklen=strlen(comname3);
		comname3[checklen]='.';
		comname3[checklen+1]='2';
		comname3[checklen+2]='\0';
	 }
	 else if((name=strchr(comname3, '.'))!=NULL) {
		name++;
		*name++='2';
		*name++='\0';
	 }
 }
 if (comnumber>3){
         if ((strncmp(comname2, "md\n", 2))!=0) {
	 strcpy(comname3, comname2);
	 if ((name=strchr(comname2, '.'))==NULL) {
		checklen=strlen(comname2);
		comname2[checklen]='.';
		comname2[checklen+1]='1';
		comname2[checklen+2]='\0';
	 }
	 else if((name=strchr(comname2, '.'))!=NULL) {
		name++;
		*name++='1';
		*name++='\0';
	 }
	 if ((name=strchr(comname3, '.'))==NULL) {
		checklen=strlen(comname3);
		comname3[checklen]='.';
		comname3[checklen+1]='2';
		comname3[checklen+2]='\0';
	 }
	 else if((name=strchr(comname3, '.'))!=NULL) {
		name++;
		*name++='2';
		*name++='\0';
	 }
 }
 else  {strcpy(comname3, comname2);
	if ((checklen=strlen(comname3))<7){
		for (j=checklen; j<7; j++){
			comname2[j]='_';
			comname3[j]='_';}
	}
	comname3[7]='b';
	comname2[7]='a';
	if (noheader==1){
	if ((name=strchr(comname2, '.'))==NULL) {
		checklen=strlen(comname2);
		comname2[checklen]='.';
		comname2[checklen+1]='0';
		comname2[checklen+2]='7';
		comname2[checklen+3]='8';
		comname2[checklen+4]='\0';
	 }
	 else if((name=strchr(comname2, '.'))!=NULL) {
		name++;
		*name++='0';
		*name++='7';
		*name++='8';
		*name++='\0';
	 }
	 if ((name=strchr(comname3, '.'))==NULL) {
		checklen=strlen(comname3);
		comname3[checklen]='.';
		comname3[checklen+1]='0';
		comname3[checklen+2]='7';
		comname3[checklen+3]='8';
		comname3[checklen+4]='\0';
	 }
	 else if((name=strchr(comname3, '.'))!=NULL) {
		name++;
		*name++='0';
		*name++='7';
		*name++='8';
		*name++='\0';
	 }
    }
 }
}
 if ((comnumber==3) && (noheader==1)) {
	strcpy(comname2, comname1);
	if ((name=strchr(comname2, '.'))==NULL) {
		checklen=strlen(comname2);
		if (checklen<7)
			for (i=checklen; i<7; i++)
				comname2[i]='_';
		comname2[7]='a';
		comname2[8]='.';
		comname2[9]='0';
		comname2[10]='7';
		comname2[11]='8';
		comname2[12]='\0';
		strcpy(comname3, comname2);
		comname3[7]='b';
	}
	if ((name=strchr(comname2, '.'))!=NULL) {
		*name='\0';
		checklen=strlen(comname2);
		if (checklen<7)
			for (i=checklen; i<7; i++)
				comname2[i]='_';
		comname2[7]='a';
		comname2[8]='.';
		comname2[9]='0';
		comname2[10]='7';
		comname2[11]='8';
		comname2[12]='\0';
		strcpy(comname3, comname2);
		comname3[7]='b';
	}
 }
 file_ptr=fopen(comname1, "rb");
 chkiffileexist(file_ptr, comname1);

 file_ptr2=fopen(comname2, "w+b");
 file_ptr3=fopen(comname3, "w+b");
 fseek(file_ptr, 0, SEEK_END);
 file_size=ftell(file_ptr);
 fseek(file_ptr, 0, SEEK_SET);
 file_size-=ftell(file_ptr);
 meter=(file_size-512)/131072;
 metersize=64/meter;
 printf("Splitting File: File #1=> %s\n", comname2);

 memset(header, 0, sizeof(header));
 if ((file_size/8192)>255)
	{
	header[1]=0x03;
	header[0]=char(((file_size/8192)/2)/2);
	}
 else
	{
	header[0]=char(((file_size/8192)/2)/2);
	header[1]=0x03;
	}
 header[2]=0x40;
 header[8]=0xaa;
 header[9]=0xbb;
 header[10]=0x06;
 if (noheader==0) {
	 fwrite(header, sizeof(header), 1, file_ptr2);
	 file_size=file_size/8192;
	 fseek(file_ptr, 512, SEEK_SET);
 o=0;
 for (i=0; i<file_size/2; i++){
	o++;
	fread(readchunk, sizeof(readchunk), 1, file_ptr);
	fwrite(readchunk, sizeof(readchunk), 1, file_ptr2);
 }
 header[2]=0x00;
 fwrite(header, sizeof(header), 1, file_ptr3);
 fseek(file_ptr, ((file_size*8192)/2)+512, SEEK_SET);
 printf("Splitting File: File #2=> %s\n", comname3);
 o=0;
 for (i=0; i<file_size/2; i++){
	o++;
	fread(readchunk, sizeof(readchunk), 1, file_ptr);
	fwrite(readchunk, sizeof(readchunk), 1, file_ptr3);
 }
}
else if (noheader==1) {
	file_size=file_size/8192;
	fseek(file_ptr, 0, SEEK_SET);
	o=0;
	for (i=0; i<file_size/4; i++){
		o++;
		fread(readchunk, sizeof(readchunk), 1, file_ptr);
		fwrite(readchunk, sizeof(readchunk), 1, file_ptr2);
	 }
	 fseek(file_ptr, (file_size*8192)/2, SEEK_SET);
	 o=0;
	 for (i=0; i<file_size/4; i++){
		o++;
		if ((o%(file_size/meter))==0){
			for (j=0; j<metersize; j++)
                                printf("Û");
		}
		fread(readchunk, sizeof(readchunk), 1, file_ptr);
		fwrite(readchunk, sizeof(readchunk), 1, file_ptr2);
	 }
         printf("Splitting File: File #2=> %s\n", comname3);
	fseek(file_ptr, (file_size*8192)/4, SEEK_SET);
	o=0;
	for (i=0; i<file_size/4; i++){
		o++;
		fread(readchunk, sizeof(readchunk), 1, file_ptr);
		fwrite(readchunk, sizeof(readchunk), 1, file_ptr3);
	 }
	 fseek(file_ptr, (((file_size*8192)/4)*3), SEEK_SET);
	 o=0;
	 for (i=0; i<file_size/4; i++){
		o++;
		fread(readchunk, sizeof(readchunk), 1, file_ptr);
		fwrite(readchunk, sizeof(readchunk), 1, file_ptr3);
	 }
 }

 printf("Image Split.\n");
}

void mghsconvert()
{
 unsigned long   i;
 FILE   *file_ptr;
 FILE   *file_ptr2;
 char   header[512];
 unsigned long file_size;
 int    j;
 char   *name;
 int    checklen=0, errorchk;
 int    meter, metersize, o;


 if ((comnumber==3) && (k==0))
	{
	strcpy(comname2, comname1);
	for (i=0; i<6; i++)
		comname2[i+2]=comname1[i];
	comname2[0]='m';
	comname2[1]='d';
	if ((name=strchr(comname2, '.'))==NULL)
		{
		checklen=strlen(comname2);
		comname2[checklen]='.';
		comname2[checklen+1]='0';
		comname2[checklen+2]='0';
		comname2[checklen+3]='8';
		comname2[checklen+4]='\0';
		}
	 else if((name=strchr(comname2, '.'))!=NULL)
		{
		name++;
		*name++='0';
		*name++='0';
		*name++='8';
		*name++='\0';
		}
	}

 if ((comnumber==3) && (k==1))
	{
	strcpy(comname2, comname1);
	if ((name=strchr(comname2, '.'))==NULL)
		{
		checklen=strlen(comname2);
		comname2[checklen]='.';
		comname2[checklen+1]='s';
		comname2[checklen+2]='m';
		comname2[checklen+3]='d';
		comname2[checklen+4]='\0';
		}
	 else if((name=strchr(comname2, '.'))!=NULL)
		{
		name++;
		*name++='s';
		*name++='m';
		*name++='d';
		*name++='\0';
		}
	}

 checklen=strlen(comname2);
 for (j=0; j<checklen; j++)
	if (comname1[j]!=comname2[j])
		errorchk=-1;

 if (errorchk==0)
	{
	printf("uCON Error:  The input and output filenames are the same!\n");
	exit(8);
	}

 file_ptr=fopen(comname1, "rb");
 chkiffileexist(file_ptr, comname1);
 file_ptr2=fopen(comname2, "w+b");
 fseek(file_ptr, 0, SEEK_END);
 file_size=ftell(file_ptr);
 fseek(file_ptr, 0, SEEK_SET);
 file_size-=ftell(file_ptr);

 if (k==0)
	{
	meter=(file_size-512)/131072;
	metersize=64/meter;
	file_size=(file_size-512)/8192;
        printf("Converting SMD->MGD2 Format");
	o=0;
	for (i=0; i<file_size; i=i+2)
		{
		o++;
		fseek(file_ptr, i*8192+512, SEEK_SET);
		fread(readchunk, sizeof(readchunk), 1, file_ptr);
		fwrite(readchunk, sizeof(readchunk), 1, file_ptr2);
		}
	for (i=1; i<file_size; i=i+2)
		{
		o++;
		fseek(file_ptr, i*8192+512, SEEK_SET);
		fread(readchunk, sizeof(readchunk), 1, file_ptr);
		fwrite(readchunk, sizeof(readchunk), 1, file_ptr2);
		}
	}

 if (k==1)
	{
	meter=file_size/131072;
	metersize=64/meter;
	memset(header, 0, sizeof(header));
	header[0]=((file_size/8192)/2);
	header[1]=0x03;
	header[8]=0xaa;
	header[9]=0xbb;
	header[10]=0x06;
	fwrite(header, sizeof(header), 1, file_ptr2);
	file_size=((file_size)/8192)/2;
        printf("Converting MGD2->SMD Format");
	o=0;
	for (i=0; i<file_size; i++)
		{
		o++;
		if ((o%(file_size/meter))==0)
			for (j=0; j<metersize; j++)
                                printf("Û");
		fseek(file_ptr, i*8192, SEEK_SET);
		fread(readchunk, sizeof(readchunk), 1, file_ptr);
		fwrite(readchunk, sizeof(readchunk), 1, file_ptr2);
		fseek(file_ptr, i*8192+((file_size*8192*2)/2), SEEK_SET);
		fread(readchunk, sizeof(readchunk), 1, file_ptr);
		fwrite(readchunk, sizeof(readchunk), 1, file_ptr2);
		}
	}

 printf("Image Converted.\n");
}

void mghsegap()
{
 int    i;
 FILE   *file_ptr;
 FILE   *file_ptr2;
 char   inchar[512];
 char   onechar[1];
 int    size;
 int    last;
 int    first;
 int    olast;
 int    ofirst;
 unsigned long file_size;
 unsigned long add_size;

 if (comnumber==4)
	 file_ptr2=fopen(comname2, "w+b");

 file_ptr=fopen(comname1, "a+b");
 chkiffileexist(file_ptr, comname1);
 
 memset(inchar, char(255), sizeof(inchar));
 onechar[0]=char(255);

 fseek(file_ptr, 0, SEEK_END);
 file_size=ftell(file_ptr);
 fseek(file_ptr, 0, SEEK_SET);
 file_size-=ftell(file_ptr);
 printf("Padding File...\n");
 size=int((file_size-512)/131072);
 add_size=(131072*(size+1)+512)-file_size;
 if ((add_size-131072)!=0 && comnumber==3) {
     printf("[to %d Mbit] ", size+1);
     last=add_size%512;
     first=int(add_size/512);
     for (i=0; i<first; i++){
		fwrite(inchar, sizeof(inchar), 1, file_ptr);
     }
     for (i=0; i<last; i++){
		fwrite(onechar, sizeof(onechar), 1, file_ptr);
     }
 }
 else if ((add_size-131072)!=0 && comnumber==4) {
     last=add_size%512;
     first=int(add_size/512);
     olast=file_size%512;
     ofirst=int(file_size/512);
     printf("[to %d Mbit] ", size+1);
     for (i=0; i<ofirst; i++) {
		fread(inchar, sizeof(inchar), 1, file_ptr);
		fwrite(inchar, sizeof(inchar), 1, file_ptr2);
     }
     for (i=0; i<olast; i++) {
		fread(onechar, sizeof(onechar), 1, file_ptr);
		fwrite(onechar, sizeof(onechar), 1, file_ptr2);
     }
     for (i=0; i<first; i++){
		fwrite(inchar, sizeof(inchar), 1, file_ptr2);
     }
     for (i=0; i<last; i++){
		fwrite(onechar, sizeof(onechar), 1, file_ptr2);
     }
 }

 printf("Image Padded.\n");
}

void mghsegaheader()
{
 FILE   *file_ptr;
 char   header[512];
 unsigned long file_size;
 int    split=0;

 if (k==1){
	printf("uCON Error:  Image is not in Super Magic Drive (SMD) format.\n");
	exit(8);
 }

 file_ptr=fopen(comname1, "r+b");
 chkiffileexist(file_ptr, comname1);

 if (strcmp(comname2, "/m")==0) split=1;

 memset(header, 0, sizeof(header));
 fseek(file_ptr, 0, SEEK_END);
 file_size=ftell(file_ptr);
 fseek(file_ptr, 0, SEEK_SET);
 file_size-=ftell(file_ptr);

 header[0]=(((file_size-512)/8192)/2);
 header[1]=0x03;
 
 header[8]=0xaa;
 header[9]=0xbb;
 header[10]=0x06;
 if (split==1) header[2]=char(0x40);

 printf("Rewriting Header...\n");
 rewind(file_ptr);
 fseek(file_ptr, 0, SEEK_SET);
 fwrite(header, sizeof(header), 1, file_ptr);
 printf("Header Rewritten.");
 printf("\n");
}

void mghswtospf()
{
 FILE   *file_ptr;
 char   flag[1];
 int    optype;

 if (k!=0)
	{
	 printf("Image is not in FIG or SWC Format.\n");
	 exit(8);
	}

 file_ptr=fopen(comname1, "r+b");
 chkiffileexist(file_ptr, comname1);

 fseek(file_ptr, 2, SEEK_SET);
 fread(flag, sizeof(flag), 1, file_ptr);
 if (flag[0]!=char(0))
	{
	optype=0;
        printf("Converting SWC->FIG Format...\n");

	flag[0]=0;
	fseek(file_ptr, 0x02, SEEK_SET);
	fwrite(flag, sizeof(flag), 1, file_ptr);
	fseek(file_ptr, 0x08, SEEK_SET);
	fwrite(flag, sizeof(flag), 1, file_ptr);
	fseek(file_ptr, 0x09, SEEK_SET);
	fwrite(flag, sizeof(flag), 1, file_ptr);
	fseek(file_ptr, 0x0a, SEEK_SET);
	fwrite(flag, sizeof(flag), 1, file_ptr);

	if (h==1)
		{
		flag[0]=0x80;
		fseek(file_ptr, 3, SEEK_SET);
		fwrite(flag, sizeof(flag), 1, file_ptr);
		}
	if (ramsize==0)
		{
		flag[0]=0x77;
		fseek(file_ptr, 4, SEEK_SET);
		fwrite(flag, sizeof(flag), 1, file_ptr);
		flag[0]=0x83;
		fseek(file_ptr, 5, SEEK_SET);
		fwrite(flag, sizeof(flag), 1, file_ptr);
		}
	if ((romindex==3) || (romindex==4) || (romindex==5))
		{
		if (ramsize==0)
			{
			flag[0]=0x47;
			fseek(file_ptr, 4, SEEK_SET);
			fwrite(flag, sizeof(flag), 1, file_ptr);
			flag[0]=0x83;
			fseek(file_ptr, 5, SEEK_SET);
			fwrite(flag, sizeof(flag), 1, file_ptr);
			}
		if (ramsize>0)
			{
			flag[0]=0xfd;
			fseek(file_ptr, 4, SEEK_SET);
			fwrite(flag, sizeof(flag), 1, file_ptr);
			flag[0]=0x82;
			fseek(file_ptr, 5, SEEK_SET);
			fwrite(flag, sizeof(flag), 1, file_ptr);
			}
		}
	}
 else if (flag[0]==char(0))
	{
	optype=1;
        printf("Converting FIG->SWC Format...\n");

	flag[0]=0x20;
	if (h==1)
		flag[0]=0x30;
	fseek(file_ptr, 2, SEEK_SET);
	fwrite(flag, sizeof(flag), 1, file_ptr);
	flag[0]=0;
	fseek(file_ptr, 3, SEEK_SET);
	fwrite(flag, sizeof(flag), 1, file_ptr);
	fwrite(flag, sizeof(flag), 1, file_ptr);
	fwrite(flag, sizeof(flag), 1, file_ptr);

	flag[0]=0xaa;
	fseek(file_ptr, 0x08, SEEK_SET);
	fwrite(flag, sizeof(flag), 1, file_ptr);
	flag[0]=0xbb;
	fseek(file_ptr, 0x09, SEEK_SET);
	fwrite(flag, sizeof(flag), 1, file_ptr);
	flag[0]=0x04;
	fseek(file_ptr, 0x0a, SEEK_SET);
	fwrite(flag, sizeof(flag), 1, file_ptr);
	}

 if (optype==0)
        printf("SWC->FIG Completed.");
 else if (optype==1)
        printf("FIG->SWC Completed.");
 printf("\n");
}

int segaio(char *outstring, char *instring, int testbyte, int firstflag)
{
 switch(instring[testbyte])
	{
	   case 'J': if (firstflag==1)
                        strcat(outstring, ", ");
		     if (firstflag==0)
			firstflag=1;
		     strcat(outstring, "3 Button Pad");
		     break;
	   case '4': if (firstflag==1)
                        strcat(outstring, ", ");
		     if (firstflag==0)
			firstflag=1;
		     strcat(outstring, "Team Player");
		     break;
	   case '6': if (firstflag==1)
                        strcat(outstring, ", ");
		     if (firstflag==0)
			firstflag=1;
		     strcat(outstring, "6 Button Pad");
		     break;
	   case 'L': if (firstflag==1)
                        strcat(outstring, ", ");
		     if (firstflag==0)
			firstflag=1;
		     strcat(outstring, "Activator");
		     break;
	   case 'M': if (firstflag==1)
                        strcat(outstring, ", ");
		     if (firstflag==0)
			firstflag=1;
		     strcat(outstring, "Mega Mouse");
		     break;
	   default:  break;
	 }
 return(firstflag);
}

int segacountry(char *outstring, char *instring, int testbyte, int firstflag)
{
 switch(instring[testbyte])
	{
		case 'J': if (firstflag==1)
                             strcat(outstring, ", ");
			  if (firstflag==0)
			     firstflag=1;
			  strcat(outstring, "Japan");
			  break;
		case 'U': if (firstflag==1)
                             strcat(outstring, ", ");
			  if (firstflag==0)
			     firstflag=1;
			  strcat(outstring, "U.S.A.");
			  break;
		case 'E': if (firstflag==1)
                             strcat(outstring, ", ");
			  if (firstflag==0)
			     firstflag=1;
			  strcat(outstring, "Europe");
			  break;
		case 'B': if (firstflag==1)
                             strcat(outstring, ", ");
			  if (firstflag==0)
			     firstflag=1;
			  strcat(outstring, "Brazil");
			  break;
		case 'A': if (firstflag==1)
                             strcat(outstring, ", ");
			  if (firstflag==0)
			     firstflag=1;
			  strcat(outstring, "Asia");
			  break;
		default:  break;
	 }
 return(firstflag);
}

void getsinfo()
{
  float filesize;
  unsigned long file_size;
  FILE  *file_ptr;
  char  testname[13];
  char  oddchar[128];
  char  evenchar[128];
  char  scrstring[48], scrstring2[48], scrstring3[48];
  char  syss[2][16];
  char  whichsys[16];
  char  inchar[1];
  char  ram[20];
  char  softtype[2][15];
  float subsize=512.0;
  char  format[2][25];
  int   i,j,softtypein=0;
  int   cartsize;
  int   sizeh, sizem, sizel;
  int   numfile=1;
  int   nextfile=1;
  char  *name;
  int   attrib;
  int   split=0, match=0, errorchk;

 strcpy(format[0], "Super Magic Drive (SMD)");
 strcpy(format[1], "Multi Game Doctor II");
 strcpy(syss[0], "Sega Genesis");
 strcpy(syss[1], "Sega Mega Drive");
 strcpy(softtype[0], "Game");
 strcpy(softtype[1], "Application");

 file_ptr=fopen(comname1, "rb");
 fseek(file_ptr, 0, SEEK_END);
 file_size=ftell(file_ptr);
 fseek(file_ptr, 0, SEEK_SET);
 file_size-=ftell(file_ptr);
 attrib=O_RDWR|O_RDONLY;
 checkminsize(file_size);


 if ((file_size%131072)==0)
	z=1;
 if (z==0)
	{
	fseek(file_ptr, 0x80, SEEK_SET);
	fread(inchar, sizeof(inchar), 1, file_ptr);
	if ((errorchk=inchar[0])<0)
		errorchk=errorchk+256;
	if (errorchk==0x45)
		{
		fread(inchar, sizeof(inchar), 1, file_ptr);
		if ((errorchk=inchar[0])<0)
			errorchk=errorchk+256;
		if (errorchk==0x41)
			{
			fseek(file_ptr, ((file_size/2)+0x80), SEEK_SET);
			fread(inchar, sizeof(inchar), 1, file_ptr);
			if ((errorchk=inchar[0])<0)
				errorchk=errorchk+256;
			if (errorchk==0x53)
				{
				fread(inchar, sizeof(inchar), 1, file_ptr);
				if ((errorchk=inchar[0])<0)
					errorchk=errorchk+256;
				if (errorchk==0x47)
					z=1;
				}
			}
		}
	}


 strcpy(testname, comname1);

 if ((z==0) && ((name=strchr(testname, '.'))!=NULL)) {
	 while(nextfile==1){
		strcpy(testname, comname1);
		if((name=strchr(testname, '.'))!=NULL) {
			name++;
			*name++=char(49+numfile);
		 }
		split=1;
		numfile++;
         nextfile=0;
	}
 }
 strcpy(testname, comname1);

 if ((z==1) && ((name=strchr(testname, '.'))!=NULL) && (testname[7]=='a')){
	 while(nextfile==1){
		strcpy(testname, comname1);
		if((name=strchr(testname, '.'))!=NULL) {
			name--;
			*name=char(97+numfile);
		 }
		split=1;
		numfile++;
		nextfile=0;
	}
 }

 if (z==0)
	{
	k=0;
	subsize=512.0;
	filesize=(file_size-int(subsize))/131072.0;

// ;        cartsize=int((file_size-int(subsize))/131072);

	fseek(file_ptr, 640, SEEK_SET);
	fread(oddchar, sizeof(oddchar), 1, file_ptr);
	fseek(file_ptr, 8832, SEEK_SET);
	fread(evenchar, sizeof(evenchar), 1, file_ptr);
	}

 if (z==1)
	{
	k=1;
	subsize=0.0;
	filesize=(file_size-int(subsize))/131072.0;

	fseek(file_ptr, 128, SEEK_SET);
	fread(oddchar, sizeof(oddchar), 1, file_ptr);
	fseek(file_ptr, (file_size/2)+128, SEEK_SET);
	fread(evenchar, sizeof(evenchar), 1, file_ptr);
	}

 fseek(file_ptr, 0x02d2, SEEK_SET);
 if (z==1)
	fseek(file_ptr, 0x00d2, SEEK_SET);
 fread(inchar, sizeof(inchar), 1, file_ptr);
 if ((sizeh=inchar[0])<0)
	sizeh=sizeh+256;
 fread(inchar, sizeof(inchar), 1, file_ptr);
 if ((sizel=inchar[0])<0)
	sizel=sizel+256;
 fseek(file_ptr, 0x22d3, SEEK_SET);
 if (z==1)
	fseek(file_ptr, (file_size/2)+0x00d3, SEEK_SET);
 fread(inchar, sizeof(inchar), 1, file_ptr);
 if ((sizem=inchar[0])<0)
	sizem=sizem+256;
 if ((sizem==255) && (sizel==255))
	cartsize=(sizeh+1)/2;
 else
	cartsize=int((file_size-int(subsize))/131072);

 if (oddchar[2]=='G')
	strcpy(whichsys, syss[0]);
 else
	strcpy(whichsys, syss[1]);
 if (evenchar[64]=='G')
	if (oddchar[64]=='M')
		softtypein=0;
 else
	softtypein=1;

 printf("\nuCON %s", uconver);
 printf("\t\"What Every Console User Needs\" - By Chicken & chp\n");
 printf("\t\t\t\t\t\t\t Unix port: splice\n");

 printf("Filename      : %s\n", comname1);
 printf("Console       : %s\n", whichsys);

	 j=0; 
	 for (i=16; i<66; i=i+2) {
		scrstring[(i+2)-17]=oddchar[i-j];
		j++;
	 }
	 j=0;
	 for (i=16; i<66; i=i+2) {
		scrstring[(i+1)-17]=evenchar[i-j];
		j++;
	 }
         scrstring[sizeof(scrstring)]='\0';

 printf("Japanese Name : %s\n", scrstring);
	 j=0;

         strncat(scrstring, " ", 1);
	 for (i=40; i<89; i=i+2) {
		scrstring[(i+2)-41]=oddchar[i-j];
		j++;
	 }
	 j=0;
	 for (i=40; i<89; i=i+2) {
		scrstring[(i+1)-41]=evenchar[i-j];
		j++;
	 }

        scrstring[sizeof(scrstring)]='\0';

// The following code copies the American Name for later use
// when creating a MGH Name File.
 for (i=0; i<15; i++)
	seganame[i]=scrstring[i];

        scrstring[sizeof(scrstring)]='\0';

 printf("American Name : %s\n", scrstring);

        scrstring2[0]=':';
        scrstring2[1]=' ';

	i=0;
	 i=segacountry(scrstring2, evenchar, 120, i);
	 i=segacountry(scrstring2, oddchar, 120, i);
	 i=segacountry(scrstring2, evenchar, 121, i);
	 i=segacountry(scrstring2, oddchar, 121, i);
	 i=segacountry(scrstring2, evenchar, 122, i);

 printf("For Use In    %s\n", scrstring2);

 scrstring[0]=evenchar[70];
 scrstring[1]=oddchar[70];
 scrstring[2]='\0';

 printf("Revision      : 1.%s\n", scrstring);

	  strncat(scrstring, " ", 1);
         j=0;
	 for (i=8; i<15; i=i+2) {
		 scrstring[(i+2)-9]=oddchar[i-j];
		 j++;
	 }
	 j=0;
	 for (i=8; i<15; i=i+2) {
		 scrstring[(i+1)-9]=evenchar[i-j];
		 j++;
	 }

	 scrstring[8]='\0';

// (C)ACLD_
 if (match==0)
	{
		if (scrstring[3]=='A')
			if ((scrstring[4]=='C') && (scrstring[5]=='L') && (scrstring[6]=='D') && (scrstring[7]==' '))
				{
				match=1;
				strcpy(scrstring, "(C)Accolade ");
				}
	}

// (C)VRGN_
 if (match==0)
	{
		if (scrstring[3]=='V')
			if ((scrstring[4]=='R') && (scrstring[5]=='G') && (scrstring[6]=='N') && (scrstring[7]==' '))
				{
				match=1;
				strcpy(scrstring, "(C)Virgin Games ");
				}
	}

// (C)WADN_
 if (match==0)
	{
		if (scrstring[3]=='W')
			if ((scrstring[4]=='A') && (scrstring[5]=='D') && (scrstring[6]=='N') && (scrstring[7]==' '))
				{
				match=1;
				strcpy(scrstring, "(C)Parker Brothers ");
				}
	}

// (C)WSTN_
 if (match==0)
	{
		if (scrstring[3]=='W')
			if ((scrstring[4]=='S') && (scrstring[5]=='T') && (scrstring[6]=='N') && (scrstring[7]==' '))
				{
				match=1;
				strcpy(scrstring, "(C)Westone ");
				}
	}

// (C)T-10_
 if (match==0)
	{
		if (scrstring[3]=='T')
			if ((scrstring[4]=='-') && (scrstring[5]=='1') && (scrstring[6]=='0') && (scrstring[7]==' '))
				{
				match=1;
				strcpy(scrstring, "(C)Takara ");
				}
	}

// (C)T-11_
 if (match==0)
	{
		if (scrstring[3]=='T')
			if ((scrstring[4]=='-') && (scrstring[5]=='1') && (scrstring[6]=='1') && (scrstring[7]==' '))
				{
				match=1;
				strcpy(scrstring, "(C)Taito ");
				}
	}

// (C)T-12_
 if (match==0)
	{
		if (scrstring[3]=='T')
			if ((scrstring[4]=='-') && (scrstring[5]=='1') && (scrstring[6]=='2') && (scrstring[7]==' '))
				{
				match=1;
				strcpy(scrstring, "(C)Capcom ");
				}
	}

// (C)T-13_
 if (match==0)
	{
		if (scrstring[3]=='T')
			if ((scrstring[4]=='-') && (scrstring[5]=='1') && (scrstring[6]=='3') && (scrstring[7]==' '))
				{
				match=1;
				strcpy(scrstring, "(C)Data East ");
				}
	}

// (C)T-14_
 if (match==0)
	{
		if (scrstring[3]=='T')
			if ((scrstring[4]=='-') && (scrstring[5]=='1') && (scrstring[6]=='4') && (scrstring[7]==' '))
				{
				match=1;
				strcpy(scrstring, "(C)Namco ");
				}
	}

// (C)T-15_
 if (match==0)
	{
		if (scrstring[3]=='T')
			if ((scrstring[4]=='-') && (scrstring[5]=='1') && (scrstring[6]=='5') && (scrstring[7]==' '))
				{
				match=1;
				strcpy(scrstring, "(C)Sunsoft ");
				}
	}

// (C)T-22_
 if (match==0)
	{
		if (scrstring[3]=='T')
			if ((scrstring[4]=='-') && (scrstring[5]=='2') && (scrstring[6]=='2') && (scrstring[7]==' '))
				{
				match=1;
				strcpy(scrstring, "(C)Extreme ");
				}
	}

// (C)T-23_
 if (match==0)
	{
		if (scrstring[3]=='T')
			if ((scrstring[4]=='-') && (scrstring[5]=='2') && (scrstring[6]=='3') && (scrstring[7]==' '))
				{
				match=1;
				strcpy(scrstring, "(C)Vic Tokai ");
				}
	}

// (C)T-33_
 if (match==0)
	{
		if (scrstring[3]=='T')
			if ((scrstring[4]=='-') && (scrstring[5]=='3') && (scrstring[6]=='3') && (scrstring[7]==' '))
				{
				match=1;
				strcpy(scrstring, "(C)Kaneko ");
				}
	}

// (C)T-36_
 if (match==0)
	{
		if (scrstring[3]=='T')
			if ((scrstring[4]=='-') && (scrstring[5]=='3') && (scrstring[6]=='6') && (scrstring[7]==' '))
				{
				match=1;
				strcpy(scrstring, "(C)Tecmo ");
				}
	}

// (C)T-45_
 if (match==0)
	{
		if (scrstring[3]=='T')
			if ((scrstring[4]=='-') && (scrstring[5]=='4') && (scrstring[6]=='5') && (scrstring[7]==' '))
				{
				match=1;
				strcpy(scrstring, "(C)Game Arts ");
				}
	}

// (C)T-48_
 if (match==0)
	{
		if (scrstring[3]=='T')
			if ((scrstring[4]=='-') && (scrstring[5]=='4') && (scrstring[6]=='8') && (scrstring[7]==' '))
				{
				match=1;
				strcpy(scrstring, "(C)Tengen ");
				}
	}

// (C)T-50_
 if (match==0)
	{
		if (scrstring[3]=='T')
			if ((scrstring[4]=='-') && (scrstring[5]=='5') && (scrstring[6]=='0') && (scrstring[7]==' '))
				{
				match=1;
				strcpy(scrstring, "(C)Electronic Arts ");
				}
	}

// (C)T-55_
 if (match==0)
	{
		if (scrstring[3]=='T')
			if ((scrstring[4]=='-') && (scrstring[5]=='5') && (scrstring[6]=='5') && (scrstring[7]==' '))
				{
				match=1;
				strcpy(scrstring, "(C)Psygnosis ");
				}
	}

// (C)T-60_
 if (match==0)
	{
		if (scrstring[3]=='T')
			if ((scrstring[4]=='-') && (scrstring[5]=='6') && (scrstring[6]=='0') && (scrstring[7]==' '))
				{
				match=1;
				strcpy(scrstring, "(C)JVC ");
				}
	}

// (C)T-68_
 if (match==0)
	{
		if (scrstring[3]=='T')
			if ((scrstring[4]=='-') && (scrstring[5]=='6') && (scrstring[6]=='8') && (scrstring[7]==' '))
				{
				match=1;
				strcpy(scrstring, "(C)CRI ");
				}
	}

// (C)T-70_
 if (match==0)
	{
		if (scrstring[3]=='T')
			if ((scrstring[4]=='-') && (scrstring[5]=='7') && (scrstring[6]=='0') && (scrstring[7]==' '))
				{
				match=1;
				strcpy(scrstring, "(C)Virgin Games ");
				}
	}

// (C)T-76_
 if (match==0)
	{
		if (scrstring[3]=='T')
			if ((scrstring[4]=='-') && (scrstring[5]=='7') && (scrstring[6]=='6') && (scrstring[7]==' '))
				{
				match=1;
				strcpy(scrstring, "(C)KOEI ");
				}
	}

// (C)T-79_
 if (match==0)
	{
		if (scrstring[3]=='T')
			if ((scrstring[4]=='-') && (scrstring[5]=='7') && (scrstring[6]=='9') && (scrstring[7]==' '))
				{
				match=1;
				strcpy(scrstring, "(C)U.S. Gold ");
				}
	}

// (C)T-81_
 if (match==0)
	{
		if (scrstring[3]=='T')
			if ((scrstring[4]=='-') && (scrstring[5]=='8') && (scrstring[6]=='1') && (scrstring[7]==' '))
				{
				match=1;
				strcpy(scrstring, "(C)Acclaim ");
				}
	}

// (C)T-83_
 if (match==0)
	{
		if (scrstring[3]=='T')
			if ((scrstring[4]=='-') && (scrstring[5]=='8') && (scrstring[6]=='3') && (scrstring[7]==' '))
				{
				match=1;
				strcpy(scrstring, "(C)Gametek ");
				}
	}

// (C)T-86_
 if (match==0)
	{
		if (scrstring[3]=='T')
			if ((scrstring[4]=='-') && (scrstring[5]=='8') && (scrstring[6]=='6') && (scrstring[7]==' '))
				{
				match=1;
				strcpy(scrstring, "(C)Absolute ");
				}
	}

// (C)T-87_
 if (match==0)
	{
		if (scrstring[3]=='T')
			if ((scrstring[4]=='-') && (scrstring[5]=='8') && (scrstring[6]=='7') && (scrstring[7]==' '))
				{
				match=1;
				strcpy(scrstring, "(C)Mindscape ");
				}
	}

// (C)T-88_
 if (match==0)
	{
		if (scrstring[3]=='T')
			if ((scrstring[4]=='-') && (scrstring[5]=='8') && (scrstring[6]=='8') && (scrstring[7]==' '))
				{
				match=1;
				strcpy(scrstring, "(C)Domark ");
				}
	}

// (C)T-93_
 if (match==0)
	{
		if (scrstring[3]=='T')
			if ((scrstring[4]=='-') && (scrstring[5]=='9') && (scrstring[6]=='3') && (scrstring[7]==' '))
				{
				match=1;
				strcpy(scrstring, "(C)Sony Imagesoft ");
				}
	}

// (C)T-94_
 if (match==0)
	{
		if (scrstring[3]=='T')
			if ((scrstring[4]=='-') && (scrstring[5]=='9') && (scrstring[6]=='4') && (scrstring[7]==' '))
				{
				match=1;
				strcpy(scrstring, "(C)Sony Imagesoft ");
				}
	}

// (C)T-95_
 if (match==0)
	{
		if (scrstring[3]=='T')
			if ((scrstring[4]=='-') && (scrstring[5]=='9') && (scrstring[6]=='5') && (scrstring[7]==' '))
				{
				match=1;
				strcpy(scrstring, "(C)Konami ");
				}
	}

// (C)T-97_
 if (match==0)
	{
		if (scrstring[3]=='T')
			if ((scrstring[4]=='-') && (scrstring[5]=='9') && (scrstring[6]=='7') && (scrstring[7]==' '))
				{
				match=1;
				strcpy(scrstring, "(C)Tradewest ");
				}
	}

// (C)T-99_
 if (match==0)
	{
		if (scrstring[3]=='T')
			if ((scrstring[4]=='-') && (scrstring[5]=='9') && (scrstring[6]=='9') && (scrstring[7]==' '))
				{
				match=1;
				strcpy(scrstring, "(C)Codemasters ");
				}
	}

// (C)T-100
 if (match==0)
	{
		if (scrstring[3]=='T')
			if ((scrstring[4]=='-') && (scrstring[5]=='1') && (scrstring[6]=='0') && (scrstring[7]=='0'))
				{
				match=1;
				strcpy(scrstring, "(C)T*HQ ");
				}
	}

// (C)T-101
 if (match==0)
	{
		if (scrstring[3]=='T')
			if ((scrstring[4]=='-') && (scrstring[5]=='1') && (scrstring[6]=='0') && (scrstring[7]=='1'))
				{
				match=1;
				strcpy(scrstring, "(C)TecMagik ");
				}
	}

// (C)T-103
 if (match==0)
	{
		if (scrstring[3]=='T')
			if ((scrstring[4]=='-') && (scrstring[5]=='1') && (scrstring[6]=='0') && (scrstring[7]=='3'))
				{
				match=1;
				strcpy(scrstring, "(C)Takara ");
				}
	}

// (C)T-112
 if (match==0)
	{
		if (scrstring[3]=='T')
			if ((scrstring[4]=='1') && (scrstring[5]=='1') && (scrstring[6]=='2'))
				{
				match=1;
				strcpy(scrstring, "(C)Hi Tech Entertainment ");
				}
	}

// (C)T-113
 if (match==0)
	{
		if (scrstring[3]=='T')
			if ((scrstring[4]=='-') && (scrstring[5]=='1') && (scrstring[6]=='1') && (scrstring[7]=='3'))
				{
				match=1;
				strcpy(scrstring, "(C)Psygnosis ");
				}
	}

// (C)T-119
 if (match==0)
	{
		if (scrstring[3]=='T')
			if ((scrstring[4]=='-') && (scrstring[5]=='1') && (scrstring[6]=='1') && (scrstring[7]=='9'))
				{
				match=1;
				strcpy(scrstring, "(C)Accolade ");
				}
	}

// (C)T-124
 if (match==0)
	{
		if (scrstring[3]=='T')
			if ((scrstring[4]=='-') && (scrstring[5]=='1') && (scrstring[6]=='2') && (scrstring[7]=='4'))
				{
				match=1;
				strcpy(scrstring, "(C)Spectrum HoloByte ");
				}
	}

// (C)T-125
 if (match==0)
	{
		if (scrstring[3]=='T')
			if ((scrstring[4]=='-') && (scrstring[5]=='1') && (scrstring[6]=='2') && (scrstring[7]=='5'))
				{
				match=1;
				strcpy(scrstring, "(C)Interplay ");
				}
	}

 printf("Copyright     : %s", scrstring);

	  strncat(scrstring, " ", 1);

         j=0;
	 for (i=12; i<19; i=i+2) {
		scrstring[(i+2)-13]=oddchar[i-j];
		j++;
	 }
	 j=0;
	 for (i=12; i<19; i=i+2) {
		scrstring[(i+1)-13]=evenchar[i-j];
		j++;
	 }

scrstring[8]='\0';

         printf("%s\n", scrstring);
         strncat(scrstring, " ", 1);

	 j=0;
	 for (i=65; i<73; i=i+2) {
		scrstring[(i+1)-66]=evenchar[i-j];
		j++;
	 }
	 j=0;
	 for (i=65; i<73; i=i+2) {
		scrstring[(i+2)-66]=oddchar[i-j];
		j++;
	 }

 printf("Product Code  :%s\n", scrstring);

	 i=0;
	 i=segaio(scrstring3, evenchar, 72, i);
	 i=segaio(scrstring3, oddchar, 72, i);
	 i=segaio(scrstring3, evenchar, 73, i);
	 i=segaio(scrstring3, oddchar, 73, i);

 printf("I/O Support   : %s\n", scrstring3);

 if (k==0)
	{
	fseek(file_ptr, 8920, SEEK_SET);
	fread(inchar, sizeof(inchar), 1, file_ptr);
	if (inchar[0]=='R')
		{
		fseek(file_ptr, 728, SEEK_SET);
		fread(inchar, sizeof(inchar), 1, file_ptr);
		if (inchar[0]=='A')
			strcpy(ram, "Backup RAM");
		}
	 else
	strcpy(ram, "None");
       }

 if (k==1)
	{
	fseek(file_ptr, (file_size/2)+0xd8, SEEK_SET);
	fread(inchar, sizeof(inchar), 1, file_ptr);
	if (inchar[0]=='R')
		{
		fseek(file_ptr, 0xd8, SEEK_SET);
		fread(inchar, sizeof(inchar), 1, file_ptr);
		if (inchar[0]=='A')
			strcpy(ram, "Backup RAM");
		}
	 else
		strcpy(ram, "None");
	 }

 printf("Product Type  : %s\n", softtype[softtypein]);
 printf("ROM Size      : %d Mbit\n", cartsize);
 printf("RAM Type      : %s\n", ram);
 printf("File Size     : %4.2f Mbit\n", filesize);
 printf("Image Format  : %s", format[k]);

 if (split==1)
        printf(" [Multi File]\n");
 else
        printf("\n");

 printf("Intro/Trainer : ");
 if (filesize>cartsize)
	 if((filesize-cartsize)!=0.0)
		{
                printf("Yes, %4.0f bytes\n", ((filesize*131072)-(cartsize*131072)) );
		}
	 else
		{
                printf("No\n");
		}
 else
	{
        printf("No\n");
	}

 printf("\n");
}

void mghbaseline()
{
 FILE   *file_ptr;
 FILE   *file_ptr2;
 char   inchar[1], addstr[10], datstr[10], inchar2[4096];
 long   numdat, i;
 long   done=0, add;
 int    dat;

 file_ptr=fopen(comname1, "r+b");
 chkiffileexist(file_ptr, comname1);

 file_ptr2=fopen(comname2, "rb");
 chkiffileexist(file_ptr2, comname2);

 printf("Applying Baseline patch...\n");

 while (done==0)
	{
	memset(addstr, ' ', sizeof(addstr));
        fscanf(file_ptr2, "%[-1234567890]\n", addstr);
	fread(inchar, sizeof(inchar), 1, file_ptr2);

	add=atol(addstr);

	memset(datstr, ' ', sizeof(datstr));
        fscanf(file_ptr2, "%[-1234567890]\n", addstr);
	fread(inchar, sizeof(inchar), 1, file_ptr2);

	dat=atoi(datstr);
	inchar[0]=dat;

	if ((add==-1) && (dat==-1))
		done=1;

	if (done==0)
		{
                printf("(Offset:  %lX)\n", add);

		fseek(file_ptr, add, SEEK_SET);
		fwrite(inchar, sizeof(inchar), 1, file_ptr);
		}
	}

	memset(addstr, ' ', sizeof(addstr));
        fscanf(file_ptr2, "%[-1234567890]\n", addstr);
	fread(inchar, sizeof(inchar), 1, file_ptr2);

	add=atol(addstr);

	memset(datstr, ' ', sizeof(datstr));
        fscanf(file_ptr2, "%[-1234567890]\n", addstr);
	fread(inchar, sizeof(inchar), 1, file_ptr2);

	numdat=atol(datstr);
	fseek(file_ptr, add, SEEK_SET);

	if (numdat>0)
		{
		while (numdat>4096)
			{
                        printf("(Offset:  %lX)\n", add);
			fread(inchar2, sizeof(inchar2), 1, file_ptr2);
			fwrite(inchar2, sizeof(inchar2), 1, file_ptr);
			numdat=numdat-4096;
			add=add+4096;
			}
		for (i=0; i<(numdat+1); i++)
			{
                        printf("(Offset:  %lX)\n", (add+i));
			fread(inchar, sizeof(inchar), 1, file_ptr2);
			fwrite(inchar, sizeof(inchar), 1, file_ptr);
			}
		}

 printf("Baseline Patch Applied.\n");
}

void mghips()
{
 long   r;
 FILE   *file_ptr;
 FILE   *file_ptr2;
 char   inchar[1];
 long   offset, numdat;
 long   done=0, add2, add1, add0, dat1, dat0;
 int    errorchk=1;

 file_ptr=fopen(comname1, "r+b");
 chkiffileexist(file_ptr, comname1);

 file_ptr2=fopen(comname2, "rb");
 chkiffileexist(file_ptr2, comname2);

 printf("Applying IPS patch...\n");

 fseek(file_ptr2, 5, SEEK_SET);

 while (done==0)
	{
	fread(inchar, sizeof(inchar), 1, file_ptr2);
	if ((add2=inchar[0])<0) add2=add2+256;
	fread(inchar, sizeof(inchar), 1, file_ptr2);
	if ((add1=inchar[0])<0) add1=add1+256;

	errorchk=fread(inchar, sizeof(inchar), 1, file_ptr2);

	if ((add0=inchar[0])<0) add0=add0+256;

	offset=((add2*65536)+(add1*256)+add0);

	if ((offset==4542278) || (errorchk==0))
		done=1;

	if (done==0)
		{
                printf("(Offset:  %lX)\n", offset);

		fseek(file_ptr, offset, SEEK_SET);
		fread(inchar, sizeof(inchar), 1, file_ptr2);
		if ((dat1=inchar[0])<0) dat1=dat1+256;
		fread(inchar, sizeof(inchar), 1, file_ptr2);
		if ((dat0=inchar[0])<0) dat0=dat0+256;

		numdat=((dat1*256)+dat0);
		if (numdat>0)
			for (r=0; (r<numdat); r++)
				{
				fread(inchar, sizeof(inchar), 1, file_ptr2);
				fwrite(inchar, sizeof(inchar), 1, file_ptr);
				}
		else
			{
			fread(inchar, sizeof(inchar), 1, file_ptr2);
			if ((dat1=inchar[0])<0) dat1=dat1+256;
			fread(inchar, sizeof(inchar), 1, file_ptr2);
			if ((dat0=inchar[0])<0) dat0=dat0+256;
			fread(inchar, sizeof(inchar), 1, file_ptr2);
			numdat=((dat1*256)+dat0);
			for (r=0; (r<numdat); r++)
				fwrite(inchar, sizeof(inchar), 1, file_ptr);
			}
		}
	}

 if (errorchk==0)
        printf("IPS Patch EOF Reached.");
 else
        printf("IPS Patch Applied.");

 printf("\n");
}

void mghtoic2()
{
 unsigned long   i;
 FILE   *file_ptr;
 FILE   *file_ptr2;
 FILE   *file_ptr3;
 char   header[512];
 char   head[1];
 char   *name;
 unsigned long file_size;
 unsigned long file_size2;
 int    meter;
 int    metersize;
 int    checklen=0,o;

 if (k==1){
	printf("\nuCON Error:  Image is not in FIG/SWC format!\n");
	exit(8);
 }
 if (h==0){
	printf("\nuCON Error:  Image is not a HiROM Super NES!\n");
	exit(8);
 }
 if (comnumber==3){
	 strcpy(comname2, comname1);
	 strcpy(comname3, comname1);
	 if ((name=strchr(comname2, '.'))==NULL) {
		checklen=strlen(comname2);
		comname2[checklen]='.';
		comname2[checklen+1]='1';
		comname2[checklen+2]='\0';
	 }
	 else if((name=strchr(comname2, '.'))!=NULL) {
		name++;
		*name++='1';
		*name++='\0';
	 }
	 if ((name=strchr(comname3, '.'))==NULL) {
		checklen=strlen(comname3);
		comname3[checklen]='.';
		comname3[checklen+1]='2';
		comname3[checklen+2]='\0';
	 }
	 else if((name=strchr(comname3, '.'))!=NULL) {
		name++;
		*name++='2';
		*name++='\0';
	 }
 }
 if (comnumber==4){
	if (((strlen(comname2))<8) && ((name=strchr(comname2, '1'))==NULL)) {
	 strcpy(comname2, comname1);
	 strcpy(comname3, comname1);
	 if ((name=strchr(comname2, '.'))==NULL) {
		checklen=strlen(comname2);
		comname2[checklen]='.';
		comname2[checklen+1]='1';
		comname2[checklen+2]='\0';
	 }
	 else if((name=strchr(comname2, '.'))!=NULL) {
		name++;
		*name++='1';
		*name++='\0';
	 }
	 if ((name=strchr(comname3, '.'))==NULL) {
		checklen=strlen(comname3);
		comname3[checklen]='.';
		comname3[checklen+1]='2';
		comname3[checklen+2]='\0';
	 }
	 else if((name=strchr(comname3, '.'))!=NULL) {
		name++;
		*name++='2';
		*name++='\0';
	 }
 }
 else {
	strcpy(comname3, comname2);
	if ((name=strchr(comname3, '.'))!=NULL){
		name++;
		*name++='2';
		*name++='\0';
	}
 }
 }
 file_ptr=fopen(comname1, "rb");
 chkiffileexist(file_ptr, comname1);

 file_ptr2=fopen(comname2, "w+b");
 file_ptr3=fopen(comname3, "w+b");
 fseek(file_ptr, 0, SEEK_END);
 file_size=ftell(file_ptr);
 fseek(file_ptr, 0, SEEK_SET);
 file_size-=ftell(file_ptr);
 
 memset(header, 0, sizeof(header));
 file_size2=(file_size-512);
 header[0]=0x80;
 header[2]=0x40;
 header[8]=0xaa;
 header[9]=0xbb;
 header[10]=0x04;
 fwrite(header, sizeof(header), 1, file_ptr2);
 
 meter=((file_size-512)/131072);
 metersize=64/meter;
 file_size=(file_size-512)/32768;
 printf("FIG/SWC->SMC IC2 File #1\n");
 o=0;
 for (i=1; i<file_size; i=i+2){
	o++;
	fseek(file_ptr, i*32768+512, SEEK_SET);
	fread(readchunk3, sizeof(readchunk3), 1, file_ptr);
	fwrite(readchunk3, sizeof(readchunk3), 1, file_ptr2);
 }
 memset(readchunk3, 255, sizeof(readchunk3));
 for (i=(file_size/2); i<32; i++)
	fwrite(readchunk3, sizeof(readchunk3), 1, file_ptr2);
 head[0]=253;
 fseek(file_ptr2, 33237, SEEK_SET);
 fwrite(head, sizeof(head), 1, file_ptr2);
 
 file_size2=(file_size2/2);

 header[1]=((file_size2/8192)/256);
 header[0]=((file_size2/8192)-(header[1]*256));

 header[2]=0x00;
 fwrite(header, sizeof(header), 1, file_ptr3);

 printf("FIG/SWC->SMC IC2 File #2\n");
 o=0;
 for (i=0; i<file_size; i=i+2){
	o++;
	fseek(file_ptr, i*32768+512, SEEK_SET);
	fread(readchunk3, sizeof(readchunk3), 1, file_ptr);
	fwrite(readchunk3, sizeof(readchunk3), 1, file_ptr3);
 }

 printf("FIG/SWC->SMC IC2 Completed.\n");
}

void mghfromic2()
{
 long   i;
 FILE   *file_ptr;
 FILE   *file_ptr2;
 FILE   *file_ptr3;
 char   header[512];
 char   head[1];
 unsigned long file_size;
 int    lfile_size;
 int    meter, metersize, o, errorchk;
 char   *name;

 if (k==1){
	printf("\nuCON Error: Image is not in SMC IC2 format.\n");
	exit(8);
 }

 if (comnumber==3){
	 strcpy(comname2, comname1);
	 strcpy(comname3, comname1);
	 if((name=strchr(comname2, '.'))!=NULL) {
		name++;
		*name++='2';
		*name++='\0';
	 }
	 if((name=strchr(comname3, '.'))!=NULL) {
		name++;
		*name++='f';
		*name++='i';
		*name++='g';
		*name++='\0';
	 }
 }
 file_ptr=fopen(comname1, "rb");
 chkiffileexist(file_ptr, comname1);
 file_ptr2=fopen(comname2, "rb");
 chkiffileexist(file_ptr2, comname2);
 file_ptr3=fopen(comname3, "w+b");
 fseek(file_ptr2, 0, SEEK_END);
 file_size=ftell(file_ptr2);
 fseek(file_ptr2, 0, SEEK_SET);
 file_size-=ftell(file_ptr2);
 meter=file_size/131072;
 metersize=64/meter;
 lfile_size=(file_size)/32768;
 memset(header, 0, sizeof(header));

 header[1]=(((file_size*2)/8192)/256);
 header[0]=(((file_size*2)/8192)-(header[1]*256));

 header[3]=0x80;
 if (ramsize==0)
	{
	header[4]=0x77;
	header[5]=0x83;
	}
 header[8]=0xaa;
 header[9]=0xbb;
 header[10]=0x04;
 fwrite(header, sizeof(header), 1, file_ptr3);
 fread(header, sizeof(header), 1, file_ptr);
 fread(header, sizeof(header), 1, file_ptr2);
 printf("SMC IC2->FIG...\n");
 o=0;
 for (i=0; i<lfile_size; i++){
	o++;
	fread(readchunk3, sizeof(readchunk3), 1, file_ptr2);
	fwrite(readchunk3, sizeof(readchunk3), 1, file_ptr3);
	fread(readchunk3, sizeof(readchunk3), 1, file_ptr);
	fwrite(readchunk3, sizeof(readchunk3), 1, file_ptr3);
 }

 fseek(file_ptr3, 66005, SEEK_SET);
 fread(head, sizeof(head), 1, file_ptr3);
 if ((errorchk=head[0])<0)
	errorchk=errorchk+256;
 if (errorchk==253)
	{
	head[0]=char(49);
	fwrite(head, sizeof(head), 1, file_ptr3);
	}

 printf("SMC IC2->FIG Completed.\n");
}

void mghsconvertbin()
{
 unsigned long   i;
 FILE   *file_ptr;
 FILE   *file_ptr2;
 char   outchar[1];
 unsigned long file_size;
 unsigned long tmp;
 int    in;
 char   *name;
 int    checklen=0;
 int    meter, metersize, o;

 if ((comnumber==3) && (k==0))
	{
	strcpy(comname2, comname1);
	if ((name=strchr(comname2, '.'))==NULL)
		{
		checklen=strlen(comname2);
		comname2[checklen]='.';
		comname2[checklen+1]='b';
		comname2[checklen+2]='i';
		comname2[checklen+3]='n';
		comname2[checklen+4]='\0';
		}
	 else if((name=strchr(comname2, '.'))!=NULL)
		{
		name++;
		*name++='b';
		*name++='i';
		*name++='n';
		*name++='\0';
		}
	}


 if ((comnumber==3) && (k==1))
	{
	strcpy(comname2, comname1);
	if ((name=strchr(comname2, '.'))==NULL)
		{
		checklen=strlen(comname2);
		comname2[checklen]='.';
		comname2[checklen+1]='b';
		comname2[checklen+2]='i';
		comname2[checklen+3]='n';
		comname2[checklen+4]='\0';
		}
	else if((name=strchr(comname2, '.'))!=NULL)
		{
		name++;
		*name++='b';
		*name++='i';
		*name++='n';
		*name++='\0';
		}
	}


 file_ptr=fopen(comname1, "rb");
 chkiffileexist(file_ptr, comname1);

 file_ptr2=fopen(comname2, "w+b");
 fseek(file_ptr, 0, SEEK_END);
 file_size=ftell(file_ptr);
 fseek(file_ptr, 0, SEEK_SET);
 file_size-=ftell(file_ptr);


 if (k==0)
	{
	if (((file_size-512)%8192)!=0)
		{
	 	printf("uCON Error:  Image file size is not a multiple of 16 KB.\n");  
		printf("It must be padded before converting to binary.\n\n");
		remove(comname2);
		exit(8);
		}

	meter=(file_size-512)/131072;
	metersize=64/meter;
	file_size=(file_size-512)/16384;
        printf("Converting SMD->Binary...\n");
	o=0;
	for (i=0; i<file_size; i++)
		{
		o++;
		fseek(file_ptr, (i*2)*8192+512, SEEK_SET);
		fread(readchunk, sizeof(readchunk), 1, file_ptr);
		fread(readchunk2, sizeof(readchunk2), 1, file_ptr);
		for (in=0; in<8192; in++)
			{
			outchar[0]=readchunk2[in];
			fwrite(outchar, sizeof(outchar), 1, file_ptr2);
			outchar[0]=readchunk[in];
			fwrite(outchar, sizeof(outchar), 1, file_ptr2);
			}
		}
	}

 if (k==1)
	{
	if (((file_size)%8192)!=0)
		{
		printf("uCON Error:  Image file size is not a multiple of 16 KB.\n");
		printf("It must be padded before converting to binary.\n\n");
		remove(comname2);
		exit(8);
		}

	meter=file_size/131072;
	metersize=64/meter;
	tmp=file_size/2;
	file_size=(file_size)/16384;
        printf("Converting MGD2->Binary...\n");
	o=0;
	for (i=0; i<file_size; i++)
		{
		o++;
		fseek(file_ptr, i*8192, SEEK_SET);
		fread(readchunk, sizeof(readchunk), 1, file_ptr);
		fseek(file_ptr, i*8192+tmp, SEEK_SET);
		fread(readchunk2, sizeof(readchunk2), 1, file_ptr);
		for (in=0; in<8192; in++)
			{
			outchar[0]=readchunk2[in];
			fwrite(outchar, sizeof(outchar), 1, file_ptr2);
			outchar[0]=readchunk[in];
			fwrite(outchar, sizeof(outchar), 1, file_ptr2);
			}
		}
	}

 printf("Image Converted to Binary\n");
}

void mghsname()
{
FILE    *file_ptr;
FILE    *file_ptr2;
char    *name;
char    mgh[512];
int     i, j, checklen;
char    mghcharset[1024]={
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		 /* ! */ 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x18, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		 /* & */ 0x3c, 0x66, 0x18, 0x3c, 0x66, 0x66, 0x3c, 0x02,
		 /* ' */ 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,
		 /* ( */ 0x0c, 0x18, 0x18, 0x18, 0x18, 0x18, 0x0c, 0x00,
		 /* ) */ 0x30, 0x18, 0x18, 0x18, 0x18, 0x18, 0x30, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		 /* , */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x08,
		 /* - */ 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x00,
		 /* . */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00,
		 /* / */ 0x06, 0x06, 0x0c, 0x18, 0x30, 0x60, 0x60, 0x00,
		 /* 0 */ 0x3c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00,
		 /* 1 */ 0x38, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x00,
		 /* 2 */ 0x3c, 0x66, 0x06, 0x06, 0x7c, 0x60, 0x7e, 0x00,
		 /* 3 */ 0x3c, 0x66, 0x06, 0x1c, 0x06, 0x66, 0x3c, 0x00,
		 /* 4 */ 0x18, 0x38, 0x58, 0x7c, 0x18, 0x18, 0x3c, 0x00,
		 /* 5 */ 0x7e, 0x60, 0x7c, 0x06, 0x06, 0x66, 0x3c, 0x00,
		 /* 6 */ 0x3c, 0x66, 0x60, 0x7c, 0x66, 0x66, 0x3c, 0x00,
		 /* 7 */ 0x7e, 0x06, 0x0c, 0x0c, 0x18, 0x18, 0x18, 0x00,
		 /* 8 */ 0x3c, 0x66, 0x66, 0x3c, 0x66, 0x66, 0x3c, 0x00,
		 /* 9 */ 0x3c, 0x66, 0x66, 0x3e, 0x06, 0x66, 0x3c, 0x00,
		 /* : */ 0x00, 0x00, 0x18, 0x00, 0x18, 0x00, 0x00, 0x00,
		 /* ; */ 0x00, 0x00, 0x18, 0x00, 0x18, 0x08, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		 /* = */ 0x00, 0x00, 0x7e, 0x00, 0x7e, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		 /* A */ 0x18, 0x3c, 0x66, 0x66, 0x7e, 0x66, 0x66, 0x00,
		 /* B */ 0x7c, 0x66, 0x66, 0x7c, 0x66, 0x66, 0x7c, 0x00,
		 /* C */ 0x3c, 0x66, 0x60, 0x60, 0x60, 0x66, 0x3c, 0x00,
		 /* D */ 0x7c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7c, 0x00,
		 /* E */ 0x7e, 0x60, 0x60, 0x7e, 0x60, 0x60, 0x7e, 0x00,
		 /* F */ 0x7e, 0x60, 0x60, 0x7e, 0x60, 0x60, 0x60, 0x00,
		 /* G */ 0x3e, 0x60, 0x60, 0x6e, 0x66, 0x66, 0x3e, 0x00,
		 /* H */ 0x66, 0x66, 0x66, 0x7e, 0x66, 0x66, 0x66, 0x00,
		 /* I */ 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00,
		 /* J */ 0x06, 0x06, 0x06, 0x06, 0x66, 0x66, 0x3c, 0x00,
		 /* K */ 0x66, 0x6c, 0x78, 0x70, 0x78, 0x6c, 0x66, 0x00,
		 /* L */ 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7e, 0x00,
		 /* M */ 0xc6, 0xee, 0xfe, 0xd6, 0xc6, 0xc6, 0xc6, 0x00,
		 /* N */ 0x66, 0x76, 0x7e, 0x7e, 0x6e, 0x66, 0x66, 0x00,
		 /* O */ 0x3c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00,
		 /* P */ 0x7c, 0x66, 0x66, 0x7c, 0x60, 0x60, 0x60, 0x00,
		 /* Q */ 0x3c, 0x66, 0x66, 0x66, 0x66, 0x6e, 0x3e, 0x00,
		 /* R */ 0x7c, 0x66, 0x66, 0x7c, 0x6c, 0x66, 0x66, 0x00,
		 /* S */ 0x3c, 0x66, 0x60, 0x3c, 0x06, 0x66, 0x3c, 0x00,
		 /* T */ 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00,
		 /* U */ 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3e, 0x00,
		 /* V */ 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x3c, 0x00,
		 /* W */ 0x56, 0x56, 0x56, 0x56, 0x56, 0x56, 0x2c, 0x00,
		 /* X */ 0x66, 0x66, 0x3c, 0x10, 0x3c, 0x66, 0x66, 0x00,
		 /* Y */ 0x66, 0x66, 0x66, 0x3c, 0x18, 0x18, 0x18, 0x00,
		 /* Z */ 0x7e, 0x06, 0x0c, 0x18, 0x30, 0x60, 0x7e, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		 /* _ */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		 /* a */ 0x00, 0x00, 0x3c, 0x06, 0x3e, 0x66, 0x3e, 0x00,
		 /* b */ 0x60, 0x60, 0x7c, 0x66, 0x66, 0x66, 0x7c, 0x00,
		 /* c */ 0x00, 0x00, 0x3c, 0x66, 0x60, 0x66, 0x3c, 0x00,
		 /* d */ 0x06, 0x06, 0x6e, 0x66, 0x66, 0x66, 0x3e, 0x00,
		 /* e */ 0x00, 0x00, 0x3c, 0x66, 0x7e, 0x60, 0x3c, 0x00,
		 /* f */ 0x0c, 0x18, 0x18, 0x3c, 0x18, 0x18, 0x3c, 0x00,
		 /* g */ 0x00, 0x00, 0x3c, 0x66, 0x66, 0x3e, 0x06, 0x7c,
		 /* h */ 0x60, 0x60, 0x7c, 0x66, 0x66, 0x66, 0x66, 0x00,
		 /* i */ 0x18, 0x00, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x00,
		 /* j */ 0x00, 0x0c, 0x00, 0x0c, 0x0c, 0x0c, 0x6c, 0x38,
		 /* k */ 0x60, 0x60, 0x66, 0x6c, 0x78, 0x6c, 0x66, 0x00,
		 /* l */ 0x38, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x00,
		 /* m */ 0x00, 0x00, 0x5c, 0x76, 0x56, 0x56, 0x56, 0x00,
		 /* n */ 0x00, 0x00, 0x6c, 0x7e, 0x66, 0x66, 0x66, 0x00,
		 /* o */ 0x00, 0x00, 0x3c, 0x66, 0x66, 0x66, 0x3c, 0x00,
		 /* p */ 0x00, 0x00, 0x7c, 0x66, 0x66, 0x7c, 0x60, 0x60,
		 /* q */ 0x00, 0x00, 0x3e, 0x66, 0x66, 0x3e, 0x06, 0x06,
		 /* r */ 0x00, 0x00, 0x18, 0x1a, 0x18, 0x18, 0x3c, 0x00,
		 /* s */ 0x00, 0x00, 0x3e, 0x60, 0x3c, 0x06, 0x7c, 0x00,
		 /* t */ 0x00, 0x18, 0x3c, 0x18, 0x18, 0x18, 0x3c, 0x00,
		 /* u */ 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x3a, 0x00,
		 /* v */ 0x00, 0x00, 0x66, 0x66, 0x3c, 0x3c, 0x18, 0x00,
		 /* w */ 0x00, 0x00, 0x56, 0x56, 0x56, 0x56, 0x7e, 0x00,
		 /* x */ 0x00, 0x00, 0x66, 0x66, 0x18, 0x66, 0x66, 0x00,
		 /* y */ 0x00, 0x00, 0x66, 0x66, 0x66, 0x3e, 0x06, 0x7c,
		 /* z */ 0x00, 0x00, 0x7e, 0x0c, 0x18, 0x30, 0x7e, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		 /* | */ 0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 };

 if (comnumber==3)
	{
	strcpy(comname2, comname1);
	if ((name=strchr(comname2, '.'))==NULL)
		{
		checklen=strlen(comname2);
		comname2[checklen]='.';
		comname2[checklen+1]='m';
		comname2[checklen+2]='g';
		comname2[checklen+3]='h';
		comname2[checklen+4]='\0';
		}
	 else if((name=strchr(comname2, '.'))!=NULL)
		{
		name++;
		*name++='m';
		*name++='g';
		*name++='h';
		*name++='\0';
		}
	}

 if (comnumber==4)
	{
	for (i=0; i<15; i++)
		seganame[i]=' ';
	strcpy(seganame, comname2);

	strcpy(comname2, comname1);
	if ((name=strchr(comname2, '.'))==NULL)
		{
		checklen=strlen(comname2);
		comname2[checklen]='.';
		comname2[checklen+1]='m';
		comname2[checklen+2]='g';
		comname2[checklen+3]='h';
		comname2[checklen+4]='\0';
		}
	else if((name=strchr(comname2, '.'))!=NULL)
		{
		name++;
		*name++='m';
		*name++='g';
		*name++='h';
		*name++='\0';
		}
	}

 file_ptr=fopen(comname1, "rb");
 chkiffileexist(file_ptr, comname1);

 file_ptr2=fopen(comname2, "w+b");
 printf("Creating MGH Name File...\n");

 memset(mgh, 0, sizeof(mgh));
 mgh[0]='M';
 mgh[1]='G';
 mgh[2]='H';
 mgh[3]=0x1a;
 mgh[4]=0x06;
 mgh[5]=0xf0;
 mgh[16]='M';
 mgh[17]='G';
 mgh[18]='H';
 mgh[19]=' ';
 mgh[20]='B';
 mgh[21]='y';
 mgh[22]=' ';
 mgh[23]='u';
 mgh[24]='C';
 mgh[25]='O';
 mgh[26]='N';
 mgh[27]='/';
 mgh[28]='c';
 mgh[29]='h';
 mgh[30]='p';
 mgh[31]=0xff;

 for (i=0; i<15; i++)
	{
	for (j=0; j<4; j++)
		mgh[((i+2)*16)+j+4]=mghcharset[(seganame[i]*8)+j];
	for (j=4; j<8; j++)
		mgh[((i+2)*16)+j+244]=mghcharset[(seganame[i]*8)+j];
	}

 fwrite(mgh, sizeof(mgh), 1, file_ptr2);
 printf("MGH Name File Created.\n");
}

void uconhelpscrn()
{
 printf("\nuCON %s", uconver);
 printf("\t\"What Every Console User Needs\" - By Chicken & chp\n");
 printf("\t\t\t\t\t\t\t Unix port: splice\n");

 printf("uCON [command] [filename(s)] [switch] [/sg] [/sn] [/q]\n\n");
 printf("Common commands:\n");
 printf("  c   Convert between image formats.      n   Create MGH name file.\n");
 printf("  e   Apply Baseline patch.               o   Show image information.\n");
 printf("  h   Re-write image header.              p   Pad image size.\n");
 printf("  i   Apply IPS patch.                    s   Split image.\n");
 printf("  j   Join multi images.\n");
 printf("Sega Genesis only:\n");
 printf("  z   Convert to Binary.\n");
 printf("Super NES only:\n");
 printf("  b   Convert Boxer to FIG.               r   Attempt slowROM fix.\n");
 printf("  f   Convert FIG to Boxer.               w   Convert SWC<->FIG.\n");
 printf("  k   Attempt crack.                      x   Convert FIG/SWC->SMC IC2.\n");
 printf("  l   Attempt NTSC/PAL fix.               y   Convert SMC IC2->FIG.\n");
 printf("System switches:\n");
 printf("  /sg   Force Sega Genesis operations.    /sn   Force Super NES operations.\n");
 printf("  /q    Suppress image information (quiet mode).\n");
 printf("\nUse ? for the filename to get HELP on any command.\n");
 printf("Do not use pathnames or wildcards in the filenames.\n");
 exit(0);
}

void uconhelp()
{
 printf("uCON %s", uconver);
 printf("\t\"What Every Console User Needs\" - By Chicken & chp\n");
 printf("\t\t\t\t\t\t\t Unix port: splice\n");

 switch(commands[0])
	{
	case 'c':  printf("HELP:  Convert between image formats.\n\n");
			   printf(" This command converts an image from Super Magicom (SMC)/Pro Fighter (FIG)/\n");
			   printf(" Super Wild Card (SWC) format to Multi Game Doctor II (MGD2) format and\n");
			   printf(" vice versa.\n\n");
			   printf("  uCON c [input filename] [output filename]\n\n");
			   printf(" Default output filename will be named as appropriate to the image format.\n\n");
			   printf(" See the W command for help regarding SWC<->FIG conversions.\n");
			   exit(0);
			   break;
	case 'e':  printf("HELP:  Apply Baseline patch.\n\n");
			   printf(" This command is used to apply a Baseline patch to an image.\n\n");
			   printf("  uCON e [input filename] [patch filename.BSL]\n\n");
			   printf(" See the I command for help regarding IPS (.IPS) patches.\n");
			   exit(0);
			   break;
	case 'h':  printf("HELP:  Re-write image header.\n\n");
			   printf(" This command rewrites the header on images in Super Magicom (SMC)/Pro\n");
			   printf(" Fighter (FIG)/Super Wild Card (SWC) or Super Magic Drive (SMD) format.\n\n");
			   printf(" NOTE:  On HiROM images (Super NES only), the header will be in FIG format.\n\n");
			   printf("  uCON h [input filename] [/h|l|m|s]\n\n");
			   printf(" If an output filename is not supplied, the original file will be padded.\n\n");
			   printf("   The /h and /l switches can be used to force the image to be treated as\n");
			   printf("   HiROM or LoROM, respectively.  (Super NES images only)\n");
			   printf("   The /m switch will force the header to signify multi file.\n");
			   printf("   The /s switch will force SRAM ON for files in FIG format.  (Super NES only)\n");
			   printf("   Any number of switches can be used in any order.\n");
			   exit(0);
			   break;
	case 'i':  printf("HELP:  Apply IPS patch.\n\n");
			   printf(" This command is used to apply an IPS (International Patching Standard) patch\n");
			   printf(" to an image.\n\n");
			   printf("   uCON i [input filename] [patch filename.IPS]\n\n");
			   printf(" See the E command for help regarding Baseline (.BSL) patches.\n");
			   exit(0);
			   break;
	case 'j':  printf("HELP:  Join multi images.\n\n");
			   printf(" This command joins an image that has been split into a number of files.\n\n");
			   printf("  uCON j [input filename] [output filename]\n\n");
			   printf(" Default output filename will be named as appropriate to the image format.\n\n");
			   printf(" See the S command for help on splitting images.\n");
			   exit(0);
			   break;
	case 'o':  printf("HELP:  Show image information.\n\n");
			   printf(" This command show the image information stored within, such as the name,\n");
			   printf(" licensee, and so forth.\n\n");
			   printf(" NOTE:  This is the default operation if no command is specified.\n\n");
			   printf("  uCON o [input filename] [/sg] [/sn]\n\n");
			   printf("   The /sg and /sn switches can be used to force an image to be treated as a\n");
			   printf("   Sega Genesis or Super NES image, respectively.\n");
			   exit(0);
			   break;
	case 'p':  printf("HELP:  Pad image size.\n\n");
			   printf(" This command pads any uneven image to the next highest size evenly divisible\n");
			   printf(" by a Mbit.  This is necessary for an image in Super Magicom (SMC)/Pro Fighter\n");
			   printf(" (FIG)/Super Wild Card (SWC) format to work on the Super NES side of the\n");
			   printf(" Multi-Game Hunter.\n\n");
			   printf("  uCON p [input filename] [output filename]\n\n");
			   printf(" If an output file is not supplied, the original file will be padded.\n");
			   exit(0);
			   break;
	case 's':  printf("HELP:  Split image.\n\n");
			   printf(" This command splits an image into two even files.\n\n");
			   printf("   uCON s [input filename] [output filename]\n\n");
			   printf("  Default output filename will be named as appropriate to the image format.\n\n");
			   printf("  NOTE:  A Super NES image in SMC format can be converted and split in one\n");
			   printf("         operation.  To do so, supply a MGD2-style output filename (e.g.\n");
			   printf("         sfgame_a.  uCON will create two files, sfgame_a and sf_gameb which\n");
			   printf("         will be the SMC image converted to MGD2 and then split.\n\n");
			   printf("  See the J command for help with joining multi files.\n");
			   exit(0);
			   break;

	case 'z':  printf("HELP:  Convert to Binary.\n\n");
			   printf(" This command converts a Sega Genesis image from Super Magic Drive (SMD) or\n");
			   printf(" Multi Game Doctor II (MGD2) format to Binary.  uCON has not been modified to\n");
			   printf(" do any operations with images in Binary form.\n\n");
			   printf("  uCON z [input filename] [output filename]\n\n");
			   printf(" Default output filename will be filename.BIN.\n");
			   exit(0);
			   break;

	case 'b':  printf("HELP:  Convert Boxer to FIG.\n\n");
			   printf(" This command converts a HiROM image in Game Boxer format to the Pro Fighter\n");
			   printf(" (FIG) format.\n\n");
			   printf("  uCON b [input filename] [output filename]\n\n");
			   printf(" Default output filename is filename1.FIG.\n\n");
			   printf(" See the F command for help on FIG to Boxer conversions.\n");
			   exit(0);
			   break;
	case 'f':  printf("HELP:  Convert FIG to Boxer.\n\n");
			   printf(" This command converts a HiROM image in Pro Fighter (FIG)/Super Wild Card\n");
			   printf(" (SWC) format to the Game Boxer format.\n");
			   printf(" This is necessary for the image to work on the Multi-Game Hunter, the Game\n");
			   printf(" Boxer and the Game Doctor SF III.\n\n");
			   printf(" NOTE: Images larger than 16M are split as needed for the MGH.  Use the\n");
			   printf("       /3 switch to split as needed by the GDSF3 (including the header).\n\n");
			   printf("  uCON f [input filename] [output filename] [/3]\n\n");
			   printf(" Default output consists of SFfilenameA, SFfilenameB, etc.\n\n");
			   printf(" If an output filename is supplied, remember to start it with SF.  The\n");
			   printf(" suffix required at the end of the filenames will be inserted appropriately.\n\n");
			   printf(" See the B command for help with Boxer to FIG conversions.\n");
			   exit(0);
			   break;
	case 'k':  printf("HELP:  Attempt crack.\n\n");
			   printf(" This command will cause uCON to attempt to check for and crack any SRAM\n");
			   printf(" copy protection within a Super NES image.  The current success rate is\n");
			   printf(" about 75%%.\n\n");
			   printf("  uCON k [input filename] [output filename]\n\n");
			   printf(" Default output filename is filename.CRK.\n");
			   exit(0);
			   break;
	case 'l':  printf("HELP:  Attempt NTSC/PAL fix.\n\n");
			   printf(" This command will cause uCON to attempt to check for and fix any NTSC\n");
			   printf(" or PAL protection within a Super NES image.  The current success rate is\n");
			   printf(" about 85%%.\n\n");
			   printf("  uCON l [input filename] [output filename]\n\n");
			   printf(" Default output filename is filename.FIX.\n");
			   exit(0);
			   break;
	case 'n':  printf("HELP:  Create MGH name file.\n\n");
			   printf(" This command creates a name file describing an image, for use by the Multi-\n");
			   printf(" Game Hunter.  uCON examines the image and will usually extract the internal\n");
			   printf(" title for you.  But, if you prefer to use your own name, or because some\n\n");
			   printf(" áetas do not have an internal name, you may supply your own name.\n");
			   printf(" There is a maximum of 13 characters for names manually entered.\n\n");
			   printf(" Sega Genesis:   With this release debuts the Sega support of the name\n");
			   printf("                 file.  Because of it's flexibility, there are many different\n");
			   printf("                 things that can be done with it, but for simplicity, only\n");
			   printf("                 dual color support, and a maximum of 15 internal characters\n");
			   printf("                 are picked up.\n\n");
			   printf("  uCON n [input filename] [name]\n\n");
			   printf(" Default output filename is filename.MGH, as required.\n");
			   exit(0);
			   break;
	case 'r':  printf("HELP:  Attempt slowROM fix.\n\n");
			   printf(" This command will attempt to convert an image so that it will work properly\n");
			   printf(" on older copiers with slow RAM.  Current success rate is 90%%.\n\n");
			   printf("  uCON r [input filename] [output filename]\n\n");
			   printf(" Default output filename is filename.SLO.\n");
			   exit(0);
			   break;
	case 'w':  printf("HELP:  Convert SWC<->FIG.\n\n");
			   printf(" This command converts an image from Pro Fighter (FIG) format to Super\n");
			   printf(" Wild Card (SWC) format and vice versa.\n\n");
			   printf("  uCON w [input filename]\n\n");
			   printf(" See the C command for help with other format conversions.\n");
			   exit(0);
			   break;
	case 'x':  printf("HELP:  Convert FIG/SWC->SMC IC2.\n\n");
			   printf(" This command converts a HiROM image in Pro Fighter (FIG) or Super Wild Card\n");
			   printf(" (SWC) format to SMC IC2 format.\n\n");
			   printf("  uCON x [input filename]\n\n");
			   printf(" See the Y command for help with SMC IC2->FIG.\n");
			   exit(0);
			   break;
	case 'y':  printf("HELP:  Convert SMC IC2->FIG.\n\n");
			   printf(" This command converts a HiROM image in SMC IC2 format to Pro Fighter (FIG)\n");
			   printf(" format.\n\n");
			   printf("  uCON y [input filename]\n\n");
			   printf(" See the X command for help with FIG/SWC->SMC IC2.\n");
			   exit(0);
			   break;

	default:   printf("Unrecognized command.\n");
			   exit(0);
			   break;

	}
}

void snescrack()
{
 unsigned long   i,r;
 FILE   *file_ptr;
 FILE   *file_ptr2;
 char   header[512];
 unsigned long file_size, file_overflow=0;
 int    checklen=0;
 char   *name;
 int    meter, metersize, o;
 int    fixflag=0;

 if (comnumber==3)
	{
	strcpy(comname2, comname1);
	if ((name=strchr(comname2, '.'))==NULL)
		{
		checklen=strlen(comname2);
		comname2[checklen]='.';
		comname2[checklen+1]='c';
		comname2[checklen+2]='r';
		comname2[checklen+3]='k';
		comname2[checklen+4]='\0';
		}
	 else if((name=strchr(comname2, '.'))!=NULL)
		{
		name++;
		*name++='c';
		*name++='r';
		*name++='k';
		*name++='\0';
		}
	}

 file_ptr=fopen(comname1, "rb");
 chkiffileexist(file_ptr, comname1);

 file_ptr2=fopen(comname2, "w+b");
 fseek(file_ptr, 0, SEEK_END);
 file_size=ftell(file_ptr);
 fseek(file_ptr, 0, SEEK_SET);
 file_size-=ftell(file_ptr);
 if (k==0)
	 meter=(file_size-512)/130172;
 else meter=(file_size)/131072;
 metersize=64/meter;
 printf("Attempting Crack...\n");

 if (k==0)
	{
	file_size=file_size-512;
	fread(header, sizeof(header), 1, file_ptr);
	fwrite(header, sizeof(header), 1, file_ptr2);
	}

 file_overflow=(file_size-((file_size/32768)*32768));

 file_size=file_size/32768;
 o=0;
 for (r=0; r<file_size; r++)
	{
	o++;
	fread(readchunk3, sizeof(readchunk3), 1, file_ptr);

// 8F/9F xx xx 70 CF/DF xx xx 70 D0
 i=0;
 while (i<32768)
	{
	if ((readchunk3[i]==char(0x8f)) || (readchunk3[i]==char(0x9f)))
		{
		i++;
		i++;
		i++;
		if (readchunk3[i]==char(0x70))
			{
			i++;
			if ((readchunk3[i]==char(0xcf)) || (readchunk3[i]==char(0xdf)))
				{
				i++;
				i++;
				i++;
				if (readchunk3[i]==char(0x70))
					{
					i++;
					if (readchunk3[i]==char(0xd0))
						{
						if (ramsize==64)
							{
							readchunk3[i]=char(0xea);
							readchunk3[i+1]=char(0xea);
							}
						else
							readchunk3[i]=char(0x80);
						fixflag=1;
						i++;
						}
					}
				else
					i++;
				}
			else
				i++;
			}
		else
			i++;
		}
	else
		i++;
	}

// 8F/9F xx xx 30/31/32/33 CF/DF xx xx 30/31/32/33 D0|F0
 i=0;
 while (i<32768)
	{
	if ((readchunk3[i]==char(0x8f)) || (readchunk3[i]==char(0x9f)))
		{
		i++;
		i++;
		i++;
		if ((readchunk3[i]==char(0x30)) || (readchunk3[i]==char(0x31)) || (readchunk3[i]==char(0x32)) || (readchunk3[i]==char(0x33)))
			{
			i++;
			if ((readchunk3[i]==char(0xcf)) || (readchunk3[i]==char(0xdf)))
				{
				i++;
				i++;
				i++;
				if ((readchunk3[i]==char(0x30)) || (readchunk3[i]==char(0x31)) || (readchunk3[i]==char(0x32)) || (readchunk3[i]==char(0x33)))
					{
					i++;
					if (readchunk3[i]==char(0xd0))
						{
						readchunk3[i]=char(0x80);
						fixflag=2;
						i++;
						}
					else if (readchunk3[i]==char(0xf0))
						{
						readchunk3[i]=char(0xea);
						readchunk3[i+1]=char(0xea);
						fixflag=2;
						i++;
						}
					}
				else
					i++;
				}
			else
				i++;
			}
		else
			i++;
		}
	else
		i++;
	}

// 8F/9F xx xx 30/31/32/33 AF xx xx 30/31/32/33 C9 xx xx D0
 i=0;
 while (i<32768)
	{
	if ((readchunk3[i]==char(0x8f)) || (readchunk3[i]==char(0x9f)))
		{
		i++;
		i++;
		i++;
		if ((readchunk3[i]==char(0x30)) || (readchunk3[i]==char(0x31)) || (readchunk3[i]==char(0x32)) || (readchunk3[i]==char(0x33)))
			{
			i++;
			if (readchunk3[i]==char(0xaf))
				{
				i++;
				i++;
				i++;
				if ((readchunk3[i]==char(0x30)) || (readchunk3[i]==char(0x31)) || (readchunk3[i]==char(0x32)) || (readchunk3[i]==char(0x33)))
					{
					i++;
					if (readchunk3[i]==char(0xc9))
						{
						i++;
						i++;
						i++;
						if (readchunk3[i]==char(0xd0))
							{
							readchunk3[i]=char(0x80);
							fixflag=2;
							i++;
							}
						}
					else
						i++;
					}
				else
					i++;
				}
			else
				i++;
			}
		else
			i++;
		}
	else
		i++;
	}

 fwrite(readchunk3, sizeof(readchunk3), 1, file_ptr2);
  }

 if (file_overflow>0)
	{
	fread(readchunk3, file_overflow, 1, file_ptr);
	fwrite(readchunk3, file_overflow, 1, file_ptr2);
	}

 if (fixflag==2)
        printf("SWC Crack Successful.");
 else if (fixflag==1)
        printf("Crack Successful.");
 else
	{
        printf("Crack Unsuccessful.");
	remove(comname2);
	}
 printf("\n");
}


main(int argc, char* argv[])
{
 strcpy(uconver, "950117-unix");
 int    noinfo=0, flag=0;

 comnumber=argc;
 if ((argc<2) || ((argc==2) && ((strcmp(argv[1], "?")==0) || (strcmp(argv[1], "/?")==0))))
	uconhelpscrn();

 if (argc==2)
	{
	strcpy(commands, "o");
	strcpy(comname1, argv[1]);
	}

 if (argc==3)
	{
	strcpy(commands, argv[1]);
	strcpy(comname1, argv[2]);
	}

 if (argc==3)
	{
	if ((strcmp(argv[2], "/sg")==0) || (strcmp(argv[2], "/sn")==0) || (strcmp(argv[2], "/snl")==0) || (strcmp(argv[2], "/snh")==0))
		{
		strcpy(commands, "o");
		strcpy(comname1, argv[1]);
		strcpy(comname2, argv[2]);
		}
	}

 if ((strcmp(comname1, "?")==0) || (strcmp(comname1, "/?")==0))
	uconhelp();

 if (argc==4)
	{
	strcpy(commands, argv[1]);
	strcpy(comname1, argv[2]);
	strcpy(comname2, argv[3]);
	if (strcmp(comname2, "/h")==0)
		hirom=1;
	if ((strcmp(comname2, "/hs")==0) || (strcmp(comname2, "/sh")==0))
		hirom=1;
	}

 if (argc==5)
	{
	strcpy(commands, argv[1]);
	strcpy(comname1, argv[2]);
	strcpy(comname2, argv[3]);
	strcpy(comname3, argv[4]);
	}

 if ((argc==3) & ((strcmp(comname1, "/q"))==0))
	{
	printf("Warning: Missing command parameter.\n");
	exit(0);
	}

 if ((strcmp(comname1, "/q")==0) || (strcmp(comname2, "/q")==0) || (strcmp(comname3, "/q")==0) || (strcmp(argv[5], "/q")==0) || (strcmp(argv[6], "/q")==0))
	if (strcmp(commands, "o")!=0)
		{
		 noinfo=1;
		 comnumber--;
		}

 if ((commands[0]=='e') || (commands[0]=='i'))
	noinfo=1;

 console=whichsystem();

 if ((argc==3) & ((strcmp(comname1, "/sn"))==0))
	{
	strcpy(commands, "o");
	strcpy(comname1, argv[1]);
	console=0;
	noinfo=0;
	comnumber--;
	}

 if ((argc==3) & ((strcmp(comname1, "/snl"))==0))
	{
	strcpy(commands, "o");
	strcpy(comname1, argv[1]);
	strcpy(comname2, argv[2]);
	console=0;
	noinfo=0;
	comnumber--;
	}

 if ((argc==3) & ((strcmp(comname1, "/snh"))==0))
	{
	strcpy(commands, "o");
	strcpy(comname1, argv[1]);
	strcpy(comname2, argv[2]);
	console=0;
	noinfo=0;
	comnumber--;
	}

 if ((argc==3) & ((strcmp(comname1, "/sg"))==0))
	{
	strcpy(commands, "o");
	strcpy(comname1, argv[1]);
	console=1;
	noinfo=0;
	comnumber--;
	}

 if ((strcmp(argv[argc-1], "/sn")==0) || (strcmp(argv[argc-1], "/snl")==0) || (strcmp(argv[argc-1], "/snh")==0))
	{
	console=0;
	noinfo=0;
	comnumber--;
	}

 if (strcmp(argv[argc-1], "/sg")==0)
	{
	console=1;
	noinfo=0;
	comnumber--;
	}

 if ((console==-1) && ((commands[0]=='e') || (commands[0]=='i')))
	{
	console=0;
	flag=1;
	}

 if (console==-1)
	id_error();

 if (console==0)
	{

	if ((commands[0]=='e') || (commands[0]=='i')) noinfo=1;

	if (!noinfo) getcinfo();

	switch(commands[0])
		{
		case 'c': mghconvert();
			  break;
		case 'e': mghbaseline();
			  break;
		case 'h': mghheader();
			  break;
		case 'i': mghips();
			  break;
		case 'j': mghsnesj();
			  break;
		case 'k': snescrack();
			  break;
		case 'o': if (noinfo) getcinfo();
			  break;
		case 'p': mghpatch();
			  break;
		case 's': mghchop();
			  break;
		case 'z': commanderror(commands[0], 1);
                          break;
		case 'b': mghbtf();
			  break;
		case 'f': mghftb();
			  break;
		case 'l': mghpalfix();
			  break;
		case 'n': mghname();
			  break;
		case 'r': mghslowrom();
			  break;
		case 'w': mghswtospf();
			  break;
		case 'x': mghtoic2();
			  break;
		case 'y': mghfromic2();
			  break;

		default:  printf("Unrecognized command.\n");
			  exit(0);
			  break;
		}
	}
 else if (console==1)
	{
	if (!noinfo) getsinfo();


	switch(commands[0])
		{
		case 'c': mghsconvert();
			  break;
		case 'e': mghbaseline();
			  break;
		case 'h': mghsegaheader();
			  break;
		case 'i': mghips();
			  break;
		case 'j': mghsegaj();
			  break;
		case 'o': if (noinfo) getsinfo();
			  break;
		case 'p': mghsegap();
			  break;
		case 's': mghssplit();
			  break;

		case 'n': mghsname();
			  break;
		case 'z': mghsconvertbin();
			  break;

		case 'b': commanderror(commands[0], console);
		case 'f': commanderror(commands[0], console);
		case 'k': commanderror(commands[0], console);
		case 'l': commanderror(commands[0], console);
		case 'r': commanderror(commands[0], console);
		case 't': commanderror(commands[0], console);
		case 'w': commanderror(commands[0], console);
		case 'x': commanderror(commands[0], console);
		case 'y': commanderror(commands[0], console);

		default:  printf("Unrecognized command.\n");
			  exit(0);
			  break;
		}
	}

// if (noinfo) printf("\n");

 return(0);
}
