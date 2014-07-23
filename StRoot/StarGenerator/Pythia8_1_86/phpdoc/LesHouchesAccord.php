<html>
<head>
<title>Les Houches Accord</title>
<link rel="stylesheet" type="text/css" href="pythia.css"/>
<link rel="shortcut icon" href="pythia32.gif"/>
</head>
<body>

<script language=javascript type=text/javascript>
function stopRKey(evt) {
var evt = (evt) ? evt : ((event) ? event : null);
var node = (evt.target) ? evt.target :((evt.srcElement) ? evt.srcElement : null);
if ((evt.keyCode == 13) && (node.type=="text"))
{return false;}
}

document.onkeypress = stopRKey;
</script>
<?php
if($_POST['saved'] == 1) {
if($_POST['filepath'] != "files/") {
echo "<font color='red'>SETTINGS SAVED TO FILE</font><br/><br/>"; }
else {
echo "<font color='red'>NO FILE SELECTED YET.. PLEASE DO SO </font><a href='SaveSettings.php'>HERE</a><br/><br/>"; }
}
?>

<form method='post' action='LesHouchesAccord.php'>
 
<h2>Les Houches Accord</h2> 
 
The Les Houches Accord (LHA) for user processes [<a href="Bibliography.php" target="page">Boo01</a>] is the 
standard way to input parton-level information from a 
matrix-elements-based generator into PYTHIA. The conventions for 
which information should be stored has been defined in a Fortran context, 
as two commonblocks. Here a C++ equivalent is defined, as a single class. 
The most common application is to read input from a Les Houches Event File 
(LHEF) [<a href="Bibliography.php" target="page">Alw06</a>], but it is also possible to have a runtime 
interface to another program.

<p/> 
A "no-beams" extension, currently not part of the standard, has been 
implemented. In this case only one part of a complete event is studied, 
and so no meaningful beam information can be set. The prime example is
to study the decay properties of a resonance, where a parton-level decay
chain is provided as input, and then showers and nadronization should be
added. Another example would be where a given partonic configuration 
would be hadronized, without any previous showers. See further below and 
in the <?php $filepath = $_GET["filepath"];
echo "<a href='HadronLevelStandalone.php?filepath=".$filepath."' target='page'>";?>Hadron-Level Standalone</a> 
description.
 
<p/> 
The <code>LHAup</code> class is a base class, containing reading and 
printout functions, plus two pure virtual functions, one to set 
initialization information and one to set information on each new event. 
Derived classes have to provide these two virtual functions to do 
the actual work. The existing derived classes are for reading information 
from a Les Houches Event File, from the respective Fortran 
commonblocks, or from PYTHIA 8 itself. 
 
<p/> 
You are free to write your own derived classes, using the rules and 
methods to be described below. Normally, pointers to objects of such 
derived classes should be handed in with the 
<code><?php $filepath = $_GET["filepath"];
echo "<a href='ProgramFlow.php?filepath=".$filepath."' target='page'>";?>Pythia::init( LHAup*)</a></code> 
method. However, with the LHEF format a filename can replace the 
pointer, see further below. 
 
<p/> 
Let us now describe the methods at your disposal to do the job. 
 
<a name="method1"></a>
<p/><strong>LHAup::LHAup( int strategy = 3) &nbsp;</strong> <br/>
the base class constructor takes the choice of mixing/weighting 
strategy as optional input argument, and calls <code>setStrategy</code>, 
see below. It also reserves some space for processes and particles. 
   
 
<a name="method2"></a>
<p/><strong>virtual LHAup::~LHAup() &nbsp;</strong> <br/>
the destructor does not need to do anything. 
   
 
<a name="method3"></a>
<p/><strong>void LHAup::setPtr(Info* infoPtr) &nbsp;</strong> <br/>
this method only sets the pointer that allows some information 
to be accessed, and is automatically called by 
<code>Pythia::init(...)</code>. 
   
 
<h3>Initialization</h3> 
 
The <code>LHAup</code> class stores information equivalent to the 
<code>/HEPRUP/</code> commonblock, as required to initialize the event 
generation chain. The main difference is that the vector container 
now allows a flexible number of subprocesses to be defined. For the 
rest, names have been modified, since the 6-character-limit does not 
apply, and variables have been regrouped for clarity, but nothing 
fundamental is changed. 
 
<a name="method4"></a>
<p/><strong>virtual bool LHAup::setInit() &nbsp;</strong> <br/>
this pure virtual method has to be implemented in the derived class, 
to set relevant information when called. It should return false if it 
fails to set the info. In the no-beams extension this method need not
do anything, since by default strategy 3 is chosen and the rest is set
vanishing, but the method must exist.
   
 
<p/> 
Inside <code>setInit()</code>, such information can be set by the following 
methods: 
<a name="method5"></a>
<p/><strong>void LHAup::setBeamA( int identity, double energy, int pdfGroup = 0, int pdfSet = 0) &nbsp;</strong> <br/>
   
<strong>void LHAup::setBeamB( int identity, double energy, int pdfGroup = 0, int pdfSet = 0) &nbsp;</strong> <br/>
sets the properties of the first and second incoming beam, respectively 
(cf. the Fortran <code>IDBMUP(1), EBMUP(i), PDFGUP(i), PDFSUP(i)</code>, 
with <code>i</code> 1 or 2). These numbers can be used to tell which PDF 
sets were used when the hard process was generated, while the normal 
<?php $filepath = $_GET["filepath"];
echo "<a href='PDFSelection.php?filepath=".$filepath."' target='page'>";?>PDF Selection</a> is used for the further 
event generation in PYTHIA. 
   
 
<a name="method6"></a>
<p/><strong>void LHAup::setStrategy( int strategy) &nbsp;</strong> <br/>
sets the event weighting and cross section strategy. The default, 
provided in the class constructor, is 3, which is the natural value 
e.g. for an LHEF. 
<br/><code>argument</code><strong> strategy </strong>  :  
chosen strategy (cf. <code>IDWTUP</code>; see [<a href="Bibliography.php" target="page">Sjo06</a>] 
section 9.9.1 for extensive comments). 
<br/><code>argumentoption </code><strong> 1</strong> :  events come with non-negative weight, given in units 
of pb, with an average that converges towards the cross section of the 
process. PYTHIA is in charge of the event mixing, i.e. for each new 
try decides which process should be generated, and then decides whether 
is should be kept, based on a comparison with <code>xMax</code>. 
Accepted events therefore have unit weight.   
<br/><code>argumentoption </code><strong> -1</strong> :  as option 1, except that cross sections can now be 
negative and events after unweighting have weight +-1. You can use 
<code><?php $filepath = $_GET["filepath"];
echo "<a href='EventInformation.php?filepath=".$filepath."' target='page'>";?>Info::weight()</a></code> 
to find the weight of the current event. A correct event mixing requires 
that a process that can take both signs should be split in two, one limited 
to positive or zero and the other to negative or zero values, with 
<code>xMax</code> chosen appropriately for the two.   
<br/><code>argumentoption </code><strong> 2</strong> :  events come with non-negative weight, in unspecified 
units, but such that <code>xMax</code> can be used to unweight the events 
to unit weight. Again PYTHIA is in charge of the event mixing. 
The total cross section of a process is stored in 
<code>xSec</code>.   
<br/><code>argumentoption </code><strong> -2</strong> :  as option 2, except that cross sections can now be 
negative and events after unweighting have weight +-1. As for option -1 
processes with indeterminate sign should be split in two.   
<br/><code>argumentoption </code><strong> 3</strong> :  events come with unit weight, and are thus accepted 
as is. The total cross section of the process is stored in 
<code>xSec</code>.   
<br/><code>argumentoption </code><strong> -3</strong> :  as option 3, except that events now come with weight 
+-1. Unlike options -1 and -2 processes with indeterminate sign need not be 
split in two, unless you intend to mix with internal PYTHIA processes 
(see below).   
<br/><code>argumentoption </code><strong> 4</strong> :  events come with non-negative weight, given in units 
of pb, with an average that converges towards the cross section of the 
process, like for option 1. No attempt is made to unweight the events, 
however, but all are generated in full, and retain their original weight. 
For consistency with normal PYTHIA units, the weight stored in 
<code>Info::weight()</code> has been converted to mb, however. 
   
<br/><code>argumentoption </code><strong> -4</strong> :  as option 4, except that events now can come 
either with positive or negative weights.   
<br/><b>Note 1</b>: if several processes have already been mixed and 
stored in a common event file, either LHEF or some private format, it 
would be problematical to read back events in a different order. Since it 
is then not feasible to let PYTHIA pick the next process type, strategies 
+-1 and +-2 would not work. Instead strategy 3 would be the recommended 
choice, or -3 if negative-weight events are required. 
<br/><b>Note 2</b>: it is possible to switch on internally implemented 
processes and have PYTHIA mix these with LHA ones according to their relative 
cross sections for strategies +-1, +-2 and 3. It does not work for strategy 
-3 unless the positive and negative sectors of the cross sections are in 
separate subprocesses (as must always be the case for -1 and -2), since 
otherwise the overall mixture of PYTHIA and LHA processes will be off. 
Mixing is not possible for strategies +-4, since the weighting procedure 
is not specified by the standard. (For instance, the intention may be to 
have events biased towards larger <i>pT</i> values in some particular 
functional form.) 
   
   
 
<a name="method7"></a>
<p/><strong>void LHAup::addProcess( int idProcess, double xSec, double xErr, double xMax) &nbsp;</strong> <br/>
sets info on an allowed process (cf. <code>LPRUP, XSECUP, XERRUP, 
XMAXUP</code>). 
Each new call will append one more entry to the list of processes. 
The choice of strategy determines which quantities are mandatory: 
<code>xSec</code> for strategies +-2 and +-3, 
<code>xErr</code> never, and 
<code>xMax</code> for strategies +-1 and +-2. 
   
 
<br/><b>Note</b>: PYTHIA does not make active use of the (optional) 
<code>xErr</code> values, but calculates a statistical cross section 
error based on the spread of event-to-event weights. This should work 
fine for strategy options +-1, but not for the others. Specifically, 
for options +-2 and +-3 the weight spread may well vanish, and anyway 
is likely to be an underestimate of the true error. If the author of the 
LHA input information does provide error information you may use that - 
this information is displayed at initialization. If not, then a relative 
error decreasing like <i>1/sqrt(n_acc)</i>, where <i>n_acc</i> 
is the number of accepted events, should offer a reasonable estimate. 
 
<a name="method8"></a>
<p/><strong>void LHAup::setXSec( int i, double xSec) &nbsp;</strong> <br/>
update the <code>xSec</code> value of the <code>i</code>'th process 
added with <code>addProcess</code> method (i.e. <code>i</code> runs 
from 0 through <code>sizeProc() - 1</code>, see below). 
   
 
<a name="method9"></a>
<p/><strong>void LHAup::setXErr( int i, double xErr) &nbsp;</strong> <br/>
update the <code>xErr</code> value of the <code>i</code>'th process 
added with <code>addProcess</code> method. 
   
 
<a name="method10"></a>
<p/><strong>void LHAup::setXMax( int i, double xMax) &nbsp;</strong> <br/>
update the <code>xMax</code> value of the <code>i</code>'th process 
added with <code>addProcess</code> method. 
   
 
<a name="method11"></a>
<p/><strong>void LHAup::setInfoHeader(string &key, string &val) &nbsp;</strong> <br/>
set the header <code>key</code> to have value <code>val</code>. 
This is a wrapper function to the 
<?php $filepath = $_GET["filepath"];
echo "<a href='EventInformation.php?filepath=".$filepath."' target='page'>";?>Info::setHeader</a> function that 
should be used in any classes derived from LHAup. 
   
 
<p/> 
Information is handed back by the following methods 
(that normally you would not need to touch): 
<a name="method12"></a>
<p/><strong>int LHAup::idBeamA() &nbsp;</strong> <br/>
   
<strong>int LHAup::idBeamB() &nbsp;</strong> <br/>
   
<strong>double LHAup::eBeamA() &nbsp;</strong> <br/>
   
<strong>double LHAup::eBeamB() &nbsp;</strong> <br/>
   
<strong>int LHAup::pdfGroupBeamA() &nbsp;</strong> <br/>
   
<strong>int LHAup::pdfGroupBeamB() &nbsp;</strong> <br/>
   
<strong>int LHAup::pdfSetBeamA() &nbsp;</strong> <br/>
   
<strong>int LHAup::pdfSetBeamB() &nbsp;</strong> <br/>
for the beam properties. 
   
<a name="method13"></a>
<p/><strong>int LHAup::strategy() &nbsp;</strong> <br/>
for the strategy choice. 
   
<a name="method14"></a>
<p/><strong>int LHAup::sizeProc() &nbsp;</strong> <br/>
for the number of subprocesses. 
   
<a name="method15"></a>
<p/><strong>int LHAup::idProcess(i) &nbsp;</strong> <br/>
   
<strong>double LHAup::xSec(i) &nbsp;</strong> <br/>
   
<strong>double LHAup::xErr(i) &nbsp;</strong> <br/>
   
<strong>double LHAup::xMax(i) &nbsp;</strong> <br/>
for process <code>i</code> in the range <code>0 &lt;= i &lt; 
sizeProc()</code>. 
 
   
<a name="method16"></a>
<p/><strong>double LHAup::xSecSum() &nbsp;</strong> <br/>
   
<strong>double LHAup::xErrSum() &nbsp;</strong> <br/>
the sum of the cross sections and errors (the latter added quadratically). 
Note that cross section errors are only meaningful for strategies +-3. 
   
 
<a name="method17"></a>
<p/><strong>void LHAup::listInit(ostream& os = cout) &nbsp;</strong> <br/>
prints the above initialization information. This method is 
automatically called from <code>Pythia::init(...)</code>, 
so would normally not need to be called directly by the user. 
   
 
<p/> 
 
 
<h3>Event input</h3> 
 
The <code>LHAup</code> class also stores information equivalent to the 
<code>/HEPEUP/</code> commonblock, as required to hand in the next 
parton-level configuration for complete event generation. The main 
difference is that the vector container now allows a flexible number 
of partons to be defined. For the rest, names have been modified, 
since the 6-character-limit does not apply, and variables have been 
regrouped for clarity, but nothing fundamental is changed. 
 
<p/> 
The LHA standard is based on Fortran arrays beginning with 
index 1, and mother information is defined accordingly. In order to 
be compatible with this convention, the zeroth line of the C++ particle 
array is kept empty, so that index 1 also here corresponds to the first 
particle. One small incompatibility is that the <code>sizePart()</code> 
method returns the full size of the particle array, including the 
empty zeroth line, and thus is one larger than the true number of 
particles (<code>NUP</code>). 
 
<a name="method18"></a>
<p/><strong>virtual bool LHAup::setEvent(int idProcess = 0, double mRecalculate = -1.) &nbsp;</strong> <br/>
this pure virtual method has to be implemented in the derived class, 
to set relevant information when called. For strategy options +-1 
and +-2 the input <code>idProcess</code> value specifies which process 
that should be generated, while <code>idProcess</code> is irrelevant 
for strategies +-3 and +-4. The <code>mRecalculate</code> input alllows 
an optional recalculation of the mass from the four-momentum, see 
<code>LesHouches::mRecalculate</code> below. The method should return 
false if it fails to set the info, i.e. normally that the supply of 
events in a file is exhausted. If so, no event is generated, and 
<code>Pythia::next()</code> returns false. You can then interrogate 
<code><?php $filepath = $_GET["filepath"];
echo "<a href='EventInformation.php?filepath=".$filepath."' target='page'>";?>Info::atEndOfFile()</a></code> 
to confirm that indeed the failure is caused in this method, and decide 
to break out of the event generation loop. 
 
<p/> 
Inside a normal <code>setEvent(...)</code> call, information can be set 
by the following methods: 
<a name="method19"></a>
<p/><strong>void LHAup::setProcess( int idProcess, double weight, double scale, double alphaQED, double alphaQCD) &nbsp;</strong> <br/>
tells which kind of process occurred, with what weight, at what scale, 
and which <i>alpha_EM</i> and <i>alpha_strong</i> were used 
(cf. <code>IDPRUP, XWTGUP, SCALUP, AQEDUP, AQCDUP</code>). This method 
also resets the size of the particle list, and adds the empty zeroth 
line, so it has to be called before the <code>addParticle</code> method below. 
   
<a name="method20"></a>
<p/><strong>void LHAup::addParticle( int id, int status, int mother1, int mother2, int colourTag1, int colourTag2, double p_x, double p_y, double p_z, double e, double m, double tau, double spin, double scale) &nbsp;</strong> <br/>
gives the properties of the next particle handed in (cf. <code>IDUP, ISTUP, 
MOTHUP(1,..), MOTHUP(2,..), ICOLUP(1,..), ICOLUP(2,..),  PUP(J,..), 
VTIMUP, SPINUP</code>; while <code>scale</code> is a new optional property, 
see further below) . 
   
 
<p/> 
Information is handed back by the following methods: 
<a name="method21"></a>
<p/><strong>int LHAup::idProcess() &nbsp;</strong> <br/>
process number. 
   
 
<a name="method22"></a>
<p/><strong>double LHAup::weight() &nbsp;</strong> <br/>
Note that the weight stored in <code>Info::weight()</code> as a rule 
is not the same as the above <code>weight()</code>: the method here gives 
the value before unweighting while the one in <code>info</code> gives 
the one after unweighting and thus normally is 1 or -1. Only with strategy 
options +-3 and +-4 would the value in <code>info</code> be the same as 
here, except for a conversion from pb to mb for +-4. 
   
 
<a name="method23"></a>
<p/><strong>double LHAup::scale() &nbsp;</strong> <br/>
   
<strong>double LHAup::alphaQED() &nbsp;</strong> <br/>
   
<strong>double LHAup::alphaQCD() &nbsp;</strong> <br/>
scale and couplings at that scale. 
   
 
<a name="method24"></a>
<p/><strong>int LHAup::sizePart() &nbsp;</strong> <br/>
the size of the particle array, which is one larger than the number 
of particles in the event, since the zeroth entry is kept empty 
(see above). 
   
 
<a name="method25"></a>
<p/><strong>int LHAup::id(int i) &nbsp;</strong> <br/>
   
<strong>int LHAup::status(int i) &nbsp;</strong> <br/>
   
<strong>int LHAup::mother1(int i) &nbsp;</strong> <br/>
   
<strong>int LHAup::mother2(int i) &nbsp;</strong> <br/>
   
<strong>int LHAup::col1(int i) &nbsp;</strong> <br/>
   
<strong>int LHAup::col2(int i) &nbsp;</strong> <br/>
   
<strong>double LHAup::px(int i) &nbsp;</strong> <br/>
   
<strong>double LHAup::py(int i) &nbsp;</strong> <br/>
   
<strong>double LHAup::pz(int i) &nbsp;</strong> <br/>
   
<strong>double LHAup::e(int i) &nbsp;</strong> <br/>
   
<strong>double LHAup::m(int i) &nbsp;</strong> <br/>
   
<strong>double LHAup::tau(int i) &nbsp;</strong> <br/>
   
<strong>double LHAup::spin(int i) &nbsp;</strong> <br/>
   
<strong>double LHAup::scale(int i) &nbsp;</strong> <br/>
for particle <code>i</code> in the range 
<code>0 &lt;= i &lt; sizePart()</code>. (But again note that 
<code>i = 0</code> is an empty line, so the true range begins at 1.) 
   
 
<p/> 
From the information in the event record it is possible to set 
the flavour and <i>x</i> values of the initiators 
<a name="method26"></a>
<p/><strong>void LHAup::setIdX(int id1, int id2, double x1, double x2) &nbsp;</strong> <br/>
   
 
<p/> 
This information is returned by the methods 
<a name="method27"></a>
<p/><strong>int LHAup::id1() &nbsp;</strong> <br/>
   
<strong>int LHAup::id2() &nbsp;</strong> <br/>
   
<strong>double LHAup::x1() &nbsp;</strong> <br/>
   
<strong>double LHAup::x2() &nbsp;</strong> <br/>
   
 
<p/> 
In the LHEF description [<a href="Bibliography.php" target="page">Alw06</a>] an extension to 
include information on the parton densities of the colliding partons 
is suggested. This optional further information can be set by 
<a name="method28"></a>
<p/><strong>void LHAup::setPdf( int id1pdf, int id2pdf, double x1pdf, double x2pdf, double scalePDF, double pdf1, double pdf2, bool pdfIsSet) &nbsp;</strong> <br/>
which gives the flavours , the <i>x</i> and the <ie>Q</i> scale 
(in GeV) at which the parton densities <i>x*f_i(x, Q)</i> have been 
evaluated. The last argument is normally <code>true</code>. 
   
 
<p/> 
This information is returned by the methods 
<a name="method29"></a>
<p/><strong>bool LHAup::pdfIsSet() &nbsp;</strong> <br/>
   
<strong>int LHAup::id1pdf() &nbsp;</strong> <br/>
   
<strong>int LHAup::id2pdf() &nbsp;</strong> <br/>
   
<strong>double LHAup::x1pdf() &nbsp;</strong> <br/>
   
<strong>double LHAup::x2pdf() &nbsp;</strong> <br/>
   
<strong>double LHAup::scalePDF() &nbsp;</strong> <br/>
   
<strong>double LHAup::pdf1() &nbsp;</strong> <br/>
   
<strong>double LHAup::pdf2() &nbsp;</strong> <br/>
where the first one tells whether this optional information has been set 
for the current event. (<code>setPdf(...)</code> must be called after the 
<code>setProcess(...)</code> call of the event for this to work.) 
Note that the flavour and <i>x</i> values usually but not always 
agree with those obtained by the same methods without <code>pdf</code> 
in their names, see explanation in the 
<?php $filepath = $_GET["filepath"];
echo "<a href='EventInformation.php?filepath=".$filepath."' target='page'>";?>Event Information</a> description. 
   
 
<p/> 
As a further, currently completely non-standard feature, it is also 
possible to read in the separate scale values of all final particles. 
Currently such information should appear as a line begun by a single 
hashtag character (#) and then sequentially the scale value of all 
particles with status 1. Such scale values could be used e.g. to 
restrict the maximum scale for shower evolutions for each parton 
separately. This information is returned by the method 
<code>double LHAup::scale(int i)</code>. When no such information 
has been read from the LHEF, the scale defaults to -1. 
 
<p/> 
<a name="method30"></a>
<p/><strong>void LHAup::listEvent(ostream& os = cout) &nbsp;</strong> <br/>
prints the above information for the current event.  In cases where the 
<code>LHAup</code> object is not available to the user, the 
<code>Pythia::LHAeventList(ostream& os = cout)</code> method can 
be used, which is a wrapper for the above. 
   
 
<a name="method31"></a>
<p/><strong>virtual bool LHAup::skipEvent(int nSkip) &nbsp;</strong> <br/>
skip ahead <code>nSkip</code> events in the Les Houches generation 
sequence, without doing anything further with them. Mainly 
intended for debug purposes, e.g. when an event at a known 
location in a Les Houches Event File is causing problems. 
Will return false if operation fails, specifically if the 
end of an LHEF has been reached. The implementation in the base class 
simply executes <code>setEvent()</code> the requested number of times. 
The derived <code>LHAupLHEF</code> class (see below) only uses the 
<code>setNewEventLHEF(...)</code> part of its <code>setEvent()</code> 
method, and other derived classes could choose other shortcuts. 
   
 
<p/> 
The LHA expects the decay of resonances to be included as part of the 
hard process, i.e. if unstable particles are produced in a process then 
their decays are also described. This includes <i>Z^0, W^+-, H^0</i> 
and other short-lived particles in models beyond the Standard Model. 
Should this not be the case then PYTHIA will perform the decays of all 
resonances it knows how to do, in the same way as for internal processes. 
Note that you will be on slippery ground if you then restrict the decay of 
these resonances to specific allowed channels since, if this is not what 
was intended, you will obtain the wrong cross section and potentially the 
wrong mix of different event types. (Since the original intention is 
unknown, the cross section will not be corrected for the fraction of 
open channels, i.e. the procedure used for internal processes is not 
applied in this case.) 
 
<p/> 
Even if PYTHIA can select resonance decay modes according to its 
internal tables, there is normally no way for it to know which 
decay angular correlations should exist in the simulated process. 
Therefore almost all decays are isotropic. The exceptions are Higgs and 
top decays, in the decay chains <i>H &rarr; WW/ZZ &rarr; f fbar f' fbar'</i> 
and <i>t &rarr; b W &rarr; b f fbar</i>, where the process-independent 
correlations implemented for internal processes are used. If part of 
the decay chain has already been set, however (e.g. <i>H &rarr; WW/ZZ</i> 
or <i>t &rarr; b W</i>), then decay is still isotropic. 
 
<p/> 
There are two settings available for event input. 
 
<br/><br/><table><tr><td><strong>LesHouches:idRenameBeams  </td><td></td><td> <input type="text" name="1" value="1000022" size="20"/>  &nbsp;&nbsp;(<code>default = <strong>1000022</strong></code>; <code>minimum = 0</code>)</td></tr></table>
PYTHIA only implements a certain number of incoming beam particles.
Specifically it needs to have PDFs for every composite particle to
be used. Sometimes exotic beam particles are used, e.g. when a 
neutralino is supposed to be the Dark Matter particle and therefore
neutralino pairs can collide and annihilate. Such a particle identity
code, picked by this mode, is mapped onto an incoming tau neutrino 
beam (or antineutrino for the second beam), to bring it to a familiar 
situation. The trick cannot be used for composite particles, nor for 
a pair of different particles. 
   
 
<br/><br/><table><tr><td><strong>LesHouches:setLifetime  </td><td>  &nbsp;&nbsp;(<code>default = <strong>1</strong></code>; <code>minimum = 0</code>; <code>maximum = 2</code>)</td></tr></table>
handling of the decay time information stored in <code>VTIMUP</code> 
when the Les Houches event record is stored into the PYTHIA 
<code>process</code> one. The reason is that some matrix-element 
generators (like POWHEG) do not set decay times, so that it is up to 
PYTHIA to make that selection. This is particularly important for 
the <ei>tau</ei> lepton. 
<br/>
<input type="radio" name="2" value="0"><strong>0 </strong>:  all decay times are taken from the Les Houches input.  <br/>
<input type="radio" name="2" value="1" checked="checked"><strong>1 </strong>:  the decay time of <ei>tau</ei> leptons is generated  like for internal PYTHIA <ei>tau</ei>s, whereas all other decay times  are taken from the Les Houches input.  <br/>
<input type="radio" name="2" value="2"><strong>2 </strong>:  all decay times are generated by PYTHIA, thus  completely disregarding the Les Houches values. This option could  go wrong in BSM scenarios with long-lived particles, if PYTHIA  has not been provided with the information to select those lifetimes  correctly.  <br/>
 
<p/> 
There is also one parameter that can be used to modify the setting 
of event information, if implemented in the derived class. It is 
prompted by an unforeseen choice made in some programs (like CalcHEP) 
of storing the nominal mass of a particle species rather than the 
mass of the current member of that species. This is likely to 
induce energy-momentum nonconservation when the event is further 
processed. 
 
<br/><br/><table><tr><td><strong>LesHouches:mRecalculate </td><td></td><td> <input type="text" name="3" value="-1." size="20"/>  &nbsp;&nbsp;(<code>default = <strong>-1.</strong></code>)</td></tr></table>
Does not have any effect by default, or more generally when it is negative. 
If it is positive then all particles with an input mass above this 
value will have the mass recalculated and reset from the four-momentum, 
<i>m^2 = E^2 - p^2</i>. Obviously such a step is problematic 
numerically for light particles, so it should only be used for the 
programs and particles where it is needed. Thus the value ought to be 
at least 10 GeV, so that only massive particles like <i>W^+-</i>, 
<i>Z^0</i> and <i>t</i> are affected. 
   
 
<h3>An interface to Les Houches Event Files</h3> 
 
The LHEF standard [<a href="Bibliography.php" target="page">Alw06</a>] specifies a format where a single file 
packs initialization and event information. This has become the most 
frequently used procedure to process external parton-level events in 
Pythia. Therefore a special 
<code><?php $filepath = $_GET["filepath"];
echo "<a href='ProgramFlow.php?filepath=".$filepath."' target='page'>";?>Pythia::init(fileName)</a></code> 
initialization option exists, where the LHEF name is provided as input. 
Internally this name is then used to create an instance of the derived 
class <code>LHAupLHEF</code>, which can do the job of reading an LHEF. 

<p/> 
The two key compulsory parts of an LHEF is the initialization information
stored in an init block, enclosed by a matching <code>&lt;init&gt;</code>
- <code>&lt;/init&gt;</code> pair of lines, and the event input, with each 
event enclosed by a matching <code>&lt;event&gt;</code> - 
<code>&lt;/event&gt;</code> pair of lines. In the case of the no-beams 
extension the init block may be empty, but the <code>&lt;init&gt;</code>
and <code>&lt;/init&gt;</code> lines must be included for the file parsing
to work as expected. It is also possible to have a non-empty init block, 
with the beams assigned code 0, and optionally a number of specified 
"processes".
 
<p/> 
The LHEF reader can also read in and store header blocks. By default 
this option is switched on, but may be controlled through the 
<?php $filepath = $_GET["filepath"];
echo "<a href='BeamParameters.php?filepath=".$filepath."' target='page'>";?>Beams:readLHEFheaders</a> flag if 
necessary. The information can later be read out through the 
<?php $filepath = $_GET["filepath"];
echo "<a href='EventInformation.php?filepath=".$filepath."' target='page'>";?>Info</a> class for further processing. 
Due to the non-standard nature of the information in these blocks they 
are stored whole, and PYTHIA itself makes no further attempt to process 
their meaning. 
 
<p/> 
Because Les Houches Event files tend not to adhere strictly to XML 
conventions, to consistently read in header information, certain 
choices must be made. The primary goal is to make as much information 
available as possible. First, information sitting directly in the 
&lt;header&gt; block is stored under the key "base". Second, the tags 
starting and ending each sub block must be on their own line. Finally, 
the contents of comment blocks, &lt;!-- --&gt;, are still stored. The 
header keys are formed hierarchically from the names of the header 
blocks. This behaviour is illustrated in the following example: 
<pre> 
  &lt;header&gt; 
    BaseA 
    &lt;hblock1&gt; 
      1A 
      &lt;hblock11&gt; 
        11A &lt;hblock111&gt; 
        &lt;/hblock111&gt; 11B 
      &lt;/hblock11&gt; 
      1B 
    &lt;/hblock1&gt; 
    &lt;hblock2&gt; 
      2A 
      &lt;!-- 2B --&gt; 
    &lt;/hblock2&gt; 
    BaseB 
  &lt;/header&gt; 
</pre> 
which would lead to the following information being stored in the 
<?php $filepath = $_GET["filepath"];
echo "<a href='EventInformation.php?filepath=".$filepath."' target='page'>";?>Info</a> class: 
<table border="1"> 
  <tr> <th>Key</th> <th>Value</th> </tr> 
  <tr> 
    <td>base</td> 
    <td>BaseA<br/>BaseB</td> 
  </tr> 
  <tr> 
    <td>hblock1</td> 
    <td>1A<br/>1B</td> 
  </tr> 
  <tr> 
    <td>hblock1.hblock11</td> 
    <td>11A &lt;hblock111&gt;<br/>&lt;/hblock111&gt; 11B</td> 
  </tr> 
  <tr> 
    <td>hblock2</td> 
    <td>2A<br/>&lt;!-- 2B --&gt;</td> 
  </tr> 
</table> 
<br/> 
<p/> 
Normally the LHEF would be in uncompressed format, and thus human-readable 
if opened in a text editor. A possibility to read gzipped files has 
been added, based on the Boost and zlib libraries, which therefore 
have to be linked appropriately in order for this option to work. 
See the <code>README</code> file in the main directory for details 
on how to do this. 
 
<p/> 
An example how to generate events from an LHEF is found in 
<code>main11.cc</code>. Note the use of 
<code>Info::atEndOfFile()</code> to find out when the whole 
LHEF has been processed. 
 
<p/> 
To allow the sequential use of several event files the 
<code>Pythia::init(...)</code> method has an optional second argument: 
<code>Pythia::init(fileName, bool skipInit = false)</code>. 
If called with this argument <code>true</code> then there will be no 
initialization, except that the existing <code>LHAupLHEF</code> class 
instance will be deleted and replaced by ones pointing to the new file. 
It is assumed (but never checked) that the initialization information is 
identical, and that the new file simply contains further events of 
exactly the same kind as the previous one. An example of this possibility, 
and the option to mix with internal processes, is found in 
<code>main12.cc</code>. A variant, based on input in a command file, 
is given in <code>main13.cc</code>. 
 
<p/> 
In C++, real numbers are printed with an 'E' to denote the exponent part, 
e.g. 1.23E+04, and are read in accordingly. Other languages may use other 
letters, e.g. Fortran allows either 'E' or 'D'. A file using 
the latter convention would not be readable by the standard routines. 
In case you have such an "incorrectly formatted" file, a conversion to 
a new corrected file could be done e.g. using <code>sed</code>, as a 
one-line command 
<pre> 
  sed -e 's/\([0-9]\.\{0,1\}\)[dD]\([+-]\{0,1\}[0-9]\)/\1E\2/g' old.lhe &gt; new.lhe 
</pre> 
This replaces a 'd' or 'D' with an 'E' only when it occurs in the combination 
<br/>
<i>(digit) ('.' or absent) ('d' or 'D') ('+', '-' or absent) (digit)</i> 
<br/>It will work on all parts of the file, also inside a 
<code>&lt;header&gt;...&lt;/header&gt;</code> block. For conversion only 
inside the <code>&lt;init&gt;...&lt;/init&gt;</code> and 
<code>&lt;event&gt;...&lt;/event&gt;</code> blocks, create a file 
<code>convert.sed</code> containing 
<pre> 
  /&lt;init&gt;/,/&lt;\/init&gt;/bconv 
  /&lt;event&gt;/,/&lt\/event&gt;/bconv 
  b 
  :conv 
  s/\([0-9]\.\{0,1\}\)[dD]\([+-]\{0,1\}[0-9]\)/\1E\2/g 
</pre> 
and run it with 
<pre> 
  sed -f convert.sed old.lhe &gt; new.lhe 
</pre> 
 
<p/> 
The workhorses of the <code>LHAupLHEF</code> class are three methods 
found in the base class, so as to allow them to be reused in other 
contexts. Specifically, it allows derived classes where one parton-level 
configuration can be reused several times, e.g. in the context of 
matrix-element-to-parton-shower matching (example in preparation). 
Also two small utility routines. 
 
<a name="method32"></a>
<p/><strong>bool LHAup::setInitLHEF(ifstream& is, bool readHeaders = false) &nbsp;</strong> <br/>
read in and set all required initialization information from the 
specified stream. With second argument true it will also read and store 
header information, as described above. Return false if it fails. 
   
 
<a name="method33"></a>
<p/><strong>bool LHAup::setNewEventLHEF(ifstream& is) &nbsp;</strong> <br/>
read in event information from the specified stream into a staging area 
where it can be reused by <code>setOldEventLHEF</code>. 
   
 
<a name="method34"></a>
<p/><strong>bool LHAup::setOldEventLHEF() &nbsp;</strong> <br/>
store the event information from the staging area into the normal 
location. Thus a single <code>setNewEventLHEF</code> call can be 
followed by several <code>setOldEventLHEF</code> ones, so as to 
process the same configuration several times. This method currently 
only returns true, i.e. any errors should be caught by the preceding 
<code>setNewEventLHEF</code> call. 
   
 
<a name="method35"></a>
<p/><strong>bool LHAup::fileFound() &nbsp;</strong> <br/>
always returns true in the base class, but in <code>LHAupLHEF</code> 
it returns false if the LHEF provided in the constructor is not 
found and opened correctly. 
   
 
<a name="method36"></a>
<p/><strong>void LHAup::setInfoHeader(const string &key, const string &val) &nbsp;</strong> <br/>
is used to send header information on to the <code>Info</code> class. 
   
 
<p/> 
A few other methods, most of them derived from the base class, 
streamlines file opening and closing, e.g. if several LHE files are 
to be read consecutively, without the need for a complete 
reinitialization. This presupposes that the events are of the same 
kind, only split e.g. to limit file sizes. 
 
<a name="method37"></a>
<p/><strong>bool LHAup::newEventFile(const char* fileIn) &nbsp;</strong> <br/>
close current event input file/stream and open a new one, to 
continue reading events of the same kind as before. 
   
 
<a name="method38"></a>
<p/><strong>istream* LHAup::openFile(const char *fn, ifstream &ifs) &nbsp;</strong> <br/>
   
<strong>void LHAup::closeFile(istream *&is, ifstream &ifs) &nbsp;</strong> <br/>
open and close a file, also gzip files, where an intermediate 
decompression layer is needed. 
   
 
<a name="method39"></a>
<p/><strong>void LHAupLHEF::closeAllFiles() &nbsp;</strong> <br/>
close main event file (LHEF) and, if present, separate header file. 
   
  
<h3>A runtime Fortran interface</h3> 
 
The runtime Fortran interface requires linking to an external Fortran 
code. In order to avoid problems with unresolved external references 
when this interface is not used, the code has been put in a separate 
<code>LHAFortran.h</code> file, that is not included in any of the 
other library files. Instead it should be included in the 
user-supplied main program, together with the implementation of two 
methods below that call the Fortran program to do its part of the job. 
 
<p/> 
The <code>LHAupFortran</code> class derives from <code>LHAup</code>. 
It reads initialization and event information from the LHA standard 
Fortran commonblocks, assuming these commonblocks behave like two 
<code>extern "C" struct</code> named <code>heprup_</code> and 
<code>hepeup_</code>. (Note the final underscore, to match how the 
gcc compiler internally names Fortran files.) 
 
<p/> 
The instantiation does not require any arguments. 
 
<p/> 
The user has to supply implementations of the <code>fillHepRup()</code> 
and <code>fillHepEup()</code> methods, that is to do the actual calling 
of the external Fortran routines that fill the <code>HEPRUP</code> and 
<code>HEPEUP</code> commonblocks. The translation of this information to 
the C++ structure is provided by the existing <code>setInit()</code> and 
<code>setEvent()</code> code. 
 
<p/> 
Up to and including version 8.125 the <code>LHAupFortran</code> class 
was used to construct a runtime interface to PYTHIA 6.4. This was 
convenient in the early days of PYTHIA 8 evolution, when this program 
did not yet contain hard-process generation, and the LHEF standard 
did not yet exist. Nowadays it is more of a bother, since a full 
cross-platform support leads to many possible combinations. Therefore 
this support has been removed, but can still be recuperated from 
previous code versions, in a reduced form up to version 8.176. 
 
<h3>Methods for LHEF output</h3> 
 
The main objective of the <code>LHAup</code> class is to feed information 
from an external program into PYTHIA. It can be used to export information 
as well, however. Specifically, there are four routines in the base class 
that can be called to write a Les Houches Event File. These should be 
called in sequence in order to build up the proper file structure. 
 
<a name="method40"></a>
<p/><strong>bool LHAup::openLHEF(string filename) &nbsp;</strong> <br/>
Opens a file with the filename indicated, and writes a header plus a brief 
comment with date and time information. 
   
 
<a name="method41"></a>
<p/><strong>bool LHAup::initLHEF() &nbsp;</strong> <br/>
Writes initialization information to the file above. Such information should 
already have been set with the methods described in the "Initialization" 
section above. 
   
 
<a name="method42"></a>
<p/><strong>bool LHAup::eventLHEF(bool verbose = true) &nbsp;</strong> <br/>
Writes event information to the file above. Such information should 
already have been set with the methods described in the "Event input" 
section above. This call should be repeated once for each event to be 
stored. By default the event information is lined up in columns. 
To save space, the alternative <code>verbose = false</code> only 
leaves a single blank between the information fields. 
   
 
<a name="method43"></a>
<p/><strong>bool LHAup::closeLHEF(bool updateInit = false) &nbsp;</strong> <br/>
Writes the closing tag and closes the file. Optionally, if 
<code>updateInit = true</code>, this routine will reopen the file from 
the beginning, rewrite the same header as <code>openLHEF()</code> did, 
and then call <code>initLHEF()</code> again to overwrite the old 
information. This is especially geared towards programs, such as PYTHIA 
itself, where the cross section information is not available at the 
beginning of the run, but only is obtained by Monte Carlo integration 
in parallel with the event generation itself. Then the 
<code>setXSec( i, xSec)</code>, <code>setXErr( i, xSec)</code> and 
<code>setXMax( i, xSec)</code> can be used to update the relevant 
information before <code>closeLHEF</code> is called. 
<br/><b>Warning:</b> overwriting the beginning of a file without 
upsetting anything is a delicate operation. It only works when the new 
lines require exactly as much space as the old ones did. Thus, if you add 
another process in between, the file will be corrupted. 
   
 
<a name="method44"></a>
<p/><strong>string LHAup::getFileName() &nbsp;</strong> <br/>
Return the name of the LHE file above.
   
 
<h3>PYTHIA 8 output to an LHEF</h3> 
 
The above methods could be used by any program to write an LHEF. 
For PYTHIA 8 to do this, a derived class already exists, 
<code>LHAupFromPYTHIA8</code>. In order for it to do its job, 
it must gain access to the information produced by PYTHIA, 
specifically the <code>process</code> event record and the 
generic information stored in <code>info</code>. Therefore, if you 
are working with an instance <code>pythia</code> of the 
<code>Pythia</code> class, you have to instantiate 
<code>LHAupFromPYTHIA8</code> with pointers to the 
<code>process</code> and <code>info</code> objects of 
<code>pythia</code>: 
<br/><code>LHAupFromPYTHIA8 myLHA(&pythia.process, &pythia.info);</code> 
 
<p/> 
The method <code>setInit()</code> should be called to store the 
<code>pythia</code> initialization information in the LHA object, 
and <code>setEvent()</code> to store event information. 
Furthermore, <code>updateSigma()</code> can be used at the end 
of the run to update cross-section information, cf. 
<code>closeLHEF(true)</code> above. An example how the 
generation, translation and writing methods should be ordered is 
found in <code>main20.cc</code>. 
 
<p/> 
Currently there are some limitations, that could be overcome if 
necessary. Firstly, you may mix many processes in the same run, 
but the cross-section information stored in <code>info</code> only 
refers to the sum of them all, and therefore they are all classified 
as a common process 9999. Secondly, you should generate your events 
in the CM frame of the collision, since this is the assumed frame of 
stored Les Houches events, and no boosts have been implemented 
for the case that <code>Pythia::process</code> is not in this frame. 
 
<p/> 
The LHEF standard is the agreed format to store the particles of a 
hard process, as input to generators, whereas output of final states 
is normally handled using the <?php $filepath = $_GET["filepath"];
echo "<a href='HepMCInterface.php?filepath=".$filepath."' target='page'>";?>HepMC</a> 
standard. It is possible to use LHEF also here, however. It requires 
that the above initialization is replaced by 
<br/><code>LHAupFromPYTHIA8 myLHA(&pythia.event, &pythia.info);</code> 
<br/> i.e. that <code>process</code> is replaced by <code>event</code>. 
In addition, the <code>PartonLevel:all = off</code> command found in 
<code>main20.cc</code> obviously must be removed if one wants to 
obtain complete events. 
  
<input type="hidden" name="saved" value="1"/>

<?php
echo "<input type='hidden' name='filepath' value='".$_GET["filepath"]."'/>"?>

<table width="100%"><tr><td align="right"><input type="submit" value="Save Settings" /></td></tr></table>
</form>

<?php

if($_POST["saved"] == 1)
{
$filepath = $_POST["filepath"];
$handle = fopen($filepath, 'a');

if($_POST["1"] != "1000022")
{
$data = "LesHouches:idRenameBeams = ".$_POST["1"]."\n";
fwrite($handle,$data);
}
if($_POST["2"] != "1")
{
$data = "LesHouches:setLifetime = ".$_POST["2"]."\n";
fwrite($handle,$data);
}
if($_POST["3"] != "-1.")
{
$data = "LesHouches:mRecalculate = ".$_POST["3"]."\n";
fwrite($handle,$data);
}
fclose($handle);
}

?>
</body>
</html>
 
<!-- Copyright (C) 2014 Torbjorn Sjostrand --> 
