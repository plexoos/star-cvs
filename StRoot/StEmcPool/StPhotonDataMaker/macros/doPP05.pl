#!/usr/bin/perl -w
my %runs = ();
open INPUT, "GoodRunsPPproduction.txt";
while(<INPUT>){
   my @input = $_;
   next unless ($input[0]=~m/^6/);
   $runs{$input[0]} = 1;
}
close INPUT;

for my $string (sort keys %runs) {
    my $runday = substr($string, 1,3);
    #next unless ($runday>144);
    #if($runday>144){
	#print "max runday reached!";
	#die;
    #}
    $string = substr($string,0,-1);
    open OUT,">job_${runday}.xml" or die;
    print OUT <<EOF;
<?xml version="1.0" encoding="utf-8" ?>
<job simulateSubmission="true" name="pp_${runday}" maxFilesPerProcess="15" filesPerHour="20" >
    <command>
stardev
cd /star/u/russcher/MuDST2MyEvent/
setenv STAR_LOGGER_PROPERTY "/star/u/russcher/log4j.xml"
root4star -b -q /star/u/russcher/MuDST2MyEvent/doMuDST2MyEvent.C\\(1000000,\\"\$FILELIST\\",\\"\$SCRATCH/run_${runday}_\$JOBID\\",\\"real\\",\\"pp05\\"\\)
</command>
    <stdout URL="file:/star/u/russcher/MuDST2MyEvent/out/${runday}_\$JOBID.out" />
    <stderr URL="file:/star/u/russcher/MuDST2MyEvent/err/${runday}_\$JOBID.err" />
    <input URL="catalog:star.bnl.gov?production=P05if,runnumber=${string},trgsetupname=ppProduction,runtype=physics,filename~physics,filetype=daq_reco_MuDst,available=1,sanity=1,emc=1,bsmd=1,tpc=1,storage!=HPSS,events>100" nFiles="all" preferStorage="local" />
    <output  fromScratch="*.root" toURL="file:/star/data01/pwg/highpt/russcher/pp05/ppProduction_rcf/" />
</job>
EOF

close OUT;

    system "star-submit job_${runday}.xml";


}
