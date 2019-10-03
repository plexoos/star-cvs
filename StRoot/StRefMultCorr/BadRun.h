#ifndef __BadRun_h__
#define __BadRun_h__
#include <TROOT.h>
const Int_t nBadRun_refmult_2010 = 717; 
const Int_t nBadRun_refmult_2011 = 134;
const Int_t badrun_refmult_2010[nBadRun_refmult_2010] 
= {
11002120,11002121,11002126,11002127,11002129,11003010,11003011,11003101,11003102,11004007,11004008,11004009,11004010,11004011,11004012,11004013,11004014,11004015,11004016,11004018,11004020,11004021,11004023,11004024,11004025,11004026,11004028,11004029,11004030,11004032,11004033,11004034,11004035,11004037,11004038,11005042,11006004,11006005,11006008,11007015,11010031,11011019,11011053,11015069,11015071,11016024,11017006,11018003,11018007,11018008,11018036,11019001,11019080,11019081,11021027,11021028,11021031,11023048,11025034,11025038,11025054,11025067,11025069,11026005,11026008,11026021,11026022,11026023,11026025,11026067,11026068,11028004,11028005,11028006,11028007,11028008,11028009,11028010,11028011,11028012,11028013,11028018,11028019,11028020,11028021,11028022,11028023,11028024,11028025,11028026,11028027,11030041,11030080,11031061,11031064,11035008,11035009,11035072,11036026,11037035,11037037,11037060,11037066,11037067,11038048,11038049,11038050,11039047,11039067,11040078,11040083,11041022,11041023,11041040,11041041,11042001,11042002,11042003,11042004,11042005,11042006,11042007,11042008,11042011,11042012,11042018,11042019,11042020,11042021,11042022,11042023,11042024,11042025,11042026,11042027,11042042,11042043,11042044,11042045,11042046,11042047,11042048,11042049,11044029,11047059,11047065,11047066,11047067,11048037,11049001,11049002,11049005,11049023,11051038,11051049,11051051,11051055,11051063,11051064,11051068,11052011,11053057,11054021,11054022,11054024,11054059,11054062,11054066,11057012,11057035,11057036,11058005,11058050,11058083,11059043,11059055,11059060,11059075,11059076,11059077,11060008,11060049,11060059,11060069,11060076,11061008,11061009,11061021,11061034,11061037,11061038,11061095,11063006,11063007,11063008,11063011,11063013,11063014,11063015,11063016,11063017,11063036,11063083,11064003,11064023,11065038,11066024,11066045,11071056,11072032,11072044,11072045,11073001,11073002,11073003,11073049,11075039,11075045,11075048,11080072,11081023,11081025,11082012,11082013,11082046,11082056,11082057,11084009,11084011,11084012,11084013,11084020,11084021,11084035,11084044,11084064,11085015,11085025,11085030,11085046,11085055,11085056,11085057,11086005,11086007,11087001,11087002,11087003,11087004,11088013,11089026,11089028,11089029,11089055,11089068,11089072,11091007,11091015,11091021,11091078,11092010,11092011,11092012,11092032,11092033,11092034,11092067,11092096,11093001,11094016,11094017,11094018,11094019,11094020,11094021,11094022,11094023,11094024,11094027,11094028,11094042,11094044,11094045,11094046,11094047,11094048,11094050,11094051,11094052,11094053,11094054,11094055,11094074,11094075,11094077,11095001,11095002,11095003,11095004,11095005,11095006,11095009,11095010,11095011,11095012,11095013,11095014,11095015,11095022,11095040,11095048,11095050,11095051,11095061,11095062,11095063,11095064,11095082,11095087,11096024,11096039,11096043,11096044,11097093,11099124,11100002,11100045,11101046,11102012,11102051,11102052,11102053,11102054,11102055,11102058,11103035,11103056,11103058,11103092,11103093,11105052,11105053,11105054,11105055,11107007,11107042,11107057,11107061,11107065,11107074,11108101,11109013,11109077,11109088,11109090,11109127,11110013,11110034,11110073,11110076,11111084,11111085,11114084,11114085,11114086,11114088,11114089,11114094,11114095,11114100,11114109,11115005,11115007,11115013,11115019,11115025,11115027,11115028,11115030,11115032,11115051,11115062,11115064,11115069,11115072,11115078,11115079,11115080,11115086,11115088,11115094,11116001,11116002,11116005,11116006,11116010,11116014,11116020,11116023,11116028,11116060,11116061,11116062,11116064,11116068,11116070,11116072,11116073,11116075,11117002,11117006,11117031,11117033,11117034,11117036,11117039,11117044,11117045,11117046,11117052,11117055,11117063,11117064,11117071,11117075,11117085,11117088,11117089,11117090,11117093,11117094,11117095,11117098,11117100,11117103,11117104,11117107,11118007,11118008,11118016,11118024,11118025,11118026,11118039,11118044,11119001,11119003,11119006,11119007,11119009,11119012,11119013,11119015,11119016,11119017,11119022,11119024,11119026,11119029,11119030,11119056,11119057,11119060,11119062,11119067,11119069,11119070,11119071,11119074,11119075,11119077,11119079,11119081,11119090,11119091,11119100,11119101,11120003,11120006,11120008,11120011,11120014,11120019,11120023,11120030,11120034,11120037,11120039,11120040,11120045,11120052,11120057,11120062,11120063,11120069,11120070,11120071,11120074,11120077,11120078,11120084,11120092,11121006,11121014,11121015,11121019,11121029,11121030,11121034,11121035,11121043,11121044,11121054,11121058,11121066,11121067,11121070,11121075,11121082,11122001,11122007,11122008,11122010,11122017,11122024,11122037,11122038,11122047,11122048,11122049,11122050,11122053,11122058,11122062,11122069,11122073,11122078,11122085,11122097,11123003,11123004,11123015,11123026,11123028,11123040,11123044,11123055,11123057,11123058,11123059,11123067,11123075,11123076,11123077,11123079,11123081,11123084,11123086,11123088,11123089,11123093,11123094,11123095,11123100,11123101,11123102,11123104,11124001,11124005,11124007,11124008,11124015,11124016,11124018,11124041,11124046,11124050,11124051,11124052,11124053,11124058,11124060,11124061,11124062,11124063,11124064,11124065,11124066,11124069,11125002,11125003,11125004,11125005,11125006,11125008,11125012,11125013,11125014,11125015,11125016,11125017,11125020,11125021,11125022,11125023,11125073,11125081,11125089,11125090,11125096,11125097,11126005,11126006,11126007,11126016,11126018,11126022,11126023,11127001,11127002,11127043,11128005,11128012,11128018,11128050,11128056,11128072,11129018,11129022,11129028,11129051,11130027,11130034,11130057,11131038,11131062,11132013,11132070,11133006,11133019,11134053,11134060,11134067,11134076,11135068,11136003,11136005,11136006,11136007,11136008,11136012,11136013,11136014,11136061,11136076,11136101,11136130,11136160,11136163,11137019,11138027,11138049,11138086,11138124,11139014,11140076,11140086,11141063,11142117,11143026,11143028,11144001,11144009,11144031,11144033,11144040,11144043,11144052,11145008,11145028,11145035,11146061,11146076,11146079,11147004,11147006,11147014,11147017,11147021,11147023,11148039,11148045,11149001,11149008,11149010,11149011,11149015,11149047,11150016,11150025,11150028,11151036,11151040,11151050,11152016,11152036,11152078,11153032,11153042,11155001,11155009,11156003,11156009,11157012,11158006,11158022,11158024};
const Int_t badrun_refmult_2011[nBadRun_refmult_2011] = {
12113091,12114007,12114035,12114078,12114092,12114116,12115009,12115014,12115015,12115016,12115018,12115019,12115020,12115022,12115023,12115062,12115073,12115093,12115094,12116012,12116054,12117010,12117016,12117020,12117065,12119040,12119042,12120017,12120026,12121017,12121022,12121034,12121050,12121067,12122019,12127003,12127010,12127011,12127017,12127018,12127032,12128025,12132043,12132061,12133018,12134023,12136005,12136006,12136014,12136017,12136022,12136023,12136024,12136025,12136027,12136028,12136029,12136030,12136031,12136034,12136054,12138005,12138017,12138021,12146004,12146006,12146007,12146008,12151035,12153002,12153004,12153007,12153013,12157038,12157051,12158040,12158041,12158054,12158056,12158057,12162055,12162056,12162057,12162058,12164037,12164078,12164079,12166002,12166003,12167015,12167024,12167052,12168002,12168009,12168022,12168077,12170044,12170045,12170054,12170056,12172050,12172051,12172055,12173030,12173031,12173032,12173033,12173034,12174067,12174085,12175062,12175087,12175113,12175114,12175115,12176001,12176044,12176054,12176071,12177015,12177061,12177092,12177099,12177101,12177106,12177107,12177108,12178003,12178004,12178005,12178006,12178013,12178099,12178120
};

const Int_t nBadRun_refmult_2017 = 24; 
const Int_t badrun_refmult_2017[nBadRun_refmult_2017] = {
18153057,18153063,18154037,18154038,18154039,18157003,18157011,18158020,18158021,18161005,18161021,18164044,18165039,18166033,18166052,18167014,18167015,18167016,18167017,18167018,18167041,18168015,18169036,18170021};

const Int_t nBadRun_refmult_2018 = 86; 
const Int_t badrun_refmult_2018[nBadRun_refmult_2018] = 
{
	19130085,19131009,19131010,19131012,19132063,19133009,19133010,19133012,19133013,19133014,19133018,19134010,19134011,19135011,19135013,19135014,19136016,19137003,19137022,19137047,19137050,19137051,19137052,19137053,19137056,19137057,19138008,19138009,19138014,19139022,19139023,19139024,19139026,19139027,19139028,19139032,19139033,19139034,19139037,19140009,19140014,19141008,19142005,19142048,19143008,19143009,19143010,19143011,19143012,19143013,19143014,19143015,19143016,19143017,19146016,19147007,19147008,19147009,19147010,19147014,19147015,19147016,19156002,19156032,19156044,19156045,19156046,19157013,19158003,19158007,19158009,19158010,19158011,19158013,19158014,19158015,19158017,19158018,19158019,19160018,19162002,19162005,19165015,19165020,19165021,19167042 
};
#endif
