<html>
<head>
<title>Event Statistics</title>
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

<form method='post' action='EventStatistics.php'>
 
<h2>Event Statistics</h2> 
 
At the end of the run you will want to write out the final statistics 
on number of events generated, the corresponding cross sections and 
the number of errors encountered. This is done with the 
<code>pythia.stat()</code> method, assuming <code>pythia</code> is an 
instance of the <code>Pythia</code> class.The method is steered entirely 
by settings values, see <?php $filepath = $_GET["filepath"];
echo "<a href='MainProgramSettings.php?filepath=".$filepath."' target='page'>";?>here</a>. 
 
<h3>Cross-section statistics</h3> 
 
The <code>ProcessLevel::statistics()</code> method cannot be accessed 
directly, but only via the <code>Pythia::stat()</code> call above. 
When called it will loop over the list of existing processes, and for 
each write out name, code, the number of tried, selected and accepted 
events, the cross section and the estimated error on the latter. 
The three different event numbers are related to the Monte Carlo method 
used, whereby an initial upper estimate of the cross section is used to 
select a large number of trial phase-space points, whereof then not all 
survive. Rejections are normally done by the internal machinery, but can 
also be obtained by <?php $filepath = $_GET["filepath"];
echo "<a href='UserHooks.php?filepath=".$filepath."' target='page'>";?>user hooks</a>. 
Therefore: 
<ul> 
<li><b>tried</b> events reflect the original number of 
phase-space points probed, as part of the upper estimate;</li> 
<li><b>selected</b> events correspond to those that survive 
the internal Monte-Carlo selection procedure;</li> 
<li><b>accepted</b> events are those that also survive 
the additional user cuts.</li> 
</ul> 
In most runs there would be no user hooks implemented, and then the 
numbers of selected and of accepted events will agree. Aborted events 
(see below) usually appear in the selected statistics but not in the 
accepted one. 
 
<p/> 
For Les Houches events the total cross section will be correctly 
displayed; however the (optional) error value will not be used, so that 
the reported error will be smaller than the correct statistical ones, 
and often vanish completely. Furthermore, while the number of events 
is shown for each user process, the cross section is only for the sum 
of them. 
 
<h3>Error messages</h3> 
 
When Pythia is run, errors may occur, and give rise to warning messages. 
These may be of varying severity, as follows: 
<ul> 
<li><b>Abort</b> means things went seriously wrong, and the 
initialization or event generation failed. In the former case it is 
not possible to generate events at all, in the latter the current 
event is flawed and should be skipped. In either case the respective 
method, <code>Pythia::init()</code> or <code>Pythia::next()</code>, 
then also returns the value <code>false</code>. There are occasions 
where an abort may be deliberate, such as when a file of Les Houches 
Events is read and the end of the file is reached.</li> 
<li><b>Error</b> normally is less severe. Typically the program will 
back up one step and try again. There are cases where this is not possible, 
in particular during the initialization and the generation of a hard 
process, and then the error may be followed by an abort as a direct 
consequence (with two separate messages).</li> 
<li><b>Warning</b> is even less severe. In some cases the program will 
try again, with  good chances of success, in others no measure at all 
need to be taken.</li> 
</ul> 
 
<p/> 
The error messages is handled by a small part of the <code>Info</code> 
class. It is handed any abort, error or warning messages during the event 
generation phase, and will store each distinct message, with a counter 
for how many times it is issued. Thus it is possible to limit the number 
of identical messages issued, currently hardcoded so that each kind of 
error message is only printed once 
(<code>static const int TIMESTOPRINT = 1</code>). 
This can be overridden by the calling routine, so that all messages of 
this kind are shown, which is particularly relevant for the 
initialization  stage. 
The summary table printed by <code>Pythia::stat()</code> 
provides a table with all the different messages issued, in 
alphabetical order, with the total number of times each was generated. 
 
<h3>Multiparton-interactions statistics</h3> 
 
If you call <code>Pythia::stat()</code> with 
<code>Stat:showPartonLevel = true</code> also statistics on multiparton 
interactions is printed, comprising a list of all allowed subprocesses 
with how many times each of them has been generated. For the nondiffractive 
process this also includes the hardest interaction, while else the 
hardest process is excluded from the statistics. (This is because 
the hardest process is of the same character and generated by the same 
machinery in the former case but not in the latter. Also, for the 
former case only, the standard statistics listing only lists 
nondiffractive as one single process, i.e. does not further specify 
the character of the hardest subprocess, so there is not any overlap 
between the two.) 
 
</body>
</html>
 
<!-- Copyright (C) 2018 Torbjorn Sjostrand --> 
