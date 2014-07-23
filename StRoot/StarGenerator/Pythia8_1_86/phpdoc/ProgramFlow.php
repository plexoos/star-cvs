<html>
<head>
<title>Program Flow</title>
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

<form method='post' action='ProgramFlow.php'>
 
<h2>Program Flow</h2> 
 
Recall that, to first order, the event generation process can be 
subdivided into three stages: 
<ol> 
<li>Initializaion.</li> 
<li>The event loop.</li> 
<li>Finishing.</li> 
</ol> 
This is reflected in how the top-level <code>Pythia</code> class should 
be used in the user-supplied main program, further outlined in the 
following. Since the nature of the run is defined at the initialization 
stage, this is where most of the PYTHIA user code has to be written. 
So as not to confuse the reader unduly, the description of initialization 
options has been subdivided into what would normally be used and what is 
intended for more special applications. 
 
<p/> 
At the bottom of this webpage is a complete survey of all public 
<code>Pythia</code> methods and data members, in a more formal style 
than the task-oriented descriptions found in the preceding sections. 
This offers complementary information. 
    
<h3>Initialization - normal usage</h3> 
 
<ol> 
 
<li> 
Already at the top of the main program file, you need to include the proper 
header file 
<pre> 
    #include "Pythia8/Pythia.h" 
</pre> 
To simplify typing, it also makes sense to declare 
<pre> 
    using namespace Pythia8; 
</pre> 
</li> 
 
<p/> 
<li> 
The first step is to create a generator object, 
e.g. with 
<pre> 
     Pythia pythia; 
</pre> 
It is this object that we will use from now on. Normally a run 
will only contain one <code>Pythia</code> object. (But you can 
use several <code>Pythia</code> objects, which then will be 
independent of each other.)<br/> 
By default all output from <code>Pythia</code> will be on the 
<code>cout</code> stream, but the <code>list</code> methods below do 
allow output to alternative streams or files. 
</li> 
 
<p/> 
<li> 
You next want to set up the character of the run. 
The pages under the "Setup Run Tasks" heading in the index 
describe all the options available (with some very few exceptions, 
found on the other pages). 
The default values and your modifications are stored in two databases, 
one for <?php $filepath = $_GET["filepath"];
echo "<a href='SettingsScheme.php?filepath=".$filepath."' target='page'>";?>generic settings</a> 
and one for <?php $filepath = $_GET["filepath"];
echo "<a href='ParticleDataScheme.php?filepath=".$filepath."' target='page'>";?>particle data</a>. 
Both of these are initialized with their default values by the 
<code>Pythia</code> constructor. The default values can then be 
changed, primarily by one of the two ways below, or by a combination 
of them. 
 
<p/> 
a) You can use the 
<pre> 
    pythia.readString(string); 
</pre> 
method repeatedly to do a change of a property at a time. 
The information in the string is case-insensitive, but upper- and 
lowercase can be combined for clarity. The rules are that<br/> 
(i) if the first nonblank character of the string is a letter 
it is assumed to contain a setting, and is sent on to 
<code>pythia.settings.readString(string)</code>;<br/> 
(ii) if instead the string begins with a digit it is assumed to 
contain particle data updates, and so sent on to 
<code>pythia.particleData.readString(string)</code>;<br/> 
(iii) if none of the above, the string is assumed to be a comment, 
i.e. nothing will be done.<br/> 
In the former two cases, a warning is issued whenever a string 
cannot be recognized (maybe because of a spelling mistake).<br/> 
Some examples would be 
<pre> 
    pythia.readString("TimeShower:pTmin = 1.0"); 
    pythia.readString("111:mayDecay = false"); 
</pre> 
The <code>readString(string)</code> method is intended primarily for 
a few changes. It can also be useful if you want to construct a 
parser for input files that contain commands both to PYTHIA and to 
other libraries.<br/> 
 
<p/> 
b) You can read in a file containing a list of those variables 
you want to see changed, with a 
<pre> 
    pythia.readFile(fileName); 
</pre> 
Each line in this file with be processes by the 
<code>readString(string)</code> method introduced above. You can thus 
freely mix comment lines and lines handed on to <code>Settings</code> 
or to <code>ParticleData</code>.<br/> 
This approach is better suited for more extensive changes than a direct 
usage of <code>readString(string)</code>, and can also avoid having to 
recompile and relink your main program between runs.<br/> 
It is also possible to read input from an <code>istream</code>, by 
default <code>cin</code>, rather than from a file. This may be convenient 
if information is generated on-the-fly, within the same run. 
 
<p/> 
Changes are made sequentially in the order the commands are encountered 
during execution, meaning that if a parameter is changed several times 
it is the last one that counts. The two special 
<code><?php $filepath = $_GET["filepath"];
echo "<a href='Tunes.php?filepath=".$filepath."' target='page'>";?>Tune:ee</a></code> and 
<code><?php $filepath = $_GET["filepath"];
echo "<a href='Tunes.php?filepath=".$filepath."' target='page'>";?>Tune:pp</a></code> 
modes are expanded to change several settings in one go, but these obey 
the same ordering rules. 
<br/> 
</li> 
 
<p/> 
<li> 
Next comes the initialization stage, where all 
remaining details of the generation are to be specified. 
There is one standard method to use for this 
 
<p/> 
<code>pythia.init();</code><br/> 
with no arguments will read all relevant information from the 
<code><?php $filepath = $_GET["filepath"];
echo "<a href='SettingsScheme.php?filepath=".$filepath."' target='page'>";?>Settings</a></code> 
and <code><?php $filepath = $_GET["filepath"];
echo "<a href='ParticleDataScheme.php?filepath=".$filepath."' target='page'>";?>ParticleData</a></code> 
databases. Specifically the setup of incoming beams and energies 
is governed by the the beam parameters from the 
<code><?php $filepath = $_GET["filepath"];
echo "<a href='BeamParameters.php?filepath=".$filepath."' target='page'>";?>Beams</a></code> 
group of variables. If you don't change any of those you will 
default to proton-proton collisions at 14 TeV, i.e. the nominal LHC 
values. 
 
<p/> 
A few alternative forms are available, where the arguments of the 
<code>init(...)</code> call can be used to set the beam parameters. 
These alternatives are now deprecated, and will be removed for 
PYTHIA 8.2. 
 
<p/> 
a) <code>pythia.init( idA, idB, eCM);</code><br/> 
lets you specify the identities and the CM energy of the two incoming 
beam particles, with A (B) assumed moving in the <i>+z (-z)</i> 
direction. 
 
<p/> 
b) <code>pythia.init( idA, idB, eA, eB);</code><br/> 
is similar, but the two beam energies can be different, so the 
collisions do not occur in the CM frame. If one of the beam energies 
is below the particle mass you obtain a fixed-target topology. 
 
<p/> 
c) <code>pythia.init( idA, idB, pxA, pyA, pzA, pxB, pyB, pzB);</code><br/> 
is similar, but here you provide the three-momenta 
<i>(p_x, p_y, p_z)</i> of the two incoming particles, 
to allow for arbitrary beam directions. 
 
<p/> 
d) <code>pythia.init(fileName);</code> <br/> 
assumes a file in the <?php $filepath = $_GET["filepath"];
echo "<a href='LesHouchesAccord.php?filepath=".$filepath."' target='page'>";?>Les Houches 
Event File</a> format is provided. 
 
<p/> 
e) <code>pythia.init( LHAup*);</code> <br/> 
assumes <?php $filepath = $_GET["filepath"];
echo "<a href='LesHouchesAccord.php?filepath=".$filepath."' target='page'>";?>Les Houches Accord</a> 
initialization and event information is available in an <code>LHAup</code> 
class object, and that a pointer to this object is handed in. 
 
<p/> 
<li> 
If you want to have a list of the generator and particle data used, 
either only what has been changed or everything, you can use 
<pre> 
    pythia.settings.listChanged(); 
    pythia.settings.listAll(); 
    pythia.particleData.listChanged(); 
    pythia.particleData.listAll(); 
</pre> 
</li> 
 
</ol> 
 
<h3>The event loop</h3> 
 
<ol> 
 
<li> 
Inside the event generation loop you generate the 
next event using the <code>next()</code> method, 
<pre> 
    pythia.next(); 
</pre> 
This method takes no arguments; everything has already been specified. 
It does return a bool value, however, <code>false</code> when the 
generation failed. This can be a "programmed death" when the 
supply of input parton-level configurations on file is exhausted. 
It can alternatively signal a failure of <code>Pythia</code> to 
generate an event, or unphysical features in the event record at the 
end of the generation step. It makes sense to allow a few <code>false</code> 
values before a run is aborted, so long as the related faulty 
events are skipped. 
</li> 
  
<p/> 
<li> 
The generated event is now stored in the <code>event</code> 
object, of type <code><?php $filepath = $_GET["filepath"];
echo "<a href='EventRecord.php?filepath=".$filepath."' target='page'>";?>Event</a></code>, 
which is a public member of <code>pythia</code>. You therefore have 
access to all the tools described on the pages under the "Study Output" 
header in the index. For instance, an event can be listed with 
<code>pythia.event.list()</code>, the identity of the <i>i</i>'th 
<?php $filepath = $_GET["filepath"];
echo "<a href='ParticleProperties.php?filepath=".$filepath."' target='page'>";?>particle</a> is given by 
<code>pythia.event[i].id()</code>, and so on.<br/> 
The hard process - roughly the information normally stored in the 
Les Houches Accord event record - is available as a second object, 
<code>process</code>, also of type <code>Event</code>.<br/> 
A third useful public object is 
<code><?php $filepath = $_GET["filepath"];
echo "<a href='EventInformation.php?filepath=".$filepath."' target='page'>";?>info</a></code>, which offers 
a set of one-of-a kind pieces of information about the most recent 
event. 
</li> 
 
</ol> 
 
<h3>Finishing</h3> 
 
<ol> 
 
<li>At the end of the generation process, you can call 
<pre> 
    pythia.stat(); 
</pre> 
to get some run statistics, on cross sections and the number of errors 
and warnings encountered. The alternative 
<code>pythia.statistics(...);</code> is equivalent but deprecated. 
</li> 
 
</ol> 
 
<h3>Advanced usage, mainly for initialization</h3> 
 
A) Necessary data are automatically loaded when you use the 
default PYTHIA installation directory structure and run the main 
programs in the <code>examples</code> subdirectory. However, in the 
general case, you must provide the path of the <code>xmldoc</code> 
directory, where default settings and particle data are found. 
This can be done in two ways. 
 
<ol> 
 
<li> 
You can set the environment variable <code>PYTHIA8DATA</code> to 
contain the location of the <code>xmldoc</code> directory. In the 
<code>csh</code> and <code>tcsh</code> shells this could e.g. be 
<pre> 
     setenv PYTHIA8DATA /home/myname/pythia81xx/xmldoc 
</pre> 
while in other shells it could be 
<pre> 
     export PYTHIA8DATA=/home/myname/pythia81xx/xmldoc 
</pre> 
where xx is the subversion number.<br/> 
Recall that environment variables set locally are only defined in the 
current instance of the shell. The above lines should go into your 
<code>.cshrc</code> and <code>.bashrc</code> files, respectively, 
if you want a more permanent assignment. 
</li> 
 
<p/> 
<li> 
You can provide the path as argument to the <code>Pythia</code> 
constructor, e.g. 
<pre> 
     Pythia pythia("/home/myname/pythia81xx/xmldoc"); 
</pre> 
</li> 
</ol> 
where again xx is the subversion number.<br/> 
When <code>PYTHIA8DATA</code> is set it takes precedence, else 
the path in the constructor is used, else one defaults to the 
<code>../xmldoc</code> directory. 
 
<p/> 
B) You can override the default behaviour of PYTHIA not only by the 
settings and particle data, but also by replacing some of the 
PYTHIA standard routines by ones of your own. Of course, this is only 
possible if your routines fit into the general PYTHIA framework. 
Therefore they must be coded according to the the rules relevant 
in each case, as a derived class of a PYTHIA base class, and a pointer 
to such an object must be handed in by one of the methods below. 
These calls must be made before the <code>pythia.init(...)</code> call. 
 
<ol> 
 
<li> 
If you are not satisfied with the list of parton density functions that 
are implemented internally or available via the LHAPDF interface 
(see the <?php $filepath = $_GET["filepath"];
echo "<a href='PDFSelection.php?filepath=".$filepath."' target='page'>";?>PDF Selection</a> page), you 
can supply your own by a call to the <code>setPDFPtr(...)</code> method 
<pre> 
      pythia.setPDFptr( pdfAPtr, pdfBPtr); 
</pre> 
where <code>pdfAPtr</code> and <code>pdfBPtr</code> are pointers to 
two <code>Pythia</code> <?php $filepath = $_GET["filepath"];
echo "<a href='PartonDistributions.php?filepath=".$filepath."' target='page'>";?>PDF 
objects</a>. Note that <code>pdfAPtr</code> and <code>pdfBPtr</code> 
cannot point to the same object; even if the PDF set is the same, 
two copies are needed to keep track of two separate sets of <i>x</i> 
and density values.<br/> 
If you further wish to use separate PDF's for the hard process of an 
event than the ones being used for everything else, the extended form 
<pre> 
      pythia.setPDFptr( pdfAPtr, pdfBPtr, pdfHardAPtr, pdfHardBPtr); 
</pre> 
allows you to specify those separately, and then the first two sets 
would only be used for the showers and for multiparton interactions. 
</li> 
 
<p/> 
<li> 
If you want to link to an external generator that feeds in events 
in the LHA format, you can call the <code>setLHAupPtr(...)</code> 
method 
<pre> 
      pythia.setLHAupPtr( lhaUpPtr); 
</pre> 
where the  <code>lhaUpPtr</code> derives from the 
<?php $filepath = $_GET["filepath"];
echo "<a href='LesHouchesAccord.php?filepath=".$filepath."' target='page'>";?>LHAup</a> base class. 
</li> 
 
<p/> 
<li> 
If you want to perform some particle decays with an 
external generator, you can call the <code>setDecayPtr(...)</code> 
method 
<pre> 
      pythia.setDecayPtr( decayHandlePtr, particles); 
</pre> 
where the <code>decayHandlePtr</code> derives from the 
<code><?php $filepath = $_GET["filepath"];
echo "<a href='ExternalDecays.php?filepath=".$filepath."' target='page'>";?>DecayHandler</a></code> base 
class and <code>particles</code> is a vector of particle codes to be 
handled. 
</li> 
 
<p/> 
<li> 
If you want to use an external random number generator, 
you can call the <code>setRndmEnginePtr(...)</code> method 
<pre> 
      pythia.setRndmEnginePtr( rndmEnginePtr); 
</pre> 
where <code>rndmEnginePtr</code> derives from the 
<code><?php $filepath = $_GET["filepath"];
echo "<a href='RandomNumbers.php?filepath=".$filepath."' target='page'>";?>RndmEngine</a></code> base class. 
The <code>Pythia</code> default random number generator is perfectly 
good, so this is only intended for consistency in bigger frameworks. 
</li> 
 
<p/> 
<li> 
If you want to interrupt the evolution at various stages, 
to interrogate the event and possibly veto it, or you want to 
reweight the cross section, you can use 
<pre> 
      pythia.setUserHooksPtr( userHooksPtr); 
</pre> 
where <code>userHooksPtr</code> derives from the 
<code><?php $filepath = $_GET["filepath"];
echo "<a href='UserHooks.php?filepath=".$filepath."' target='page'>";?>UserHooks</a></code> base class. 
</li> 
 
<p/> 
<li> 
If you want to use your own merging scale definition for 
matrix element + parton shower merging, you can call 
<pre> 
      pythia.setMergingHooksPtr( mergingHooksPtr); 
</pre> 
where <code>mergingHooksPtr</code> derives from the 
<code><?php $filepath = $_GET["filepath"];
echo "<a href='MatrixElementMerging.php?filepath=".$filepath."' target='page'>";?>MergingHooks</a></code> base class. 
</li> 
 
<p/> 
<li> 
If you want to use your own parametrization of beam momentum spread and 
interaction vertex, rather than the provided simple Gaussian 
parametrization (off by default), you can call 
<pre> 
      pythia.setBeamShapePtr( beamShapePtr); 
</pre> 
where <code>beamShapePtr</code> derives from the 
<code><?php $filepath = $_GET["filepath"];
echo "<a href='BeamShape.php?filepath=".$filepath."' target='page'>";?>BeamShape</a></code> base class. 
</li> 
 
<p/> 
<li> 
If you want to implement a cross section of your own, but still make use 
of the built-in phase space selection machinery, you can use 
<pre> 
      pythia.setSigmaPtr( sigmaPtr); 
</pre> 
where <code>sigmaPtr</code> of type <code>SigmaProcess*</code> is an 
instance of a class derived from one of the <code>Sigma1Process</code>, 
<code>Sigma2Process</code> and  <code>Sigma3Process</code> base classes 
in their turn derived from 
<code><?php $filepath = $_GET["filepath"];
echo "<a href='SemiInternalProcesses.php?filepath=".$filepath."' target='page'>";?>SigmaProcess</a></code>. 
This call can be used repeatedly to hand in several different processes. 
</li> 
 
<p/> 
<li> 
If your cross section contains the production of a new resonance 
with known analytical expression for all the relevant partial widths, 
you can make this resonance available to the program with 
<pre> 
      pythia.setResonancePtr( resonancePtr); 
</pre> 
where <code>resonancePtr</code> of type <code>ResonanceWidths*</code> 
is an instance of a class derived from the 
<code><?php $filepath = $_GET["filepath"];
echo "<a href='SemiInternalResonances.php?filepath=".$filepath."' target='page'>";?>ResonanceWidths</a></code> 
base class. In addition you need to add the particle to the normal 
<?php $filepath = $_GET["filepath"];
echo "<a href='ParticleDataScheme.php?filepath=".$filepath."' target='page'>";?>particle and decay database</a>. 
This procedure can be used repeatedly to hand in several different 
resonances. 
</li> 
 
<p/> 
<li> 
If you are a real expert and want to <?php $filepath = $_GET["filepath"];
echo "<a href='ImplementNewShowers.php?filepath=".$filepath."' target='page'>";?>replace 
the PYTHIA initial- and final-state showers</a>, you can use 
<pre> 
      pythia.setShowerPtr( timesDecPtr, timesPtr, spacePtr); 
</pre> 
where <code>timesDecPtr</code> and <code>timesPtr</code> 
derive from the <code>TimeShower</code> base class, and 
<code>spacePtr</code> from <code>SpaceShower</code>. 
</li> 
 
</ol> 
 
<p/> 
C) Some comments on collecting several tasks in the same run. 
<ol> 
 
<li> 
PYTHIA has not been written for threadsafe execution on multicore 
processors. If you want to use all cores, 
the most efficient way presumably is to start correspondingly many jobs, 
with different random number seeds, and add the statistics at the end. 
However, note that several instances  can be set up in the same main 
program, since instances are completely independent of each other, 
so each instance could be run inside a separate thread. 
</li> 
 
<p/> 
<li> 
In some cases it is convenient to use more than one <code>Pythia</code> 
object. The key example would be the simultaneous generation of signal 
and pileup events, see <code>main19.cc</code>. The two objects are then 
set up and initialized separately, and generate events completely 
independently of each other. It is only afterwards that the event records 
are combined into one single super-event per beam crossing. 
</li> 
 
<p/> 
<li> 
When time is not an issue, it may be that you want to perform several 
separate subruns sequentially inside a run, e.g. to combine results for 
several kinematical regions or to compare results for some different 
tunes of the underlying event. One way to go is to create (and destroy) 
one <code>pythia</code> object for each subrun, in which case they are 
completely separate. You can also use the same <code>pythia</code> object, 
only doing a new <code>init(...)</code> call for each subrun. In that 
case, the settings and particle databases remain as they were in the 
previous subrun, only affected by the specific changes you introduced in 
the meantime. You can put those changes in the main program, with 
<code>pythia.readString(string)</code>, using your own logic to decide 
which ones to execute in which subrun. A corresponding possibility 
exists with <code>pythia.readFile(fileName, subrun)</code> (or an 
<code>istream</code> instead of a <code>fileName</code>), which as second 
argument can take a non-negative subrun number. Then only those 
sections of the file before any <code>Main:subrun = ...</code> line 
or with matching <code>subrun</code> number will be read. That is, the 
file could have a structure like 
<pre> 
    ( lines always read, i.e. "default values" always (re)set ) 
    Main:subrun = 1 
    ( lines only read with readFile(fileName, 1) ) 
    Main:subrun = 2 
    ( lines only read with readFile(fileName, 2) ) 
</pre> 
Both of these possibilities are illustrated in <code>main08.cc</code>. 
</li> 
 
<p/> 
<li> 
When working with Les Houches Event Files, it may well be that your 
intended input event sample is spread over several files, that you all 
want to turn into complete events in one and the same run. There is no 
problem with looping over several subruns, where each new subrun 
is initialized with a new file, with name set in <code>Beams:LHEF</code>. 
However, in that case you will do a complete re-initialization each time 
around. If you want to avoid this, note that the flag 
<code>Beams:newLHEFsameInit = true</code> can be set for the second and 
subsequent subruns. Then the new file will be simulated with the same 
initialization data as already set in a previous 
<code>pythia.init()</code> call. The burden rests on you to ensure 
that this is indeed correct, e.g. that the two event samples have not 
been generated for different beam energies. Also note that cross 
sections for processes will be based on the information in the 
first-read file, when the full initialization is performed. 
</li> 
 
</ol> 
 
<h2>The Pythia Class</h2> 
 
Here follows the complete survey of all public <code>Pythia</code> 
methods and data members. 
 
<h3>Constructor and destructor</h3> 
 
<a name="method1"></a>
<p/><strong>Pythia::Pythia(string xmlDir = &quot;../xmldoc&quot;, bool printBanner = true) &nbsp;</strong> <br/>
creates an instance of the <code>Pythia</code> event generators, 
and sets initial default values, notably for all settings and 
particle data. You may use several <code>Pythia</code> instances 
in the same run; only when you want to access external static 
libraries could this cause problems. (This includes in particular 
Fortran libraries such as <?php $filepath = $_GET["filepath"];
echo "<a href='PDFSelection.php?filepath=".$filepath."' target='page'>";?>LHAPDF</a>.) 
<br/><code>argument</code><strong> xmlDir </strong> (<code>default = <strong>../xmldoc</strong></code>) : allows you to choose 
from which directory the default settings and particle data values 
are read in. If the <code>PYTHIA8DATA</code> environment variable 
has been set it takes precedence. Else this optional argument allows 
you to choose another directory location than the default one. Note 
that it is only the directory location you can change, its contents 
must be the ones of the <code>xmldoc</code> directory in the 
standard distribution. 
   
<br/><code>argument</code><strong> printBanner </strong> (<code>default = <strong>true</strong></code>) :  can be set 
<code>false</code> to stop the program from printing a banner. 
The banner contains useful information, so this option is only 
intended for runs with multiple <code>Pythia</code> instances, 
where output needs to be restricted. 
   
   
 
<a name="method2"></a>
<p/><strong>Pythia::~Pythia &nbsp;</strong> <br/>
the destructor deletes the objects created by the constructor. 
 
<h3>Set up run</h3> 
 
<a name="method3"></a>
<p/><strong>bool Pythia::readString(string line, bool warn = true) &nbsp;</strong> <br/>
reads in a single string, that is interpreted as an instruction to 
modify the value of a <?php $filepath = $_GET["filepath"];
echo "<a href='SettingsScheme.php?filepath=".$filepath."' target='page'>";?>setting</a> or 
<?php $filepath = $_GET["filepath"];
echo "<a href='ParticleDataScheme.php?filepath=".$filepath."' target='page'>";?>particle data</a>, as already described 
above. 
<br/><code>argument</code><strong> line </strong>  :  
the string to be interpreted as an instruction. 
   
<br/><code>argument</code><strong> warn </strong> (<code>default = <strong>true</strong></code>) :  
write a warning message or not whenever the instruction does not make 
sense, e.g. if the variable does not exist in the databases. 
   
<br/><b>Note:</b> the method returns false if it fails to 
make sense out of the string. 
   
 
<a name="method4"></a>
<p/><strong>bool Pythia::readFile(string fileName, bool warn = true, int subrun = SUBRUNDEFAULT) &nbsp;</strong> <br/>
   
<strong>bool Pythia::readFile(string fileName, int subrun = SUBRUNDEFAULT) &nbsp;</strong> <br/>
   
<strong>bool Pythia::readFile(istream& inStream = cin, bool warn = true, int subrun = SUBRUNDEFAULT) &nbsp;</strong> <br/>
   
<strong>bool Pythia::readFile(istream& inStream = cin, int subrun = SUBRUNDEFAULT) &nbsp;</strong> <br/>
reads in a whole file, where each line is interpreted as an instruction 
to modify the value of a <?php $filepath = $_GET["filepath"];
echo "<a href='SettingsScheme.php?filepath=".$filepath."' target='page'>";?>setting</a> or 
<?php $filepath = $_GET["filepath"];
echo "<a href='ParticleDataScheme.php?filepath=".$filepath."' target='page'>";?>particle data</a>, cf. the above 
<code>readString</code> method. All four forms of the 
<code>readFile</code> command share code for actually reading a file. 
<br/><code>argument</code><strong> fileName </strong>  :  
the file from which instructions are read. 
   
<br/><code>argument</code><strong> inStream </strong>  :  
an istream from which instructions are read. 
   
<br/><code>argument</code><strong> warn </strong> (<code>default = <strong>true</strong></code>) :  
write a warning message or not whenever the instruction does not make 
sense, e.g. if the variable does not exist in the databases. In the 
command forms where <code>warn</code> is omitted it is true. 
   
<br/><code>argument</code><strong> subrun </strong>  :  
allows you have several optional sets of commands within the same file. 
Only those sections of the file before any <code>Main:subrun = ...</code> 
line or following such a line with matching subrun number will be read. 
The subrun number should not be negative; negative codes like 
<code>SUBRUNDEFAULT</code> corresponds to no specific subrun. 
   
<br/><b>Note:</b> the method returns false if it fails to 
make sense out of any one line. 
   
 
<a name="method5"></a>
<p/><strong>bool Pythia::setPDFPtr( PDF* pdfAPtr, PDF* pdfBPtr, PDF* pdfHardAPtr = 0, PDF* pdfHardBPtr = 0) &nbsp;</strong> <br/>
offers the possibility to link in external PDF sets for usage inside 
the program. The rules for constructing your own class from 
the <code>PDF</code> base class are described 
<?php $filepath = $_GET["filepath"];
echo "<a href='PartonDistributions.php?filepath=".$filepath."' target='page'>";?>here</a>. 
<br/><code>argument</code><strong> pdfAPtr, pdfBPtr </strong>  :  
pointers to two <code>PDF</code>-derived objects, one for each of 
the incoming beams. The two objects have to be instantiated by you 
in your program. Even if the two beam particles are the same 
(protons, say) two separate instances are required, since current 
information is cached in the objects. If both arguments are zero 
then any previous linkage to external PDF's is disconnected, 
see further Note 2 below. 
   
<br/><code>argument</code><strong> pdfHardAPtr, pdfHardBPtr </strong> (<code>default = <strong>0</strong></code>) :  
pointers to two further <code>PDF</code>-derived objects, one for each 
of the incoming beams. Normally only the first two arguments above would 
be used, and then the same PDF sets would be invoked everywhere. If you 
provide these two further pointers then two different sets of PDF's are 
used. This second set is then exclusively for the generation of the hard 
process from the process matrix elements library. The first set above 
is for everything else, notably parton showers and multiparton interactions. 
   
<br/><b>Note 1:</b> The method returns false if the input is obviously 
incorrect, e.g. if two (nonzero) pointers agree. 
<br/><b>Note 2:</b> If you want to combine several subruns you can 
call <code>setPDFPtr</code> with new arguments before each 
<code>Pythia::init(...)</code> call. To revert from external PDF's 
to the normal internal PDF selection you must call 
<code>setPDFPtr(0, 0)</code> before <code>Pythia::init(...)</code>. 
   
 
<a name="method6"></a>
<p/><strong>bool Pythia::setLHAupPtr( LHAup* lhaUpPtrIn) &nbsp;</strong> <br/>
offers linkage to an external generator that feeds in events 
in the LHA format, see 
<?php $filepath = $_GET["filepath"];
echo "<a href='LesHouchesAccord.php?filepath=".$filepath."' target='page'>";?>Les Houches Accord</a>, 
assuming that 
<code><?php $filepath = $_GET["filepath"];
echo "<a href='BeamParameters.php?filepath=".$filepath."' target='page'>";?>Beams:frameType = 5</a></code> 
has been set. 
<br/><code>argument</code><strong> lhaUpPtrIn </strong>  :  
pointer to a <code>LHAup</code>-derived object. 
   
<br/><b>Note:</b> The method currently always returns true. 
   
 
<a name="method7"></a>
<p/><strong>bool Pythia::setDecayPtr( DecayHandler* decayHandlePtr, vector&lt;int&gt; handledParticles) &nbsp;</strong> <br/>
offers the possibility to link to an external program that can do some 
of the particle decays, instead of using the internal decay machinery. 
With particles we here mean the normal hadrons and leptons, not 
top quarks, electroweak bosons or new particles in BSM scenarios. 
The rules for constructing your own class from the 
<code>DecayHandler</code> base class are described 
<?php $filepath = $_GET["filepath"];
echo "<a href='ExternalDecays.php?filepath=".$filepath."' target='page'>";?>here</a>. Note that you can only 
provide one external object, but this object in its turn could 
very well hand on different particles to separate decay libraries. 
<br/><code>argument</code><strong> decayHandlePtr </strong>  :  
pointer to a <code>DecayHandler</code>-derived object. This object 
must be instantiated by you in your program. 
   
<br/><code>argument</code><strong> handledParticles </strong>  :  vector with the PDG identity codes 
of the particles that should be handled by the external decay package. 
You should only give the particle (positive) codes; the respective 
antiparticle is always included as well. 
   
<br/><b>Note:</b> The method currently always returns true. 
   
 
<a name="method8"></a>
<p/><strong>bool Pythia::setRndmEnginePtr( RndmEngine* rndmEnginePtr) &nbsp;</strong> <br/>
offers the possibility to link to an external random number generator. 
The rules for constructing your own class from the 
<code>RndmEngine</code> base class are described 
<?php $filepath = $_GET["filepath"];
echo "<a href='RandomNumbers.php?filepath=".$filepath."' target='page'>";?>here</a>. 
<br/><code>argument</code><strong> rndmEnginePtr </strong>  :  
pointer to a <code>RndmEngine</code>-derived object. This object 
must be instantiated by you in your program. 
   
<br/><b>Note:</b> The method returns true if the pointer is different 
from 0. 
   
 
<a name="method9"></a>
<p/><strong>bool Pythia::setUserHooksPtr( UserHooks* userHooksPtr) &nbsp;</strong> <br/>
offers the possibility to interact with the generation process at 
a few different specified points, e.g. to reject undesirable events 
at an early stage to save computer time. The rules for constructing 
your own class from the <code>UserHooks</code> base class are described 
<?php $filepath = $_GET["filepath"];
echo "<a href='UserHooks.php?filepath=".$filepath."' target='page'>";?>here</a>. You can only hand in one such 
pointer, but this may be to a class that implements several of the 
different allowed possibilities. 
<br/><code>argument</code><strong> userHooksPtr </strong>  :  
pointer to a <code>userHooks</code>-derived object. This object 
must be instantiated by you in your program. 
   
<br/><b>Note:</b> The method currently always returns true. 
   
 
<a name="method10"></a>
<p/><strong>bool Pythia::setBeamShapePtr( BeamShape* beamShapePtr) &nbsp;</strong> <br/>
offers the possibility to provide your own shape of the momentum and 
space-time spread of the incoming beams. The rules for constructing 
your own class from the <code>BeamShape</code> base class are described 
<?php $filepath = $_GET["filepath"];
echo "<a href='BeamShape.php?filepath=".$filepath."' target='page'>";?>here</a>. 
<br/><code>argument</code><strong> BeamShapePtr </strong>  :  
pointer to a <code>BeamShape</code>-derived object. This object 
must be instantiated by you in your program. 
   
<br/><b>Note:</b> The method currently always returns true. 
   
 
<a name="method11"></a>
<p/><strong>bool Pythia::setSigmaPtr( SigmaProcess* sigmaPtr) &nbsp;</strong> <br/>
offers the possibility to link your own implementation of a process 
and its cross section, to make it a part of the normal process 
generation machinery, without having to recompile the 
<code>Pythia</code> library itself.  The rules for constructing your 
own class from the <code>SigmaProcess</code> base class are described 
<?php $filepath = $_GET["filepath"];
echo "<a href='SemiInternalProcesses.php?filepath=".$filepath."' target='page'>";?>here</a>. You may call this 
routine repeatedly, to add as many new processes as you wish. 
<br/><code>argument</code><strong> sigmaPtr </strong>  :  
pointer to a <code>SigmaProcess</code>-derived object. This object 
must be instantiated by you in your program. 
   
<br/><b>Note:</b> The method currently always returns true. 
   
 
<a name="method12"></a>
<p/><strong>bool Pythia::setResonancePtr( ResonanceWidths* resonancePtr) &nbsp;</strong> <br/>
offers the possibility to link your own implementation of the 
calculation of partial resonance widths, to make it a part of the 
normal process generation machinery, without having to recompile the 
<code>Pythia</code> library itself.  This allows the decay of new 
resonances to be handled internally, when combined with new particle 
data. Note that the decay of normal hadrons cannot be modeled here; 
this is for New Physics resonances. The rules for constructing your 
own class from the <code>ResonanceWidths</code> base class are described 
<?php $filepath = $_GET["filepath"];
echo "<a href='SemiInternalResonances.php?filepath=".$filepath."' target='page'>";?>here</a>. You may call this 
routine repeatedly, to add as many new resonances as you wish. 
<br/><code>argument</code><strong> resonancePtr </strong>  :  
pointer to a <code>ResonanceWidths</code>-derived object. This object 
must be instantiated by you in your program. 
   
<br/><b>Note:</b> The method currently always returns true. 
   
 
<a name="method13"></a>
<p/><strong>bool Pythia::setShowerPtr( TimeShower* timesDecPtr, TimeShower* timesPtr = 0, SpaceShower* spacePtr = 0) &nbsp;</strong> <br/>
offers the possibility to link your own parton shower routines as 
replacements for the default ones. This is much more complicated 
since the showers are so central and are so interlinked with other 
parts of the program. Therefore it is also possible to do the 
replacement in stages, from the more independent to the more 
intertwined. The rules for constructing your own classes from the 
<code>TimeShower</code> and <code>SpaceShower</code>base classes 
are described <?php $filepath = $_GET["filepath"];
echo "<a href='ImplementNewShowers.php?filepath=".$filepath."' target='page'>";?>here</a>. These 
objects must be instantiated by you in your program. 
<br/><code>argument</code><strong> timesDecPtr </strong>  :  
pointer to a <code>TimeShower</code>-derived object for doing 
timelike shower evolution in resonance decays, e.g. of a 
<i>Z^0</i>. This is decoupled from beam remnants and parton 
distributions, and is therefore the simplest kind of shower 
to write. If you provide a value 0 then the internal shower 
routine will be used. 
   
<br/><code>argument</code><strong> timesPtr </strong> (<code>default = <strong>0</strong></code>) :  
pointer to a <code>TimeShower</code>-derived object for doing 
all other timelike shower evolution, which is normally interleaved 
with multiparton interactions and spacelike showers, introducing 
both further physics and further technical issues. If you retain 
the default value 0 then the internal shower routine will be used. 
You are allowed to use the same pointer as above for the 
<code>timesDecPtr</code> if the same shower can fulfill both tasks. 
   
<br/><code>argument</code><strong> spacePtr </strong> (<code>default = <strong>0</strong></code>) :  
pointer to a <code>SpaceShower</code>-derived object for doing 
all spacelike shower evolution, which is normally interleaved 
with multiparton interactions and timelike showers. If you retain 
the default value 0 then the internal shower routine will be used. 
   
<br/><b>Note:</b> The method currently always returns true. 
   
 
<h3>Initialize</h3> 
 
At the initialization stage all the information provided above is 
processed, and the stage is set up for the subsequent generation 
of events. Currently several alternative forms of the <code>init</code> 
method are available for this stage, but only the first one is 
recommended. 
 
<a name="method14"></a>
<p/><strong>bool Pythia::init() &nbsp;</strong> <br/>
initialize for collisions, in any of the five separate possibilities 
below. In this option the beams are not specified by input arguments, 
but instead by the settings in the 
<?php $filepath = $_GET["filepath"];
echo "<a href='BeamParameters.php?filepath=".$filepath."' target='page'>";?>Beam Parameters</a> section. 
This allows the beams to be specified in the same file as other 
run instructions. The default settings give pp collisions at 14 TeV. 
<br/><b>Note:</b> The method returns false if the 
initialization fails. It is then not possible to generate any 
events. 
   
 
<a name="method15"></a>
<p/><strong>bool Pythia::init( int idA, int idB, double eCM) &nbsp;</strong> <br/>
initialize for collisions in the center-of-mass frame, with the 
beams moving in the <i>+-z</i> directions. 
<br/><code>argument</code><strong> idA, idB </strong>  :  
particle identity code for the two incoming beams. 
   
<br/><code>argument</code><strong> eCM </strong>  :  
the CM energy of the collisions. 
   
<br/><b>Notes:</b> Deprecated. The method returns false if the 
initialization fails. It is then not possible to generate any 
events. 
   
 
<a name="method16"></a>
<p/><strong>bool Pythia::init( int idA, int idB, double eA, double eB) &nbsp;</strong> <br/>
initialize for collisions with back-to-back beams, 
moving in the <i>+-z</i> directions, but with different energies. 
<br/><code>argument</code><strong> idA, idB </strong>  :  
particle identity code for the two incoming beams. 
   
<br/><code>argument</code><strong> eA, eB </strong>  :  
the energies of the two beams. If an energy is set to be below 
the mass of the respective beam particle that particle is taken to 
be at rest. This offers a simple possibility to simulate 
fixed-target collisions. 
   
<br/><b>Notes:</b> Deprecated. The method returns false if the 
initialization fails. It is then not possible to generate any 
events. 
   
 
<a name="method17"></a>
<p/><strong>bool Pythia::init( int idA, int idB, double pxA, double pyA, double pzA, double pxB, double pyB, double pzB) &nbsp;</strong> <br/>
initialize for collisions with arbitrary beam directions. 
<br/><code>argument</code><strong> idA, idB </strong>  :  
particle identity code for the two incoming beams. 
   
<br/><code>argument</code><strong> pxA, pyA, pzA </strong>  :  
the three-momentum vector <i>(p_x, p_y, p_z)</i> of the first 
incoming beam. 
   
<br/><code>argument</code><strong> pxB, pyB, pzB </strong>  :  
the three-momentum vector <i>(p_x, p_y, p_z)</i> of the second 
incoming beam. 
   
<br/><b>Notes:</b> Deprecated. The method returns false if the 
initialization fails. It is then not possible to generate any 
events. 
   
 
<a name="method18"></a>
<p/><strong>bool Pythia::init( string LesHouchesEventFile, bool skipInit = false) &nbsp;</strong> <br/>
initialize for hard-process collisions fed in from an external file 
with events, written according to the 
<?php $filepath = $_GET["filepath"];
echo "<a href='LesHouchesAccord.php?filepath=".$filepath."' target='page'>";?>Les Houches Event File</a> 
standard. 
<br/><code>argument</code><strong> LesHouchesEventFile </strong>  :  
the file name (including path, where required) where the 
events are stored, including relevant information on beam 
identities and energies. 
   
<br/><code>argument</code><strong> skipInit </strong> (<code>default = <strong>false</strong></code>) :  
By default this method does a complete reinitialization of the 
generation process. If you set this argument to true then 
no reinitialization will occur, only the pointer to the event 
file is updated. This may come in handy if the full event sample 
is split across several files generated under the same conditions 
(except random numbers, of course). You then do the first 
initialization with the default, and all subsequent ones with 
true. Note that things may go wrong if the files are not created 
under the same conditions. 
   
<br/><b>Notes:</b> Deprecated. The method returns false if the 
initialization fails. It is then not possible to generate any 
events. 
   
 
<a name="method19"></a>
<p/><strong>bool Pythia::init( LHAup* lhaUpPtr) &nbsp;</strong> <br/>
initialize for hard-process collisions fed in from an external 
source of events, consistent with the Les Houches Accord standard. 
The rules for constructing your own class from the <code>LHAup</code> 
base class are described <?php $filepath = $_GET["filepath"];
echo "<a href='LesHouchesAccord.php?filepath=".$filepath."' target='page'>";?>here</a>. 
This class is also required to provide the beam parameters. 
<br/><code>argument</code><strong> lhaUpPtr </strong>  :  
pointer to a <code>LHAup</code>-derived object. This object 
must be instantiated by you in your program. 
   
<br/><b>Notes:</b> Deprecated. The method returns false if the 
initialization fails. It is then not possible to generate any 
events. 
   
 
<h3>Generate events</h3> 
 
The <code>next()</code> method is the main one to generate events. 
In this section we also put a few other specialized methods that 
may be useful in some circumstances. 
 
<a name="method20"></a>
<p/><strong>bool Pythia::next() &nbsp;</strong> <br/>
generate the next event. No input parameters are required; all 
instructions have already been set up in the initialization stage. 
<br/><b>Note:</b> The method returns false if the event generation 
fails. The event record is then not consistent and should not be 
studied. When reading in hard collisions from a Les Houches Event File 
the problem may be that the end of the file has been reached. This 
can be checked with the 
<code><?php $filepath = $_GET["filepath"];
echo "<a href='EventInformation.php?filepath=".$filepath."' target='page'>";?>Info::atEndOfFile()</a></code> 
method. 
   
 
<a name="method21"></a>
<p/><strong>int Pythia::forceTimeShower( int iBeg, int iEnd, double pTmax, int nBranchMax = 0) &nbsp;</strong> <br/>
perform a final-state shower evolution on partons in the 
<code>event</code> event record. This could be used for externally 
provided simple events, or even parts of events, for which 
a complete generation is not foreseen. Since the mother source of 
the parton system is not known, one cannot expect as good accuracy 
as in a normal generation. When two different timelike shower 
instances are set up, it is the one used for showering in resonance 
decays that is used here. The <code>forceTimeShower</code> method 
can be used in conjunction with the <code>forceHadronLevel</code> 
one below. Further comments are found 
<?php $filepath = $_GET["filepath"];
echo "<a href='HadronLevelStandalone.php?filepath=".$filepath."' target='page'>";?>here</a>. 
<br/><code>argument</code><strong> iBeg, iEnd </strong>  :  the first and last entry of the event 
record to be affected by the call. 
   
<br/><code>argument</code><strong> pTmax </strong>  :  the maximum <i>pT</i> scale of emissions. 
Additionally, as always, the <code>scale</code> variable of each parton 
sets the maximum <i>pT</i> scale of branchings of this parton. 
Recall that this scale defaults to 0 if not set, so that no radiation 
can occur. 
   
<br/><code>argument</code><strong> nBranchMax </strong> (<code>default = <strong>0</strong></code>) :  when positive, it sets the 
maximum number of branchings that are allowed to occur in the shower, 
i.e. the shower may stop evolving before reaching the lower cutoff. 
The argument has no effect when zero or negative, i.e. then the shower 
will continue to the lower cutoff. 
   
<br/><b>Note:</b> The method returns the number of branchings that 
has been generated. 
   
 
<a name="method22"></a>
<p/><strong>bool Pythia::forceHadronLevel(bool findJunctions = true) &nbsp;</strong> <br/>
hadronize the existing event record, i.e. perform string fragmentation 
and particle decays. There are two main applications. Firstly, 
you can use the same parton-level content as a basis for repeated 
hadronization attempts, in schemes intended to save computer time. 
Secondly, you may have an external program that can simulate the full 
partonic level of the event - hard process, parton showers, multiparton 
interactions, beam remnants, colour flow, and so on - but not 
hadronization. Further details are found 
<?php $filepath = $_GET["filepath"];
echo "<a href='HadronLevelStandalone.php?filepath=".$filepath."' target='page'>";?>here</a>. 
<br/><code>argument</code><strong> findJunctions </strong> (<code>default = <strong>true</strong></code>) :  
normally this routine will search through the event record and try to 
figure out if any colour junctions are present. If so, the colour 
topology of such junctions must be sorted out. In tricky cases this 
might fail, and then hadronization will not work. A user who is 
aware of this and knows the intended colour flow can set up the 
junction information (if any) in the event record, and then call 
<code>forceHadronLevel(false)</code> so as not to have this information 
overwritten. (If the event record contains no unhadronized partons 
then no junction search will be performed in any case.) 
   
<br/><b>Note:</b> The method returns false if the hadronization 
fails. The event record is then not consistent and should not be 
studied. 
   
 
<a name="method23"></a>
<p/><strong>bool Pythia::moreDecays() &nbsp;</strong> <br/>
perform decays of all particles in the event record that have not been 
decayed but should have been done so. This can be used e.g. for 
repeated decay attempts, in schemes intended to save computer time. 
Further details are found <?php $filepath = $_GET["filepath"];
echo "<a href='HadronLevelStandalone.php?filepath=".$filepath."' target='page'>";?>here</a>. 
<br/><b>Note:</b> The method returns false if the decays fail. The 
event record is then not consistent and should not be studied. 
   
 
<a name="method24"></a>
<p/><strong>bool Pythia::forceRHadronDecays() &nbsp;</strong> <br/>
perform decays of R-hadrons that were previously considered stable. 
This could be if an R-hadron is sufficiently long-lived that 
it may interact in the detector between production and decay, so that 
its four-momentum is changed. Further details are found 
<?php $filepath = $_GET["filepath"];
echo "<a href='RHadrons.php?filepath=".$filepath."' target='page'>";?>here</a>. 
<br/><b>Note:</b> The method returns false if the decays fail. The 
event record is then not consistent and should not be studied. 
   
 
<a name="method25"></a>
<p/><strong>void Pythia::LHAeventList(ostream& os = cout) &nbsp;</strong> <br/>
list the Les Houches Accord information on the current event, see 
<code><?php $filepath = $_GET["filepath"];
echo "<a href='LesHouchesAccord.php?filepath=".$filepath."' target='page'>";?>LHAup::listEvent(...)</a></code>. 
(Other listings are available via the class members below, so this 
listing is a special case that would not fit elsewhere.) 
<br/><code>argument</code><strong> os </strong> (<code>default = <strong>cout</strong></code>) :  
output stream where the listing occurs. 
   
   
 
<a name="method26"></a>
<p/><strong>bool Pythia::LHAeventSkip(int nSkip) &nbsp;</strong> <br/>
skip ahead a number of events in the Les Houches generation 
sequence, without doing anything further with them, see 
<code><?php $filepath = $_GET["filepath"];
echo "<a href='LesHouchesAccord.php?filepath=".$filepath."' target='page'>";?>LHAup::skipEvent(nSkip)</a></code>. 
Mainly intended for debug purposes, e.g. when an event at a known 
location in a Les Houches Event File is causing problems. 
<br/><code>argument</code><strong> nSkip </strong>  :  
number of events to skip. 
   
<br/><b>Note:</b> The method returns false if the operation fails, 
specifically if the end of a LHEF has been reached, cf. 
<code>next()</code> above. 
   
 
<h3>Finalize</h3> 
 
There is no required finalization step; you can stop generating events 
when and how you want. It is still recommended that you make it a 
routine to call the following method at the end. A second method provides 
a deprecated alternative. 
 
<a name="method27"></a>
<p/><strong>void Pythia::stat() &nbsp;</strong> <br/>
list statistics on the event generation, specifically total and partial 
cross sections and the number of different errors. For more details see 
<?php $filepath = $_GET["filepath"];
echo "<a href='EventStatistics.php?filepath=".$filepath."' target='page'>";?>here</a> and for available options 
<?php $filepath = $_GET["filepath"];
echo "<a href='MainProgramSettings.php?filepath=".$filepath."' target='page'>";?>here</a>. 
   
 
<a name="method28"></a>
<p/><strong>void Pythia::statistics(bool all = false, bool reset = false) &nbsp;</strong> <br/>
list statistics on the event generation, specifically total and partial 
cross sections and the number of different errors. For more details see 
<?php $filepath = $_GET["filepath"];
echo "<a href='EventStatistics.php?filepath=".$filepath."' target='page'>";?>here</a>. 
<br/><code>argument</code><strong> all </strong> (<code>default = <strong>false</strong></code>) :  
if true also statistics on multiparton interactions is shown, by default not. 
   
<br/><code>argument</code><strong> reset </strong> (<code>default = <strong>false</strong></code>) :  if true then all counters, 
e.g on events generated and errors experienced, are reset to zero 
 whenever the routine is called. The default instead is that 
all stored statistics information is unaffected by the call. Counters 
are automatically reset in each new <code>Pythia::init(...)</code> 
call, however, so the only time the <code>reset</code> option makes a 
difference is if <code>statistics(...)</code> is called several times 
in a (sub)run. 
   
<br/><b>Note:</b> Deprecated. 
   
 
<h3>Interrogate settings</h3> 
 
Normally settings are used in the setup and initialization stages 
to determine the character of a run, e.g. read from a file with the 
above-described <code>Pythia::readFile(...)</code> method. 
There is no strict need for a user to interact with the 
<code>Settings</code> database in any other way. However, as an option, 
some settings variables have been left free for the user to set in 
such a file, and then use in the main program to directly affect the 
performance of that program, see 
<?php $filepath = $_GET["filepath"];
echo "<a href='MainProgramSettings.php?filepath=".$filepath."' target='page'>";?>here</a>. A typical example would 
be the number of events to generate. For such applications the 
following shortcuts to some <code>Settings</code> methods may be 
convenient. 
 
<a name="method29"></a>
<p/><strong>bool Pythia::flag(string key) &nbsp;</strong> <br/>
read in a boolean variable from the <code>Settings</code> database. 
<br/><code>argument</code><strong> key </strong>  :  
the name of the variable to be read. 
   
   
  
<a name="method30"></a>
<p/><strong>int Pythia::mode(string key) &nbsp;</strong> <br/>
read in an integer variable from the <code>Settings</code> database. 
<br/><code>argument</code><strong> key </strong>  :  
the name of the variable to be read. 
   
   
  
<a name="method31"></a>
<p/><strong>double Pythia::parm(string key) &nbsp;</strong> <br/>
read in a double-precision variable from the <code>Settings</code> 
database. 
<br/><code>argument</code><strong> key </strong>  :  
the name of the variable to be read. 
   
   
  
<a name="method32"></a>
<p/><strong>string Pythia::word(string key) &nbsp;</strong> <br/>
read in a string variable from the <code>Settings</code> database. 
<br/><code>argument</code><strong> key </strong>  :  
the name of the variable to be read. 
   
   
   
<h3>Get a PDF set</h3> 
 
<code>Pythia</code> contains an number of parton density sets 
internally, plus an interface to LHAPDF. With the method below, 
this machinery is also made available for external usage. 
  
<a name="method33"></a>
<p/><strong>PDF* getPDFPtr(int id, int sequence = 1) &nbsp;</strong> <br/>
get a pointer to a PDF object. Which PDF is returned depends on the 
<?php $filepath = $_GET["filepath"];
echo "<a href='PDFSelection.php?filepath=".$filepath."' target='page'>";?>PDF Selection</a> settings. 
<br/><code>argument</code><strong> id </strong>  :  
the identity code of the incoming particle. 
   
<br/><code>argument</code><strong> sequence </strong>  :  
should normally be 1, but 2 can be used for protons to let the PDF 
selection be determined by the special settings for hard processes 
(<code>PDF:useHard</code> etc.). 
   
   
   
<h3>Data members</h3> 
 
The <code>Pythia</code> class contains a few public data members, 
several of which play a central role. We list them here, with 
links to the places where they are further described. 
  
<a name="method34"></a>
<p/><strong>Event Pythia::process &nbsp;</strong> <br/>
the hard-process event record, see <?php $filepath = $_GET["filepath"];
echo "<a href='EventRecord.php?filepath=".$filepath."' target='page'>";?>here</a> 
for further details. 
   
  
<a name="method35"></a>
<p/><strong>Event Pythia::event &nbsp;</strong> <br/>
the complete event record, see <?php $filepath = $_GET["filepath"];
echo "<a href='EventRecord.php?filepath=".$filepath."' target='page'>";?>here</a> 
for further details. 
   
  
<a name="method36"></a>
<p/><strong>Info Pythia::info &nbsp;</strong> <br/>
further information on the event-generation process, see 
<?php $filepath = $_GET["filepath"];
echo "<a href='EventInformation.php?filepath=".$filepath."' target='page'>";?>here</a> for further details. 
   
  
<a name="method37"></a>
<p/><strong>Settings Pythia::settings &nbsp;</strong> <br/>
the settings database, see <?php $filepath = $_GET["filepath"];
echo "<a href='SettingsScheme.php?filepath=".$filepath."' target='page'>";?>here</a> 
for further details. 
   
  
<a name="method38"></a>
<p/><strong>ParticleData Pythia::particleData &nbsp;</strong> <br/>
the particle properties and decay tables database, see 
<?php $filepath = $_GET["filepath"];
echo "<a href='ParticleDataScheme.php?filepath=".$filepath."' target='page'>";?>here</a> for further details. 
   
  
<a name="method39"></a>
<p/><strong>Rndm Pythia::rndm &nbsp;</strong> <br/>
the random number generator, see <?php $filepath = $_GET["filepath"];
echo "<a href='RandomNumberSeed.php?filepath=".$filepath."' target='page'>";?>here</a> 
and <?php $filepath = $_GET["filepath"];
echo "<a href='RandomNumbers.php?filepath=".$filepath."' target='page'>";?>here</a> for further details. 
   
  
<a name="method40"></a>
<p/><strong>CoupSM Pythia::coupSM &nbsp;</strong> <br/>
Standard Model couplings and mixing matrices, see 
<?php $filepath = $_GET["filepath"];
echo "<a href='StandardModelParameters.php?filepath=".$filepath."' target='page'>";?>here</a> for further details. 
   
  
<a name="method41"></a>
<p/><strong>SusyLesHouches Pythia::slha &nbsp;</strong> <br/>
parameters and particle data in the context of supersymmetric models, 
see <?php $filepath = $_GET["filepath"];
echo "<a href='SUSYLesHouchesAccord.php?filepath=".$filepath."' target='page'>";?>here</a> for further details. 
   
  
<a name="method42"></a>
<p/><strong>PartonSystems Pythia::partonSystems &nbsp;</strong> <br/>
a grouping of the partons in the event record by subsystem, 
see <?php $filepath = $_GET["filepath"];
echo "<a href='AdvancedUsage.php?filepath=".$filepath."' target='page'>";?>here</a> for further details. 
   
    
</body>
</html>
 
<!-- Copyright (C) 2014 Torbjorn Sjostrand --> 
