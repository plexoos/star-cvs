#! /opt/star/bin/perl -w
#
#  
#
# dbupdateDAQProd.pl - script to update Production FileCatalog and JobStatus
#
# L.Didenko
############################################################################

use Mysql;
use Class::Struct;
use File::Basename;
use File::Find;
use Net::FTP;

require "/afs/rhic/star/packages/DEV00/mgr/dbCpProdSetup.pl";
require "/afs/rhic/star/packages/DEV00/mgr/dbOnLineSetup.pl";
require "/afs/rhic/star/packages/DEV00/mgr/dbDescriptorSetup.pl";

my $debugOn=0;


my $DISK1 = "/star/rcf/disk00001/star";

my @DISKR = (
              "/star/rcf/data03/reco",
              "/star/rcf/disk00001/star/reco",
#              "/star/rcf/data09/reco",
#              "/star/rcf/data10/reco",
#              "/star/rcf/data05/reco",
#              "/star/rcf/data08/reco",
#              "/star/rcf/data07/reco",
); 

my $prodSr = "P00hi";
my $jobFDir = "/star/u2e/starreco/" . $prodSr ."/requests/";

my $topHpssReco  =  "/home/starreco/reco";

my @SetD = (
#             "P00hg/2000/06",
#             "P00hg/2000/07",
             "P00hi/2000/08", 
             "P00hi/2000/09", 
);

my @SetS = (
#             "daq/2000/06",
#             "daq/2000/07",
             "daq/2000/08",
             "daq/2000/09", 
);

my @DirD = (
#            "2000/06",
#            "2000/07",
            "2000/08",
            "2000/09",
);

my $recoDir = ("daq");


struct JFileAttr => {
          prSer  => '$',
          job_id => '$', 
          smFile => '$',
          smDir  => '$',
          jbFile => '$',
          NoEvt  => '$',
          NoEvSk => '$',
          jobSt  => '$',  
          FstEvt => '$',
          LstEvt => '$',
		    };
 
 struct FileAttr => {
    filename  => '$',
    fpath     => '$', 
    dsize     => '$',
    timeS     => '$',
    faccess   => '$',
    fowner    => '$',
};

 struct DaqAttr =>   {
          dName   => '$',
          drun    => '$',
          bgEvt   => '$',
          endEvt  => '$',
          numEvt  => '$',
          evType  => '$',
               };  

 my %monthHash = (
                  "Jan" => 1,
                  "Feb" => 2, 
                  "Mar" => 3, 
                  "Apr" => 4, 
                  "May" => 5, 
                  "Jun" => 6, 
                  "Jul" => 7, 
                  "Aug" => 8, 
                  "Sep" => 9, 
                  "Oct" => 10, 
                  "Nov" => 11, 
                  "Dec" => 12
                  );

my %daqHash = ();
my %collHash = (
                "Gold"    => "Au", 
                "Protons" => "p",
                "Gas"     => "gas"
              );  


struct RunAttr => {
        drun   => '$',
        iname  => '$',
        jname  => '$',
        iMomnt => '$',
        jMomnt => '$',
        magFld => '$',
        rtpc   => '$', 
        rftpc  => '$',
        rsvt   => '$',
        remc   => '$',
        rsmd   => '$',
        rtof   => '$',
        rrich  => '$',
        rgl3   => '$',
        rsl3   => '$', 
}; 

my @runDetector = ("tpc","svt","ftpc","emc","smd","tof","rich","gl3","sl3"); 
my @DetecOn;
my @runDescr;
my $nrunDescr = 0;

my @runSet;
my $nrunSet = 0;
my @jobSum_set;
my $jobSum_no = 0;
my @jobFSum_set;
my $jobFSum_no = 0;
my $jbSt = "n\/a";

########## Find reco for daq files on HPSS

my $nDHpssFiles = 0;
my $nDHpssDirs = 3;
my @hpssDstDirs;
my @hpssDstFiles;
my @dbDaqFiles;
my $ndbDaqFiles = 0;
my @dbOnFiles;
my $ndbOnFiles = 0;


 $nDHpssFiles = 0;

&StDbOnLineConnect();

 $sql="SELECT file, eventType,beginEvent, endEvent, numberOfEvents FROM $daqTagFilesT WHERE eventType = '3' ";

  $cursor =$dbh->prepare($sql)
   || die "Cannot prepare statement: $DBI::errstr\n";
  $cursor->execute;
 
   while(@fields = $cursor->fetchrow) {
    my $cols=$cursor->{NUM_OF_FIELDS};
       $fObjAdr = \(DaqAttr->new());

       for($i=0;$i<$cols;$i++) {
         my $fvalue=$fields[$i];
         my $fname=$cursor->{NAME}->[$i];
#       print "$fname = $fvalue\n" ;

        ($$fObjAdr)->dName($fvalue)     if( $fname eq 'file');
        ($$fObjAdr)->evType($fvalue)    if( $fname eq 'eventType'); 
        ($$fObjAdr)->bgEvt($fvalue)     if( $fname eq 'beginEvent');
        ($$fObjAdr)->endEvt($fvalue)    if( $fname eq 'endEvent'); 
        ($$fObjAdr)->numEvt($fvalue)    if( $fname eq 'numberOfEvents'); 
   }

       $dbOnFiles[$ndbOnFiles] = $fObjAdr;
       $ndbOnFiles++; 
}
   print "Total Number of files = ", $ndbOnFiles, "\n";

 &StDbOnLineDisconnect();


 for( $ll = 0; $ll<scalar(@SetD); $ll++) {
   $hpssDstDirs[$ll] = $topHpssReco . "/" . $SetD[$ll];
   print "hpssDstDir:", $hpssDstDirs[$ll], "\n";
 }
 
 print "\nFinding daq DST files in HPSS\n";
 my $ftpRDaq = Net::FTP->new("hpss.rcf.bnl.gov", Port => 2121, Timeout=>200)
   or die "HPSS access failed";
 $ftpRDaq->login("starreco","MockData") or die "HPSS access failed";

 &walkDHpss( $ftpRDaq, \@hpssDstDirs, \@hpssDstFiles );
 print "Total files: ".@hpssDstFiles."\n";
  $ftpRDaq->quit();


my $maccess; 
my $mdowner; 
my $flname;
my $nDiskFiles = 0;
my $ndir = 0;

#####  find daq reco files on disk

 my @diskDstDirs;
 $nDiskFiles = 0;
 print "\nFinding daq reco files in disk\n";

 for( $kk = 0; $kk<scalar(@DISKR); $kk++)  { 
 for( $ll = 0; $ll<scalar(@SetD); $ll++) {
   $diskDstDirs[$ndir] = $DISKR[$kk] . "/" . $SetD[$ll];
   print "diskDstDir: $diskDstDirs[$ndir]\n";
   $ndir++;   
 }
}


 foreach $diskDir (@diskDstDirs) {
             if (-d $diskDir) {
    opendir(DIR, $diskDir) or die "can't open $diskDir\n";
   while( defined($flname = readdir(DIR)) ) {
      next if $flname =~ /^\.\.?$/;

         $maccess = "-rw-r--r--"; 
         $mdowner = "starreco";

      $fullname = $diskDir."/".$flname;
   
      my @dirF = split(/\//, $diskDir); 
#      my $set = sprintf("%s\/%s\/%s\/%s",$dirF[3],$dirF[4],$dirF[5],$dirF[6]);
#    print "Dst Set = ", $set, "\n";                                       
     ($size, $mTime) = (stat($fullname))[7, 9];

    ($sec,$min,$hr,$dy,$mo,$yr) = (localtime($mTime))[0,1,2,3,4,5];
     $mo = sprintf("%2.2d", $mo+1);
     $dy = sprintf("%2.2d", $dy);
  
     if( $yr > 98 ) {
       $fullyear = 1900 + $yr;
     } else {
       $fullyear = 2000 + $yr;
     }

     $timeS = sprintf ("%4.4d%2.2d%2.2d",
                       $fullyear,$mo,$dy);
    
     $fObjAdr = \(FileAttr->new());
     ($$fObjAdr)->filename($flname);
     ($$fObjAdr)->fpath($diskDir);
     ($$fObjAdr)->dsize($size);
     ($$fObjAdr)->timeS($timeS);
     ($$fObjAdr)->faccess($maccess);
     ($$fObjAdr)->fowner($mdowner);
     $hpssDstFiles[$nDHpssFiles] = $fObjAdr;
    $nDHpssFiles++;
    $nDiskFiles++;
  }
 closedir DIR;
 }
}
 print "Total daq reco files: $nDiskFiles\n";

##### connect to the DB

 &StDbProdConnect();

 for ($ll = 0; $ll<scalar(@SetS); $ll++) {

 $sql="SELECT fName, Nevents FROM $FileCatalogT WHERE path like '%$SetS[$ll]%' AND fName like '%daq' AND Nevents = 0";

   $cursor =$dbh->prepare($sql)
    || die "Cannot prepare statement: $DBI::errstr\n";
   $cursor->execute;
 
    while(@fields = $cursor->fetchrow) {
     my $cols=$cursor->{NUM_OF_FIELDS};
        $fObjAdr = \(DaqAttr->new());

        for($i=0;$i<$cols;$i++) {
          my $fvalue=$fields[$i];
          my $fname=$cursor->{NAME}->[$i];
#        print "$fname = $fvalue\n" ;

        ($$fObjAdr)->dName($fvalue)     if( $fname eq 'fName');
        ($$fObjAdr)->numEvt($fvalue)    if( $fname eq 'Nevents');  
   }

       $dbDaqFiles[$ndbDaqFiles] = $fObjAdr;
       $ndbDaqFiles++; 
 }
 }

##### update daq files in FileCatalog with number of events from Online DB
######## declare variables needed to update daq files in FileCatalog

 my $mNevts = 0;
 my $mNevtLo = 0;
 my $mNevtHi = 0;
 my $mFile;
 my $mEvType = 0;
    foreach $eachDaqFile (@dbDaqFiles) {
          
 $mNevts = 0;
 $mNevtLo = 0;
 $mNevtHi = 0;
 $mEvType = 0;          
         $mFile = ($$eachDaqFile)->dName;
    
        foreach $eachOnFile (@dbOnFiles) {   

          $dbFile = ($$eachOnFile)->dName;
          $mNevts = ($$eachOnFile)->numEvt;
          $mNevtLo = ($$eachOnFile)->bgEvt;
          $mNevtHi = ($$eachOnFile)->endEvt;    
          $mEvType = ($$eachOnFile)->evType; 
  	 if (($mFile eq $dbFile) and ($mNevts > 0)) {

    print "File, Nevent, FirstEv, LastEv: ", $mFile," % ",$mNevts," % ", $mNevtLo," % ", $mNevtHi, "\n";

  &updateDAQTable();

      last;
     }else{
     next;
    }
   }
 }

##### select from JobStatus table files which should be updated

 $sql="SELECT prodSeries, JobID, sumFileName, sumFileDir, jobfileName FROM $JobStatusT WHERE prodSeries = '$prodSr' AND jobfileName like 'P00h%' AND jobStatus = 'n/a' ";


   $cursor =$dbh->prepare($sql)
    || die "Cannot prepare statement: $DBI::errstr\n";
   $cursor->execute;
 
    while(@fields = $cursor->fetchrow) {
     my $cols=$cursor->{NUM_OF_FIELDS};
        $fObjAdr = \(JFileAttr->new());
 

    for($i=0;$i<$cols;$i++) {
     my $fvalue=$fields[$i];
       my $fname=$cursor->{NAME}->[$i];
#        print "$fname = $fvalue\n" ;

        ($$fObjAdr)->prSer($fvalue)    if( $fname eq 'prodSeries');
        ($$fObjAdr)->job_id($fvalue)   if( $fname eq 'JobID'); 
        ($$fObjAdr)->smFile($fvalue)   if( $fname eq 'sumFileName'); 
        ($$fObjAdr)->smDir($fvalue)    if( $fname eq 'sumFileDir'); 
        ($$fObjAdr)->jbFile($fvalue)   if( $fname eq 'jobfileName');
  
   }

       $jobSum_set[$jobSum_no] = $fObjAdr;
       $jobSum_no++; 

 }
 
 my $mchainOp;
 my $pr_chain;
 my $mjobFname;
 my $jb_news;
 my $jb_archive;
 my $jb_jobfile;
 my $jb_hold;
 my $filename;
 my $JOB_DIR;
 my @parts;
 my $mjobDg = "none";
 my $first_evts = 0;
 my $last_evts = 0;

######### declare variables needed to fill the JobStatus table

 my $msJobId = "n\/a";
 my $mjobSt = "n\/a";
 my $mNev  = 0;
 my $mEvtSk = 0;
 my $mCPU = 0;
 my $mRealT = 0;
 my $mmemSz = 0;
 my $mNoTrk = 0;
 my $mNoVert = 0;
 my $mnodeId = "n\/a";
 my $jb_sumFile;
 my $msumFile;
 my $msumDir;
 my $mproSr;
 my $mjbDir = "new_jobs";

 $jobFSum_no = 0;

 foreach my $jobnm (@jobSum_set){
       $mproSr   = ($$jobnm)->prSer;
       $msJobId   = ($$jobnm)->job_id;
       $msumFile = ($$jobnm)->smFile;
       $msumDir  = ($$jobnm)->smDir;
       $mjobFname = ($$jobnm)->jbFile;
     my $jfile = $msumFile;
      $jfile =~ s/.sum//g;
      $first_evts = 0;
      $last_evts = 0;
      $mEvtSk = 0;
      $mjobSt = "n\/a";
      $mNev  = 0;
      $mCPU = 0;
      $mRealT = 0; 
      $mmemSz = 0;
      $mNoTrk = 0;
      $mNoVert = 0;
      $mnodeId = "n\/a";

      @parts = split ("/",$msumDir);
 
     $JOB_DIR = $jobFDir . $parts[7];
#    print "Job Dir = ", $JOB_DIR, "\n";
 
    $jb_news = $JOB_DIR . "/new_jobs/" . $mjobFname;
    $jb_archive = $JOB_DIR . "/archive/" . $mjobFname;
    $jb_jobfile = $JOB_DIR . "/jobfiles/" . $mjobFname;
    $jb_hold = $JOB_DIR . "/jobs_hold/" . $mjobFname;
    if (-f $jb_news)     {$mjbDir = "new_jobs"};
    if (-f $jb_archive)  {$mjbDir = "archive"};
    if (-f $jb_jobfile)  {$mjbDir = "jobfiles"};
    if (-f $jb_hold)     {$mjbDir = "jobs_hold"};  

       opendir(DIR, $msumDir) or die "can't open $msumDir\n";
     while( defined($filename = readdir(DIR)) ) {
       next if $filename =~ /^\.\.?$/;
         next if ( !($filename =~ /.sum$/) );
        if ( $filename =~ /$msumFile/ ) {
      $jb_sumFile = $msumDir . "/" . $msumFile;
       $mjobDg = "none";
       $mjobSt = "n\/a"; 

           &sumInfo("$jb_sumFile",1);

      print "JobFile=", $mjobFname," % ", "Job Status: ", $mjobSt,"\n";
#     print "Event first, last, Done, Skip :", $first_evts," % ",$last_evts," % ",$mNev," % ", $mEvtSk, "\n";

##### update JobStatus table with info for jobs completed

     print "updating JobStatus table\n";
 
     &updateJSTable(); 

      $fObjAdr = \(JFileAttr->new());
       
      ($$fObjAdr)->prSer($mproSr);    
      ($$fObjAdr)->job_id($msJobId);  
      ($$fObjAdr)->smFile($msumFile);
      ($$fObjAdr)->jbFile($mjobFname);   
      ($$fObjAdr)->NoEvt($mNev);
      ($$fObjAdr)->NoEvSk($mEvtSk); 
      ($$fObjAdr)->FstEvt($first_evts);
      ($$fObjAdr)->LstEvt($last_evts);               

     $jobFSum_set[$jobFSum_no] = $fObjAdr;
     $jobFSum_no++; 

       last;
     } else {
        next;
      }
    }   
     closedir DIR;

 }

######## declare variables needed to fill the database table

 my $mJobId = "n\/a";
 my $mrunId = 0;
 my $mfileSeq = 0;
 my $mevtType = 0;
 my $mfName = "n\/a";
 my $mpath  = "n\/a";
 my $mdataSet = "n\/a";
 my $msize = 0;
 my $mcTime = 00-00-00;
 my $mowner = "n\/a";
 my $mprotc = "-rw-r-----";
 my $mtype = "n\/a";
 my $mcomp = "n\/a";
 my $mformat = "n\/a";
 my $msite = "n\/a";
 my $mhpss = "Y";
 my $mstatus = 0;


#####=======================================================
##### hpss reco daq file check

 my @flsplit;
 my $mfileS;
 my $extn;
 my $mrun;

      foreach $eachDstFile (@hpssDstFiles) {

##### reinitialize variables

 $mJobId = "n\/a"; 
 $mrunId = 0;
 $mfileSeq = 0;
 $mevtType = 0;
 $mfName = "n\/a";
 $mpath  = "n\/a";
 $mdataSet = "n\/a";
 $msize = 0;
 $mcTime = 00-00-00;
 $mNevts = 0;
 $mNevtLo = 0;
 $mNevtHi = 0;
 $mowner = "n\/a";
 $mprotc = "-rw-r-----";
 $mtype = "n\/a";
 $mcomp = "n\/a";
 $mformat = "n\/a";
 $msite = "n\/a";
 $mhpss = "Y";
 $mstatus = 0;
    
##### end of reinitialization

my $dfile;
my $daqName;
my $daqType = 0;

  $mfName = ($$eachDstFile)->filename;
  $mpath  = ($$eachDstFile)->fpath;
  $mcTime  = ($$eachDstFile)->timeS;
  $mprotc = ($$eachDstFile)->faccess;
  $mowner = ($$eachDstFile)->fowner;
  $msize = ($$eachDstFile)->dsize;

 if($mfName =~ /root/) {
    $mformat = "root";
    $basename = basename("$mfName",".root");   
    my $compont = $basename;
    if ($compont =~ m/\.([a-z0-9_]{3,})$/) {
    $mcomp = $1;
  } 
  @flsplit = split ("_",$basename);  
  $mfileS = $flsplit[4];
  $mrun =  $flsplit[2];
  $mrunId = $mrun;
  $extn = "." . $mcomp;
  $mfileSeq = basename("$mfileS","$extn"); 

  if ($mpath =~ /starreco/) {  
  $msite = "hpss_rcf";
  $mhpss = "Y";
   }else {
  $msite = "disk_rcf";
  $mhpss = "N";
 }
  $mtype = "daq_reco";

  $daqType = 0; 

  foreach my $daqFile (@dbOnFiles){
        $daqName =  ($$daqFile)->dName;
        $daqType =  ($$daqFile)->evType;
        $daqName =~ s/.daq//g; 
 	if ($mfName =~ /$daqName/) {
           $mevtType = $daqType;

 foreach my $jobnm (@jobFSum_set){
       $mproSr   = ($$jobnm)->prSer;
       $msumFile = ($$jobnm)->smFile;
       $mJobId   = ($$jobnm)->job_id;
       $mNevts = ($$jobnm)->NoEvt;
       $mNevtLo =($$jobnm)->FstEvt;
       $mNevtHi =($$jobnm)->LstEvt;
       $dfile = $msumFile;
       $dfile =~ s/.sum//g;
        
    if ( $mfName =~ /$dfile/) {

 print "File Name :", $mpath, " % ", $mfName, " % ", "Num Events :", $mNevts, "\n";     
    print "updating FileCatalogT table\n";
 
    &fillDbTable();   

      last;
     } else {
        next;
     }
   }   
    last;
  }else{
  next;
   } 
  }
 }else{
  next;
 }
}

##### select files where dataset is not defined

my $myRun;

 for ($ll = 0; $ll<scalar(@DirD); $ll++) {

 $sql="SELECT DISTINCT runID FROM $FileCatalogT WHERE path like '%$DirD[$ll]' AND dataset = 'n/a' ";

   $cursor =$dbh->prepare($sql)
    || die "Cannot prepare statement: $DBI::errstr\n";
   $cursor->execute;
 
    while(@fields = $cursor->fetchrow) {
     my $cols=$cursor->{NUM_OF_FIELDS};

        for($i=0;$i<$cols;$i++) {
          my $fvalue=$fields[$i];
          my $fname=$cursor->{NAME}->[$i];
#        print "$fname = $fvalue\n" ;

        $myRun = $fvalue     if( $fname eq 'runID');  
   }
        $runSet[$nrunSet] = $myRun;
        $nrunSet++;
 }
 }

 &StDbProdDisconnect(); 

##### connect to the DB RunLog

 &StDbDescriptorConnect();

  for ($ii = 0; $ii<scalar(@runSet); $ii++) { 

 $sql="SELECT $runDescriptorT.runNumber as runDNum, cwName, ccwName, cwMomentum, ccwMomentum, magFieldCurrent, $daqDescriptorT.runNumber as runTNum, tpc, svt, ftpc, emc, smd, tof, rich, gl3, sl3 FROM $runDescriptorT, $daqDescriptorT WHERE category = 'physics' AND $runDescriptorT.runNumber = '$runSet[$ii]' AND $daqDescriptorT.runNumber = $runDescriptorT.runNumber ";

   $cursor =$dbh->prepare($sql)
    || die "Cannot prepare statement: $DBI::errstr\n";
   $cursor->execute;
 
    while(@fields = $cursor->fetchrow) {
      my $cols=$cursor->{NUM_OF_FIELDS};
        $fObjAdr = \(RunAttr->new());

        for($i=0;$i<$cols;$i++) {
           my $fvalue=$fields[$i];
           my $fname=$cursor->{NAME}->[$i];
#        print "$fname = $fvalue\n" ;

    ($$fObjAdr)->drun($fvalue)      if( $fname eq 'runDNum');
    ($$fObjAdr)->iname($fvalue)     if( $fname eq 'cwName');   
    ($$fObjAdr)->jname($fvalue)     if( $fname eq 'ccwName');
    ($$fObjAdr)->iMomnt($fvalue)    if( $fname eq 'cwMomentum');
    ($$fObjAdr)->jMomnt($fvalue)    if( $fname eq 'ccwMomentum');
    ($$fObjAdr)->magFld($fvalue)    if( $fname eq 'magFieldCurrent');
    ($$fObjAdr)->rtpc($fvalue)      if( $fname eq 'tpc');
    ($$fObjAdr)->rsvt($fvalue)      if( $fname eq 'svt');
    ($$fObjAdr)->rftpc($fvalue)     if( $fname eq 'ftpc');
    ($$fObjAdr)->remc($fvalue)      if( $fname eq 'emc');
    ($$fObjAdr)->rsmd($fvalue)      if( $fname eq 'smd');
    ($$fObjAdr)->rtof($fvalue)      if( $fname eq 'tof');
    ($$fObjAdr)->rrich($fvalue)     if( $fname eq 'rich');
    ($$fObjAdr)->rgl3($fvalue)      if( $fname eq 'gl3');
    ($$fObjAdr)->rsl3($fvalue)      if( $fname eq 'sl3'); 

   }
     $runDescr[$nrunDescr] = $fObjAdr;
     $nrunDescr++;
 } 
}
 &StDbDescriptorDisconnect();

my $Numrun;
my $cWname;
my $cEname;
my $cWMnt;
my $cEMnt; 
my $ccn;
my $enrg;
my $magF;
my $dataset = "n/a";
my $mrunID;

 &StDbProdConnect();

foreach my $runDsc (@runDescr) {

       $Numrun     = ($$runDsc)->drun;
       $cWname     = ($$runDsc)->iname;
       $cEname     = ($$runDsc)->jname;   
       $cWMnt      = ($$runDsc)->iMomnt;
       $cEMnt      = ($$runDsc)->jMomnt;
       $magF       = ($$runDsc)->magFld;
       $DetecOn[0] = ($$runDsc)->rtpc;
       $DetecOn[1] = ($$runDsc)->rsvt;
       $DetecOn[2] = ($$runDsc)->rftpc;
       $DetecOn[3] = ($$runDsc)->remc;
       $DetecOn[4] = ($$runDsc)->rsmd;
       $DetecOn[5] = ($$runDsc)->rtof;
       $DetecOn[6] = ($$runDsc)->rrich;
       $DetecOn[7] = ($$runDsc)->rgl3;
       $DetecOn[8] = ($$runDsc)->rsl3;

       $ccn = $collHash{$cWname}.$collHash{$cEname};
       $engr = int($cWMnt + $cEMnt);
       if( !defined $magF) {$magF = 0};  

       if ($magF < 2000) {
       $daqHash{$Numrun} = $ccn . $engr ."_" ."FieldOff" . "_";
     }
       elsif ( $magF > 2240 && $magF < 2252 ) {
       $daqHash{$Numrun} = $ccn . $engr ."_" ."HalfField" . "_";
    }
       elsif ( $magF > 3000 ) {
       $daqHash{$Numrun} = $ccn . $engr ."_" ."FullField" . "_";
   } else{
       $daqHash{$Numrun} = $ccn . $engr ."_" ."Unknown" . "_";
   }
       for ($ll = 0; $ll < scalar(@runDetector); $ll++) {

       if($DetecOn[$ll] != 0) { 
       $daqHash{$Numrun} .= $runDetector[$ll]. "."; 
     }
     }
    chop $daqHash{$Numrun};
    print "RunID : ", $Numrun, " % " ,$daqHash{$Numrun}, "\n";

       $mrunID = $Numrun;
       $mdataset = $daqHash{$Numrun}; 
    if ( defined $mdataset) { 
    &updateDataSet();
   }
 } 

#### finished with data base
   &StDbProdDisconnect();

  exit;

################################################################################
 sub updateJSTable {

   $sql="update $JobStatusT set ";
   $sql.="jobfileDir='$mjbDir',";
   $sql.="jobStatus='$mjobSt',";
   $sql.="NoEvents='$mNev',";
   $sql.="mem_size_MB='$mmemSz',";
   $sql.="CPU_per_evt_sec='$mCPU',";
   $sql.="avg_no_tracks='$mNoTrk',";
   $sql.="avg_no_vertex='$mNoVert',";
   $sql.="RealTime_per_evt='$mRealT',";
   $sql.="NoEventSkip='$mEvtSk',";
   $sql.="nodeID='$mnodeId'";
   $sql.=" WHERE sumFileName = '$msumFile' AND sumFileDir = '$msumDir' AND prodSeries = '$mproSr'";
   print "$sql\n" if $debugOn;
#   print "$sql\n";
   $rv = $dbh->do($sql) || die $dbh->errstr;
  
 }

##############################################################################

sub fillDbTable {

   $sql="insert into $FileCatalogT set ";
   $sql.="jobID='$mJobId',";
   $sql.="runID='$mrunId',";
   $sql.="fileSeq='$mfileSeq',";
   $sql.="eventType='$mevtType',";
   $sql.="fName='$mfName',";
   $sql.="path='$mpath',";
   $sql.="dataset='$mdataSet',";
   $sql.="size='$msize',";
   $sql.="createTime='$mcTime',";
   $sql.="Nevents='$mNevts',";
   $sql.="NevLo='$mNevtLo',";
   $sql.="NevHi='$mNevtHi',";
   $sql.="owner='$mowner',";
   $sql.="protection='$mprotc',";
   $sql.="type='$mtype',";
   $sql.="component='$mcomp',";
   $sql.="format='$mformat',";
   $sql.="site='$msite',"; 
   $sql.="hpss='$mhpss',";
   $sql.="status= 0,";
   $sql.="comment=''";
   print "$sql\n" if $debugOn;
   $rv = $dbh->do($sql) || die $dbh->errstr;

  }
#############################################################################

 sub updateDAQTable {
  
  $sql="update $FileCatalogT set ";   
  $sql.="Nevents='$mNevts',";
  $sql.="NevLo='$mNevtLo',";
  $sql.="NevHi='$mNevtHi',";
  $sql.="eventType='$mEvType'";  
  $sql.=" WHERE fName = '$mFile'"; 
  print "$sql\n" if $debugOn;
  $rv = $dbh->do($sql) || die $dbh->errstr;
  
 }

##############################################################################

   sub updateDataSet {
  
    $sql="update $FileCatalogT set ";   
    $sql.="dataset='$mdataset'";
    $sql.=" WHERE runID = '$mrunID'"; 
    print "$sql\n" if $debugOn;
    $rv = $dbh->do($sql) || die $dbh->errstr;
  
   }

##############################################################################

 sub walkDHpss {

    my ( $ftp, $dirs, $files ) = @_;
 
 my @fields;
 my $access;
 my $downer;
 my $size;
 my $month;
 my $day;
 my $year;
 my $name; 
 my @dirF;
 my $fullDir;
 my $set; 
 
  for ($ii=0; $ii<$nDHpssDirs; $ii++) {
    my @dird = $ftp->dir($dirs->[$ii]);

     for ($jj=0; $jj<@dird; $jj++) {
         @fields = split(/\s+/, $dird[$jj]);
         $access = $fields[0]; 
         $downer = $fields[2];
         $size   = $fields[4];
         $month  = $fields[5];
         $day    = $fields[6];
         $year   = $fields[7];
         $name   = $fields[8];
         $fullDir = $dirs->[$ii];
          
         next if ( $name =~ /^delete_me/);
         next if ( $name =~ /^file/);
         next if ( $name =~ /^Star.Test/);

      
#         @dirF = split(/\//, $dirs->[$ii]); 

#        $set = sprintf("%s\/%s\/%s\/%s",$dirF[4],$dirF[5],$dirF[6],
#                                                 $dirF[7]);

      my $monthD = $monthHash{$month};
      my $sec = 0;
      my $min = 0;
      my $hr = 0;
      
      if ( $year =~ m/:/ ) {
        ( $hr, $min ) = split(/:/,$year);
        $year = (localtime())[5];
      } else {
        $year = $year - 1900;
      }
      
      if( $year > 98 ) {
        $year = 1900 + $year;
      } else {
        $year = 2000 + $year;
      }
   
   
      $timeS = sprintf ("%4.4d-%2.2d-%2.2d %2.2d:%2.2d:00",
                        $year,$monthD,$day,$hr,$min);
   
#      $timeS = sprintf ("%4.4d%2.2d%2.2d",
#                       $year,$monthD,$day);
      
      $fObjAdr = \(FileAttr->new());
      ($$fObjAdr)->filename($name);
      ($$fObjAdr)->fpath($fullDir);
      ($$fObjAdr)->dsize($size);
      ($$fObjAdr)->timeS($timeS);
      ($$fObjAdr)->faccess($access);
      ($$fObjAdr)->fowner($downer);

      $files->[$nDHpssFiles] = $fObjAdr;
      $nDHpssFiles++;
      print "File ".$name."\n" if $debugOn;
      }
    } 
}


##############################################################################

sub sumInfo {

my ($jb_sum,$useless) = @_;   
my $sum_line ;

my @word_sum;
 
my @output = `more $jb_sum`; 
   foreach my $sum_line (@output) {
            chop $sum_line;

##get node name
             if ($sum_line =~ /Starting job execution/) {
              @word_sum = split (" ", $sum_line);
              $mnodeId = $word_sum[11];
	    }
##get job status

     if ($sum_line =~ /Job status:/) {
        @word_sum = split (":", $sum_line);
         $mjobSt = $word_sum[1];
     } 
      if ($sum_line =~ /Segmentation violation/) {
              $mjobDg = "Segmentation violation";
          }
     elsif ($sum_line =~ /buss error/) {
             $mjobDg = "bus_error";
        }  
    if($sum_line =~ /Error message/)  {
        @word_sum = split (":", $sum_line); 
           $mjobDg = $word_sum[1];
      }

## get number of events done
   
   if ($sum_line =~ /Number of Events Done/ ) {
     @word_sum = split (":", $sum_line);          
       $mNev = $word_sum[1];
   } 
  	    if ( $sum_line =~ /Number of Events Skiped/ ) {
                 @word_sum = split (":", $sum_line);          
       $mEvtSk = $word_sum[1];
	       }
	    if ( $sum_line =~ /First event/ ) {
                 @word_sum = split (":", $sum_line);          
       $first_evts = $word_sum[1];
	       }
            if ( $sum_line =~ /Last event/ ) {
                   @word_sum = split (":", $sum_line);          
       $last_evts = $word_sum[1];

		 }
##get chain
         if( $sum_line =~ /QAInfo:Requested chain is/ ) {
           @word_sum = split (":", $sum_line); 
             $pr_chain = $word_sum[2];
             $pr_chain =~ s/ /_/g;
            if ( $pr_chain =~ /^\s*_/ ) {
              my $mIndex = index $pr_chain, "_";
                $pr_chain = substr( $pr_chain, $mIndex+1);
               } 
              
            }
##get max memory size during execution
            if ($sum_line =~ /Package   tree:/ ) {
              @word_sum = split (" ", $sum_line);
                $mmemSz = $word_sum[5];
            }
## get CPU and REAL Time per event
             next if ($sum_line =~ /Command string/);
            if($sum_line =~ /Total: bfc/ ) {             
           @word_sum = split (" ", $sum_line);
#          print "CPU = ", $sum_line, "\n";   
            if($word_sum[8] =~ /Cpu/) { 
              $mCPU  = $word_sum[11];  
              $mRealT = $word_sum[6];  
           }
          }
## get everage number of tracks in the event

      if($sum_line =~ /QAinfo: Average number of tracks/) {
       @word_sum = split (" ", $sum_line) ;  
              $mNoTrk = $word_sum[5];
     }   
## get everage number of vertex in the event

     if($sum_line =~ /QAinfo: Average number of vertices/) {     
         @word_sum = split (" ", $sum_line) ;
          $mNoVert = $word_sum[5]; 
     }
   }
}

