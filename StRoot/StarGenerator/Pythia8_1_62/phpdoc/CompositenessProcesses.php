<html>
<head>
<title>Compositeness Processes</title>
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

<form method='post' action='CompositenessProcesses.php'>

<h2>Compositeness Processes</h2>

Compositeness scenarios may give rise to sharp resonances of excited 
quarks and leptons. An excited copy of the first generation is 
implemented, consisting of spin 1/2 particles. The possibility of 
contact interactions between SM fermions is also implemented in the 
context of <i>2 -> 2</i> quark or fermion-lepton scattering. 

<p/>  
Related to excited fermions, the current implementation contains gauge 
interaction production by quark-gluon fusion or lepton-photon fusion 
and contact interaction production by quark-quark or quark-antiquark 
scattering. For both the <i>2 -> 1</i> and <i>2 -> 2</i> processes 
a non-trivial angular dependence is included in the decay, however, 
only decays into gauge bosons are supported, i.e. not decays through 
contact interactions. In additions to the compositness scale and couplings 
listed below, you are expected to change the excited-fermion masses in 
accordance with what is desired. See [<a href="Bibliography.php" target="page">Bau90</a>] for conventions.

<p/>
The contact interactions are implemented according to [<a href="Bibliography.php" target="page">Eic83</a>]. 
The processes include the SM contributions as well as interference. 
For this reason the processes below converge toward the SM equivalents 
when the contact interaction contributions are close to zero, e.g. 
<code>HardQCD:qq2qq</code> and <code>HardQCD:qqbar2qqbarNew</code> in 
the case of quark scattering. 

<h3>Excited fermions, production processes</h3>

A few different production processes have been implemented, which normally 
would not overlap and therefore could be run together.

<br/><br/><strong>ExcitedFermion:all</strong>  <input type="radio" name="1" value="on"><strong>On</strong>
<input type="radio" name="1" value="off" checked="checked"><strong>Off</strong>
 &nbsp;&nbsp;(<code>default = <strong>off</strong></code>)<br/>
Common switch for the group of implemented processes that produce an
excited fermion.
  

<br/><br/><strong>ExcitedFermion:dg2dStar</strong>  <input type="radio" name="2" value="on"><strong>On</strong>
<input type="radio" name="2" value="off" checked="checked"><strong>Off</strong>
 &nbsp;&nbsp;(<code>default = <strong>off</strong></code>)<br/>
Scatterings <i>d g -> d^*</i>. 
Code 4001.
  

<br/><br/><strong>ExcitedFermion:ug2uStar</strong>  <input type="radio" name="3" value="on"><strong>On</strong>
<input type="radio" name="3" value="off" checked="checked"><strong>Off</strong>
 &nbsp;&nbsp;(<code>default = <strong>off</strong></code>)<br/>
Scatterings <i>u g -> u^*</i>. 
Code 4002.
  

<br/><br/><strong>ExcitedFermion:sg2sStar</strong>  <input type="radio" name="4" value="on"><strong>On</strong>
<input type="radio" name="4" value="off" checked="checked"><strong>Off</strong>
 &nbsp;&nbsp;(<code>default = <strong>off</strong></code>)<br/>
Scatterings <i>s g -> s^*</i>. 
Code 4003.
  

<br/><br/><strong>ExcitedFermion:cg2cStar</strong>  <input type="radio" name="5" value="on"><strong>On</strong>
<input type="radio" name="5" value="off" checked="checked"><strong>Off</strong>
 &nbsp;&nbsp;(<code>default = <strong>off</strong></code>)<br/>
Scatterings <i>c g -> c^*</i>. 
Code 4004.
  

<br/><br/><strong>ExcitedFermion:bg2bStar</strong>  <input type="radio" name="6" value="on"><strong>On</strong>
<input type="radio" name="6" value="off" checked="checked"><strong>Off</strong>
 &nbsp;&nbsp;(<code>default = <strong>off</strong></code>)<br/>
Scatterings <i>b g -> b^*</i>. 
Code 4005.
  

<br/><br/><strong>ExcitedFermion:egm2eStar</strong>  <input type="radio" name="7" value="on"><strong>On</strong>
<input type="radio" name="7" value="off" checked="checked"><strong>Off</strong>
 &nbsp;&nbsp;(<code>default = <strong>off</strong></code>)<br/>
Scatterings <i>e gamma -> e^*</i>. 
Code 4011.
  

<br/><br/><strong>ExcitedFermion:mugm2muStar</strong>  <input type="radio" name="8" value="on"><strong>On</strong>
<input type="radio" name="8" value="off" checked="checked"><strong>Off</strong>
 &nbsp;&nbsp;(<code>default = <strong>off</strong></code>)<br/>
Scatterings <i>mu gamma -> mu^*</i>. 
Code 4013.
  

<br/><br/><strong>ExcitedFermion:taugm2tauStar</strong>  <input type="radio" name="9" value="on"><strong>On</strong>
<input type="radio" name="9" value="off" checked="checked"><strong>Off</strong>
 &nbsp;&nbsp;(<code>default = <strong>off</strong></code>)<br/>
Scatterings <i>tau gamma -> tau^*</i>. 
Code 4015.
  

<br/><br/><strong>ExcitedFermion:qq2dStarq</strong>  <input type="radio" name="10" value="on"><strong>On</strong>
<input type="radio" name="10" value="off" checked="checked"><strong>Off</strong>
 &nbsp;&nbsp;(<code>default = <strong>off</strong></code>)<br/>
Scatterings <i>q q(bar) -> d^* q(bar)</i>. 
Code 4021.
  

<br/><br/><strong>ExcitedFermion:qq2uStarq</strong>  <input type="radio" name="11" value="on"><strong>On</strong>
<input type="radio" name="11" value="off" checked="checked"><strong>Off</strong>
 &nbsp;&nbsp;(<code>default = <strong>off</strong></code>)<br/>
Scatterings <i>q q(bar) -> u^* q(bar)</i>. 
Code 4022.
  

<br/><br/><strong>ExcitedFermion:qq2sStarq</strong>  <input type="radio" name="12" value="on"><strong>On</strong>
<input type="radio" name="12" value="off" checked="checked"><strong>Off</strong>
 &nbsp;&nbsp;(<code>default = <strong>off</strong></code>)<br/>
Scatterings <i>q q(bar) -> s^* q(bar)</i>. 
Code 4023.
  

<br/><br/><strong>ExcitedFermion:qq2cStarq</strong>  <input type="radio" name="13" value="on"><strong>On</strong>
<input type="radio" name="13" value="off" checked="checked"><strong>Off</strong>
 &nbsp;&nbsp;(<code>default = <strong>off</strong></code>)<br/>
Scatterings <i>q q(bar) -> c^* q(bar)</i>. 
Code 4024.
  

<br/><br/><strong>ExcitedFermion:qq2bStarq</strong>  <input type="radio" name="14" value="on"><strong>On</strong>
<input type="radio" name="14" value="off" checked="checked"><strong>Off</strong>
 &nbsp;&nbsp;(<code>default = <strong>off</strong></code>)<br/>
Scatterings <i>q q(bar) -> b^* q(bar)</i>. 
Code 4025.
  

<br/><br/><strong>ExcitedFermion:qqbar2eStare</strong>  <input type="radio" name="15" value="on"><strong>On</strong>
<input type="radio" name="15" value="off" checked="checked"><strong>Off</strong>
 &nbsp;&nbsp;(<code>default = <strong>off</strong></code>)<br/>
Scatterings <i>q qbar -> e^*+- e^-+</i>. 
Code 4031.
  

<br/><br/><strong>ExcitedFermion:qqbar2nueStarnue</strong>  <input type="radio" name="16" value="on"><strong>On</strong>
<input type="radio" name="16" value="off" checked="checked"><strong>Off</strong>
 &nbsp;&nbsp;(<code>default = <strong>off</strong></code>)<br/>
Scatterings <i>q qbar -> nu_e^* nu_ebar</i>. 
Code 4032.
  

<br/><br/><strong>ExcitedFermion:qqbar2muStarmu</strong>  <input type="radio" name="17" value="on"><strong>On</strong>
<input type="radio" name="17" value="off" checked="checked"><strong>Off</strong>
 &nbsp;&nbsp;(<code>default = <strong>off</strong></code>)<br/>
Scatterings <i>q qbar -> mu^*+- mu^-+</i>. 
Code 4033.
  

<br/><br/><strong>ExcitedFermion:qqbar2numuStarnumu</strong>  <input type="radio" name="18" value="on"><strong>On</strong>
<input type="radio" name="18" value="off" checked="checked"><strong>Off</strong>
 &nbsp;&nbsp;(<code>default = <strong>off</strong></code>)<br/>
Scatterings <i>q qbar -> nu_mu^* nu_mubar</i>. 
Code 4034.
  

<br/><br/><strong>ExcitedFermion:qqbar2tauStartau</strong>  <input type="radio" name="19" value="on"><strong>On</strong>
<input type="radio" name="19" value="off" checked="checked"><strong>Off</strong>
 &nbsp;&nbsp;(<code>default = <strong>off</strong></code>)<br/>
Scatterings <i>q qbar -> tau^*+- tau^-+</i>. 
Code 4035.
  

<br/><br/><strong>ExcitedFermion:qqbar2nutauStarnutau</strong>  <input type="radio" name="20" value="on"><strong>On</strong>
<input type="radio" name="20" value="off" checked="checked"><strong>Off</strong>
 &nbsp;&nbsp;(<code>default = <strong>off</strong></code>)<br/>
Scatterings <i>q qbar -> nu_tau^* nu_taubar</i>. 
Code 4036.
  

<h3>Excited fermions, parameters</h3>

The basic couplings of the model are

<br/><br/><table><tr><td><strong>ExcitedFermion:Lambda </td><td></td><td> <input type="text" name="21" value="1000." size="20"/>  &nbsp;&nbsp;(<code>default = <strong>1000.</strong></code>; <code>minimum = 100.</code>)</td></tr></table>
Compositeness scale <i>Lambda</i> in GeV.
  

<br/><br/><table><tr><td><strong>ExcitedFermion:coupF </td><td></td><td> <input type="text" name="22" value="1.0" size="20"/>  &nbsp;&nbsp;(<code>default = <strong>1.0</strong></code>; <code>minimum = 0.0</code>)</td></tr></table>
Strength <i>f</i> of the <i>SU(2)</i> coupling.
  

<br/><br/><table><tr><td><strong>ExcitedFermion:coupFprime </td><td></td><td> <input type="text" name="23" value="1.0" size="20"/>  &nbsp;&nbsp;(<code>default = <strong>1.0</strong></code>; <code>minimum = 0.0</code>)</td></tr></table>
Strength <i>f'</i> of the <i>U(1)</i> coupling.
  

<br/><br/><table><tr><td><strong>ExcitedFermion:coupFcol </td><td></td><td> <input type="text" name="24" value="1.0" size="20"/>  &nbsp;&nbsp;(<code>default = <strong>1.0</strong></code>; <code>minimum = 0.0</code>)</td></tr></table>
Strength <i>f_c</i> of the <i>SU(3)</i> coupling.
  

<h3>Contact interactions, production processes</h3>

The processes including contact interactions are

<br/><br/><strong>ContactInteractions:QCqq2qq</strong>  <input type="radio" name="25" value="on"><strong>On</strong>
<input type="radio" name="25" value="off" checked="checked"><strong>Off</strong>
 &nbsp;&nbsp;(<code>default = <strong>off</strong></code>)<br/>
Scatterings <i>q q -> q q</i> including contact interactions. 
Code 4201.
  

<br/><br/><strong>ContactInteractions:QCqqbar2qqbar</strong>  <input type="radio" name="26" value="on"><strong>On</strong>
<input type="radio" name="26" value="off" checked="checked"><strong>Off</strong>
 &nbsp;&nbsp;(<code>default = <strong>off</strong></code>)<br/>
Scatterings <i>q qbar -> q' qbar'</i> including contact interactions. 
Code 4202.
  

<br/><br/><strong>ContactInteractions:QCffbar2eebar</strong>  <input type="radio" name="27" value="on"><strong>On</strong>
<input type="radio" name="27" value="off" checked="checked"><strong>Off</strong>
 &nbsp;&nbsp;(<code>default = <strong>off</strong></code>)<br/>
Scatterings <i>f fbar -> e- e+</i> including contact interactions. 
Code 4203.
  

<br/><br/><strong>ContactInteractions:QCffbar2mumubar</strong>  <input type="radio" name="28" value="on"><strong>On</strong>
<input type="radio" name="28" value="off" checked="checked"><strong>Off</strong>
 &nbsp;&nbsp;(<code>default = <strong>off</strong></code>)<br/>
Scatterings <i>f fbar -> mu- mu+</i> including contact interactions. 
Code 4204.
  

<br/><br/><strong>ContactInteractions:QCffbar2tautaubar</strong>  <input type="radio" name="29" value="on"><strong>On</strong>
<input type="radio" name="29" value="off" checked="checked"><strong>Off</strong>
 &nbsp;&nbsp;(<code>default = <strong>off</strong></code>)<br/>
Scatterings <i>f fbar -> tau- tau+</i> including contact interactions. 
Code 4205.
  

<h3>Contact interactions, parameters</h3>

<br/><br/><table><tr><td><strong>ContactInteractions:nQuarkNew  </td><td></td><td> <input type="text" name="30" value="3" size="20"/>  &nbsp;&nbsp;(<code>default = <strong>3</strong></code>; <code>minimum = 0</code>; <code>maximum = 5</code>)</td></tr></table>
Number of allowed outgoing new quark flavours in the above
<i>q qbar -> q' qbar'</i> process. Similar to <i>HardQCD:nQuarkNew</i> 
for the QCD processes.
  

<br/><br/><table><tr><td><strong>ContactInteractions:Lambda </td><td></td><td> <input type="text" name="31" value="1000." size="20"/>  &nbsp;&nbsp;(<code>default = <strong>1000.</strong></code>; <code>minimum = 100.</code>)</td></tr></table>
Compositeness scale <i>Lambda</i> in GeV.
  

<br/><br/><table><tr><td><strong>ContactInteractions:etaLL  </td><td>  &nbsp;&nbsp;(<code>default = <strong>0</strong></code>; <code>minimum = -1</code>; <code>maximum = 1</code>)</td></tr></table>
Helicity parameter. 
<br/>
<input type="radio" name="32" value="1"><strong>1 </strong>: <br/>
<input type="radio" name="32" value="0" checked="checked"><strong>0 </strong>: <br/>
<input type="radio" name="32" value="-1"><strong>-1 </strong>: <br/>

<br/><br/><table><tr><td><strong>ContactInteractions:etaRR  </td><td>  &nbsp;&nbsp;(<code>default = <strong>0</strong></code>; <code>minimum = -1</code>; <code>maximum = 1</code>)</td></tr></table>
Helicity parameter. 
<br/>
<input type="radio" name="33" value="1"><strong>1 </strong>: <br/>
<input type="radio" name="33" value="0" checked="checked"><strong>0 </strong>: <br/>
<input type="radio" name="33" value="-1"><strong>-1 </strong>: <br/>

<br/><br/><table><tr><td><strong>ContactInteractions:etaLR  </td><td>  &nbsp;&nbsp;(<code>default = <strong>0</strong></code>; <code>minimum = -1</code>; <code>maximum = 1</code>)</td></tr></table>
Helicity parameter. 
<br/>
<input type="radio" name="34" value="1"><strong>1 </strong>: <br/>
<input type="radio" name="34" value="0" checked="checked"><strong>0 </strong>: <br/>
<input type="radio" name="34" value="-1"><strong>-1 </strong>: <br/>

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

if($_POST["1"] != "off")
{
$data = "ExcitedFermion:all = ".$_POST["1"]."\n";
fwrite($handle,$data);
}
if($_POST["2"] != "off")
{
$data = "ExcitedFermion:dg2dStar = ".$_POST["2"]."\n";
fwrite($handle,$data);
}
if($_POST["3"] != "off")
{
$data = "ExcitedFermion:ug2uStar = ".$_POST["3"]."\n";
fwrite($handle,$data);
}
if($_POST["4"] != "off")
{
$data = "ExcitedFermion:sg2sStar = ".$_POST["4"]."\n";
fwrite($handle,$data);
}
if($_POST["5"] != "off")
{
$data = "ExcitedFermion:cg2cStar = ".$_POST["5"]."\n";
fwrite($handle,$data);
}
if($_POST["6"] != "off")
{
$data = "ExcitedFermion:bg2bStar = ".$_POST["6"]."\n";
fwrite($handle,$data);
}
if($_POST["7"] != "off")
{
$data = "ExcitedFermion:egm2eStar = ".$_POST["7"]."\n";
fwrite($handle,$data);
}
if($_POST["8"] != "off")
{
$data = "ExcitedFermion:mugm2muStar = ".$_POST["8"]."\n";
fwrite($handle,$data);
}
if($_POST["9"] != "off")
{
$data = "ExcitedFermion:taugm2tauStar = ".$_POST["9"]."\n";
fwrite($handle,$data);
}
if($_POST["10"] != "off")
{
$data = "ExcitedFermion:qq2dStarq = ".$_POST["10"]."\n";
fwrite($handle,$data);
}
if($_POST["11"] != "off")
{
$data = "ExcitedFermion:qq2uStarq = ".$_POST["11"]."\n";
fwrite($handle,$data);
}
if($_POST["12"] != "off")
{
$data = "ExcitedFermion:qq2sStarq = ".$_POST["12"]."\n";
fwrite($handle,$data);
}
if($_POST["13"] != "off")
{
$data = "ExcitedFermion:qq2cStarq = ".$_POST["13"]."\n";
fwrite($handle,$data);
}
if($_POST["14"] != "off")
{
$data = "ExcitedFermion:qq2bStarq = ".$_POST["14"]."\n";
fwrite($handle,$data);
}
if($_POST["15"] != "off")
{
$data = "ExcitedFermion:qqbar2eStare = ".$_POST["15"]."\n";
fwrite($handle,$data);
}
if($_POST["16"] != "off")
{
$data = "ExcitedFermion:qqbar2nueStarnue = ".$_POST["16"]."\n";
fwrite($handle,$data);
}
if($_POST["17"] != "off")
{
$data = "ExcitedFermion:qqbar2muStarmu = ".$_POST["17"]."\n";
fwrite($handle,$data);
}
if($_POST["18"] != "off")
{
$data = "ExcitedFermion:qqbar2numuStarnumu = ".$_POST["18"]."\n";
fwrite($handle,$data);
}
if($_POST["19"] != "off")
{
$data = "ExcitedFermion:qqbar2tauStartau = ".$_POST["19"]."\n";
fwrite($handle,$data);
}
if($_POST["20"] != "off")
{
$data = "ExcitedFermion:qqbar2nutauStarnutau = ".$_POST["20"]."\n";
fwrite($handle,$data);
}
if($_POST["21"] != "1000.")
{
$data = "ExcitedFermion:Lambda = ".$_POST["21"]."\n";
fwrite($handle,$data);
}
if($_POST["22"] != "1.0")
{
$data = "ExcitedFermion:coupF = ".$_POST["22"]."\n";
fwrite($handle,$data);
}
if($_POST["23"] != "1.0")
{
$data = "ExcitedFermion:coupFprime = ".$_POST["23"]."\n";
fwrite($handle,$data);
}
if($_POST["24"] != "1.0")
{
$data = "ExcitedFermion:coupFcol = ".$_POST["24"]."\n";
fwrite($handle,$data);
}
if($_POST["25"] != "off")
{
$data = "ContactInteractions:QCqq2qq = ".$_POST["25"]."\n";
fwrite($handle,$data);
}
if($_POST["26"] != "off")
{
$data = "ContactInteractions:QCqqbar2qqbar = ".$_POST["26"]."\n";
fwrite($handle,$data);
}
if($_POST["27"] != "off")
{
$data = "ContactInteractions:QCffbar2eebar = ".$_POST["27"]."\n";
fwrite($handle,$data);
}
if($_POST["28"] != "off")
{
$data = "ContactInteractions:QCffbar2mumubar = ".$_POST["28"]."\n";
fwrite($handle,$data);
}
if($_POST["29"] != "off")
{
$data = "ContactInteractions:QCffbar2tautaubar = ".$_POST["29"]."\n";
fwrite($handle,$data);
}
if($_POST["30"] != "3")
{
$data = "ContactInteractions:nQuarkNew = ".$_POST["30"]."\n";
fwrite($handle,$data);
}
if($_POST["31"] != "1000.")
{
$data = "ContactInteractions:Lambda = ".$_POST["31"]."\n";
fwrite($handle,$data);
}
if($_POST["32"] != "0")
{
$data = "ContactInteractions:etaLL = ".$_POST["32"]."\n";
fwrite($handle,$data);
}
if($_POST["33"] != "0")
{
$data = "ContactInteractions:etaRR = ".$_POST["33"]."\n";
fwrite($handle,$data);
}
if($_POST["34"] != "0")
{
$data = "ContactInteractions:etaLR = ".$_POST["34"]."\n";
fwrite($handle,$data);
}
fclose($handle);
}

?>
</body>
</html>

<!-- Copyright (C) 2012 Torbjorn Sjostrand -->

