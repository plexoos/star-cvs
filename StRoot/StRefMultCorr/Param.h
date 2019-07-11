#ifndef __Param_h__
#define __Param_h__

#include <string>
using std::string;

const Int_t nID_gref = 1;
const Int_t nSet_gref = 6;
const string mParamStr_gref[nID_gref][nSet_gref] = {
	{
	"2014:200:15075005,15167014:-6,6",
	"10,14,21,29,40,54,71,92,116,145,179,218,263,315,373,441",
	"350",
	"518.544,0.475972,-0.130005,0.0218457,0.00694496,-0.000353465,-0.000142918,0",
	"1.45392,-38.6779,1.72374,10.4157,-0.00147623,0,2184.41,1.20188e-06",
	"187.3,-0.150837"
	}
};

const Int_t nID_ref1 = 30;
const Int_t nSet_ref1 = 6;
const string mParamStr_ref1[nID_ref1][nSet_ref1] = {
	{
	"2010:200:11002145,11028088:-30,30",
	"10,15,21,30,41,55,73,94,119,149,184,223,269,321,379,446",
	"200",
	"512.936,0.46943,0.00217,0,0,0,0,1.60636",
	"0.99051,22.0065,4.02331,-20.8187,-8.88498e-06,0.000886727,0,0",
	"171,-0.33"
	},
	{
	"2010:200:11028089,11035026:-30,30",
	"10,15,21,30,41,55,73,94,119,149,184,223,269,321,379,446",
	"200",
	"513.745,0.55589,0.00111,0,0,0,0,0.79761",
	"0.964406,23.6525,4.09475,-20.4268,1.51988e-05,0.000649946,0,0",
	"171,-0.33"
	},
	{
	"2010:200:11035061,11054067:-30,30",
	"10,15,21,30,41,55,73,94,119,149,184,223,269,321,379,446",
	"200",
	"511.805,0.54487,0.00235,0,0,0,0,2.73749",
	"0.98071,25.8699,4.06555,-20.4771,-3.55299e-06,0.000832471,0,0",
	"171,-0.33"
	},
	{
	"2010:200:11055095,11077018:-30,30",
	"10,15,21,30,41,55,73,94,119,149,184,223,269,321,379,446",
	"200",
	"514.542,0.53085,0.0019,0,0,0,0,0",
	"0.983422,23.8855,4.14735,-21.4205,-9.88422e-06,0.000865886,0,0",
	"171,-0.33"
	},
	{
	"2010:62.4:11077023,11081058:-40,40",
	"7,11,16,22,30,41,54,69,88,109,135,165,199,239,285,339",
	"100",
	"401.995,0.404492,-0.000173464,4.31794e-05,2.77825e-06,-4.6255e-08,-1.68239e-09,0",
	"0.784198,96.3743,18.0256,43.3586,0.000193941,5.31901e-05,0,0",
	"0,0"
	},
	{
	"2010:62.4:11081059,11098060:-40,40",
	"7,11,16,22,30,41,54,69,88,109,135,165,199,239,285,339",
	"100",
	"401.995,0.404492,-0.000173464,4.31794e-05,2.77825e-06,-4.6255e-08,-1.68239e-09,0",
	"1.02181,64.2068,46.7363,-86.7449,-2.81908e-08,4.50725e-05,0,0",
	"0,0"
	},
	{
	"2010:39:11098062,11106007:-40,40",
	"7,10,15,21,28,38,50,64,81,102,125,153,185,222,265,316",
	"80",
	"375.463,0.13738,-0.000736856,2.86425e-05,3.26438e-06,-2.02371e-08,-1.66782e-09,0",
	"1.01837,40.9401,55.7224,-225.094,-6.76875e-06,2.90872e-05,0,0",
	"0,0"
	},
	{
	"2010:39:11106008,11113047:-40,40",
	"7,10,15,21,28,38,50,64,81,102,125,153,185,222,265,316",
	"80",
	"370.305,0.0979523,0.000714647,-2.13586e-05,5.33714e-07,1.0941e-08,-4.96639e-10,5.158",
	"1.02912,40.4629,56.9218,-236.815,-7.60843e-06,2.68036e-05,0,0",
	"0,0"
	},
	{
	"2010:7.7:11113050,11124053:-70,70",
	"4,6,8,12,16,22,28,37,46,58,72,87,106,128,154,185",
	"40",
	"213.441,0.221321,0.0107025,-0.000220191,-3.93785e-06,4.71263e-08,2.86974e-10,8.36",
	"0.8352,86.974,50.0054,-83.1208,7.37246e-05,3.98413e-05,0,0",
	"0,0"
	},
	{
	"2010:7.7:11124054,11147040:-70,70",
	"4,6,8,12,16,22,28,37,46,58,72,87,106,128,154,185",
	"40",
	"221.809,0.0826672,0.00301726,-1.50271e-05,-1.88329e-06,1.0683e-09,2.60063e-10,0",
	"0.90686,60.1308,54.4187,-144.449,3.4559e-05,7.07684e-05,0,0",
	"0,0"
	},
	{
	"2010:11.5:11147041,11152058:-50,50",
	"5,7,10,14,19,26,34,44,56,70,86,105,127,153,184,221",
	"60",
	"266.589,0.0425617,0.000556783,2.45526e-05,1.43634e-06,-9.18007e-09,-8.0457e-10,0",
	"0.901325,68.5632,53.1088,-149.965,3.77108e-05,4.1719e-05,0,0",
	"0,0"
	},
	{
	"2010:11.5:11152059,11160010:-50,50",
	"5,7,10,14,19,26,34,44,56,70,86,105,127,153,184,221",
	"60",
	"262.993,0.0193229,-0.00100541,-1.00716e-06,2.14219e-06,3.38108e-09,-7.91829e-10,3.6",
	"0.921088,65.7025,52.9872,-153.578,3.25228e-05,5.05313e-05,0,0",
	"0,0"
	},
	{
	"2011:19.6:12111032,12119029:-70,70",
	"6,8,12,17,23,30,40,52,66,83,102,125,152,183,220,263",
	"40",
	"318.142,0.0808996,0.00315388,-9.51408e-06,-2.12705e-06,7.79328e-11,3.17772e-10,0",
	"1.00746,35.1459,55.9769,-299.014,-6.32608e-06,5.2202e-05,0,0",
	"0,0"
	},
	{
	"2011:19.6:12119030,12122025:-70,70",
	"6,8,12,17,23,30,40,52,66,83,102,125,152,183,220,263",
	"40",
	"318.486,0.0790558,0.00277076,-2.24739e-05,-1.84283e-06,3.17651e-09,2.71201e-10,0",
	"0.953142,63.1022,51.98,-243.39,1.54744e-05,9.26225e-05,0,0",
	"0,0"
	},
	{
	"2011:200:12126101,12138024:-30,30",
	"10,15,22,31,43,58,76,98,125,156,193,234,281,335,396,466",
	"200",
	"544.168,-0.0611001,0.0027107,0,0,0,0,-6.974",
	"0.9725,23.69,4.226,-16.59,-3.37e-06,0.0009126,0,0",
	"183.9,-0.2811"
	},
	{
	"2011:200:12138081,12145020:-30,30",
	"10,15,22,31,43,58,76,98,125,156,193,234,281,335,396,466",
	"200",
	"524.691,-0.0695037,0.00355507,0,0,0,0,12.509",
	"1.006,24.58,4.158,-17.55,-3.534e-05,0.0007344,0,0",
	"180.9,-0.3407"
	},
	{
	"2011:200:12149028,12152016:-30,30",
	"10,15,22,31,43,58,76,98,125,156,193,234,281,335,396,466",
	"200",
	"547.277,-0.0423451,0.00253649,0,0,0,0,-10.083",
	"0.9823,21.08,4.209,-18.01,-1.382e-05,0.0009383,0,0",
	"184.8,-0.134"
	},
	{
	"2011:200:12153008,12154021:-30,30",
	"10,15,22,31,43,58,76,98,125,156,193,234,281,335,396,466",
	"200",
	"538.904,-0.0600895,0.00146384,0,0,0,0,-1.71",
	"0.9973,21.42,4.197,-18.3,-2.601e-05,0.0009335,0,0",
	"183.4,-0.1059"
	},
	{
	"2011:200:12154038,12165031:-30,30",
	"10,15,22,31,43,58,76,98,125,156,193,234,281,335,396,466",
	"200",
	"537.194,-0.0743487,0.00278037,0,0,0,0,0",
	"0.9995,21.89,4.191,-18.17,-2.723e-05,0.0009326,0,0",
	"183.1,-0.0749"
	},
	{
	"2011:200:12165037,12171016:-30,30",
	"10,15,22,31,43,58,76,98,125,156,193,234,281,335,396,466",
	"200",
	"541.24,-0.0999333,0.00277175,0,0,0,0,-4.046",
	"0.9837,24.47,4.161,-17.42,-1.502e-05,0.0008967,0,0",
	"185.2,-0.1916"
	},
	{
	"2011:27:12171018,12180056:-70,70",
	"6,9,13,19,26,35,45,58,74,92,114,139,168,201,241,288",
	"70",
	"343.799,-0.0572345,0.00244418,5.7238e-06,-1.71997e-06,-1.01153e-09,2.64559e-10,0",
	"0.997565,57.0141,52.4999,-92.3577,-2.7663e-08,-1.06086e-05,0,0",
	"0,0"
	},
	{
	"2012:193:13116062,13118024:-30,30",
	"10,16,24,35,50,68,90,118,150,189,233,282,338,399,466,535",
	"100",
	"604.5,-0.008512,0.00099002,-1.65807e-05,-1.82955e-07,0,0,0",
	"0,0,0,0,0,0,0,0",
	"604.5,-4.76"
	},
	{
	"2012:193:13118033,13125006:-30,30",
	"10,16,24,35,50,68,90,118,150,189,233,282,338,399,466,535",
	"100",
	"604.5,-0.0175259,0.00295621,-2.1151e-05,-2.2038e-06,0,0,0",
	"0,0,0,0,0,0,0,0",
	"604.5,-1.63"
	},
	{
	"2012:193:13125010,13129018:-30,30",
	"10,16,24,35,50,68,90,118,150,189,233,282,338,399,466,535",
	"100",
	"604.5,-0.00779759,0.00272206,-3.1323e-05,-1.73146e-06,0,0,0",
	"0,0,0,0,0,0,0,0",
	"604.5,-1.9"
	},
	{
	"2012:193:13129026,13130033:-30,30",
	"10,16,24,35,50,68,90,118,150,189,233,282,338,399,466,535",
	"100",
	"604.5,-0.00486866,0.00330927,-2.60188e-05,-2.83235e-06,0,0,0",
	"0,0,0,0,0,0,0,0",
	"604.5,-2.27"
	},
	{
	"2012:193:13131006,13132010:-30,30",
	"10,16,24,35,50,68,90,118,150,189,233,282,338,399,466,535",
	"100",
	"604.5,-0.00422237,0.00353124,-1.49756e-05,-2.46773e-06,0,0,0",
	"0,0,0,0,0,0,0,0",
	"604.5,-3"
	},
	{
	"2012:193:13132014,13136015:-30,30",
	"10,16,24,35,50,68,90,118,150,189,233,282,338,399,466,535",
	"100",
	"604.5,-0.00431906,0.00376397,-1.33976e-05,-3.6945e-06,0,0,0",
	"0,0,0,0,0,0,0,0",
	"604.5,-2.42"
	},
	{
	"2014:14.5:15053000,15070021:-70,70",
	"5,7,11,15,20,27,36,47,59,75,93,113,138,166,200,239",
	"30",
	"285.092,0.0297893,0.000758875,6.18761e-05,-3.62925e-06,-1.05743e-08,7.33482e-10,0",
	"0.797748,5.44478,1.3871,-0.96068,0.00127508,6.53791e-06,0,0",
	"0,0"
	},
	// mParameterIndex = 28;
	// --> Vz dependent centrality definition for Vz<-27 and Vz>25
	// --> Corrections are done for |Vz|<30 cm
	// Run17 54.4 GeV, trigid = 580001
	{
	"2017:54.4:18153052,18156032:-30,30",
	"7,11,16,22,31,41,54,70,89,111,138,169,205,248,299,361",
	"70",
	"435.9,-0.02468,-0.003202,0.0002065,1.164e-05,-2.59e-07,-1.452e-08",
	"0.8115,244.6,24.29,-21.8,5.799e-05,0,0,0",
	"0,0"
	},
	// mParameterIndex = 29;
	// Run17 54.4 GeV, trigid = 580021
	{
	"2017:54.4:18156035,18172013:-40,40",
	"7,11,16,22,31,41,54,70,89,111,138,169,205,248,299,361",
	"70",
	"435.4,0.005937,-0.001628,7.807e-06,1.951e-06,-6.299e-09,-6.339e-10",
	"0.811,238.9,24.31,-25.16,6.325e-05",
	"0,0"
	}
};

const Int_t CentBin16_vzdep[5][16] = {
	{ 7,10,15,21,29,39,52,66,84,106,131,161,195,236,285,344 }, // -30<Vz<-29
	{ 7,11,16,22,30,41,54,69,88,110,137,168,204,247,298,359 }, // -29<Vz<-27
	{ 7,10,15,21,29,39,51,66,84,106,131,161,195,236,285,344 }, // 25<Vz<27
	{ 5,9,13,19,27,36,47,60,76,96,119,146,177,214,259,315 },   // 27<Vz<29
	{ 5,8,13,18,26,35,46,59,75,94,117,143,175,211,256,311 }    // 29<Vz<30
};
const Int_t CentBin9_vzdep[5][9] = {
	{ 7,15,29,52,84,131,195,285,344 }, // -30<Vz<-29
	{ 7,16,30,54,88,137,204,298,359 }, // -29<Vz<-27
	{ 7,15,29,51,84,131,195,285,344 }, // 25<Vz<27
	{ 5,13,27,47,76,119,177,259,315 },   // 27<Vz<29
	{ 5,13,26,46,75,117,175,256,311 }    // 29<Vz<30
};


const Int_t nID_ref2 = 16;
const Int_t nSet_ref2 = 6;
const string mParamStr_ref2[nID_ref2][nSet_ref2] = {
	{
	"2010:200:11002145,11028088:-30,30",
	"9,13,19,27,37,50,65,85,108,135,167,204,247,297,355,421",
	"200",
	"489.809,0.162624,-0.0134221,0.00028274,-8.15115e-06,-2.68941e-07,7.57503e-09,8.93273",
	"0,0,0,0,0,0,0,0",
	"0,0"
	},
	{
	"2010:200:11028089,11035026:-30,30",
	"9,13,19,27,37,50,65,85,108,135,167,204,247,297,355,421",
	"200",
	"494.922,0.237492,-0.00994104,4.06043e-05,-1.1281e-05,-4.26368e-08,6.62933e-09,3.82049",
	"0,0,0,0,0,0,0,0",
	"0,0"
	},
	{
	"2010:200:11035061,11054067:-30,30",
	"9,13,19,27,37,50,65,85,108,135,167,204,247,297,355,421",
	"200",
	"490.906,0.232202,-0.0173243,-2.08546e-05,1.53603e-05,7.02842e-08,-1.54893e-08,7.83626",
	"0,0,0,0,0,0,0,0",
	"0,0"
	},
	{
	"2010:200:11055095,11077018:-30,30",
	"9,13,19,27,37,50,65,85,108,135,167,204,247,297,355,421",
	"200",
	"498.742,0.421297,-0.029928,-0.000562931,-2.78328e-06,6.82985e-07,5.30001e-08,-1.94446e-06",
	"0,0,0,0,0,0,0,0",
	"0,0"
	},
	{
	"2010:62.4:11077023,11081058:-30,30",
	"7,10,15,21,29,39,51,66,84,106,131,160,194,233,279,334",
	"100",
	"394.112,0.343031,0.0136394,-0.000936662,-8.50196e-05,1.09842e-06,7.73788e-08,2.64656",
	"0,0,0,0,0,0,0,0",
	"0,0"
	},
	{
	"2010:62.4:11081059,11098060:-30,30",
	"7,10,15,21,29,39,51,66,84,106,131,160,194,233,279,334",
	"100",
	"396.758,0.176512,-0.012038,-4.12775e-05,1.28006e-05,7.24591e-08,-1.07178e-08,-2.22877e-06",
	"0,0,0,0,0,0,0,0",
	"0,0"
	},
	{
	"2010:39:11098062,11106007:-50,50",
	"7,10,14,20,27,36,47,61,78,97,121,147,179,215,257,307",
	"80",
	"369.029,0.107251,-0.0073379,8.29521e-05,-2.14131e-06,-1.02428e-08,4.04791e-10,-1.4177e-05",
	"0,0,0,0,0,0,0,0",
	"0,0"
	},
	{
	"2010:39:11106008,11113047:-50,50",
	"7,10,14,20,27,36,47,61,78,97,121,147,179,215,257,307",
	"80",
	"363.899,0.127298,-0.00538309,4.66533e-05,-3.78284e-06,-7.17752e-10,8.38515e-10,5.13025",
	"0,0,0,0,0,0,0,0",
	"0,0"
	},
	{
	"2010:7.7:11113050,11124053:-70,70",
	"3,5,7,10,14,19,25,32,41,51,64,78,95,114,137,165",
	"40",
	"198.564,-0.010518,-0.00986869,5.31949e-05,4.01489e-06,-4.0934e-09,-6.2698e-10,0.224595",
	"0,0,0,0,0,0,0,0",
	"0,0"
	},
	{
	"2010:7.7:11124054,11147040:-70,70",
	"3,5,7,10,14,19,25,32,41,51,64,78,95,114,137,165",
	"40",
	"198.789,0.0427951,-0.00425224,2.30795e-05,4.60197e-07,9.92525e-10,-7.19405e-11,-7.62727e-06",
	"0,0,0,0,0,0,0,0",
	"0,0"
	},
	{
	"2010:11.5:11147041,11152058:-70,70",
	"4,6,9,13,18,24,32,41,52,65,80,98,118,143,172,206",
	"60",
	"248.878,0.0387272,-0.0054708,5.1843e-05,4.80694e-07,-2.7742e-09,-9.62433e-11,-6.62319e-06",
	"0,0,0,0,0,0,0,0",
	"0,0"
	},
	{
	"2010:11.5:11152059,11160010:-70,70",
	"4,6,9,13,18,24,32,41,52,65,80,98,118,143,172,206",
	"60",
	"246.339,0.044504,-0.00535772,2.9206e-05,4.9759e-07,1.47456e-09,-1.15511e-10,2.5386",
	"0,0,0,0,0,0,0,0",
	"0,0"
	},
	{
	"2011:19.6:12111032,12119029:-50,50",
	"5,8,12,16,22,30,40,51,65,81,100,123,149,180,215,258",
	"40",
	"308.922,0.0501661,-0.00111507,6.31108e-05,-4.85191e-06,-6.41968e-09,1.43768e-09,-2.26693e-06",
	"0,0,0,0,0,0,0,0",
	"0,0"
	},
	{
	"2011:19.6:12119030,12122025:-50,50",
	"5,8,12,16,22,30,40,51,65,81,100,123,149,180,215,258",
	"40",
	"308.419,0.0443136,0.00211899,6.09836e-05,-8.83889e-06,-7.45333e-09,2.64361e-09,0.503326",
	"0,0,0,0,0,0,0,0",
	"0,0"
	},
	{
	"2011:200:12154038,12171017:-30,30",
	"9,14,20,29,40,54,71,92,117,147,181,221,268,321,383,453",
	"200",
	"532.259,0.150208,-0.0100526,-2.7926e-05,-1.18303e-05,6.51693e-08,8.99748e-09,-2.34393e-05",
	"0,0,0,0,0,0,0,0",
	"0,0"
	},
	{
	"2011:27:12171018,12180056:-50,50",
	"6,9,13,18,25,33,43,56,71,90,111,135,164,198,237,284",
	"70",
	"337.599,0.044536,-0.00154566,3.18458e-05,-5.75185e-06,7.468e-10,1.71637e-09,-6.30064e-06",
	"0,0,0,0,0,0,0,0",
	"0,0"
	}
};

const Int_t nID_ref3 = 22;
const Int_t nSet_ref3 = 6;
const string mParamStr_ref3[nID_ref3][nSet_ref3] = {
	{
	"2010:200:11002145,11028088:-30,30",
	"17,25,35,50,68,92,121,155,197,246,302,367,441,525,619,726",
	"200",
	"828.04,0.439133,-0.0133774,0,0,0,0,1.031",
	"1.03393,133.797,27.4409,-132.859,-7.44943e-06,0.000833889,0,0",
	"284.263,-0.569844"
	},
	{
	"2010:200:11028089,11035026:-30,30",
	"17,25,35,50,68,92,121,155,197,246,302,367,441,525,619,726",
	"200",
	"828.765,0.525515,-0.0147449,0,0,0,0,0.306",
	"1.00459,131.471,25.8343,-123.19,-3.02581e-06,0.000604996,0,0",
	"284.263,-0.569844"
	},
	{
	"2010:200:11035061,11054067:-30,30",
	"17,25,35,50,68,92,121,155,197,246,302,367,441,525,619,726",
	"200",
	"824.478,0.508699,-0.0133052,0,0,0,0,4.593",
	"1.00981,159.824,26.9355,-121.86,-4.79064e-06,0.000808434,0,0",
	"284.263,-0.569844"
	},
	{
	"2010:200:11055095,11077018:-30,30",
	"17,25,35,50,68,92,121,155,197,246,302,367,441,525,619,726",
	"200",
	"829.071,0.50211,-0.0136528,0,0,0,0,0",
	"1.02971,135.734,26.8982,-133.582,-8.49535e-06,0.000824411,0,0",
	"284.263,-0.569844"
	},
	{
	"2010:62.4:11077023,11081058:-30,30",
	"13,19,27,38,52,70,92,119,150,188,231,281,339,406,483,572",
	"150",
	"663.421,0.661982,0.0220411,-0.00173208,-0.000123877,1.71494e-06,1.04242e-07,8.911",
	"0.910356,85.64,12.0108,-24.9384,2.52498e-05,2.06617e-05,0,0",
	"0,0"
	},
	{
	"2010:62.4:11081059,11098060:-30,30",
	"13,19,27,38,52,70,92,119,150,188,231,281,339,406,483,572",
	"150",
	"672.332,0.354958,-0.0108052,0.000160221,3.39671e-06,-1.46475,-2.06264e-09,0",
	"0.964302,255.969,39.5362,-101.762,-5.79302e-09,-3.84108e-06,0,0",
	"0,0"
	},
	{
	"2010:39:11098062,11106007:-40,40",
	"12,17,25,35,48,64,84,108,137,171,210,256,309,370,440,523",
	"150",
	"631.212,0.196074,-0.00621651,6.58546e-06,-1.08682e-06,-5.92266e-09,-9.51085e-11,-9.28",
	"0.900784,304.871,45.3256,-102.763,6.66485e-06,-5.41731e-06,0,0",
	"0,0"
	},
	{
	"2010:39:11106008,11113047:-40,40",
	"12,17,25,35,48,64,84,108,137,171,210,256,309,370,440,523",
	"150",
	"621.932,0.132438,-0.00468703,1.89014e-06,-3.6127e-06,-1.69326e-09,6.90818e-10,0",
	"0.916184,336.174,49.145,-113.318,4.18334e-06,-1.64995e-06,0,0",
	"0,0"
	},
	{
	"2010:7.7:11113050,11124053:-50,50",
	"6,9,12,17,24,32,42,54,69,86,106,129,156,188,226,271",
	"50",
	"316.557,0.167214,0.0160311,-0.000204707,-2.1845e-05,5.59741e-08,6.02768e-09,9.622",
	"0.977841,33.8533,54.6729,-173.622,-1.99035e-06,8.40487e-05,0,0",
	"0,0"
	},
	{
	"2010:7.7:11124054,11147040:-50,50",
	"6,9,12,17,24,32,42,54,69,86,106,129,156,188,226,271",
	"50",
	"326.179,0.112537,-0.00107575,-6.11445e-05,-1.74699e-06,2.33608e-08,4.13804e-10,0",
	"0.972161,34.9791,54.2499,-175.71,2.65527e-06,9.95935e-05,0,0",
	"0,0"
	},
	{
	"2010:11.5:11147041,11152058:-50,50",
	"8,11,16,22,31,41,54,70,88,110,135,165,200,241,288,344",
	"50",
	"417.985,0.0264489,0.000980243,0.000123403,-4.58899e-06,-4.17197e-08,9.44972e-10,-6.339",
	"0.953807,35.4196,46.0612,-157.031,1.57246e-05,0.000125426,0,0",
	"0,0"
	},
	{
	"2010:11.5:11152059,11160010:-50,50",
	"8,11,16,22,31,41,54,70,88,110,145,165,200,241,288,344",
	"50",
	"411.646,0.036548,-0.00333334,1.68893e-05,-1.0805e-06,-1.95689e-09,1.89723e-10,0",
	"0.989612,27.1264,46.7008,-178.38,-4.01715e-06,0.000116767,0,0",
	"0,0"
	},
	{
	"2011:19.6:12111032,12119029:-50,50",
	"10,15,21,30,41,55,72,92,117,145,179,219,264,316,377,449",
	"50",
	"534.495,0.0762519,-0.00310799,3.2172e-05,-1.39102e-06,-1.06333e-08,5.37195e-11,0",
	"0.987349,23.69,30.428,-108.139,-2.58773e-06,0.000103783,0,0",
	"0,0"
	},
	{
	"2011:19.6:12119030,12122025:-50,50",
	"10,15,21,30,41,55,72,92,117,145,179,219,264,316,377,449",
	"50",
	"534.503,0.061329,0.000796444,3.91014e-05,-6.27339e-06,-1.3073e-08,1.43744e-09,-0.447",
	"0.941444,60.6981,46.4113,-155.808,1.67539e-05,9.73032e-05,0,0",
	"0,0"
	},
	{
	"2011:27:12171018,12180056:-50,50",
	"11,16,23,33,45,60,79,101,128,160,197,240,290,347,413,491",
	"70",
	"583.61,-0.0605533,-0.000394634,5.56866e-07,-1.88695e-06,1.13837e-10,1.90279e-10,0",
	"1.00592,30.3204,49.785,-164.869,-3.65229e-06,2.97682e-05,0,0",
	"0,0"
	},
	{
	"2011:200:12126101,12140031:-30,30",
	"14,22,34,50,70,96,127,165,210,263,324,394,474,563,663,775",
	"250",
	"894.215,-0.0661891,-0.0108833,0,0,0,0,0",
	"0.924059,193.753,16.4368,-128.771,2.15105e-05,0.000833889,-4407.67,0",
	"298.761,-0.489558"
	},
	{
	"2011:200:12140033,12145056:-30,30",
	"14,22,34,50,70,96,127,165,210,263,324,394,474,563,663,775",
	"250",
	"862.604,-0.0320212,-0.0100772,0,0,0,0,0",
	"0.924059,193.753,16.4368,-128.771,2.15105e-05,0.000833889,-4407.67,0",
	"293.787,-0.452075"
	},
	{
	"2011:200:12146003,12152016:-30,30",
	"14,22,34,50,70,96,127,165,210,263,324,394,474,563,663,775",
	"250",
	"928.021,-0.0603815,-0.0138586,0,0,0,0,0",
	"0.924059,193.753,16.4368,-128.771,2.15105e-05,0.000833889,-4407.67,0",
	"309.039,-0.563395"
	},
	{
	"2011:200:12153002,12154021:-30,30",
	"14,22,34,50,70,96,127,165,210,263,324,394,474,563,663,775",
	"250",
	"882.828,-0.0565833,-0.0146347,0,0,0,0,0",
	"0.924059,193.753,16.4368,-128.771,2.15105e-05,0.000833889,-4407.67,0",
	"296.304,-0.175358"
	},
	{
	"2011:200:12154038,12179051:-30,30",
	"14,22,34,50,70,96,127,165,210,263,324,394,474,563,663,775",
	"250",
	"884.392,-0.0897552,-0.0125586,0,0,0,0,0",
	"0.924059,193.753,16.4368,-128.771,2.15105e-05,0.000833889,-4407.67,0",
	"298.892,-0.201065"
	},
	{
	"2017:54:18153049,18156032:-30,30",
	"10,16,24,35,49,67,90,118,151,191,237,291,354,429,516,621",
	"100",
	"743.719,-0.0442062,-0.00854637,0.000512491,1.71258e-05,-6.78652e-07,-2.6323e-08,0",
	"0.837051,336.349,23.0862,-65.4852,5.16418e-06,-259867,-65.6434,1.20856e-08",
	"0,0"
	},
	{
	"2017:54:18156046,18172014:-30,30",
	"10,16,24,35,49,67,90,118,151,191,237,291,354,429,516,621",
	"100",
	"746.478,0.0252381,-0.00454175,3.28164e-05,-3.19485e-06,-1.0989e-08,3.45567e-09,0",
	"0.781539,279.908,19.4764,-99.7226,9.94082e-05,-248155,-7434.86,-1.88011e-08",
	"0,0"
	}
};

///// For RefMult4
///// It's dummy currently, no official definitions currently
const Int_t nID_ref4 = 1;
const Int_t nSet_ref4 = 6;
const string mParamStr_ref4[nID_ref4][nSet_ref4] = {
	{
	"2011:200:12154038,12171017:-30,30",
	"9,14,20,29,40,54,71,92,117,147,181,221,268,321,383,453",
	"200",
	"5.322590e+02,1.502080e-01,-1.005260e-02,-2.792600e-05,-1.183030e-05,6.516930e-08,8.997480e-09,-2.343930e-05",
	"0",
	"0"
	}
};

const string getParamX( const Int_t x, const Int_t y1, const Int_t y2 ){
	string str;
	switch(x){
		case 0:
			str = mParamStr_gref[y1][y2];
			break;
		case 1:
			str = mParamStr_ref1[y1][y2];
			break;
		case 2:
			str = mParamStr_ref2[y1][y2];
			break;
		case 3:
			str = mParamStr_ref3[y1][y2];
			break;
		case 4:
			str = mParamStr_ref4[y1][y2];
			break;
		default:
			str = "0";
	}
	return str;
}

#endif
