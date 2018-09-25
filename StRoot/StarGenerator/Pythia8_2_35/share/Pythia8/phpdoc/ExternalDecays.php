<html>
<head>
<title>External Decays</title>
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

<form method='post' action='ExternalDecays.php'>
 
<h2>External Decays</h2> 
 
<code>DecayHandler</code> is a base class for the external handling of 
decays. It is intended for normal particle decays, primarily 
<i>B</i> mesons and <i>tau</i>, and cannot be used to redirect 
decays of heavy resonances like <i>t</i> or <i>Z^0</i>. 
The user-written derived class is called if a pointer to it has 
been given with the 
<code><?php $filepath = $_GET["filepath"];
echo "<a href='ProgramFlow.php?filepath=".$filepath."' target='page'>";?>pythia.decayPtr()</a></code> 
method, where it also is specified which particles it will be called for. 
This particle information is accessible with the 
<code><?php $filepath = $_GET["filepath"];
echo "<a href='ParticleDataScheme.php?filepath=".$filepath."' target='page'>";?>doExternalDecay()</a></code> 
method. 
 
<p/> 
The main pure virtual method in <code>DecayHandler</code> 
to do the decay is: 
<a name="anchor1"></a>
<p/><strong> virtual bool DecayHandler::decay(vector&lt;int&gt;&amp; idProd, vector&lt;double&gt;&amp; mProd, vector&lt;Vec4&gt;&amp; pProd, int iDec, const Event&amp; event) &nbsp;</strong> <br/>
where 
<br/><code>argument</code><strong> idProd </strong>  :  is a list of particle PDG identity codes, 
   
<br/><code>argument</code><strong> mProd </strong>  :  is a list of their respective masses (in GeV), and 
   
<br/><code>argument</code><strong> pProd </strong>  :  is a list of their respective four-momenta. 
   
   
 
<p/> 
At input, these vectors each have size one, so that <code>idProd[0]</code>, 
<code>mProd[0]</code> and <code>pProd[0]</code> contain information on the 
particle that is to be decayed. At output, the vectors should have 
increased by the addition of all the decay products. Even if initially 
defined in the rest frame of the mother, the products should have been 
boosted so that their four-momenta add up to the <code>pProd[0]</code> of 
the decaying particle. 
 
<p/> 
Should it be of interest to know the prehistory of the decaying 
particle, e.g. to set some helicity information affecting the 
decay angular distribution, the full event record is available 
read-only, with info in which slot <code>iDec</code> the decaying particle 
is stored. 
 
<p/> 
The routine should return <code>true</code> if it managed the decay and 
<code>false</code> otherwise, in which case <code>Pythia</code> will try 
to do the decay itself. This e.g. means you can choose to do some decay 
channels yourself, and leave others to <code>Pythia</code>. To avoid 
double-counting, the channels you want to handle should be switched off 
in the <code>Pythia</code> particle database. In the beginning of the 
external <code>decay</code> method you should then return 
<code>false</code> with a probability given by the sum of the branching 
ratios for those channels you do not want to handle yourself. 
 
<p/> 
Note that the decay vertex is always set by <code>Pythia</code>, and that 
<i>B-Bbar</i> oscillations have already been taken into account, 
if they were switched on. Thus <code>idProd[0]</code> may be the opposite 
of <code>event[iDec].id()</code>, where the latter provides the code at 
production. 
 
<p/> 
One limitation of the method above is that it is only intended for one 
decay step, not for a sequential decay chain. (At least not for 
displaying such intermediate steps.) That is, the control for any 
subsequent decays returns to PYTHIA. If you want to avoid this another 
method exists, with one extra argument: 
<a name="anchor2"></a>
<p/><strong> virtual bool DecayHandler::chainDecay( vector&lt;int&gt;&amp; idProd, vector&lt;int&gt;&amp; motherProd, vector&lt;double&gt;&amp; mProd, vector&lt;Vec4&gt;&amp; pProd, int iDec, const Event&amp; event) &nbsp;</strong> <br/>
where 
<br/><code>argument</code><strong> motherProd </strong>  :  is a list of the indices of the mother, 
   
<br/>and the other arguments are as above. 
   
 
<p/> 
Here the new <code>motherProd</code> vector also has size one at input, 
with <code>motherProd[0] = 0</code>. At output it should have increaed in 
size in the same way as the other arrays. Particles that come directly 
from the mother should have value 0, whereas secondary decay products 
should have the index of the mother in the arrays. To simplify parsing, 
particles having the same mother should be placed consecutively in the 
arrays, and daughters can not be put before their mothers. When the 
particles are transferred to the standard event record, the full 
mother-daughter relations will be reconstructed from the new array, 
and any particle with daughters will be considered to have decayed. 
For long-lived intermediate particles also vertex information will take 
this into account. User-selected secondary decay channels will be 
accepted as they are, however, without any knowledge whether the user 
has allowed for particle-antiparticle oscillations before that decay. 
Therefore a simple exponential decay time will be used to find secondary 
vertices. 
 
<p/> 
While primarily intended for sequential decays, of course the 
<code>chainDecay</code> method can be used also for simple decays 
in one step, and is then equivalent with <code>decay</code> one. 
This is useful if a particle species has some decay channels that 
lead to sequential decays whereas others do not. During code execution 
it is first checked whether <code>chainDecay</code> can do the decay, 
and if not <code>decay</code> is offered to. By default 
<code>chainDecay</code> returns false, so if you only overload 
<code>decay</code> it will be called. If you want to you can choose to 
handle the decays of some particles in one of the methods and other 
particles in the other method, so long as you return false for those 
decays you do not handle. 
 
<p/> 
The choice of which method to use can be done by the user in the method 
<a name="anchor3"></a>
<p/><strong> virtual bool doChainDecay(int idMother) {return false;} &nbsp;</strong> <br/>
if true then <code>chainDecay</code> is called, else <code>decay</code>. 
This method is called for each particle that is to be decayed externally. 
<br/><code>argument</code><strong> idMother </strong>  :  is the PDG identity code of the decaying mother. 
   
   
 
<p/> 
A sample test program is available in <code>main17.cc</code>, providing 
a simple example of how to use this facility. 
 
<h3>EvtGen</h3> 
 
The external <i>B</i> and <i>C</i>-hadron decay program EvtGen 
performs a chain of decays, rather than single particle decays, to 
propagate helicity information throughout the chain. Consequently, 
EvtGen cannot be simply interfaced via the <code>DecayHandler</code> 
class. A special class, <code>EvtGenDecays</code>, is provided 
in <code>Pythia8Plugins</code> which can be called after an event has 
been generated, to perform all remaining decays via EvtGen. An example 
of how to use this class is provided in <code>main48.cc</code>. 
A more detailed discussion of some physics considerations, notably 
event weights for forced decays, can be found in 
<a href="../pdfdoc/evtgen.pdf">this note</a>. 
</body>
</html>
 
<!-- Copyright (C) 2018 Torbjorn Sjostrand --> 
