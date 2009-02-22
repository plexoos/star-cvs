* $Id: geometry.g,v 1.189 2009/02/22 21:36:23 perev Exp $
* $Log: geometry.g,v $
* Revision 1.189  2009/02/22 21:36:23  perev
* Y2009 born
*
* Revision 1.188  2009/02/20 21:35:05  perev
* upgr15 full tof. Jonathan/Spiros
*
* Revision 1.187  2009/02/19 00:27:22  perev
* Upgr15==>macros
*
* Revision 1.186  2009/02/13 19:20:34  perev
* back BTOF for 2008. Again itof=6 bTofConfig=11
*
* Revision 1.185  2009/01/13 03:19:12  perev
* Mag field nou controlled from starsim. BugFix
*
* Revision 1.184  2009/01/12 00:31:44  perev
* Bug fix in ON logic(VP)
*
* Revision 1.183  2009/01/08 20:16:46  perev
* Fix y2008a and y2009 btof
*
* Revision 1.182  2009/01/06 04:05:48  perev
* For y2008a,y2009 elliptic rods
*
* Revision 1.181  2009/01/03 23:03:36  perev
* BtofConfig=6 in 2008a,2009
*
* Revision 1.180  2008/12/30 19:41:09  perev
* 1st version of y2009
*
* Revision 1.179  2008/12/15 01:03:56  perev
* CleanUp
*
* Revision 1.178  2008/12/12 20:45:13  perev
* upgr16/17 btofConfig=6
*
* Revision 1.177  2008/12/08 23:02:20  perev
* C++ style comment removed
*
* Revision 1.176  2008/12/08 19:28:29  didenko
* fixed typo
*
* Revision 1.175  2008/12/05 23:46:25  perev
* y2008 bTofConfig=6 now(jan)
*
* Revision 1.174  2008/12/01 23:45:10  perev
* ubgr16 last vers BTOF
*
* Revision 1.173  2008/11/30 01:30:53  perev
* modifs for extending alpha,theta,phi,ort commandas
*
* Revision 1.172  2008/11/19 04:08:25  perev
*  updates to the corrected(vp) starsim
*
* Revision 1.171  2008/10/13 03:21:35  perev
* upgr17 added Wei(MingZhang)
*
* Revision 1.170  2008/10/13 00:22:19  perev
* upgr16 pipe changed to provisional
*
* Revision 1.168  2008/09/25 03:05:58  perev
* upgr16 (Jan)
*
* Revision 1.167  2008/06/03 22:27:16  fisyak
* Add y2005g and y2007g geometries for SVT with latest Rene's corrections
*
* Revision 1.166  2008/04/23 22:00:29  perev
* tofZ0=0.00 ==> tofZ0=-0.50 /xin
*
* Revision 1.165  2008/03/20 18:45:28  perev
* Simplest.gerrit upgr15 added
*
* Revision 1.164  2008/01/21 01:11:02  perev
* TOF weight corrected
*
* Revision 1.163  2007/11/13 21:38:08  perev
* pipeFlag and nSvtLayer==7 added
*
* Revision 1.162  2007/11/07 21:25:41  perev
* btofgeo6 added by X.Dong
*
* Revision 1.161  2007/11/06 01:19:35  perev
* y2008 geo
*
* Revision 1.160  2007/10/13 01:27:27  perev
* u2007 ==> upgr20
*
* Revision 1.159  2007/09/28 18:54:08  perev
* dongx/TOFr/y8update
*
* Revision 1.158  2007/09/25 19:56:14  perev
* U2007A added
*
* Revision 1.157  2007/09/21 20:30:08  perev
* Add U2007 geometry
*
* Revision 1.156  2007/08/15 18:06:22  potekhin
* Seec omment at
* http://drupal.star.bnl.gov/STAR/comp/simu/geometry0/changes-beampipe-support
* about the newest round of the SVT corrections (Carbon used to construct water
* channels as opposed to Beryllium). The difference of 0.4 to 0.6% of rad length
* is big enough to warrant the creation of a new tag, in this case Y2007A.
* The SVT code version activated in this tag is 10.
*
* Revision 1.155  2007/07/12 20:16:47  potekhin
* Added the following geometry tags:
* a) Y2008   -- first cut, will be improved
* b) UPGR14  -- UPGR13 sans IST
* c) DEV2007 -- sandbox for what-if studies, non-production grade
*
* Revision 1.154  2007/04/13 17:54:58  potekhin
* Based on a comment by Akio, remove the PHMD (photon
* multiplicity detector) from the Y2006 configuration, to
* reflect the actual setup for that year. The new tag is Y2006C
*
* Revision 1.153  2007/03/21 21:08:05  potekhin
* A cleaner version of managing the HFT (pixlgeo) versions
*
* Revision 1.152  2007/03/15 19:56:16  potekhin
* Provide versioning for the thicker active Si layer
* in pixlgeo3, via setting the structure elements
*
* Revision 1.151  2007/03/09 21:40:48  potekhin
* UPGR13 modifications: (a) FSTD is out (b) modified SSD with carbon parts,
* for R&D purposes (c) modified IST with single sided inner layer for the
* April proposal
*
* Revision 1.150  2007/02/23 21:45:40  potekhin
* a) re-instated the calls to IGT codes to keep a degree
* of backward compatibility (for early UPGRXX tags)
* b) deleted the previosuly commented out IST1 tag (as
* advertised earlier, it was officially retired)
* c) put in the dead material version of SSD into UPGR13 (SSD5)
*
* Revision 1.149  2007/02/23 21:20:02  potekhin
* In the R and D tag UPGR13, removed the prototype support
* cone on the East side on Gerrit  request. Corrected the
* steering logic for the new FGT (former IGT) to integrate
* the newer code obtained form Gerrit.
*
* Revision 1.148  2007/02/22 22:37:39  potekhin
* Correcting a simple typo (accidentally hit delete)
*
* Revision 1.147  2007/02/22 22:21:18  potekhin
* As stated in the S&C meeting, the recent correction
* for the dead material in the SSD should be propagated
* into earlier model-years, therefore creating new tags.
* Only two such tags were necessary and have been created:
* Y2005F and Y2006B. In addition to the SSD configuration,
* they also feature a newer version of the CALB code,
* which they share with Y2007.
*
* Revision 1.146  2007/02/16 22:57:50  potekhin
* As per Xin's communications, the correct logic for year 2007
* in the upVPD code is triggered when the config flag is set to 7.
* I make this tweak in steering for Y2007.
*
* Revision 1.145  2007/02/13 20:42:29  potekhin
* Along the lines previously discussed, replace the IGT
* by the FGT in the UPGRXX tags; in this case, by creating
* a new tag UPGR13, which is the continuation of the UPGR07
* line and will be further tuned.
*
* Revision 1.144  2007/02/09 22:04:37  potekhin
* a) added steering for new code and settings for TOF, upVPD and FPD/FMS
* b) retired IST1 (commented out, to be deleted later)
*
* Revision 1.143  2007/02/02 18:20:46  potekhin
* The updated FMS code (fpdgeo) needs more space at the
* end of the cave, so we need to add some. We will reflect
* the more precise dimensions in cavegeo.g - here we just
* add the requisite configuration flag for Y2007
*
* Revision 1.142  2007/02/02 17:18:40  potekhin
* Added logic to include the updated SSD code
*
* Revision 1.141  2006/12/21 23:06:17  potekhin
* Previous versions of UPGRxx geometries were deficient
* is that there was a clash between the IGT disks and
* other elements of the central tracker system. This lead
* to a loss of hits in the IGT, a problem which was mitigated
* in private versions of the code. To avoid prolifirations
* of the latter, we need to introduce a corrected tag,
* which is UPGR12, based on UPGR05 but with different
* disk radii.
*
* Revision 1.140  2006/12/18 23:28:33  potekhin
* Introduced geometry tags UPGR10 and UPGR11 (as discussed
* in appropriate fora) which utilize source files istbgeo4 and 5.
* Made some cosmetic changes to the code layout.
*
* Revision 1.139  2006/12/14 21:36:25  potekhin
* Add tag UPGR09 with sttering logic
* for the ISTB with only the outer layer...
*
* Revision 1.138  2006/12/12 22:32:19  potekhin
* a) enable a cleaner barrel EMC code in Y2007
* b) re-instate UPGR06 for the upcoming simulation
*
* Revision 1.137  2006/12/01 19:26:58  potekhin
* Removing the SSD from the R&D geometry UPGR05, resulting
* in a new configuration, UPGR08
*
* Revision 1.136  2006/11/28 00:02:09  potekhin
* Added Y2007 and set it up to include the new FMS (FPD)
*
* Revision 1.135  2006/11/22 17:41:49  potekhin
* Added a tag which will be used exclusively for the material
* balance effect study, i.e. it won't contains any realistic
* detectors in the center of STAR, and feature a variable
* thickness cylinder instead.
*
* Revision 1.134  2006/11/18 02:56:17  potekhin
* Rewrote UPGR01 to better conform with
* other UPGR0X tags in terms of actual code
* (same version of SSD, identical flags in most
* places). UPGR01 is HFT+SSD in central tracking,
* and nothing else.
*
* Revision 1.133  2006/11/14 00:21:03  potekhin
* Improved steering for the IGTD (gem disks), in order to
* provide the possibility of a proper versioning. This is
* done now via the IgtdConfig variable
*
* Revision 1.132  2006/11/01 00:21:09  potekhin
* As discussed in appropriate fora, we need to introduce a HPD-less
* tag for our TUP study. Let there be UPGR07.
*
* Revision 1.131  2006/10/21 18:14:21  potekhin
* a) Added steering for the TUP support structure
* b) optionally change the radius of the FSTD (to better fit with
* the rest of TUP
* c) using a more precise version of SSD code in UPGR05
*
* Revision 1.129  2006/10/09 16:19:17  potekhin
* Due to the ongoing SSD studies, we need to refine the 2005 tag,
* so as to include Lilian code that were checked into CVS in
* early 2006 but were valid in 2005 as well. We have therefore created
* the tag Y2005E, which is an improvement over Y2005D (more precise SSD),
* bigger SVT shield (to accomodate the SSD) and a full barrel calorimeter.
*
* Revision 1.128  2006/10/02 21:37:03  potekhin
* Added steering logic for the new tag UPGR05, which
* includes the HFT (former pixel), HPD, IST and SSD,
* but no SVT. GEM detectors are also excluded.
*
* Revision 1.127  2006/09/15 19:56:31  potekhin
* Due to ongoing development, we need to create a new tag,
* UPGR04, and steering logic for the new detector HPDT
*
* Revision 1.126  2006/07/07 17:41:28  potekhin
* Fixing a very old and inconsequential typo in the
* assignment of a variable for the "minimum Si layer"
* (which I doubt was previously used)
*
* Revision 1.125  2006/06/12 18:34:28  potekhin
* Created the tag Y2006A, which will allow for the all-new
* FPD to be properly included, as well as otherpotential changes
* to be implemented in mid-year.
*
* Revision 1.124  2006/06/08 19:36:36  potekhin
* By an unfortunate slip of the wrist, I deleted year2000
* and year2001 during the previous check-in. Now they are restored.
*
* Revision 1.123  2006/06/02 17:34:37  potekhin
* a) removed the PIX1 tag that was reliably
* confirmed as obsolete
* b) added the SISD_OFF flag that facilitates
* creation of test geometries in which both the SVT
* and the SSD are taken out. Needed for R&D.
*
* Revision 1.122  2006/05/05 17:38:41  potekhin
* Just rename the IST2 to UPGR03 to stivk with
* previously chosen naming convention.
*
* Revision 1.121  2006/05/05 17:24:58  potekhin
* Need a new R&D tag, IST2, to properly manage
* the configuration of an alternative tracking upgrade project.
* Other changes -- in DEV2005 -- are due to the SVT study, and
* since this is not a production tag, are immaterial.
*
* Revision 1.120  2006/03/21 23:51:41  potekhin
* Fairly significant additions:
* (a) add steering for the muon trigger system, "MUTD"
* (b) specify a complete barrel calorimeter for Y2006
* (c) add steering for the corrected SSD (sisdgeo3)
* (d) add steering for a small modifications in the SVT shield in Y2006
*
* Revision 1.119  2006/01/18 23:06:13  potekhin
* Introducing the baseline year 2006 geometry, which is "the best"
* Y2005 geo plus the bugfix in the TPC backplane (not too important).
* Pending a better definition of the SSD from Lilian which we'll have
* to version and maybe cut new tags of 2004-5 geometries if there is
* a need.
*
* As agreed, created UPGR01 and UPGR02 (note cautious use of namespace),
* which are basically SSD+HFT and IST+HFT. Note that NONE of engineering
* detail is available in either case, and less so for the integration
* structural elements. We expect to do a lot of development in this area
* as new facts and engineering data are provided. This cut will allow us
* to proceed with tracking studies in the near term.
*
* Fixed a comment and added a couple more.
*
* Revision 1.118  2005/10/20 20:17:47  potekhin
* a) added a few parameters to the "low_em" setting to
* better simulate soft EM processes, if needed
* b) added logic for calling the latest svttgeo6
* c) added Y2003C, Y2004D and Y2005D to take advantage of this
* new SVT geometry file. Added same to DEV2005 for development
* purposes
*
* Revision 1.117  2005/10/06 17:54:57  potekhin
* a) in DEV2005, provide a way to use a highly customised version of the SVT
* b) create a key that enables the user to lower the electromagnetic processes
* GEANT cut to 10 keV, from the KUMAC script without the need to recompile.
*
* Revision 1.116  2005/09/26 21:44:18  potekhin
* We need a convenient way to optionally remove the SVT from
* the simulation, to facilitate conversion and brems studies.
* To this end, add  "SVTT_OFF" to the list of options
*
* Revision 1.115  2005/09/02 18:20:35  potekhin
* Added separate config variables for the Quad section
* (which includes D0) -- way upstream area
*
* Revision 1.114  2005/08/16 00:56:13  potekhin
* Modified the development tag DEV2005 to configure the
* geometry for the shielding studies. Added steering for
* the shield and renamed a variable to avoid naming clash.
* Also, removed ZCAL from this tag, because it would take
* time to reconsile its geometry with the shield, and it's
* not too important for teh shielding study.
*
* Revision 1.113  2005/07/14 22:13:32  potekhin
* In the tag PIX1:
* Need to actuate a thinner pipe as well as an updated
* pixel detector geometry, to include an exoskeleton
* for the beampipe (R&D request from Kai et al.)
*
* Revision 1.112  2005/06/28 16:18:22  potekhin
* Add config variables and steering for the GEM
* barrel tracker -- only inlfated in the tag IST1.
*
* Revision 1.111  2005/06/03 15:54:43  potekhin
* As agreed with all parties, we would like to run the 2004 simulation
* with  improved geometry. In particular, the pp physics groups are calling
* for an updated version of Y2003X (full calorimeter). We have no other
* option but to create a new tag, Y2004Y, to reflect that.
*
* Revision 1.110  2005/05/26 16:03:31  potekhin
* a) As advertised before, removed the various Year 1 tags,
* as they were taking space and weren't used anymore
* b) Included the updated TPC backplane into the tag Y2004C,
* along with FTRO. This makes sense as (1) it wasn't used
* in production yet (2) similar updates were done in the
* latest 2005 tag.
* c) Improved formatting and comments in select places
*
* Revision 1.109  2005/04/15 15:28:23  potekhin
* a) Corrected a comment that could surreptitiously break the Mortan
* parsing and cause a bug, of the type : !----- your text here ----
* b) Improved the formatting of comments and fixed a typo
* c) As agreed with Jerome, added a special development tag DEV2005,
* which will allows as to better insure consistency and backward
* compatibility of the code when working on the improvements in the
* current tag. Production with such a development tag will be
* prohibited and effectively disabled, as implied in its designation.
*
* Revision 1.108  2005/04/11 17:47:09  potekhin
* Add the tag Y2005C, as authorized by Jerome, in order to activate
* the latest TOF upgrades from Xin
*
* Revision 1.107  2005/04/07 19:52:30  potekhin
* As per Janet's note, update the FTPC config
* in Y2004C (Ar+CO2)
*
* Revision 1.106  2005/04/04 22:13:37  potekhin
* Creating a new Y2004C per the request from Jamie and A.Wetzler.
* It contains a better version of the SSD (unavailable back in 2004),
* the correction for the hybrid chip assembly length in the SVT
* and extra copper in the cones, i.e. the components that make
* difference in conversion studies
*
* Revision 1.105  2005/03/25 17:28:24  potekhin
* Added the corrected SSD ladder positions (as per Lilian
* communication) to the tag y2005b
*
* Revision 1.104  2005/03/25 02:13:59  potekhin
* A very significant set of code changes, related to
* versioning: The CorrNum variable turned out to be
* unwieldy, dur to combinatorially large number of various
* individual detector correctionds. Accordingly, it has
* been retired.
*
* I have extended the set of "Config" variables
* to describe the parameters previously wrapped into
* CorrNum. This appears to work nicely.
*
* The tag y2005b will contain the latest improvements,
* done in this new scheme of versioning:
* a) SVT elelctronics mother volume length bug fix
* b) Ar+C02 mix in the FTPC
* c) SSD ladder radius corection (to be double-checked with Lilian)
*
* Better diagnostic printout and code simplification.
*
* Revision 1.103  2005/03/08 01:05:35  potekhin
* Created a new tag, y2005b, which is necessary to optionally
* activate the updated version of the TPC geometry. Introduced
* simple logic to handle same, similar to the versioning of
* other detectors (TpceConfig)
*
* Revision 1.102  2005/02/19 05:26:29  potekhin
* a) Corrected the comment for tag IST1
* b) Added the FGTD activation in same
*
* Revision 1.101  2005/02/02 00:16:09  potekhin
* We now have a new estimate of the copper cable mass, for
* the cables feeding the SVT and residing on the support cones.
* Included in the code now are the switches and logic to allow
* the updated configuration to be created.
*
* Revision 1.100  2005/01/19 16:40:53  potekhin
* We extended the y2005x tag from y2004x and made
* a mistake of creating only a half barrel of the SSD,
* which in fact has now been completed. I fixed that
* in y2205x and y2005 now.
*
* Revision 1.99  2005/01/03 22:11:23  potekhin
* Need to update the experimental IST1 tag to better
* refelct the needs of the new tracking group. Took
* out the FTPC, put in the Pixel and SSD, and made
* provisions for the latter to work w/o the SVT
* volumes.
*
* Revision 1.98  2004/12/07 00:46:04  potekhin
* We need to steer the newly added FSTD (forward tracker).
* For now I add it to the experimental tag IST1, which is
* used for development only.
*
* Revision 1.97  2004/11/02 19:00:55  potekhin
* Added the Y2005 tag which we need for testing and
* for general consistency. It will be subject to possible
* changes, and for the time being is same as the latest
* cut of the year 2004 (Y2004B).
*
* Revision 1.96  2004/10/28 22:05:53  potekhin
* Changed the coding convention for the SSD geometry
* "levels", which specify which file to load.
*
* Revision 1.95  2004/10/26 21:46:23  potekhin
* 1) Cleaned out the remaining test code from Y2004B
* 2) Created Y2005X which is same as Y2004B except for
* the full barrel calorimeter as per Thomas' request
*
* Revision 1.94  2004/10/26 21:11:00  potekhin
* 1) Moved filling of GDAT to the end of code after a consultation
* with Pavel -- this is less error prone as by this time the main
* Zebra bank is properly populated
* 2) Chaged the Y2004B to activate the sisdgeo1
*
* Revision 1.93  2004/09/11 01:16:32  potekhin
* Two additions:
* (a) Geo tag Y2004B will include the most recent enhancements
* such as the FTPC readout cage. In this cut, this is a modified
* geo and work in progress
* (b) Geo tag PIX1, based on the request from the pixel group.
* The inner layer of SVT has been removed such that it can coexist with
* the innder pixel based tracker
*
* Revision 1.92  2004/07/15 16:30:08  potekhin
* Since the "MITT" detector was updated and became ISTB,
* this needs to be reflected in the main geometry steering.
* The new tag replaces MITT1 and is called IST1
*
* Revision 1.91  2004/06/28 22:53:45  potekhin
* The emergence of two new detectors, the Pixel and
* the other dubbed MITT, necessitates the creation
* of dedicated geometries that can be used for R&D
* for both. The previous COMPLETE geometry was not
* well suited for this at all, and didn't allow
* proper versioning. Hence, the new MITT1 tag has
* been created, on same footing as COMPLETE but
* with a different structure.
*
* Revision 1.90  2004/05/10 21:49:38  potekhin
* For consistency, the SSD config in the y2004a tag should
* be equal 2 (even if the SSD data won't be used)
*
* Revision 1.89  2004/04/28 23:30:37  potekhin
* Deleted an unnecessary line setting the
* PHMD version. It will be done throught the
* structure PMVR anyway.
*
* Revision 1.88  2004/04/28 00:35:40  potekhin
* Extra detail in steering the PHMD geo, see the PHMD code
*
* Revision 1.87  2004/04/14 20:54:27  potekhin
* Changed the Y2004A to Y2004X to emphasize the fact
* that this is a variation of asymptotic geometry
* and not a corecction of the actual one. This
* complies with the naming scheme we used in 2003.
*
* Revision 1.86  2004/04/14 19:02:12  potekhin
* Introducing the geometry Y2004A, which is same as
* Y2004 but with full Barrel Calorimeter, a-la Y2003X,
* as per requests of PWG's. Subject to final approval.
*
* Revision 1.85  2004/03/31 16:37:51  potekhin
* Added version control for the FPD,
* via the variable FpdmConfig
*
* Revision 1.84  2004/03/24 23:33:48  potekhin
* Added proper VPD versioning as discussed with the team.
* No numerical data here, just config flag.
*
* Revision 1.83  2004/03/10 20:11:34  potekhin
* In Y2004, set the TOF config to 7 as requested by B.Llope,
* to reflect the current configuration.
*
* Revision 1.82  2004/03/04 02:38:38  potekhin
* Added modifications COMPLETE, to exclude SISD
* as per Kai request -- won't affect anybody else
*
* Revision 1.81  2004/02/10 00:27:57  potekhin
* The SVT group wanted the correction in the SVT geometry,
* which we discovered that we needed earlier this year, to
* be applied RETROACTIVELY to year2001 geometry tag.
* This breaks compatibility of the simulated SVT data
* between earlier simulation runs and the ones to
* follow, however this has been signed off by Helen
* and Jerome as an acceptable compromise. The CorrNum
* for year2001 is now set to 1.
*
* Revision 1.80  2004/01/29 20:46:45  potekhin
* Disabled the long since obsoleted version of TOF,
* because this piece of code would need to be rewritten
* to be compiled with a variety of compiler options.
*
* Revision 1.79  2004/01/22 00:21:32  potekhin
* Provide a facility to position the SVT with the MANY option,
* which we'll likely need due to overlap of the PIXL (embedded
* in SVT) and the mother volume of the beampipe
*
* Revision 1.78  2004/01/19 22:53:27  potekhin
* A small additional piece of logic to steer the
* construction of the barrel calorimeter, better
* code layout and comments
*
* Revision 1.77  2003/12/17 22:15:18  potekhin
* a) In accordance with recent modifications, we also
* introduce the configuration variable for the beam
* pipe, instead of keeping its actual parameters here
* b) corrected the COMPLETE geometry with the newest
* version of the svtt code (with ssd separated out)
* c) removed obsolete variables and logic, streamlined
* the code
* d) better comments and formatting in a few places
*
* Revision 1.76  2003/12/03 19:53:04  potekhin
* a) Corrected a small but annoying bug in
* propagating the geo tag to reco:
* one of the characters was copied twice
* because of the indexing error
*
* b) Added the "shift" angle for the second half
* barrel in Y2004
*
* Revision 1.75  2003/11/20 02:58:10  potekhin
* Changed the correction number scheme, such that it
* allows for a new layout of the SVT to be implemented --
* the one without the nested SSD. This is achieved by
* calling the new code, svttgeo3.
*
* Changed the SSD config number in y2004 to "2": 10 ladders,
* the "1" being one ladder installed previosuly and "3"
* being the complete 20 ladder config.
*
* Made smallchanges in the barrel config flag for y2004,
* improved comments and cleaned out unused variables.
*
* Revision 1.74  2003/11/14 22:56:19  potekhin
* We are about to redo a sim run with y2003x,
* and it seems that me might put in some of the
* prior corrections as well. Therefore, I'm changing
* the correctin level to 2.
*
* Revision 1.73  2003/11/13 00:54:50  potekhin
* Create a facility to modify the TPC
* gas density programmatically
*
* Revision 1.72  2003/11/13 00:21:42  potekhin
* The modification flag we introduced earlier
* to reflect variations in the dimensions of
* certain support structured of SVT takes a borader
* meaning than just the shield, hence we rename
* the variable to SupportVer
*
* Revision 1.71  2003/11/12 18:45:09  potekhin
* As per Fabrice, change the number of layer in the
* SVT to 7 to ensure that the ssd in its current
* version is also included
*
* Revision 1.70  2003/10/30 00:15:42  potekhin
* To perfect our already sophisticated taxonomy of
* the geometry tags, we rename Y2003C into Y2004,
* because the run for which it is meant will start
* in early 2004 anyway. Anyone to confuse 2003
* and 2004, from now on, will be jailed and
* possibly deported.
*
* Revision 1.69  2003/10/29 22:07:30  potekhin
* Two changes:
* 1) As agreed, I swap the tags y2003(b,c) to arrange
* them chronologically for better mneumonics
* 2) Introduced variable for steering of the Silicon Strip
* Detector code (which needs to be written)
*
* Revision 1.68  2003/10/28 00:01:59  potekhin
* As agreed with Jerome, we shall prevent
* proliferation of custom geometries to reduce
* the various dependencies between simu and reco
* and databases. Therefore, the experimental
* geometry "ASYMPT1" has been removed and has
* taken place of the "COMPLETE", which is our
* official sandbox. The Pixel Detector is
* defined in it right now.
*
* Revision 1.67  2003/10/15 23:19:35  potekhin
* Due to an apparent need to have the "most precise"
* geometry describing the STAR configuration in the
* spring 03 run, we introduce the tag Y2003C. It
* includes all the corrections first introduced in
* Y2003A, but also has the extra material in the
* SVT that we recently added in the GEANT model
* and which had been missing before.
*
* Revision 1.66  2003/10/10 23:59:18  potekhin
* Per Jerome's suggestion, which I agree with, we should
* semantically separate our current geometry tags from the
* future development ones. Also, it makes sense to have a more
* complete (as compared to 2003) geometry, for the pixel
* studies. Therefore, the pixel testbed tag has been
* renamed and the STAR detector more fully populated.
* It's now ASYMPT1, and the indexing may continue.
*
* Revision 1.65  2003/10/10 23:15:35  potekhin
* In previous check-n: forgot to mention the new CorrNum=3
* which actually programmatically modifies the inner radius of
* the shield in the SVT -- otherwise the pixel detector won't
* fit. Plus, put in extra flags PIXL_ON and PIPE_OFF to facilitate
* experimentation.
*
* Revision 1.64  2003/10/10 23:12:56  potekhin
* The fact is, we will need a suitable specialized geometry
* for the pixel detector development, as it will require
* a different beampipe and other modifications. I hereby
* create the tag y2003c which will serve this purpose.
* Right now it disables the old beampipe w/o offering anything
* in its place -- this is subject to change as we assimilate
* the new pipe design.
*
* Improved comments and structure in a few places
*
* Revision 1.63  2003/10/01 23:44:17  potekhin
* Code modifications related to persisting the vital
* geometry/version data, for now the magnetic field
* scale and the geometry tag
*
* 1) Change the semantics of the variable Geom, which was hacky
* anyway, and put the mwx=1 in the individual year_1 branches
* themselves (a lot cleaner than a prior obscure "if")
*
* 2) Store the complete geometry tag in the variable Geom,
* which is 8 characters long
*
* 3) Change the subroutine "geometry" into a "module",
* which process instruments it (via Mortran) to access
* and manipulate ZEBRA banks as structures
*
* 4) Introduce the bank GDAT, as a sub-bank of GEOM,
* which for now contains the field scale and the tag.
*
* Revision 1.62  2003/09/29 19:48:41  potekhin
* 1) Fixed typos in comments
*
* 2) Created a few options that allow the user to selectively include
* certain detectors such as svtt, ECAL, CALB into the minimal geometry,
* thus facilitating the creation of custom geometries on the fly --
* useful for debugging and detector exploration
*
* 3) Improved the PHMD logic
*
* 4) last but not least -- the shift variable for CALB was changed from
* {75,75} (incorrect) to {75,105} in geometry y2003x (full).
*
* Revision 1.61  2003/09/18 22:09:34  potekhin
* Corrected a small comment typo and added the full
* endcap wheel to the  new flagship geometry, y2003b.
* This is done with fill mode 3.
*
* Revision 1.60  2003/09/17 23:10:42  potekhin
* Small improvements to the Correction Level
* logic.
*
* Revision 1.59  2003/08/21 20:29:27  potekhin
* As per discussion with Jerome, I'm introducing
* a cleaner versioning of the 2003 geometries:
*
* y2003a is now really the corrected year2003,
* without any additions -- the SVT layer positions
* have been updated and the old supogeo bug fixed.
*
* y2003b has same corrections as y2003a but will also
* include extra material in the SVT, new ECAL configuration
* as well as a new FPD, plus the Photon Multiplicity Detector.
* Other changes will be done as needed. So for practical
* purposes, this will be the actual Fall'03 geometry.
*
* Revision 1.58  2003/08/05 23:37:09  potekhin
* Continued to use the CorrNum "correction level" to
* correct older geometry bugs in a controlled and versioned
* manner. Keep the newer version of the FTPC support pieces,
* and add a call the new SVTT module, which will
* include a number of corrections.
*
* Revision 1.57  2003/07/03 04:45:20  potekhin
* Added the "special" variation of the year 2003 geometry, which has
* a complete set of the barrel and endcap calorimeter elements. This
* is needed primarily for heavy flavor studies and other rare signals.
* The tag is y2003x.
*
* Also, fixed a subtle bug in the configuration of the endcap calorimeter.
* It did not affect previous simulations, but would manifest itself
* in a complete configurations such as this one.
*
* Revision 1.56  2003/05/01 23:00:16  potekhin
* Photon Multiplicity Detector is now part of the
* GEANT geometry version "y2003a", with proper
* versioning of its position
*
* Revision 1.55  2003/04/29 21:04:55  potekhin
* To keep the consistency of current simulation runs,
* the geometry "year2003" is frozen. All the corrections
* will go into "y2003a", and there will be a number of those.
* Such geometry has been added to this present source file.
* In the current cut of y2003a, to be tested, we corrected
* the supogeo and the offset of the ECAL phi position.
* We are also awaiting further corrections from the SVT group.
*
* Revision 1.55  2003/04/29 16:57:00  potekhin
* New geometry y2003a -- corrected
*
* Revision 1.54  2002/12/10 01:48:25  potekhin
* Important: the hadronic interactions are now indeed actuated in GCALOR
*
* Revision 1.53  2002/12/05 23:28:41  potekhin
* Streamlined the BTOF config logic
*
* Revision 1.52  2002/11/27 21:53:14  potekhin
* code improvement for readability etc -- moved bbcmgeo call
*
* Revision 1.51  2002/11/03 02:16:10  nevski
* geometry up to 2003 introduced
*
* Revision 1.50  2002/10/28 15:49:35  nevski
* fpd as a separate detector added
*
* Revision 1.49  2002/10/28 15:42:29  nevski
* introducing 2002 version
*
* Revision 1.48  2001/09/10 17:39:34  nevski
* do not set MFLD datacards without a DETP GEOM
*
* Revision 1.47  2001/05/22 17:40:47  nevski
* field find tuning
*
* Revision 1.46  2001/05/21 21:07:05  nevski
* Steves field map added
*
* Revision 1.45  2001/04/09 15:31:35  nevski
* second version of cerenkov light properties introduced
*
* Revision 1.44  2001/03/16 22:09:13  nevski
* some clean-up
*
* Revision 1.43  2001/03/16 00:32:06  nevski
* switch on/off cooling water
*
* Revision 1.42  2001/03/15 01:24:47  nevski
* default BTOF forced to no TOF tray
*
* Revision 1.41  2001/03/15 01:14:20  nevski
* first approach to forward pion detector
*
* Revision 1.40  2001/03/13 20:56:31  nevski
* variable RICH position taken from DB
*
* Revision 1.39  2001/03/12 01:01:30  nevski
* mwc x-hits corrected
*
* Revision 1.38  2001/02/13 02:28:52  nevski
* Y2B: extend CALB patch, add VPD
*
* Revision 1.37  2001/02/07 02:09:09  nevski
* 6 silicon layers in y_2b geometry
*
* Revision 1.36  2000/11/22 17:51:41  nevski
* tof geometry versions 1/2 preserved in btofgeo1, version 3 goes in btofgeo2
***************************************************************************
********* Detector definitions*********************************************

replace [exe BBCMon;] with [;BBCM=on;]

replace [exe CALBof;] with [;CALB=off;]
replace [exe CALB00;] with [;"Full barrel in 2007"; CALB=on;
                     CalbConfig = 0;
                     emsEdit=on ; nmod={12,0}; shift={87,0};]
replace [exe CALBa0;] with [;"Full barrel in 2007"; CALB=on;
                     CalbConfig = 0;
                     emsEdit=on ; nmod={24,0}; shift={21,0};]
replace [exe CALBb0;] with [;"Full barrel in 2007"; CALB=on;
                     CalbConfig = 0;
                     emsEdit=on ; nmod={60,0}; shift={0,0};]
replace [exe CALBc0;] with [; CALB=on;
                     CalbConfig = 0;
                     emsEdit=on ; nmod={60,60}; shift={75,105};]
replace [exe CALBd0;] with [" 60 sectors "; CALB=on;
                     CalbConfig = 0;
                     emsEdit=on ; nmod={60,0}; shift={75,0};]
replace [exe CALBe0;] with [" 60 sectors "; CALB=on;
                     CalbConfig = 0;
                     emsEdit=on ; nmod={60,60}; shift={75,105}; " 60 sectors on both sides";]

replace [exe CALB01;] with [;CALB=on;
                     CalbConfig = 1;
                     emsEdit=on ; nmod={60,60}; shift={75,105}; " 60 sectors on both sides";]
replace [exe CALB02;] with [;CALB=on;
                     CalbConfig = 2;
                     emsEdit=on ; nmod={60,60}; shift={75,105}; " 60 sectors on both sides";]

replace [exe CAVE04;] with [ "We need an even bigger Cave";   CaveConfig = 4;]

replace [exe ECALof;] with [;ECAL=off;]
replace [exe ECAL31;] with [;"ECAL"; ECAL=on;
                             ecalFill=3; "all sectors filled " EcalConfig=1; " one ECAL patch, west ";]

replace [exe ECAL11;] with [;"ECAL"; ECAL=on;
                             ecalFill=1; EcalConfig=1;  ;]
replace [exe ECAL31;] with [;"ECAL"; ECAL=on;
                             ecalFill=3; EcalConfig=1;   " one ECAL patch, west ";]
replace [exe ECAL33;] with [;"ECAL"; ECAL=on;
                             ecalFill=3 "all sectors filled "; EcalConfig=3; "both wheels"  ;]

replace [exe FPDM00;] with [; "forward pion detector "; FPDM=on; FpdmConfig  = 0;]
replace [exe FPDM01;] with [; "forward pion detector "; FPDM=on; FpdmConfig  = 1;]
replace [exe FPDM03;] with [; "forward pion detector "; FPDM=on; FpdmConfig  = 3;]

replace [exe FTPCof;] with ["ftpc configuration"; FTPC=off;]
replace [exe FTPC00;] with ["ftpc configuration"; FTPC=on;
                            ;FtpcConfig = 0;"FTPC Support";SupoConfig = 1;]
replace [exe FTPC01;] with ["ftpc configuration"; FTPC=on;
                            ;FtpcConfig = 1;"FTPC Support";SupoConfig = 1;]

replace [exe FTRO01;] with ["FTPC Readout barrel "; FTRO = on; FtroConfig = 1;]

replace [exe MFLDof;] with [ MFLD=off;]
replace [exe MFLD23;] with [ MFLD=on; magField = 2.5; MfldConfig=3;]
replace [exe MFLD53;] with [ MFLD=on; magField = 5.0; MfldConfig=3;]
replace [exe MFLD54;] with [ MFLD=on; magField = 5.0; MfldConfig=4;]

replace [exe MUTD03;] with [ "Muon Trigger System"; MUTD = on; MutdConfig = 3;]

replace [exe PHMD01;] with ["Photon Multiplicity Detector Version ";PHMD=on; PhmdConfig = 1;]
replace [exe PHMD02;] with ["Photon Multiplicity Detector Version ";PHMD=on; PhmdConfig = 2;]

replace [exe PIPE00;] with [ "Simplest.Gerrit"; PipeConfig = -1;PipeFlag   = -1;]
replace [exe PIPE04;] with [ "The new pipe according to Kai"; PipeConfig = 4;
                             "pipe wrap only" ;               PipeFlag   = 0;]
replace [exe PIPE14;] with [ "The new pipe according to Kai"; PipeConfig = 4;
                             "pipe wrap only" ;               PipeFlag   = 1;]

replace [exe PIXL00;] with [ "Simplest.Gerrit" PIXL=on; PixlConfig=-1;]
replace [exe PIXL01;] with [ "Put the pixel detector in" PIXL=on; PixlConfig=1;]
replace [exe RICHof;] with [;RICH=off;]
replace [exe RICH02;] with [;RICH=on; richPos=2; richConfig=2;]

replace [exe SCON02;] with [;SCON = off; ConeConfig=2 " new cable weight estimate ";]

replace [exe SCON12;] with [;SCON = on ; ConeConfig=2 " new cable weight estimate ";]
replace [exe SCON13;] with [;SCON = on ; ConeConfig=3 " new cable weight estimate ";]

replace [exe SISDof;] with ["Silicon Strip Detector off "; SISD=off;]
replace [exe SISD02;] with ["Silicon Strip Detector on  "; SISD=on ; SisdConfig= 2;]
replace [exe SISD12;] with ["Silicon Strip Detector on  "; SISD=on ; SisdConfig=12;]
replace [exe SISD22;] with ["Silicon Strip Detector on  "; SISD=on ; SisdConfig=22;]
replace [exe SISD23;] with ["Silicon Strip Detector on  "; SISD=on ; SisdConfig=23;]
replace [exe SISD24;] with ["Silicon Strip Detector on  "; SISD=on ; SisdConfig=24;]
replace [exe SISD65;] with ["Silicon Strip Detector on  "; SISD=on ; SisdConfig=65;]

replace [exe SVTTof;] with ["SVTT version"; SVTT=off; SvttConfig = -1;]
replace [exe SVTT00;] with ["SVTT version"; SVTT=on;
                              SvttConfig = 0; svtWater=off; nSvtVafer=7; nSvtLayer=6;]
replace [exe SVT100;] with ["SVTT version"; SVTT=on;
                              SvttConfig = 0; svtWater=on; nSvtVafer=0; nSvtLayer=-3; svtWaferDim=0;]
replace [exe SVT101;] with ["SVTT version"; SVTT=on;
                              SvttConfig = 1; svtWater=on ; nSvtLayer=6;]
replace [exe SVT102;] with ["SVTT version"; SVTT=on;
                              SvttConfig = 2; svtWater=on; nSvtVafer=0; nSvtLayer=6; svtWaferDim=0;
                              SvshConfig = 0; "No SVT shield";]
replace [exe SVT103;] with ["SVTT version"; SVTT=on;
                              SvttConfig = 3; svtWater=on; nSvtVafer=0; nSvtLayer=6; svtWaferDim=0;
                              SvshConfig = 0; "No SVT shield";]
replace [exe SVT106;] with ["SVTT version"; SVTT=on;
                              SvttConfig = 6; svtWater=on; nSvtVafer=0; nSvtLayer=6; svtWaferDim=0;
                              SvshConfig = 0; "No SVT shield";]
replace [exe SVT203;] with ["SVTT version"; SVTT=on;
                              SvttConfig = 3; svtWater=on; nSvtVafer=0; nSvtLayer=6; svtWaferDim=0;
                              SvshConfig = 2; "SVT shield";]
replace [exe SVT204;] with ["SVTT version"; SVTT=on;
                              SvttConfig = 4; svtWater=on; nSvtVafer=0; nSvtLayer=6; svtWaferDim=0;
                              SvshConfig = 2; "SVT shield";]
replace [exe SVT304;] with ["SVTT version"; SVTT=on;
                              SvttConfig = 4; svtWater=on; nSvtVafer=0; nSvtLayer=6; svtWaferDim=0;
                              SvshConfig = 3; "SVT shield";]
replace [exe SVT206;] with ["SVTT version"; SVTT=on;
                              SvttConfig = 6; svtWater=on; nSvtVafer=0; nSvtLayer=6; svtWaferDim=0;
                              SvshConfig = 2; "SVT shield";]

replace [exe BTOF42;] with [;BTOF=on; BtofConfig=4;Itof=2 " call btofgeo2 ";]
replace [exe BTOF52;] with [;BTOF=on; BtofConfig=5;Itof=2 " call btofgeo2 ";]
replace [exe BTOF72;] with [;BTOF=on; BtofConfig=7;Itof=2 " call btofgeo2 ";]
replace [exe BTOF84;] with [;BTOF=on; BtofConfig=8;Itof=4 " call btofgeo4 ";]

replace [exe BTOF16;] with [;" X.Dong";BTOF=on;
                            BtofConfig=1; Itof=6 " call btofgeo6 ";
                            tofX0= 0.00; tofZ0=-0.50;]
replace [exe BTOF66;] with [;" X.Dong";BTOF=on;
                            BtofConfig=6; Itof=6 " call btofgeo6 ";
                            tofX0= 0.00; tofZ0=0;]
replace [exe BTOFb6;] with [;" X.Dong";BTOF=on;
                            BtofConfig=11; Itof=6 " call btofgeo6 ";
                            tofX0= 0.00; tofZ0=-0.50;]

replace [exe TPCE00;] with [;"New version of the TPC backplane "; TpceConfig = 1;]
replace [exe TPCE01;] with [;"New version of the TPC backplane "; TpceConfig = 1;
                             "gas density correction";            DensConfig = 1;]
replace [exe TPCE02;] with [;"New version of the TPC backplane "; TpceConfig = 2;
                             "gas density correction";            DensConfig = 1;]
replace [exe TPCE03;] with [;"New version of the TPC backplane "; TpceConfig = 3;
                             "gas density correction";            DensConfig = 1;]
replace [exe TPCE04;] with [;"New version of the TPC backplane "; TpceConfig = 4;
                             "gas density correction";            DensConfig = 1;]

replace [exe ISTB00;] with [;ISTB=on;IstbConfig=-1;]

replace [exe VPDDof;] with [;VPDD=off;]
replace [exe VPDD02;] with  [;"pseudo Vertex Position Detector";VPDD=on;VpddConfig=2;]
replace [exe VPDD03;] with  [;"pseudo Vertex Position Detector";VPDD=on;VpddConfig=3;]
replace [exe VPDD04;] with  [;"pseudo Vertex Position Detector";VPDD=on;VpddConfig=4;]
replace [exe VPDD07;] with  [;"pseudo Vertex Position Detector";VPDD=on;VpddConfig=7;]


replace [exe FGTD02;] with  [;FGTD=on;FgtdConfig=2;  "GEM forward tracker"]

replace [exe FSTDof;] with  [;FSTD=off;]
replace [exe ITSPof;] with  [;ITSP=off;] "prototype of the Inner Tracker SuPport structure"
********* Geometry definitions *******************************************************
*********   y2000   ***
replace [exe y2000;] with [;"corrected: MWC readout, RICH reconstructed position, no TOF ";
                            "actual 2000:  TPC+CTB+RICH+caloPatch+svtLadder"
        exe TPCE00; exe VPDDof;exe ECALof;exe FTPCof; exe SVTT00;exe CALB00;exe MFLD23;]

*********   y2001   ***
replace [exe y2001;] with ["2001 geometry - TPC+CTB+FTPC+RICH+CaloPatch+SVT+FPD";
" 02/09/2004  Jerome signed off on changing, retroactively, the"
" position of the wafers in year2001, which was incorrectly offset"
" by 250 um insterad of 150 um."
" -- Obsoleted CorrNum = 1;"
        exe TPCE00; exe SVT101;exe FTPC00;exe BTOF42; exe RICH02;exe ECAL31; exe CALBa0; exe MFLD53;]

*********   y2002   ***
replace [exe y2002;] with ["january 2002 geometry - TPC+CTB+FTPC+CaloPatch2+Rich+SVT3+BBC+FPD";
                           exe TPCE00; exe SVT100; exe RICH02; exe BTOF42; exe CALBa0; exe ECALof;
                           exe BBCMon; exe FPDM00; exe VPDD02; exe MFLD54;]
*========   y2003   y2003   y2003   y2003   =y2003  y2003    y2003   ============================
*********   y2003   ***
replace [exe y2003;] with ["draft 2003 geometry - TPC+CTB+FTPC+CaloPatch2+SVT3+BBC+FPD+ECAL";
                           exe TPCE00; exe SVT100; exe RICHof; exe BTOF52; exe CALBb0; exe ECAL11;
                           exe BBCMon; exe FPDM00; exe VPDD03; exe MFLD54;]
*********   y2003a   ***
replace [exe y2003a;] with [
***********************************************************************
* In y2003a:
*    removed serious bugs from SUPOGEO (incorrect positioning inside the SVT,
*    where is doesn't belong)
*    corrected CALB -- the shift variable (was 0,0 so the barrel wasn't tilted right)
*    corrected SVT  -- the layer radii (from 250 to 150 microns, see the svt code)
****************************************************************************************
        exe y2003; exe CALBd0; exe FTPC00; exe SVT101
]

*********   y2003b   ***
replace [exe y2003b;] with [
***********************************************************************
* y2003b is y2003a, but with the extra material in the SVT
* This is actually an important case (i.e. the "most precise" geometry
* approximation for the early 2003 run) which we were lacking so far.
* This is achieved by setting CorrNum to 2.
* The endcap EMC has one third of one wheel, as before
* For more info on the extra material in SVT -- see web page
***********************************************************************
        exe y2003a; exe SVT102;
]
*********   y2003c   ***
replace [exe y2003c;] with [
"Better SVT model on top of 2003B: TPC+CTB+FTPC+CaloPatch2+SVT3+BBC+FPD+ECAL";
        exe y2003b; exe SVT106;
]

*********   y2003x   ***
replace [exe y2003x;] with ["same as y2003b but with full calorimeters and PHMD";
        exe y2003b; exe CALBc0; exe ECAL33; exe PHMD01;
]

*********   complete   ***
replace [exe COMPLETE;] with ["New Complete+correction 3 in 2003 geometry:";
                              "TPC+CTB+FTPC+CaloPatch2+SVT3+BBC+FPD+ECAL+PHMD";
                       exe SVT204;exe BTOF52;exe CALBc0;exe ECAL33;exe BBCMon;exe FPDM00;
                       exe TPCE01;exe FTPC00;exe PHMD01;exe SISDof;exe PIPE04; exe PIXL01;]



*========   y2004   y2004  y2004  y2004  y2004  y2004  y2004  y2004  =================================
*********   y2004   ***
* baseline 2004 geometry: TPC+CTB+FTPC+CaloPatch2+SVT3+BBC+FPD+ECAL+PHMD with low cuts GSTPAR in PHMD;
replace [exe y2004;] with [
        exe TPCE01;exe SVT103; exe BTOF72; exe CALB01; exe ECAL31; exe BBCMon; exe FPDM01;
        exe VPDD04; exe MFLD54; exe FTPC00; exe PHMD01; exe SISD02;
]
*********   y2004a   ***
replace [exe y2004a;] with [ exe y2004; exe PHMD02;]

*********   y2004b   ***
replace [exe y2004b;] with [ exe y2004a; exe SISD12;]

*********   y2004c   ***
replace [exe y2004c;] with [ exe y2004b; exe TPCE02;exe SVT204; exe SCON02;]

*********   y2004d   ***
replace [exe y2004d;] with [
                exe SVT206; exe BTOF72; exe CALB01; exe ECAL31; exe BBCMon; exe FPDM01; exe SCON02;
                exe VPDD04; exe MFLD54; exe FTPC01; exe PHMD02; exe SISD22; exe FTRO01; exe TPCE02;
]
*********   y2004x   ***
replace [exe y2004x;] with [
                exe SVT203; exe BTOF72; exe CALBe0; exe ECAL31; exe BBCMon; exe FPDM01;
                exe VPDD04; exe MFLD54; exe FTPC01; exe PHMD02; exe SISD02; exe TPCE01;
]
*********   y2004y   ***
replace [exe y2004y;] with [
                exe SVT204; exe SCON02; exe BTOF72; exe CALBe0; exe ECAL31; exe BBCMon; exe FPDM01;
                exe VPDD04; exe MFLD54; exe FTPC01; exe FTRO01; exe PHMD02; exe SISD22; exe TPCE02;
]
*========   y2005   y2005  y2005  y2005  y2005  y2005  y2005  y2005  =================================
*********   y2005   ***
replace [exe y2005;] with [
                exe SVT203; exe SCON02; exe BTOF72; exe CALB01; exe ECAL31; exe BBCMon; exe FPDM01;
                exe VPDD04; exe MFLD54; exe FTPC01; exe PHMD02; exe FTRO01; exe SISD23; exe TPCE01;
]
*********   y2005b   ***
replace [exe y2005b;] with [
                exe SVT204; exe SCON02; exe BTOF72; exe CALB01; exe ECAL31; exe BBCMon; exe FPDM01;
                exe VPDD04; exe MFLD54; exe FTPC01; exe PHMD02; exe FTRO01; exe SISD24; exe TPCE02;
]

*********   y2005c   ***
replace [exe y2005c;] with [ exe y2005b;  exe BTOF84;]

*********   y2005d   ***
replace [exe y2005d;] with [ exe y2005c;  exe SVT206;]


*********   y2008   ***
replace [exe y2008;] with [;
{ "Year 2008 baseline: no SVT,  cones,beam support,FTPC in CAVE now"
    exe SCON02;
    exe TPCE03;
    exe BTOFb6;
    exe CALB02;
    exe ECAL31;
    exe BBCMon;
    exe FPDM03;
    exe VPDD07;
    exe FTPC01;
    exe SVTTof;
    exe PHMD02;
    exe SISDof;
    exe FTRO01;
    exe MUTD03;
    exe CAVE04;
    exe PIPE14;
};]

*********   y2008a   ***
replace [exe y2008a;] with [;exe y2008; exe SCON13;]

*********   y2009   ***
replace [exe y2009;] with [;
{ "Year 2009 baseline: now(12.29.08) only place holder. No difference with y2008a"
    exe SCON13;
    exe TPCE04;
    exe BTOF66;
    exe CALB02;
    exe ECAL31;
    exe BBCMon;
    exe FPDM03;
    exe VPDD07;
    exe FTPC01;
    exe SVTTof;
    exe PHMD02;
    exe SISDof;
    exe FTRO01;
    exe MUTD03;
    exe CAVE04;
    exe PIPE14;
};]
replace [exe UPGR15;] with ["New Tracking: HFT+IST+TPC+SSD-SVT"
                            exe SVTTof; exe FTPCof; exe BTOF66; exe CALBc0; exe ECAL31;
                            exe BBCMon; exe FPDM00; exe MFLD54;
                            DensConfig = 1; "gas density correction"
                            SupoConfig = 1; "FTPC Support"
                            exe PHMD01; exe SISD65; exe PIPE00; exe PIXL00; exe ISTB00;
                            exe FSTDof; exe FGTD02; 
"* On Gerrit request, we disable the cone:"
                            exe ITSPof; "prototype of the Inner Tracker SuPport structure"]


*********HELP***HELP***HELP***HELP***HELP***HELP***HELP***HELP***HELP***
* ORT = XYZ ! (90,  0, 90, 90,  0,  0)
* ORT = YZX ! (90, 90,  0,  0, 90,  0)
* ORT = ZXY ! ( 0,  0, 90,  0, 90, 90)
*********HELP***HELP***HELP***HELP***HELP***HELP***HELP***HELP***HELP***
***************************************************************************
   module geometry is the main steering module for the STAR geometry
   author  Pavel Nevski
   Created August 1998
*                                                                         *
*  Update history:                                                        *
*  08/19/98, PN: tof is not part of year_2a                               *
*  12/04/98, PN: RICH  + upstream part + zero degree calo                 *
*  09/26/99, E.Cains: 1H geometry added - one svt ladder at layer 3       *
*  01/27/99, PN: RICH in 1H geometry is simulated with hits is quartz & fr*
*  05/22/01, PN: starting with tag y2000 field is version 3 (direct map)  *
*  09/30/03, MP: see the many new CVS comments about recent check-ins     *
*  09/30/03, MP: converted the sub into a MODULE to allow for ZEBRA access*
***************************************************************************

   Structure  GDAT {real mfscale, char gtag(2)}

* list of system on/off switches:
   Logical    CAVE,PIPE,SVTT,SISD,TPCE,FTPC,
              BTOF,VPDD,MAGP,CALB,ECAL,UPST,
              RICH,ZCAL,MFLD,BBCM,FPDM,PHMD,
              PIXL,ISTB,GEMB,FSTD,FTRO,FGTD,
              SHLD,QUAD,MUTD,IGTD,HPDT,ITSP,
              DUMM,SCON

* Qualifiers:  TPC        TOF         etc
   Logical    emsEdit,svtWater,
              on/.true./,off/.false./




*  Codes:
*  1 - full ctb,         2 - full TOFp based tof,   3 - partial TOFp based tof,
*  4 - single TOFp tray, 5 - one TOFp and one TOFr, 6 - full TOFr based tof.
* X.Dong - global parameters for TOF trays
   real       tofX0, tofZ0

   real       Par(1000),myArg,magField,dcay(5),shift(2),svtWaferDim

   Integer    LENOCC,ICFNBL,ICFMUL,JL,JR,LL,
              IPRIN,nSvtLayer,nSvt1stLayer,i,jGotCom,l,
              kgeom,nmod(2),nonf(3),ecalFill,
              sisd_level,
              Nleft,Mleft,richConfig,richPos,nSvtVafer,Itof,mwx

   Real       magX(3) /0,0,0/,magB(3)
***************** historical note: *********************8
* CorrNum allows us to control incremental bug fixes in a more
* organized manner -- ! Obsoleted 20050324 maxim! --


* The following are the versioning flags:

   Integer    DensConfig, SvttConfig, BtofConfig, VpddConfig, FpdmConfig, SisdConfig, PipeConfig,
              CalbConfig, PixlConfig, IstbConfig, GembConfig, FstdConfig, FtroConfig, ConeConfig,
              FgtdConfig, TpceConfig, PhmdConfig, SvshConfig, SupoConfig, FtpcConfig, CaveConfig,
              ShldConfig, QuadConfig, MutdConfig, HpdtConfig, IgtdConfig, MfldConfig, EcalConfig
   Integer    pipeFlag

*             DensConfig, ! TPC gas density correction
*             SvttConfig, ! SVTT version
*             BtofConfig, ! BTOF trays
*             VpddConfig, ! VPDD
*             FpdmConfig, ! Forfward Pion Mult detectoe
*             SisdConfig, ! SSD
*             PipeConfig, ! Beam Pipe
*             CalbConfig, ! Barrel EMC
*             PixlConfig, ! Inner Pixel detector
*             HpdtConfig, ! Heavy Flavor Tracker
*             IstbConfig, ! Integrated Silicon Tracker
*             GembConfig, ! Inner GEM barrel tracker
*             IgtdConfig, ! GEM disks
*             FstdConfig, ! Forward Silicon tracker Disks
*             FtroConfig, ! FTPC Readout Electronics
*             ConeConfig, ! SVTT support cones and cables
*             FgtdConfig, ! Forward GEM tracker
*             TpceConfig, ! TPC
*             PhmdConfig  ! Photon Multiplicity Detector
*             SvshConfig  ! SVT Shield
*             SupoConfig  ! FTPC support
*             FtpcConfig  ! FTPC
*             ShldConfig  ! Beam shield
*             QuadConfig  ! All magnets from D0 and up
*             MutdConfig  ! Muon Trigger System

* Note that SisdConfig can take values in the tens, for example 20
* We do this to not proliferate additional version flags -- there has
* been a correction which resulted in new code.. We check the value
* and divide by 10 if necessary.

   character  Commands*4000

* - - - - - - - - - - - - - - - - -
+CDE,GCBANK,GCUNIT,GCPHYS,GCCUTS,GCFLAG,AGCKINE,QUEST.
*  temporarely until GCTLIT is not part of GCTMED:
   Integer        Thrind   ,Jmin,ItCkov,ImCkov,NpCkov
   common/GCTLIT/ Thrind(4),Jmin,ItCkov,ImCkov,NpCkov
* - - - - - - - - - - - - - - - - -

replace[;ON#{#;] with [
  IF ((JL.LT.JR) .and. (Commands(JL:JR) .eq. '#1')) {;
    myArg=0; jGotCom=2009;
    if (Commands(JR+1:JR+1).eq.'=') then;
      write(*,*) 'UUUUUUUUUUU ',JL,JR,Commands(1:LL);
      write(*,*) 'UUUUUUUUUUU ',par(6);
      i = ((JR+1+3+4)/4); myArg = par(i+1);
      Commands(JR+1:i*4)=' ';
    endif
    JL = ICFNBL(Commands,JR+1,LL);
    JR = ICFMUL(' =',Commands,JL,LL)-1;
    <W>; (' #1: #2');
;]

* If geometry was already built, the local DB will be dropped completely now
* but the request for the next geometry should be saved in a temp. par arrray
   call ASLGETBA ('GEOM','DETP',1000,LL,Par); LL = (LL-1)*4;
   If (JVOLUM>0) call AGDROP ('*')

* -------------------- set GSTAR absolute default ------------------------
* before parsing the request, set some default values:
   IPRIN    = IDEBUG
   NtrSubEv = 1000     " automatic !"


   BtofConfig  = 1 ! ctb only
   CalbConfig  = 0 ! really make use of it starting in y2004
   CaveConfig  = 1 ! custom for shielding studies=2, wider for muon detector=3, and longer=4
   ConeConfig  = 1 ! 1 (def) old version, 2=more copper
   DensConfig  = 0 ! gas density correction
   FgtdConfig  = 1 ! version
   FpdmConfig  = 0 ! 0 means the original source code
   FstdConfig  = 0 ! 0=no, >1=version
   FtroConfig  = 0 ! 0=no, >1=version
   FtpcConfig  = 0 ! 0  version, 1=gas correction
   HpdtConfig  = 0 ! 0=no, >1=version
   IstbConfig  = 0 ! 0=no, >1=version
   IgtdConfig  = 1 ! 1=old radii etc, 2=new ones
   GembConfig  = 0 ! 0=no, >1=version
   MutdConfig  = 0 ! same
   PhmdConfig  = 0 ! No Photon multiplicity detectorby default
   PipeConfig  = 2 ! Default, Be pipe used in most of the runs =<2003
   PixlConfig  = 0 ! 0=no, 1=inside the SVT, 2=inside CAVE, 3=with pipe support
   QuadConfig  = 0 ! No D0 and quads by default
   ShldConfig  = 0 ! No Beam Shield by default
   SisdConfig  = 0 ! No Silicon strip by default
   SupoConfig  = 0 ! 0 (def) old buggy version, 1=correction
   SvshConfig  = 0 ! SVTT shield version
   SvttConfig  = 0 ! SVTT version
   TpceConfig  = 1 ! 1 (def) old version, 2=more structures in the backplane
   VpddConfig  = 1 ! vpd...

   pipeFlag = 3 ! pipe wrap + svt shield

* Set only flags for the main configuration (everthing on, except for tof),
* but no actual parameters (CUTS,Processes,MODES) are set or modified here.
* If an empty or no DETP GEOM was issued, geometry is defined externally.

   magField=5               " default"

* "Canonical" detectors are all ON by default,

   {CAVE,PIPE,SVTT,TPCE,FTPC,BTOF,VPDD,CALB,ECAL,MAGP,MFLD,UPST,ZCAL} = on;
* whereas some newer stuff is considered optional:
   {BBCM,FPDM,PHMD,PIXL,ISTB,GEMB,FSTD,SISD,FTRO,FGTD,SHLD,QUAD,MUTD,IGTD,HPDT,ITSP,DUMM,SCON} = off;

   {emsEdit,RICH}=off        " TimeOfFlight, EM calorimeter Sector            "
   nSvtLayer=7; nSvtVafer=0;  svtWaferDim=0; " SVT+SSD, wafer number and width as in code     "
   nSvt1stLayer=1;           " the innermost layer of SVT                     "
   svtWater=on               " water+water manifold in svt, off for Y2000 only"
   mwx=2                 " for Year_1? mwx=1 limites x in mwc hits (<Y2K) "
   Itof=2                " use btofgeo2 - default starting from Y2000     "
   richConfig=2          " add non-sensitive hits to RICH system          "
   richPos=2             " real RICH position and spectra inst.of nominal "
   nonf={1,2,2}          " ECAL on right side, FPD parts on left side     "
   EcalConfig=0         " Ecal: east, west or both                       "
   ecalFill=3           " full wheel by default                          "
   MfldConfig=2                  " default field - symmetrical, as fitted by Bill "
   Commands=' ';


* -------------------- select USERS configuration ------------------------
* On a non-empty DETP GEOM every keyword results in an action and is erased.
*
* Actions consist here of selecting the appropriate parameteres and flags.
* This flags are used in the next section to create subsystems and
* to communicate DETP commands with parameters to them.
*

If LL>0
{ Call AGSFLAG  ('GEOM',1)
* convert input line into a string of upprecase characters
  CALL UHTOC(PAR(2),4,Commands,LL);  Call CLTOU(Commands);
  do JL=1,LL {if (ichar(Commands(JL:JL)).lt.ichar(' ')) Commands(JL:JL)=' ';}
  JL = ICFNBL(Commands, 1,LL);
  JR = ICFMUL(' =',Commands,JL,LL)-1;



* set geant processes and cuts only if any detp geometry was issued:
  {CUTGAM,CUTELE,CUTNEU,CUTHAD,CUTMUO,BCUTE,BCUTM,DCUTE,DCUTM,PPCUTM} =.001;
  {IDCAY,IANNI,IBREM,ICOMP,IHADR,IMUNU,IPAIR,IPHOT,ILOSS,IDRAY,IMULS} = 1;
  {IRAYL,ISTRA} = 0;
  TOFMAX        = 1.e-4
*
  for(jGotCom=1;jGotCom>0;) { jGotCom=0;
  on HELP       { you may select the following keywords: ;
                  <W>;('---------------:----------------------------- ');
                  <W>;('Configurations : complete,tpc_only,field_only ');
                  <W>;('               : year_2a                      ');
                  <W>;('               : year2000, year2001,year2002  ');
                  <W>;('               : year2003, y2003a             ');
                  <W>;('Gcalor         : Gcalor_on, Gcalor_off        ');
                  <W>;('Geant Physics  : Hadr_on, Hadr_off            ');
                  <W>;('Geant Physics  : Phys_off, Decay_Only         ');
                  <W>;('Geometry Detail: mwc_off, pse_off, 4th_off    ');
                  <W>;('Magnetic Field : Field_on/off, field=value    ');
                  <W>;('Auxillary keys : Debug_on/off, Split_on/off   ');
                  <W>;('--------------------------------------------- ');
                  <W>;('Default: complete STAR with hadr_on,auto-split');
                  <W>;('--------------------------------------------- ');
                }

  on YEAR_2B    { old 2001 geometry first guess - TPC+CTB+FTPC+RICH+CaloPatch+SVT;
                  BtofConfig=4;
                  {RICH,emsEdit}=on;  nmod={24,0}; shift={21,0};
                  nonf={0,2,2};  Itof=2;  richConfig=2;                        nSvtLayer=6; }

  on YEAR_2A    { old asymptotic STAR;    Itof=1; mwx=1;  BBCM=on;            }

*************************************************************************************************************
* as complete as Y2003X below but with all corrections AND pixel detector
*************************************************************************************************************
  on COMPLETE  { New Complete + correction 3 in 2003 geometry: TPC+CTB+FTPC+CaloPatch2+SVT3+BBC+FPD+ECAL+PHMD;
                 exe complete;
               }
****************************************************************************************
* corrected: MWC readout, RICH reconstructed position, no TOF
  on YEAR2000   { actual 2000:  TPC+CTB+RICH+caloPatch+svtLadder;
                  exe y2000; }
  on Y2000      { actual 2000:  TPC+CTB+RICH+caloPatch+svtLadder;
                  exe y2000; }

  on YEAR2001   { 2001 geometry - TPC+CTB+FTPC+RICH+CaloPatch+SVT+FPD;
                  exe y2001; }
  on Y2001      { 2001 geometry - TPC+CTB+FTPC+RICH+CaloPatch+SVT+FPD;
                  exe y2001; }

****************************************************************************************
  on YEAR2002   { january 2002 geometry - TPC+CTB+FTPC+CaloPatch2+Rich+SVT3+BBC+FPD;
                  exe y2002;
                }
  on Y2002      { january 2002 geometry - TPC+CTB+FTPC+CaloPatch2+Rich+SVT3+BBC+FPD;
                  exe y2002;
                }
****************************************************************************************
  on YEAR2003   { draft 2003 geometry - TPC+CTB+FTPC+CaloPatch2+SVT3+BBC+FPD+ECAL;
                  exe y2003;
                }

****************************************************************************************
  on Y2003      { draft 2003 geometry - TPC+CTB+FTPC+CaloPatch2+SVT3+BBC+FPD+ECAL;
                  exe y2003;
                }

***********************************************************************
* In y2003a:
*    removed serious bugs from SUPOGEO (incorrect positioning inside the SVT,
*    where is doesn't belong)
*    corrected CALB -- the shift variable (was 0,0 so the barrel wasn't tilted right)
*    corrected SVT  -- the layer radii (from 250 to 150 microns, see the svt code)
****************************************************************************************
  on Y2003A    { correction 1 in 2003 geometry - TPC+CTB+FTPC+CaloPatch2+SVT3+BBC+FPD+ECAL;
                 exe y2003a; }
***********************************************************************
* y2003b is y2003a, but with the extra material in the SVT
* This is actually an important case (i.e. the "most precise" geometry
* approximation for the early 2003 run) which we were lacking so far.
* This is achieved by setting CorrNum to 2.
* The endcap EMC has one third of one wheel, as before
* For more info on the extra material in SVT -- see web page
****************************************************************************************
  on Y2003B    { correction 2 in 2003 geometry: TPC+CTB+FTPC+CaloPatch2+SVT3+BBC+FPD+ECAL;
                 exe y2003b;
               }
****************************************************************************************
  on Y2003C    { Better SVT model on top of 2003B: TPC+CTB+FTPC+CaloPatch2+SVT3+BBC+FPD+ECAL;
                 exe y2003c;
               }

****************************************************************************************
  on Y2003X    { same as y2003b but with full calorimeters and PHMD
                 exe y2003x;
               }

*
*
**********************************************************************
* Corrections and enhancements in y2004:
*    added the Photon Multiplicity Detector (PHMD)
*    The endcap EMC has one complete wheel in the west
*    To be done: 3/4 of the second half of the barrel!
*
*                >>>THIS IS THE MASTER GEOMETRY FOR THE SPRING 04<<<
*
****************************************************************************************
  on Y2004     { baseline 2004 geometry: TPC+CTB+FTPC+CaloPatch2+SVT3+BBC+FPD+ECAL+PHMD with low cuts GSTPAR in PHMD;
                 exe y2004;
               }

****************************************************************************************
  on Y2004A    { baseline 2004 geometry: TPC+CTB+FTPC+CaloPatch2+SVT3+BBC+FPD+ECAL+PHMD with standard GSTPAR in PHMD;
                 exe y2004a; }

*
****************************************************************************************
  on Y2004B    { corrected 2004 geometry: TPC+CTB+FTPC+CaloPatch2+SVT3+BBC+FPD+ECAL+PHMD with standard GSTPAR in PHMD;
                 exe y2004b; }

****************************************************************************************
  on Y2004C    { same as Y2004B but with the SVT chip correction+cone+better SSD+TPC backplane+FTRO
                 exe y2004c; }
****************************************************************************************
  on Y2004D    { Better SVT on top of Y2004B
                 exe y2004d; }
****************************************************************************************

  on Y2004X    { hypothetical 2004 geometry: full barrel. Standard cuts in PHMD.;
                 exe y2004x;
               }

****************************************************************************************
  on Y2004Y    { same as Y2004X but with the SVT chip correction+cone+better SSD+TPC backplane+FTRO
                 exe y2004y;
                }

****************************************************************************************
  on Y2005X    { first cut of full CALB 2005 geometry: TPC+CTB+FTPC+CaloPatch2+SVT3+BBC+FPD+ECAL+PHMD_FTRO;
                  "svt: 3 layers ";
                     nSvtLayer=6  " 3 bi-plane layers, nSvtLayer<=7 ";
                     nSvtVafer=0  " numbering is in the code   ";
                     svtWaferDim=0  " width is in the code      ";

                     ConeConfig=2 " new cable weight estimate ";

                  "ctb: central trigger barrer             ";
                     Itof=2 " call btofgeo2 ";
* note the upgrade with respect to previous years:
                     BtofConfig=7;

* note the full barrel same as in y2003x:
                  "CALB"
                     emsEdit=on ;
                     nmod={60,60}; shift={75,105}; " 60 sectors on both sides"

                  "ECAL"
                     EcalConfig=1   " one ECAL patch, west "
                     ecalFill=3     " all sectors filled "

                  "beam-beam counter "
                     BBCM=on

                  "forward pion detector "
                     FPDM=on
                     FpdmConfig  = 1 "switch to a different lead glass source code"

                  "pseudo Vertex Position Detector"
                     VPDD=on;
                     VpddConfig=4;

                  "field version "
                     MfldConfig=4;      "tabulated field, with correction "

*                    -- Obsoleted CorrNum = 3;
                     SupoConfig = 1; "FTPC Support"
                     SvttConfig = 3; "SVTT version"
                     DensConfig = 1; "gas density correction"

                  "Photon Multiplicity Detector Version "
                     PHMD=on;
                     PhmdConfig = 2;

                  "Silicon Strip Detector Version "
                     SISD=on;
                     SisdConfig = 23; "second version, full barrel"


                  "FTPC Readout barrel "
                     FTRO=on;
                     FtroConfig = 1;

                }

****************************************************************************************
  on Y2005B    { TPC,FTPC,SVT and SSD correction of 2005 geometry
                exe y2005b;
               }

****************************************************************************************
  on Y2005C    { TPC,FTPC,SVT and SSD correction of 2005 geometry
                exe y2005c;
               }

****************************************************************************************
  on Y2005D    { Better SVT on top of Y2005C
                exe y2005d;
               }

****************************************************************************************
  on Y2005E    { Better SVT, bigger shield and SSD on top of Y2005C, and full barrel calorimeter
                  "svt: 3 layers ";
                     nSvtLayer=6  " 3 bi-plane layers, nSvtLayer<=7 ";
                     nSvtVafer=0  " numbering is in the code   ";
                     svtWaferDim=0  " width is in the code      ";

                     ConeConfig=2 " new cable weight estimate ";

                  "ctb: central trigger barrer             ";
                     Itof=4 " call btofgeo4 ";
* NEW CONFIG!
                     BtofConfig=8;

* note the full barrel same as in y2003x:
                  "CALB"
                     emsEdit=on ;
                     nmod={60,60}; shift={75,105}; " 60 sectors on both sides"

                  "ECAL"
                     EcalConfig=1   " one ECAL patch, west "
                     ecalFill=3     " all sectors filled "

                  "beam-beam counter "
                     BBCM=on

                  "forward pion detector "
                     FPDM=on
                     FpdmConfig  = 1 "switch to a different lead glass source code"

                  "pseudo Vertex Position Detector"
                     VPDD=on;
                     VpddConfig=4;

                  "field version "
                     MfldConfig=4;      "tabulated field, with correction "

* important: (1) new SVT version (2) FTPC gas correction tp Ar+C02 mix (3) SSD ladders raddi correction

                     SupoConfig = 1; "FTPC Support"
                     SvttConfig = 6; "SVTT version"
                     SvshConfig = 3; "SVT shield"
                     DensConfig = 1; "gas density correction"
                     FtpcConfig = 1; "ftpc configuration"

                  "Photon Multiplicity Detector Version "
                     PHMD=on;
                     PhmdConfig = 2;

                  "Silicon Strip Detector Version "
                     SISD=on;
                     SisdConfig = 35; "second version, full barrel with corrected radii"


                  "FTPC Readout barrel "
                     FTRO=on;
                     FtroConfig = 1;

                  "New version of the TPC backplane "
                     TpceConfig = 2;

                }

****************************************************************************************
  on Y2005F    { Y2005E + corrected SSD with gaps and dead area
                  "svt: 3 layers ";
                     nSvtLayer=6  " 3 bi-plane layers, nSvtLayer<=7 ";
                     nSvtVafer=0  " numbering is in the code   ";
                     svtWaferDim=0  " width is in the code      ";

                     ConeConfig=2 " new cable weight estimate ";

                  "ctb: central trigger barrer             ";
                     Itof=4 " call btofgeo4 ";
* NEW CONFIG!
                     BtofConfig=8;

* note the full barrel same as in y2003x:
                  "CALB"
                     emsEdit=on ;
                     CalbConfig = 2
                     nmod={60,60}; shift={75,105}; " 60 sectors on both sides"

                  "ECAL"
                     EcalConfig=1   " one ECAL patch, west "
                     ecalFill=3     " all sectors filled "

                  "beam-beam counter "
                     BBCM=on

                  "forward pion detector "
                     FPDM=on
                     FpdmConfig  = 1 "switch to a different lead glass source code"

                  "pseudo Vertex Position Detector"
                     VPDD=on;
                     VpddConfig=4;

                  "field version "
                     MfldConfig=4;      "tabulated field, with correction "

* important: (1) new SVT version (2) FTPC gas correction tp Ar+C02 mix (3) SSD ladders raddi correction

                     SupoConfig = 1; "FTPC Support"
                     SvttConfig = 6; "SVTT version"
                     SvshConfig = 3; "SVT shield"
                     DensConfig = 1; "gas density correction"
                     FtpcConfig = 1; "ftpc configuration"

                  "Photon Multiplicity Detector Version "
                     PHMD=on;
                     PhmdConfig = 2;

                  "Silicon Strip Detector Version "
                     SISD=on;
                     SisdConfig = 55; "fifth version, full barrel with corrected radii and dead area"


                  "FTPC Readout barrel "
                     FTRO=on;
                     FtroConfig = 1;

                  "New version of the TPC backplane "
                     TpceConfig = 2;

                }

****************************************************************************************
  on Y2005G    { Y2005F + corrected corrected SVT dead volumes from Rene
                  "svt: 3 layers ";
                     nSvtLayer=6  " 3 bi-plane layers, nSvtLayer<=7 ";
                     nSvtVafer=0  " numbering is in the code   ";
                     svtWaferDim=0  " width is in the code      ";

                     ConeConfig=2 " new cable weight estimate ";

                  "ctb: central trigger barrer             ";
                     Itof=4 " call btofgeo4 ";
* NEW CONFIG!
                     BtofConfig=8;

* note the full barrel same as in y2003x:
                  "CALB"
                     emsEdit=on ;
                     CalbConfig = 2
                     nmod={60,60}; shift={75,105}; " 60 sectors on both sides"

                  "ECAL"
                     EcalConfig=1   " one ECAL patch, west "
                     ecalFill=3     " all sectors filled "

                  "beam-beam counter "
                     BBCM=on

                  "forward pion detector "
                     FPDM=on
                     FpdmConfig  = 1 "switch to a different lead glass source code"

                  "pseudo Vertex Position Detector"
                     VPDD=on;
                     VpddConfig=4;

                  "field version "
                     MfldConfig=4;      "tabulated field, with correction "

* important: (1) new SVT version (2) FTPC gas correction tp Ar+C02 mix (3) SSD ladders raddi correction

                     SupoConfig = 1; "FTPC Support"
                     SvttConfig =11; "SVTT version"
                     SvshConfig = 3; "SVT shield"
                     DensConfig = 1; "gas density correction"
                     FtpcConfig = 1; "ftpc configuration"

                  "Photon Multiplicity Detector Version "
                     PHMD=on;
                     PhmdConfig = 2;

                  "Silicon Strip Detector Version "
                     SISD=on;
                     SisdConfig = 55; "fifth version, full barrel with corrected radii and dead area"


                  "FTPC Readout barrel "
                     FTRO=on;
                     FtroConfig = 1;

                  "New version of the TPC backplane "
                     TpceConfig = 2;

                }

****************************************************************************************
  on Y2005    { first cut of 2005 geometry: TPC+CTB+FTPC+CaloPatch2+SVT3+BBC+FPD+ECAL+PHMD_FTRO;
                exe y2005;
              }

****************************************************************************************
  on Y2006C   { Y2006B without the PHMD
                  "svt: 3 layers ";
                     nSvtLayer=6  " 3 bi-plane layers, nSvtLayer<=7 ";
                     nSvtVafer=0  " numbering is in the code   ";
                     svtWaferDim=0  " width is in the code      ";

                     ConeConfig=2 " new cable weight estimate ";

                  "ctb: central trigger barrer             ";
                     Itof=4 " call btofgeo4 ";
                     BtofConfig=8;
* Full barrel in 2006
                  "CALB"
                     emsEdit=on ;
                     CalbConfig = 2
                     nmod={60,60}; shift={75,105}; " 60 sectors on both sides"


                  "ECAL"
                     EcalConfig=1   " one ECAL patch, west "
                     ecalFill=3     " all sectors filled "

                  "beam-beam counter "
                     BBCM=on

                  "forward pion detector "
                     FPDM=on
                     FpdmConfig  = 2 "switch to a different lead glass source code"

                  "pseudo Vertex Position Detector"
                     VPDD=on;
                     VpddConfig=4;

                  "field version "
                     MfldConfig=4;      "tabulated field, with correction "

* important: (1) new SVT version (2) FTPC gas correction tp Ar+C02 mix (3) SSD ladders raddi correction

                     SupoConfig = 1; "FTPC Support"
                     SvttConfig = 6; "SVTT version"
                     SvshConfig = 3; "SVT shield"
                     DensConfig = 1; "gas density correction"
                     FtpcConfig = 1; "ftpc configuration"

                  "Photon Multiplicity Detector Version "
                     PHMD=off;
                     PhmdConfig = 0;

                  "Silicon Strip Detector Version "
                     SISD=on;
                     SisdConfig = 55; "fifth version, full barrel newly corrected radii and dead area"


                  "FTPC Readout barrel "
                     FTRO=on;
                     FtroConfig = 1;

                  "New version of the TPC backplane "
                     TpceConfig = 3;
                  "Muon Trigger System"
                     MUTD = on;
                     MutdConfig = 1;
                  "We need a bigger Cave"
                     CaveConfig = 3;
                }
****************************************************************************************
  on Y2006G   { Y2006C new SVT dead material
                  "svt: 3 layers ";
                     nSvtLayer=6  " 3 bi-plane layers, nSvtLayer<=7 ";
                     nSvtVafer=0  " numbering is in the code   ";
                     svtWaferDim=0  " width is in the code      ";

                     ConeConfig=2 " new cable weight estimate ";

                  "ctb: central trigger barrer             ";
                     Itof=4 " call btofgeo4 ";
                     BtofConfig=8;
* Full barrel in 2006
                  "CALB"
                     emsEdit=on ;
                     CalbConfig = 2
                     nmod={60,60}; shift={75,105}; " 60 sectors on both sides"


                  "ECAL"
                     EcalConfig=1   " one ECAL patch, west "
                     ecalFill=3     " all sectors filled "

                  "beam-beam counter "
                     BBCM=on

                  "forward pion detector "
                     FPDM=on
                     FpdmConfig  = 2 "switch to a different lead glass source code"

                  "pseudo Vertex Position Detector"
                     VPDD=on;
                     VpddConfig=4;

                  "field version "
                     MfldConfig=4;      "tabulated field, with correction "

* important: (1) new SVT version (2) FTPC gas correction tp Ar+C02 mix (3) SSD ladders raddi correction

                     SupoConfig = 1; "FTPC Support"
                     SvttConfig =11; "SVTT version"
                     SvshConfig = 3; "SVT shield"
                     DensConfig = 1; "gas density correction"
                     FtpcConfig = 1; "ftpc configuration"

                  "Photon Multiplicity Detector Version "
                     PHMD=off;
                     PhmdConfig = 0;

                  "Silicon Strip Detector Version "
                     SISD=on;
                     SisdConfig = 55; "fifth version, full barrel newly corrected radii and dead area"


                  "FTPC Readout barrel "
                     FTRO=on;
                     FtroConfig = 1;

                  "New version of the TPC backplane "
                     TpceConfig = 3;
                  "Muon Trigger System"
                     MUTD = on;
                     MutdConfig = 1;
                  "We need a bigger Cave"
                     CaveConfig = 3;
                }
****************************************************************************************
  on Y2006B   { Y2006A + improved SSD with dead area + improved CALB
                  "svt: 3 layers ";
                     nSvtLayer=6  " 3 bi-plane layers, nSvtLayer<=7 ";
                     nSvtVafer=0  " numbering is in the code   ";
                     svtWaferDim=0  " width is in the code      ";

                     ConeConfig=2 " new cable weight estimate ";

                  "ctb: central trigger barrer             ";
                     Itof=4 " call btofgeo4 ";
* NEW CONFIG!
                     BtofConfig=8;

* Full barrel in 2006
                  "CALB"
                     emsEdit=on ;
                     CalbConfig = 2
                     nmod={60,60}; shift={75,105}; " 60 sectors on both sides"


                  "ECAL"
                     EcalConfig=1   " one ECAL patch, west "
                     ecalFill=3     " all sectors filled "

                  "beam-beam counter "
                     BBCM=on

                  "forward pion detector "
                     FPDM=on
                     FpdmConfig  = 2 "switch to a different lead glass source code"

                  "pseudo Vertex Position Detector"
                     VPDD=on;
                     VpddConfig=4;

                  "field version "
                     MfldConfig=4;      "tabulated field, with correction "

* important: (1) new SVT version (2) FTPC gas correction tp Ar+C02 mix (3) SSD ladders raddi correction

                     SupoConfig = 1; "FTPC Support"
                     SvttConfig = 6; "SVTT version"
                     SvshConfig = 3; "SVT shield"
                     DensConfig = 1; "gas density correction"
                     FtpcConfig = 1; "ftpc configuration"

                  "Photon Multiplicity Detector Version "
                     PHMD=on;
                     PhmdConfig = 2;

                  "Silicon Strip Detector Version "
                     SISD=on;
                     SisdConfig = 55; "fifth version, full barrel newly corrected radii and dead area"


                  "FTPC Readout barrel "
                     FTRO=on;
                     FtroConfig = 1;

                  "New version of the TPC backplane "
                     TpceConfig = 3;
                  "Muon Trigger System"
                     MUTD = on;
                     MutdConfig = 1;
                  "We need a bigger Cave"
                     CaveConfig = 3;
                }
****************************************************************************************
  on Y2006A   { Year 2006 baseline which is Y2005D+fixed TPC backplane+New SSD
                  "svt: 3 layers ";
                     nSvtLayer=6  " 3 bi-plane layers, nSvtLayer<=7 ";
                     nSvtVafer=0  " numbering is in the code   ";
                     svtWaferDim=0  " width is in the code      ";

                     ConeConfig=2 " new cable weight estimate ";

                  "ctb: central trigger barrer             ";
                     Itof=4 " call btofgeo4 ";
* NEW CONFIG!
                     BtofConfig=8;

* Full barrel in 2006
                  "CALB"
                     emsEdit=on ;
                     nmod={60,60}; shift={75,105}; " 60 sectors on both sides"


                  "ECAL"
                     EcalConfig=1   " one ECAL patch, west "
                     ecalFill=3     " all sectors filled "

                  "beam-beam counter "
                     BBCM=on

                  "forward pion detector "
                     FPDM=on
                     FpdmConfig  = 2 "switch to a different lead glass source code"

                  "pseudo Vertex Position Detector"
                     VPDD=on;
                     VpddConfig=4;

                  "field version "
                     MfldConfig=4;      "tabulated field, with correction "

* important: (1) new SVT version (2) FTPC gas correction tp Ar+C02 mix (3) SSD ladders raddi correction

                     SupoConfig = 1; "FTPC Support"
                     SvttConfig = 6; "SVTT version"
                     SvshConfig = 3; "SVT shield"
                     DensConfig = 1; "gas density correction"
                     FtpcConfig = 1; "ftpc configuration"

                  "Photon Multiplicity Detector Version "
                     PHMD=on;
                     PhmdConfig = 2;

                  "Silicon Strip Detector Version "
                     SISD=on;
                     SisdConfig = 35; "third version, full barrel newly corrected radii"


                  "FTPC Readout barrel "
                     FTRO=on;
                     FtroConfig = 1;

                  "New version of the TPC backplane "
                     TpceConfig = 3;
                  "Muon Trigger System"
                     MUTD = on;
                     MutdConfig = 1;
                  "We need a bigger Cave"
                     CaveConfig = 3;
                }
****************************************************************************************
  on Y2006    { Year 2006 baseline which is Y2005D+fixed TPC backplane+New SSD
                  "svt: 3 layers ";
                     nSvtLayer=6  " 3 bi-plane layers, nSvtLayer<=7 ";
                     nSvtVafer=0  " numbering is in the code   ";
                     svtWaferDim=0  " width is in the code      ";

                     ConeConfig=2 " new cable weight estimate ";

                  "ctb: central trigger barrer             ";
                     Itof=4 " call btofgeo4 ";
* NEW CONFIG!
                     BtofConfig=8;

* Full barrel in 2006
                  "CALB"
                     emsEdit=on ;
                     nmod={60,60}; shift={75,105}; " 60 sectors on both sides"


                  "ECAL"
                     EcalConfig=1   " one ECAL patch, west "
                     ecalFill=3     " all sectors filled "

                  "beam-beam counter "
                     BBCM=on

                  "forward pion detector "
                     FPDM=on
                     FpdmConfig  = 1 "switch to a different lead glass source code"

                  "pseudo Vertex Position Detector"
                     VPDD=on;
                     VpddConfig=4;

                  "field version "
                     MfldConfig=4;      "tabulated field, with correction "

* important: (1) new SVT version (2) FTPC gas correction tp Ar+C02 mix (3) SSD ladders raddi correction

                     SupoConfig = 1; "FTPC Support"
                     SvttConfig = 6; "SVTT version"
                     SvshConfig = 3; "SVT shield"
                     DensConfig = 1; "gas density correction"
                     FtpcConfig = 1; "ftpc configuration"

                  "Photon Multiplicity Detector Version "
                     PHMD=on;
                     PhmdConfig = 2;

                  "Silicon Strip Detector Version "
                     SISD=on;
                     SisdConfig = 35; "third version, full barrel newly corrected radii"


                  "FTPC Readout barrel "
                     FTRO=on;
                     FtroConfig = 1;

                  "New version of the TPC backplane "
                     TpceConfig = 3;
                  "Muon Trigger System"
                     MUTD = on;
                     MutdConfig = 1;
                  "We need a bigger Cave"
                     CaveConfig = 3;
                }
****************************************************************************************
  on Y2007A    { Year 2007 (see below) but with corrected SVT (carbon instead of Be water channels)
                  "svt: 3 layers ";
                     nSvtLayer=7  " 3 bi-plane layers, nSvtLayer<=7 ";
                     nSvtVafer=0  " numbering is in the code   ";
                     svtWaferDim=0  " width is in the code      ";

                     ConeConfig=2 " new cable weight estimate ";

                  "ctb: central trigger barrer             ";
                     Itof=5 " call btofgeo5 ";
* NEW CONFIG!
                     BtofConfig=10;

* Full barrel in 2007
                  "CALB"
                     emsEdit=on ;
* important:
                     CalbConfig = 2
                     nmod={60,60}; shift={75,105}; " 60 sectors on both sides"
                  "ECAL"
                     EcalConfig=1   " one ECAL patch, west "
                     ecalFill=3     " all sectors filled "

                  "beam-beam counter "
                     BBCM=on

                  "forward pion detector "
                     FPDM=on
                     FpdmConfig  = 3 "switch to a different lead glass source code"

                  "pseudo Vertex Position Detector"
                     VPDD=on;
                     VpddConfig=7;

                  "field version "
                     MfldConfig=4;      "tabulated field, with correction "

* important: (1) new SVT version (2) FTPC gas correction tp Ar+C02 mix (3) SSD ladders raddi correction

                     SupoConfig = 1;  "FTPC Support"
                     SvttConfig = 10; "SVTT version"
                     SvshConfig = 3;  "SVT shield"
                     DensConfig = 1;  "gas density correction"
                     FtpcConfig = 1;  "ftpc configuration"

                  "Photon Multiplicity Detector Version "
                     PHMD=on;
                     PhmdConfig = 2;

                  "Silicon Strip Detector Version "
                     SISD=on;
                     SisdConfig = 55; "fifth version, corrected radii, gaps, dead material"


                  "FTPC Readout barrel "
                     FTRO=on;
                     FtroConfig = 1;

                  "New version of the TPC backplane "
                     TpceConfig = 3;
                  "Muon Trigger System"
                     MUTD = on;
                     MutdConfig = 1;
                  "We need an even bigger Cave"
                     CaveConfig = 4;
                }
****************************************************************************************
  on Y2007G    { Year 2007A + dead material from Rene
                  "svt: 3 layers ";
                     nSvtLayer=7  " 3 bi-plane layers, nSvtLayer<=7 ";
                     nSvtVafer=0  " numbering is in the code   ";
                     svtWaferDim=0  " width is in the code      ";

                     ConeConfig=2 " new cable weight estimate ";

                  "ctb: central trigger barrer             ";
                     Itof=5 " call btofgeo5 ";
* NEW CONFIG!
                     BtofConfig=10;

* Full barrel in 2007
                  "CALB"
                     emsEdit=on ;
* important:
                     CalbConfig = 2
                     nmod={60,60}; shift={75,105}; " 60 sectors on both sides"
                  "ECAL"
                     EcalConfig=1   " one ECAL patch, west "
                     ecalFill=3     " all sectors filled "

                  "beam-beam counter "
                     BBCM=on

                  "forward pion detector "
                     FPDM=on
                     FpdmConfig  = 3 "switch to a different lead glass source code"

                  "pseudo Vertex Position Detector"
                     VPDD=on;
                     VpddConfig=7;

                  "field version "
                     MfldConfig=4;      "tabulated field, with correction "

* important: (1) new SVT version (2) FTPC gas correction tp Ar+C02 mix (3) SSD ladders raddi correction

                     SupoConfig = 1;  "FTPC Support"
                     SvttConfig = 11; "SVTT version"
                     SvshConfig = 3;  "SVT shield"
                     DensConfig = 1;  "gas density correction"
                     FtpcConfig = 1;  "ftpc configuration"

                  "Photon Multiplicity Detector Version "
                     PHMD=on;
                     PhmdConfig = 2;

                  "Silicon Strip Detector Version "
                     SISD=on;
                     SisdConfig = 55; "fifth version, corrected radii, gaps, dead material"


                  "FTPC Readout barrel "
                     FTRO=on;
                     FtroConfig = 1;

                  "New version of the TPC backplane "
                     TpceConfig = 3;
                  "Muon Trigger System"
                     MUTD = on;
                     MutdConfig = 1;
                  "We need an even bigger Cave"
                     CaveConfig = 4;
                }
****************************************************************************************
  on Y2007    { Year 2006 baseline which is Y2006+FMS
                  "svt: 3 layers ";
                     nSvtLayer=7  " 3 bi-plane layers, nSvtLayer<=7 ";
                     nSvtVafer=0  " numbering is in the code   ";
                     svtWaferDim=0  " width is in the code      ";

                     ConeConfig=2 " new cable weight estimate ";

                  "ctb: central trigger barrer             ";
                     Itof=5 " call btofgeo5 ";
* NEW CONFIG!
                     BtofConfig=10;

* Full barrel in 2007
                  "CALB"
                     emsEdit=on ;
* important:
                     CalbConfig = 2
                     nmod={60,60}; shift={75,105}; " 60 sectors on both sides"
                  "ECAL"
                     EcalConfig=1   " one ECAL patch, west "
                     ecalFill=3     " all sectors filled "

                  "beam-beam counter "
                     BBCM=on

                  "forward pion detector "
                     FPDM=on
                     FpdmConfig  = 3 "switch to a different lead glass source code"

                  "pseudo Vertex Position Detector"
                     VPDD=on;
                     VpddConfig=7;

                  "field version "
                     MfldConfig=4;      "tabulated field, with correction "

* important: (1) new SVT version (2) FTPC gas correction tp Ar+C02 mix (3) SSD ladders raddi correction

                     SupoConfig = 1; "FTPC Support"
                     SvttConfig = 6; "SVTT version"
                     SvshConfig = 3; "SVT shield"
                     DensConfig = 1; "gas density correction"
                     FtpcConfig = 1; "ftpc configuration"

                  "Photon Multiplicity Detector Version "
                     PHMD=on;
                     PhmdConfig = 2;

                  "Silicon Strip Detector Version "
                     SISD=on;
                     SisdConfig = 55; "fifth version, corrected radii, gaps, dead material"


                  "FTPC Readout barrel "
                     FTRO=on;
                     FtroConfig = 1;

                  "New version of the TPC backplane "
                     TpceConfig = 3;
                  "Muon Trigger System"
                     MUTD = on;
                     MutdConfig = 1;
                  "We need an even bigger Cave"
                     CaveConfig = 4;
                }
****************************************************************************************
  on Y2008    {  Year 2008 baseline: no SVT,  cones,beam support,FTPC in CAVE now
                exe y2008; }

****************************************************************************************
  on Y2008a   { Year 2008 + SCONE
                exe y2008a;}

****************************************************************************************
  on Y2009   { Year 2009 
                exe y2009;}

****************************************************************************************
  on DUMM01   { R and D geometry: TPC+DUMM

                     SVTT=off; "no SVT  at all in this configuration"
                     ftpc=off; "no FTPC at all in this configuration"
                  "ctb: central trigger barrer             ";
                     Itof=2 " call btofgeo2 ";
                     BtofConfig=5;
                  "CALB"
                     emsEdit=on
                     nmod={60,60}; shift={75,105}; " 60 sectors on both sides"
                  "ECAL"
                     EcalConfig=1   " west wheel "
                     ecalFill=3     " all sectors filled "
                  "beam-beam counter "
                     BBCM=on
                  "forward pion detector "
                     FPDM=on
                  "field version "
                     MfldConfig=4;      "tabulated field, with correction "

                     SvshConfig = 0; "SVT shield"
                     DensConfig = 1; "gas density correction"
                     SupoConfig = 1; "FTPC Support"
                     SvttConfig = 0;

                  "Photon Multiplicity Detector Version "
                     PHMD=off;
                     PhmdConfig = 0;
                  "Silicon Strip Detector Version "
                     SISD=off;
                     SisdConfig = 0;
* careful! Achtung!
                   PipeConfig=4;   " provisional"
                   pipeFlag=0; ! No wrap no svt shield
                   DUMM=on;        " put the dummy in"
                }
****************************************************************************************
  on UPGR01   { R and D geometry: TPC+SSD+HFT-SVT

                     SVTT=off; "no SVT  at all in this configuration"
                     ftpc=off; "no FTPC at all in this configuration"
                  "ctb: central trigger barrer             ";
                     Itof=2 " call btofgeo2 ";
                     BtofConfig=5;
                  "CALB"
                     emsEdit=on
                     nmod={60,60}; shift={75,105}; " 60 sectors on both sides"
                  "ECAL"
                     EcalConfig=1   " west wheel "
                     ecalFill=3     " all sectors filled "
                  "beam-beam counter "
                     BBCM=on
                  "forward pion detector "
                     FPDM=on
                  "field version "
                     MfldConfig=4;      "tabulated field, with correction "

                     SvshConfig = 0; "SVT shield"
                     DensConfig = 1; "gas density correction"
                     SupoConfig = 1; "FTPC Support"
                     SvttConfig = 0;

                  "Photon Multiplicity Detector Version "
                     PHMD=on;
                     PhmdConfig = 1;
                  "Silicon Strip Detector Version "
                     SISD=on;
                     SisdConfig = 45;
* careful! Achtung!
                   PipeConfig=4;   " provisional"
                   pipeFlag=0; ! No wrap no svt shield
                   PIXL=on;        " put the pixel detector in"
                   PixlConfig=4;   " newest version by Andrew Rose"
* HPD, IST off
* Forward STAR tracker disk
                   FSTD=on;  "new pixel based forward tracker"
                   FstdConfig=2;
* Forward STAR tracker disk
                   FGTD=off;  "GEM forward tracker"
                   FgtdConfig=0;
* Forward GEM disks in this tag
                   IGTD=on;
* prototype of the Inner Tracker SuPport structure
                   ITSP=on;
                }
****************************************************************************************
  on UPGR02    { R and D geometry: TPC+IST+HFT-SVT
                  "svt: 3 layers ";
                     nSvtLayer=6  " 3 bi-plane layers, nSvtLayer<=7 ";
                     nSvtVafer=0  " numbering is in the code   ";
                     svtWaferDim=0  " width is in the code      ";

                     ConeConfig=2 " new cable weight estimate ";

                  "ctb: central trigger barrer             ";
                     Itof=4 " call btofgeo4 ";
* NEW CONFIG!
                     BtofConfig=8;

                  "CALB"
                     emsEdit=on
                     CalbConfig = 1
* remember that with this config, the following parameters have
* a different meaning because we have to (unfortunately) switch
* from divisions to copies and introduce a map, which DOES
* control the configuration
                     nmod={60,60}; shift={75,105}; " 60 sectors West plus 30 East split between 2 halves"

                  "ECAL"
                     EcalConfig=1   " one ECAL patch, west "
                     ecalFill=3     " all sectors filled "

                  "beam-beam counter "
                     BBCM=on

                  "forward pion detector "
                     FPDM=on
                     FpdmConfig  = 1 "switch to a different lead glass source code"

                  "pseudo Vertex Position Detector"
                     VPDD=on;
                     VpddConfig=4;

                  "field version "
                     MfldConfig=4;      "tabulated field, with correction "

* important: (1) new SVT version (2) FTPC gas correction tp Ar+C02 mix (3) SSD ladders raddi correction

                     SupoConfig = 1; "FTPC Support"
                     SvttConfig = 5; "SVTT version"
                     DensConfig = 1; "gas density correction"
                     FtpcConfig = 1; "ftpc configuration"

                  "Photon Multiplicity Detector Version "
                     PHMD=on;
                     PhmdConfig = 2;

                  "Silicon Strip Detector Version "
                     SISD=off;
                     SisdConfig = 0; "no ssd here"


                  "FTPC Readout barrel "
                     FTRO=on;
                     FtroConfig = 1;

* Inner STAR tracker barrel
                   ISTB=on;  "new pixel based inner tracker"
                   IstbConfig=1;

* careful! Achtung!
                   PipeConfig=5;   " thinner pipe "
                   pipeFlag=0; ! No wrap no svt shield
                   PIXL=on;        " activate "
                   PixlConfig=3;   " source version "

                  "New version of the TPC backplane "
                     TpceConfig = 3;

                }
*************************************************************************************************************
  on UPGR03   { New Tracking: IST+IGT+HFT-SVT

                     SVTT=off; "no SVT  at all in this configuration"
                     ftpc=off; "no FTPC at all in this configuration"
                  "ctb: central trigger barrer             ";
                     Itof=2 " call btofgeo2 ";
                     BtofConfig=5;
                  "CALB"
                     emsEdit=on
                     nmod={60,60}; shift={75,105}; " 60 sectors on both sides"
                  "ECAL"
                     EcalConfig=1   " west wheel "
                     ecalFill=3     " all sectors filled "
                  "beam-beam counter "
                     BBCM=on
                  "forward pion detector "
                     FPDM=on
                  "field version "
                     MfldConfig=4;      "tabulated field, with correction "
*                    -- Obsoleted: CorrNum = 4;
                     SvshConfig = 2; "SVT shield"
                     DensConfig = 1; "gas density correction"
                     SupoConfig = 1; "FTPC Support"
                     SvttConfig = 4;

                  "Photon Multiplicity Detector Version "
                     PHMD=on;
                     PhmdConfig = 1;
                  "Silicon Strip Detector Version "
                     SISD=on;
                     SisdConfig = 23;
* careful! Achtung!
                   PipeConfig=4;   " provisional"
                   pipeFlag=0; ! No wrap no svt shield
                   PIXL=on;        " put the pixel detector in"
                   PixlConfig=2;   " newer version decoupled from SVT"
* Inner STAR tracker barrel
                   ISTB=on;  "new pixel based inner tracker"
                   IstbConfig=1;
* Inner STAR GEM barrel
                   GEMB=off;
                   GembConfig=0;
* Forward STAR tracker disk
                   FSTD=on;  "new pixel based forward tracker"
                   FstdConfig=1;
* Forward STAR tracker disk
                   FGTD=off;  "GEM forward tracker"
                   FgtdConfig=0;
* the forward GEM disks
                   IGTD=on;
                }
*************************************************************************************************************
  on UPGR04   { New Tracking: HPD

                     SVTT=off; "no SVT  at all in this configuration"
                     ftpc=off; "no FTPC at all in this configuration"

                  "ctb: central trigger barrer             ";
                     Itof=2 " call btofgeo2 ";
                     BtofConfig=5;
                  "CALB"
                     emsEdit=on
                     nmod={60,60}; shift={75,105}; " 60 sectors on both sides"
                  "ECAL"
                     EcalConfig=1   " west wheel "
                     ecalFill=3     " all sectors filled "
                  "beam-beam counter "
                     BBCM=on
                  "forward pion detector "
                     FPDM=on
                  "field version "
                     MfldConfig=4;      "tabulated field, with correction "
*                    -- Obsoleted: CorrNum = 4;
                     SvshConfig = 2; "SVT shield"
                     DensConfig = 1; "gas density correction"
                     SupoConfig = 1; "FTPC Support"
                     SvttConfig = 4;

                  "Photon Multiplicity Detector Version "
                     PHMD=off;
                     PhmdConfig = 0;
                  "Silicon Strip Detector Version "
                     SISD=on;
                     SisdConfig = 35;
* careful! Achtung!
                   PipeConfig=4;   " provisional"
                   pipeFlag=0; ! No wrap no svt shield
                   HPDT=on;        " put the detector in"
                   HpdtConfig=1;   " base version"
                }
****************************************************************************************
  on UPGR05   { New Tracking: HFT+HPD+IST+TPC-SVT

                     SVTT=off; "no SVT  at all in this configuration"
                     ftpc=off; "no FTPC at all in this configuration"
                  "ctb: central trigger barrer             ";
                     Itof=2 " call btofgeo2 ";
                     BtofConfig=5;
                  "CALB"
                     emsEdit=on
                     nmod={60,60}; shift={75,105}; " 60 sectors on both sides"
                  "ECAL"
                     EcalConfig=1   " west wheel "
                     ecalFill=3     " all sectors filled "
                  "beam-beam counter "
                     BBCM=on
                  "forward pion detector "
                     FPDM=on
                  "field version "
                     MfldConfig=4;      "tabulated field, with correction "

                     SvshConfig = 0; "SVT shield"
                     DensConfig = 1; "gas density correction"
                     SupoConfig = 1; "FTPC Support"
                     SvttConfig = 0;

                  "Photon Multiplicity Detector Version "
                     PHMD=on;
                     PhmdConfig = 1;
                  "Silicon Strip Detector Version "
                     SISD=on;
                     SisdConfig = 45;
* careful! Achtung!
                   PipeConfig=4;   " provisional"
                   pipeFlag=0; ! No wrap no svt shield
                   PIXL=on;        " put the pixel detector in"
                   PixlConfig=4;   " newest version by Andrew Rose"

                   HPDT=on;        " put the Hybrid Pixel detector in"
                   HpdtConfig=1;   " base version"
* Inner STAR tracker barrel
                   ISTB=on;  "new pixel based inner tracker"
                   IstbConfig=3;
* Inner STAR GEM barrel
                   GEMB=off;
                   GembConfig=0;
* Forward STAR tracker disk
                   FSTD=on;  "new pixel based forward tracker"
                   FstdConfig=2;
* Forward STAR tracker disk
                   FGTD=off;  "GEM forward tracker"
                   FgtdConfig=0;
* Forward GEM disks in this tag
                   IGTD=on;
* prototype of the Inner Tracker SuPport structure
                   ITSP=on;
                }
****************************************************************************************
  on UPGR06   { New Tracking: HFT+HPD+SSD

                     SVTT=off; "no SVT  at all in this configuration"
                     ftpc=off; "no FTPC at all in this configuration"
                  "ctb: central trigger barrer             ";
                     Itof=2 " call btofgeo2 ";
                     BtofConfig=5;
                  "CALB"
                     emsEdit=on
                     nmod={60,60}; shift={75,105}; " 60 sectors on both sides"
                  "ECAL"
                     EcalConfig=1   " west wheel "
                     ecalFill=3     " all sectors filled "
                  "beam-beam counter "
                     BBCM=on
                  "forward pion detector "
                     FPDM=on
                  "field version "
                     MfldConfig=4;      "tabulated field, with correction "

                     SvshConfig = 0; "SVT shield"
                     DensConfig = 1; "gas density correction"
                     SupoConfig = 1; "FTPC Support"
                     SvttConfig = 0;

                  "Photon Multiplicity Detector Version "
                     PHMD=on;
                     PhmdConfig = 1;
                  "Silicon Strip Detector Version "
                     SISD=on;
                     SisdConfig = 45;
* careful! Achtung!
                   PipeConfig=4;   " provisional"
                   pipeFlag=0; ! No wrap no svt shield
                   PIXL=on;        " put the pixel detector in"
                   PixlConfig=4;   " newest version by Andrew Rose"

                   HPDT=on;        " put the Hybrid Pixel detector in"
                   HpdtConfig=1;   " base version"
* Forward STAR tracker disk
                   FSTD=on;  "new pixel based forward tracker"
                   FstdConfig=2;
* Forward STAR tracker disk
                   FGTD=off;  "GEM forward tracker"
                   FgtdConfig=0;
* Forward GEM disks in this tag
                   IGTD=on;
* prototype of the Inner Tracker SuPport structure
                   ITSP=on;
                }
****************************************************************************************
  on UPGR07   { New Tracking: HFT+IST+TPC+SSD-SVT

                     SVTT=off; "no SVT  at all in this configuration"
                     ftpc=off; "no FTPC at all in this configuration"
                  "ctb: central trigger barrer             ";
                     Itof=2 " call btofgeo2 ";
                     BtofConfig=5;
                  "CALB"
                     emsEdit=on
                     nmod={60,60}; shift={75,105}; " 60 sectors on both sides"
                  "ECAL"
                     EcalConfig=1   " west wheel "
                     ecalFill=3     " all sectors filled "
                  "beam-beam counter "
                     BBCM=on
                  "forward pion detector "
                     FPDM=on
                  "field version "
                     MfldConfig=4;      "tabulated field, with correction "

                     SvshConfig = 0; "SVT shield"
                     DensConfig = 1; "gas density correction"
                     SupoConfig = 1; "FTPC Support"
                     SvttConfig = 0;

                  "Photon Multiplicity Detector Version "
                     PHMD=on;
                     PhmdConfig = 1;
                  "Silicon Strip Detector Version "
                     SISD=on;
                     SisdConfig = 45;
* careful! Achtung!
                   PipeConfig=4;   " provisional"
                   pipeFlag=0; ! No wrap no svt shield
                   PIXL=on;        " put the pixel detector in"
                   PixlConfig=4;   " newest version by Andrew Rose"

                   ISTB=on;  "IST barrel"
                   IstbConfig=3;

                   FSTD=on;  "new pixel based forward tracker disk"
                   FstdConfig=2;

                   IGTD=on; "Forward GEM disks in this tag"
                   ITSP=on; "prototype of the Inner Tracker SuPport structure"
                }
****************************************************************************************
  on UPGR08   { New Tracking: HFT+HPD+IST+TPC-SVT-SSD

                     SVTT=off; "no SVT  at all in this configuration"
                     ftpc=off; "no FTPC at all in this configuration"
                  "ctb: central trigger barrer             ";
                     Itof=2 " call btofgeo2 ";
                     BtofConfig=5;
                  "CALB"
                     emsEdit=on
                     nmod={60,60}; shift={75,105}; " 60 sectors on both sides"
                  "ECAL"
                     EcalConfig=1   " west wheel "
                     ecalFill=3     " all sectors filled "
                  "beam-beam counter "
                     BBCM=on
                  "forward pion detector "
                     FPDM=on
                  "field version "
                     MfldConfig=4;      "tabulated field, with correction "

                     SvshConfig = 0; "SVT shield"
                     DensConfig = 1; "gas density correction"
                     SupoConfig = 1; "FTPC Support"
                     SvttConfig = 0;

                  "Photon Multiplicity Detector Version "
                     PHMD=on;
                     PhmdConfig = 1;
                  "Silicon Strip Detector Version "
                     SISD=off;
                     SisdConfig = 0;
* careful! Achtung!
                   PipeConfig=4;   " provisional"
                   pipeFlag=0; ! No wrap no svt shield
                   PIXL=on;        " put the pixel detector in"
                   PixlConfig=4;   " newest version by Andrew Rose"

                   HPDT=on;        " put the Hybrid Pixel detector in"
                   HpdtConfig=1;   " base version"
* Inner STAR tracker barrel
                   ISTB=on;  "new pixel based inner tracker"
                   IstbConfig=3;
* Inner STAR GEM barrel
                   GEMB=off;
                   GembConfig=0;
* Forward STAR tracker disk
                   FSTD=on;  "new pixel based forward tracker"
                   FstdConfig=2;
* Forward STAR tracker disk
                   FGTD=off;  "GEM forward tracker"
                   FgtdConfig=0;
* Forward GEM disks in this tag
                   IGTD=on;
* prototype of the Inner Tracker SuPport structure
                   ITSP=on;
                }
****************************************************************************************
  on UPGR09   { New Tracking: HFT+HPD+IST*outer+TPC-SVT-SSD

                     SVTT=off; "no SVT  at all in this configuration"
                     ftpc=off; "no FTPC at all in this configuration"
                  "ctb: central trigger barrer             ";
                     Itof=2 " call btofgeo2 ";
                     BtofConfig=5;
                  "CALB"
                     emsEdit=on
                     nmod={60,60}; shift={75,105}; " 60 sectors on both sides"
                  "ECAL"
                     EcalConfig=1   " west wheel "
                     ecalFill=3     " all sectors filled "
                  "beam-beam counter "
                     BBCM=on
                  "forward pion detector "
                     FPDM=on
                  "field version "
                     MfldConfig=4;      "tabulated field, with correction "

                     SvshConfig = 0; "SVT shield"
                     DensConfig = 1; "gas density correction"
                     SupoConfig = 1; "FTPC Support"
                     SvttConfig = 0;

                  "Photon Multiplicity Detector Version "
                     PHMD=on;
                     PhmdConfig = 1;
                  "Silicon Strip Detector Version "
                     SISD=off;
                     SisdConfig = 0;
* careful! Achtung!
                   PipeConfig=4;   " provisional"
                   PIXL=on;        " put the pixel detector in"
                   pipeFlag=0; ! No wrap no svt shield
                   PixlConfig=4;   " newest version by Andrew Rose"

                   HPDT=on;        " put the Hybrid Pixel detector in"
                   HpdtConfig=1;   " base version"
* Inner STAR tracker barrel
                   ISTB=on;  "new pixel based inner tracker"
                   IstbConfig=4;
* Inner STAR GEM barrel
                   GEMB=off;
                   GembConfig=0;
* Forward STAR tracker disk
                   FSTD=on;  "new pixel based forward tracker"
                   FstdConfig=2;
* Forward STAR tracker disk
                   FGTD=off;  "GEM forward tracker"
                   FgtdConfig=0;
* Forward GEM disks in this tag
                   IGTD=on;
* prototype of the Inner Tracker SuPport structure
                   ITSP=on;
                }
****************************************************************************************
  on UPGR10   { New Tracking: HFT+innerLayerIST+TPC-SVT+SSD

                     SVTT=off; "no SVT  at all in this configuration"
                     ftpc=off; "no FTPC at all in this configuration"
                  "ctb: central trigger barrer             ";
                     Itof=2 " call btofgeo2 ";
                     BtofConfig=5;
                  "CALB"
                     emsEdit=on
                     nmod={60,60}; shift={75,105}; " 60 sectors on both sides"
                  "ECAL"
                     EcalConfig=1   " west wheel "
                     ecalFill=3     " all sectors filled "
                  "beam-beam counter "
                     BBCM=on
                  "forward pion detector "
                     FPDM=on
                  "field version "
                     MfldConfig=4;      "tabulated field, with correction "

                     SvshConfig = 0; "SVT shield"
                     DensConfig = 1; "gas density correction"
                     SupoConfig = 1; "FTPC Support"
                     SvttConfig = 0;

                  "Photon Multiplicity Detector Version "
                     PHMD=on;
                     PhmdConfig = 1;
                  "Silicon Strip Detector Version "
                     SISD=off;
                     SisdConfig = 0;
* careful! Achtung!
                   PipeConfig=4;   " provisional"
                   pipeFlag=0; ! No wrap no svt shield
                   PIXL=on;        " put the pixel detector in"
                   PixlConfig=4;   " newest version by Andrew Rose"

                  "Silicon Strip Detector Version "
                   SISD=on;
                   SisdConfig = 45;
* Inner STAR tracker barrel
                   ISTB=on;  "new pixel based inner tracker"
                   IstbConfig=5;
* Inner STAR GEM barrel
                   GEMB=off;
                   GembConfig=0;
* Forward STAR tracker disk
                   FSTD=on;  "new pixel based forward tracker"
                   FstdConfig=2;
* Forward STAR tracker disk
                   FGTD=off;  "GEM forward tracker"
                   FgtdConfig=0;
* Forward GEM disks in this tag
                   IGTD=on;
* prototype of the Inner Tracker SuPport structure
                   ITSP=on;
                }
****************************************************************************************
  on UPGR11   { New Tracking: HFT+2LayerIST+TPC-SVT+SSD

                     SVTT=off; "no SVT  at all in this configuration"
                     ftpc=off; "no FTPC at all in this configuration"
                  "ctb: central trigger barrer             ";
                     Itof=2 " call btofgeo2 ";
                     BtofConfig=5;
                  "CALB"
                     emsEdit=on
                     nmod={60,60}; shift={75,105}; " 60 sectors on both sides"
                  "ECAL"
                     EcalConfig=1   " west wheel "
                     ecalFill=3     " all sectors filled "
                  "beam-beam counter "
                     BBCM=on
                  "forward pion detector "
                     FPDM=on
                  "field version "
                     MfldConfig=4;      "tabulated field, with correction "

                     SvshConfig = 0; "SVT shield"
                     DensConfig = 1; "gas density correction"
                     SupoConfig = 1; "FTPC Support"
                     SvttConfig = 0;

                  "Photon Multiplicity Detector Version "
                     PHMD=on;
                     PhmdConfig = 1;
* careful! Achtung!
                   PipeConfig=4;   " provisional"
                   pipeFlag=0; ! No wrap no svt shield
                   PIXL=on;        " put the pixel detector in"
                   PixlConfig=4;   " newest version by Andrew Rose"

                  "Silicon Strip Detector Version "
                   SISD=on;
                   SisdConfig = 45;
* Inner STAR tracker barrel
                   ISTB=on;  "new pixel based inner tracker"
                   IstbConfig=6;
* Inner STAR GEM barrel
                   GEMB=off;
                   GembConfig=0;
* Forward STAR tracker disk
                   FSTD=on;  "new pixel based forward tracker"
                   FstdConfig=2;
* Forward STAR tracker disk
                   FGTD=off;  "GEM forward tracker"
                   FgtdConfig=0;
* Forward GEM disks in this tag
                   IGTD=on;
* prototype of the Inner Tracker SuPport structure
                   ITSP=on;
                }
****************************************************************************************
  on UPGR12   { New Tracking: HFT+HPD+IST+TPC+IGT*newRadii

                     SVTT=off; "no SVT  at all in this configuration"
                     ftpc=off; "no FTPC at all in this configuration"
                  "ctb: central trigger barrer             ";
                     Itof=2 " call btofgeo2 ";
                     BtofConfig=5;
                  "CALB"
                     emsEdit=on
                     nmod={60,60}; shift={75,105}; " 60 sectors on both sides"
                  "ECAL"
                     EcalConfig=1   " west wheel "
                     ecalFill=3     " all sectors filled "
                  "beam-beam counter "
                     BBCM=on
                  "forward pion detector "
                     FPDM=on
                  "field version "
                     MfldConfig=4;      "tabulated field, with correction "

                     SvshConfig = 0; "SVT shield"
                     DensConfig = 1; "gas density correction"
                     SupoConfig = 1; "FTPC Support"
                     SvttConfig = 0;

                  "Photon Multiplicity Detector Version "
                     PHMD=on;
                     PhmdConfig = 1;
                  "Silicon Strip Detector Version "
                     SISD=on;
                     SisdConfig = 45;
* careful! Achtung!
                   PipeConfig=4;   " provisional"
                   pipeFlag=0; ! No wrap no svt shield
                   PIXL=on;        " put the pixel detector in"
                   PixlConfig=4;   " newest version by Andrew Rose"

                   HPDT=on;        " put the Hybrid Pixel detector in"
                   HpdtConfig=1;   " base version"
* Inner STAR tracker barrel
                   ISTB=on;  "new pixel based inner tracker"
                   IstbConfig=3;
* Inner STAR GEM barrel
                   GEMB=off;
                   GembConfig=0;
* Forward STAR tracker disk
                   FSTD=on;  "new pixel based forward tracker"
                   FstdConfig=2;
* Forward STAR tracker disk
                   FGTD=off;  "GEM forward tracker"
                   FgtdConfig=0;
* Forward GEM disks in this tag
                   IGTD=on;
                   IgtdConfig=2;
* prototype of the Inner Tracker SuPport structure
                   ITSP=on;
                }
****************************************************************************************
  on UPGR13   { New Tracking: HFT+IST+TPC+SSD-SVT
                     SVTT=off; "no SVT  at all in this configuration"
                     ftpc=off; "no FTPC at all in this configuration"
                  "ctb: central trigger barrer             ";
                     Itof=2 " call btofgeo2 ";
                     BtofConfig=5;
                  "CALB"
                     emsEdit=on
                     nmod={60,60}; shift={75,105}; " 60 sectors on both sides"
                  "ECAL"
                     EcalConfig=1   " west wheel "
                     ecalFill=3     " all sectors filled "
                  "beam-beam counter "
                     BBCM=on
                  "forward pion detector "
                     FPDM=on
                  "field version "
                     MfldConfig=4;      "tabulated field, with correction "

                     SvshConfig = 0; "SVT shield"
                     DensConfig = 1; "gas density correction"
                     SupoConfig = 1; "FTPC Support"
                     SvttConfig = 0;

                  "Photon Multiplicity Detector Version "
                     PHMD=on;
                     PhmdConfig = 1;
                  "Silicon Strip Detector Version "
                     SISD=on;
                     SisdConfig = 65;
* careful! Achtung!
                   PipeConfig=4;   " provisional"
                   pipeFlag=0; ! No wrap no svt shield
                   PIXL=on;        " put the pixel detector in"
                   PixlConfig=5;   " newest version, thicker active Si"

                   ISTB=on;  "IST barrel"
                   IstbConfig=7;

                   FSTD=off;  "no pixel based forward tracker in this tag"
                   FstdConfig=0;

* Forward STAR tracker disk
                   FGTD=on;  "GEM forward tracker"
                   FgtdConfig=2;
* On Gerrit  request, we disable the cone:
                   ITSP=off; "prototype of the Inner Tracker SuPport structure"
                }
****************************************************************************************
  on UPGR14   { TUP sans IST: HFT+TPC+SSD-SVT
                     SVTT=off; "no SVT  at all in this configuration"
                     ftpc=off; "no FTPC at all in this configuration"
                  "ctb: central trigger barrer             ";
                     Itof=2 " call btofgeo2 ";
                     BtofConfig=5;
                  "CALB"
                     emsEdit=on
                     nmod={60,60}; shift={75,105}; " 60 sectors on both sides"
                  "ECAL"
                     EcalConfig=1   " west wheel "
                     ecalFill=3     " all sectors filled "
                  "beam-beam counter "
                     BBCM=on
                  "forward pion detector "
                     FPDM=on
                  "field version "
                     MfldConfig=4;      "tabulated field, with correction "

                     SvshConfig = 0; "SVT shield"
                     DensConfig = 1; "gas density correction"
                     SupoConfig = 1; "FTPC Support"
                     SvttConfig = 0;

                  "Photon Multiplicity Detector Version "
                     PHMD=on;
                     PhmdConfig = 1;
                  "Silicon Strip Detector Version "
                     SISD=on;
                     SisdConfig = 65;
* careful! Achtung!
                   PipeConfig=4;   " provisional"
                   pipeFlag=0; ! No wrap no svt shield
                   PIXL=on;        " put the pixel detector in"
                   PixlConfig=5;   " newest version, thicker active Si"

                   ISTB=off;       "no IST barrel"
                   IstbConfig=0;   "ditto"

                   FSTD=off;  "no pixel based forward tracker in this tag"
                   FstdConfig=0;

* Forward STAR tracker disk
                   FGTD=on;  "GEM forward tracker"
                   FgtdConfig=2;
* On Gerrit  request, we disable the cone:
                   ITSP=off; "prototype of the Inner Tracker SuPport structure"
                }
****************************************************************************************
*The reason for naming these three file xxxxgeo00.g is to indicate that these
*are the most simple geometries that can be made. As such they Do not fit in
*with the existing naming scheme where the increasing numbers indicate a
*next step of evolution, better understanding of material, better design, etc.
*I explicitly wanted to break with this scheme to make it clear that these
*designs are just for testing purposes. Gerrit

  on UPGR15   { New Tracking: HFT+IST+TPC+SSD-SVT
                exe UPGR15; }
****************************************************************************************
  on UPGR16   { New Tracking: HFT+IST+TPC+SSD-SVT
                     SVTT=off; "no SVT  at all in this configuration"
                     ftpc=off; "no FTPC at all in this configuration"
                     SCON=on;
                     ConeConfig=2 " new cable weight estimate ";

* X.Dong
                 "ctb: central trigger barrer             ";
                     Itof=6 " call btofgeo6 ";
* NEW CONFIG!
                     tofX0= 0.00;
                     tofZ0=-0.50;
                     BtofConfig=6;

                  "CALB"
                     emsEdit=on
                     nmod={60,60}; shift={75,105}; " 60 sectors on both sides"
                     CalbConfig = 2
                  "ECAL"
                     EcalConfig=1   " west wheel "
                     ecalFill=3     " all sectors filled "
                  "beam-beam counter "
                     BBCM=on
                  "forward pion detector "
                     FPDM=on
                  "field version "
                     MfldConfig=4;      "tabulated field, with correction "

                     SvshConfig = 0; "SVT shield"
                     DensConfig = 1; "gas density correction"
                     SupoConfig = 1; "FTPC Support"
                     SvttConfig = 0;

                  "Photon Multiplicity Detector Version "
                     PHMD=on;
                     PhmdConfig = 1;
                  "Silicon Strip Detector Version "
                     SISD=on;
                     SisdConfig = 65;
* careful! Achtung!
                   PipeConfig=4;   " provisional"
                   pipeFlag=-1; !   " Simplest.Gerrit"
                   PipeFlag = 1;    "pipe wrap only"

                   PIXL=on;         " put the pixel detector in"
                   PixlConfig=-1;   " Simplest.Gerrit"

                   ISTB=on;  "IST barrel"
                   IstbConfig=-1;

                   FSTD=off;  "no pixel based forward tracker in this tag"
                   FstdConfig=0;

* Forward STAR tracker disk
                   FGTD=on;  "GEM forward tracker"
                   FgtdConfig=3;
* On Gerrit request, we disable the cone:
                   ITSP=off; "prototype of the Inner Tracker SuPport structure"
                  "New version of the TPC backplane "
                     TpceConfig = 3;
                  "We need an even bigger Cave"
                     CaveConfig = 4;
                }
****************************************************************************************
  on UPGR17   { UPGR16 - FGTD + FTPC  request Wei-Ming-Zhang
                     SVTT=off; "no SVT  at all in this configuration"
                     ftpc=off; "no FTPC at all in this configuration"
                     SCON=on;
                     ConeConfig=2 " new cable weight estimate ";

                  "ctb: central trigger barrer             ";
                     Itof=2 " call btofgeo2 ";
                     BtofConfig=6;
                  "CALB"
                     emsEdit=on
                     nmod={60,60}; shift={75,105}; " 60 sectors on both sides"
                  "ECAL"
                     EcalConfig=1   " west wheel "
                     ecalFill=3     " all sectors filled "
                  "beam-beam counter "
                     BBCM=on
                  "forward pion detector "
                     FPDM=on
                  "field version "
                     MfldConfig=4;      "tabulated field, with correction "

                     SvshConfig = 0; "SVT shield"
                     DensConfig = 1; "gas density correction"
                     SvttConfig = 0;

                  "Photon Multiplicity Detector Version "
                     PHMD=on;
                     PhmdConfig = 1;
                  "Silicon Strip Detector Version "
                     SISD=on;
                     SisdConfig = 65;
* careful! Achtung!
                   PipeConfig=4;   " provisional"
                   pipeFlag=-1; !   " Simplest.Gerrit"
                   PipeFlag = 1;    "pipe wrap only"

                   PIXL=on;         " put the pixel detector in"
                   PixlConfig=-1;   " Simplest.Gerrit"

                   ISTB=on;  "IST barrel"
                   IstbConfig=-1;

                   FSTD=off;  "no pixel based forward tracker in this tag"
                   FstdConfig=0;

* No Forward STAR tracker disk
                   FGTD=off;  "GEM forward tracker"
* On Gerrit  request, we disable the cone:
                   ITSP=off; "prototype of the Inner Tracker SuPport structure"
                  "New version of the TPC backplane "
                     TpceConfig = 3;
                  "We need an even bigger Cave"
                     CaveConfig = 4;
* FTPC from y2008
                     SupoConfig = 1;            "FTPC Support"
                     FtpcConfig = 1;            "ftpc configuration"
                  "FTPC Readout barrel "
                     FTRO=on;
                     FtroConfig = 1;

                }
****************************************************************************************
  on DEV2005    { THIS TAG IS RESERVED FOR THE 2005 DEVELOPMENT ONLY
                  "svt: 3 layers ";
                     nSvtLayer=6  " 3 bi-plane layers, nSvtLayer<=7 ";
                     nSvtVafer=0  " numbering is in the code   ";
                     svtWaferDim=0  " width is in the code      ";

                     ConeConfig=2 " new cable weight estimate ";

                  "ctb: central trigger barrer             ";
                     Itof=4 " call btofgeo4 ";
* NEW CONFIG!
                     BtofConfig=8;

                  "CALB"
                     emsEdit=on
                     CalbConfig = 1
* remember that with this config, the following parameters have
* a different meaning because we have to (unfortunately) switch
* from divisions to copies and introduce a map, which DOES
* control the configuration
                     nmod={60,60}; shift={75,105}; " 60 sectors West plus 30 East split between 2 halves"

                  "ECAL"
                     EcalConfig=1   " one ECAL patch, west "
                     ecalFill=3     " all sectors filled "

                  "beam-beam counter "
                     BBCM=on

                  "forward pion detector "
                     FPDM=on
                     FpdmConfig  = 1 "switch to a different lead glass source code"

                  "pseudo Vertex Position Detector"
                     VPDD=on;
                     VpddConfig=4;

                  "field version "
                     MfldConfig=4;      "tabulated field, with correction "

* important: (1) new SVT version (2) FTPC gas correction tp Ar+C02 mix (3) SSD ladders raddi correction

                     SupoConfig = 1; "FTPC Support"
                     SvttConfig = 7; "SVTT version"
                     DensConfig = 1; "gas density correction"
                     FtpcConfig = 1; "ftpc configuration"

                  "Photon Multiplicity Detector Version "
                     PHMD=on;
                     PhmdConfig = 2;

                  "Silicon Strip Detector Version "
                     SISD=on;
                     SisdConfig = 24; "second version, full barrel with corrected radii"


                  "FTPC Readout barrel "
                     FTRO=on;
                     FtroConfig = 1;

                  "New version of the TPC backplane "
                     TpceConfig = 2;
                }

****************************************************************************************
  on DEV2007    { THIS TAG IS RESERVED FOR THE 2007 DEVELOPMENT ONLY
                  "svt: 3 layers ";
                     nSvtLayer=6  " 3 bi-plane layers, nSvtLayer<=7 ";
                     nSvtVafer=0  " numbering is in the code   ";
                     svtWaferDim=0  " width is in the code      ";

                     ConeConfig=2 " new cable weight estimate ";

                  "ctb: central trigger barrer             ";
                     Itof=5 " call btofgeo5 ";
* NEW CONFIG!
                     BtofConfig=10;

* Full barrel in 2007
                  "CALB"
                     emsEdit=on ;
* important:
                     CalbConfig = 2
                     nmod={60,60}; shift={75,105}; " 60 sectors on both sides"
                  "ECAL"
                     EcalConfig=1   " one ECAL patch, west "
                     ecalFill=3     " all sectors filled "

                  "beam-beam counter "
                     BBCM=on

                  "forward pion detector "
                     FPDM=on
                     FpdmConfig  = 3 "switch to a different lead glass source code"

                  "pseudo Vertex Position Detector"
                     VPDD=on;
                     VpddConfig=7;

                  "field version "
                     MfldConfig=4;      "tabulated field, with correction "

* important: (1) new SVT version (2) FTPC gas correction tp Ar+C02 mix (3) SSD ladders raddi correction

                     SupoConfig = 1; "FTPC Support"
                     SvttConfig = 6; "SVTT version"
                     SvshConfig = 3; "SVT shield"
                     DensConfig = 1; "gas density correction"
                     FtpcConfig = 1; "ftpc configuration"

                  "Photon Multiplicity Detector Version "
                     PHMD=on;
                     PhmdConfig = 2;

                  "Silicon Strip Detector Version "
                     SISD=on;
                     SisdConfig = 55; "fifth version, corrected radii, gaps, dead material"


                  "FTPC Readout barrel "
                     FTRO=on;
                     FtroConfig = 1;

                  "New version of the TPC backplane "
                     TpceConfig = 3;
                  "Muon Trigger System"
                     MUTD = on;
                     MutdConfig = 1;
                  "We need an even bigger Cave"
                     CaveConfig = 4;
                }

****************************************************************************************
****************************************************************************************
  on UPGR21    { Year UPGR20 + full tof;
                     nSvtLayer=6  " 3 bi-plane layers, nSvtLayer<=7 ";
                     nSvtVafer=0  " numbering is in the code   ";
                     svtWaferDim=0  " width is in the code      ";

                     ConeConfig=2 " new cable weight estimate ";

                  "ctb: central trigger barrer             ";
                     Itof=5 " call btofgeo5 ";
* NEW CONFIG!
                     BtofConfig=6;

* Full barrel in 2007
                  "CALB"
                     emsEdit=on ;
* important:
                     CalbConfig = 2
                     nmod={60,60}; shift={75,105}; " 60 sectors on both sides"
                  "ECAL"
                     EcalConfig=1   " one ECAL patch, west "
                     ecalFill=3     " all sectors filled "

                  "beam-beam counter "
                     BBCM=on

                  "forward pion detector "
                     FPDM=on
                     FpdmConfig  = 3 "switch to a different lead glass source code"

                  "pseudo Vertex Position Detector"
                     VPDD=on;
                     VpddConfig=7;

                  "field version "
                     MfldConfig=4;      "tabulated field, with correction "

* important: (1) new SVT version (2) FTPC gas correction tp Ar+C02 mix (3) SSD ladders raddi correction

                     SupoConfig = 1; "FTPC Support"
                     SvttConfig = 6; "SVTT version"
                     SvshConfig = 3; "SVT shield"
                     DensConfig = 1; "gas density correction"
                     FtpcConfig = 1; "ftpc configuration"

                  "Photon Multiplicity Detector Version "
                     PHMD=on;
                     PhmdConfig = 2;

                  "Silicon Strip Detector Version "
                     SISD=on;
                     SisdConfig = 55; "fifth version, corrected radii, gaps, dead material"


                  "FTPC Readout barrel "
                     FTRO=on;
                     FtroConfig = 1;

                  "New version of the TPC backplane "
                     TpceConfig = 3;
                  "Muon Trigger System"
                     MUTD = on;
                     MutdConfig = 2;
                  "We need an even bigger Cave"
                     CaveConfig = 4;
                }
****************************************************************************************
  on UPGR20    { Year 2007 + one TOF
                  "svt: 3 layers ";
                     nSvtLayer=6  " 3 bi-plane layers, nSvtLayer<=7 ";
                     nSvtVafer=0  " numbering is in the code   ";
                     svtWaferDim=0  " width is in the code      ";

                     ConeConfig=2 " new cable weight estimate ";

                  "ctb: central trigger barrer             ";
                     Itof=5 " call btofgeo5 ";
* NEW CONFIG!
                     BtofConfig=10;

* Full barrel in 2007
                  "CALB"
                     emsEdit=on ;
* important:
                     CalbConfig = 2
                     nmod={60,60}; shift={75,105}; " 60 sectors on both sides"
                  "ECAL"
                     EcalConfig=1   " one ECAL patch, west "
                     ecalFill=3     " all sectors filled "

                  "beam-beam counter "
                     BBCM=on

                  "forward pion detector "
                     FPDM=on
                     FpdmConfig  = 3 "switch to a different lead glass source code"

                  "pseudo Vertex Position Detector"
                     VPDD=on;
                     VpddConfig=7;

                  "field version "
                     MfldConfig=4;      "tabulated field, with correction "

* important: (1) new SVT version (2) FTPC gas correction tp Ar+C02 mix (3) SSD ladders raddi correction

                     SupoConfig = 1; "FTPC Support"
                     SvttConfig = 6; "SVTT version"
                     SvshConfig = 3; "SVT shield"
                     DensConfig = 1; "gas density correction"
                     FtpcConfig = 1; "ftpc configuration"

                  "Photon Multiplicity Detector Version "
                     PHMD=on;
                     PhmdConfig = 2;

                  "Silicon Strip Detector Version "
                     SISD=on;
                     SisdConfig = 55; "fifth version, corrected radii, gaps, dead material"


                  "FTPC Readout barrel "
                     FTRO=on;
                     FtroConfig = 1;

                  "New version of the TPC backplane "
                     TpceConfig = 3;
                  "Muon Trigger System"
                     MUTD = on;
                     MutdConfig = 2;
                  "We need an even bigger Cave"
                     CaveConfig = 4;
                }
****************************************************************************************
  on HADR_ON    { all Geant Physics On;                                       }
  on HADR_OFF   { all Geant Physics on, except for hadronic interactions;
                                                                       IHADR=0}
  on GCALOR_ON { setting hadr 6 to activate hadronic showers;
                              IHADR=6;}

  on PHYS_OFF   { No Physics: only energy loss;
      {IDCAY,IANNI,IBREM,ICOMP,IHADR,IMUNU,IPAIR,IPHOT,IDRAY,IMULS}=0; Iloss=2}
  on DECAY_ONLY { Some Physics: decays, mult.scat and energy loss;
                  {IANNI,IBREM,ICOMP,IHADR,IMUNU,IPAIR,IPHOT,IDRAY}=0; Iloss=2}
  on NO_BREM    { No bremmstrahlung;
                  IBREM=0;}

  on LOW_EM     { Low cuts on ElectroMagnetic processes;
                  CUTGAM=0.00001;
                  CUTELE=0.00001;
                  BCUTE =0.00001;
                  BCUTM =0.00001;
                  DCUTE =0.00001;
                  DCUTM =0.00001;
                }

  on TPC_ONLY   { Minimal geometry - only TPC;
                  {PIPE,SVTT,ftpc,BTOF,VPDD,CALB,ECAL,MAGP,UPST,ZCAL,PHMD,FPDM,BBCM,SISD,FTRO}=off; }
  on TPC_AND_SVTT  { Only TPC and SVT;
                  {PIPE,ftpc,BTOF,VPDD,CALB,ECAL,MAGP,UPST,ZCAL,PHMD,FPDM,BBCM,FTRO}=off; }
  on SVTT_ON    { Optional SVTT added on top of the minimal geo;
                     SVTT=on; }
  on SVTT_OFF   { Optionally remove the SVTT;
                     SVTT=off; }
  on SISD_OFF   { Optionally remove the SISD sisd;
                     SISD=off; }
  on ONLY_SVTT   { Only SVTT;
                  {PIPE,TPCE,ftpc,BTOF,VPDD,CALB,ECAL,MAGP,UPST,ZCAL,PHMD,FPDM,BBCM,FTRO}=off; }
*
  on PIPE_ON    { Optional PIPE added on top of the minimal geo;
                     PIPE=on; }
  on PIPE_OFF   { Pipe optionally removed;
                     PIPE=off; }
*
  on FTPC_ON    { Optional FTPC added on top of the minimal geo;
                     ftpc=on; }
  on BTOF_ON    { Optional BTOF added on top of the minimal geo;
                     BTOF=on; }
  on ECAL_ON    { Optional ECAL added on top of the minimal geo;
                     ECAL=on; }
  on CALB_ON    { Optional CALB added on top of the minimal geo;
                     CALB=on; }

  on SHIELD_OFF { Can switch the shield off in the DEV geom;
                     SHLD=off; }

  on PIXL_ON    { Optional PIXL added on top of the minimal geo;
                     PIXL=on; }

  on FIELD_ONLY { No geometry - only magnetic field;
      NtrSubEv=0;
      {CAVE,PIPE,SVTT,TPCE,ftpc,BTOF,VPDD,MAGP,CALB,ECAL,RICH,UPST,ZCAL}=off; }
  on FIELD_OFF  { no magnetic field;                
                  magField=0;                  }
  on FIELD_ON   { Standard (5 KGs) field on;        
                  magField=5;                  }
  on FIELD      { defined mag field;
                  magField=myArg; }

  on 4TH_OFF    { SVT fourth layer off;
		nSvtLayer=min(nSvtLayer,6);           }
  on SPLIT_OFF  { events will not be split into subevents;
		NtrSubEv=0;    }
  on SPLIT_ON   { events will be split into subevents;
           	NtrSubEv=1000; }
  on DEBUG_ON   { verbose mode, some graphics;
		Idebug=max(Idebug,1); Itest=1; }
  on DEBUG_OFF  { standard debug mode;
           	{Idebug,Itest}=0;              }
  } !// end of main moop

* sanity check - if something left in commands (unknown keyword), we stop!
  if (JL .le. JR) {
     print *,' Unknown command left => ', commands(JL:JR), ' ',JL,JR
     if (IPRIN==0) stop 'You better stop here to avoid problems'
  }
}


* -------------------- setup selected configuration ------------------------
* Now when all parameters and flags are ready, make gstar work as usually
* ie put a MODE or/and DETP command and executing them for selected systems.
*
* - to save secondaries AFTER all decays:      DETP TRAC DCAY 210 210 0.1 0.01
   dcay={210,210,0.1,0.01}
   If LL>0 { call AgDETP new ('Trac'); call AgDETP add ('TracDCAY',dcay,4) }


   write(*,*) '****** ATTENTION ACHTUNG ATTENZIONE VNIMANIE UVAGA WEI ******'
   write(*,*) '******* THESE FLAGS ARE USED TO GENERATE THE GEOMETRY *******'
   write(*,*) '                 BtofConfig: ',BtofConfig
   write(*,*) '                 CaveConfig: ',CaveConfig
   write(*,*) '                 CalbConfig: ',CalbConfig
   write(*,*) '                 ConeConfig: ',ConeConfig
   write(*,*) '                 DensConfig: ',DensConfig
   write(*,*) '                 FgtdConfig: ',FgtdConfig
   write(*,*) '                 FpdmConfig: ',FpdmConfig
   write(*,*) '                 FstdConfig: ',FstdConfig
   write(*,*) '                 FtpcConfig: ',FtpcConfig
   write(*,*) '                 FtroConfig: ',FtroConfig
   write(*,*) '                 HpdtConfig: ',HpdtConfig
   write(*,*) '                 IstbConfig: ',IstbConfig
   write(*,*) '                 MutdConfig: ',MutdConfig
   write(*,*) '                 GembConfig: ',GembConfig
   write(*,*) '                 PhmdConfig: ',PhmdConfig
   write(*,*) '                 PipeConfig: ',PipeConfig,'  PipeFlag: ',PipeFlag
   write(*,*) '                 PixlConfig: ',PixlConfig
   write(*,*) '                 SvshConfig: ',SvshConfig
   write(*,*) '                 SisdConfig: ',SisdConfig
   write(*,*) '                 SupoConfig: ',SupoConfig
   write(*,*) '                 SvttConfig: ',SvttConfig
   write(*,*) '                 TpceConfig: ',TpceConfig
   write(*,*) '                 VpddConfig: ',VpddConfig
   write(*,*) '***** FOR EXPERTS ONLY: LOOK UP GEOMETRY.G FOR DETAIL *******'


   if (RICH) ItCKOV = 1

   if (CAVE) then
      call AgDETP new ('CAVE')
      call AgDETP add ('CVCF.config=',CaveConfig,1)
      call cavegeo
   endif

* Pipe:
   If (PIPE)   {
     call AgDETP new ('PIPE')
     call AgDETP add ('pipv.PipeConfig=',PipeConfig,1);
     call AgDETP add ('pipv.PipeFlag=',PipeFlag,1);
     if (PipeConfig == -1) {Call pipegeo00;}
     else                  {Call pipegeo;  }
   }

* Upstream (DX), shield, and D0+Q1+Q2+Q3
   if (UPST)        Call upstgeo
   if (SHLD)        Call shldgeo
   if (QUAD)        Call quadgeo

* ---
   Call AGSFLAG('SIMU',2)

* - to switch off the fourth svt layer:        DETP SVTT SVTG.nlayer=6
   if (SCON) {
     call AgDETP new ('SCON')
     call AgDETP add ('svtg.ConeVer=',ConeConfig ,1) ! could have more copper on the cone
     call scongeo
   }

   If (SVTT) {
     call AgDETP new ('SVTT')
     if (nSvtLayer < 7)     call AgDETP add ('svtg.nlayer=',   nSvtLayer,1)
     if (nSvt1stLayer > 1)  call AgDETP add ('svtg.nmin=',     nSvt1stLayer,1)
     if (PipeConfig >= 4)   call AgDETP add ('svtg.ifMany=',     1,1)
     if (nSvtVafer > 0)     call AgDETP add ('svtl(3).nwafer=',nSvtVafer,1)
     if (svtWaferDim > 0)           call AgDETP add ('swca.WaferWid=', svtWaferDim,1)
     if (svtWaferDim > 0)           call AgDETP add ('swca.WaferLen=', svtWaferDim,1)
     if (.not.svtWater)     call AgDETP add ('swam.Len=',       0, 1)

* Take care of the correction level and call the appropriate constructor:

*   This applies to the newer versions of the svt code:
*   we can now switch to a better description of the cone
*   material (copper cables) thanks to a new measurement by
*   Dave Lynn

    call AgDETP add ('svtg.ConeVer=',ConeConfig ,1) ! could have more copper on the cone

* Optionally, switch to a larger inner shield, AND smaller beampipe support
* Or, pick a shield that is slighly bigger outside according to Lilian observation
    if(SvshConfig>0) call AgDETP add ('svtg.SupportVer=',SvshConfig ,1)

* Ugly, but I Do not want to hash function pointers in Fortran:

    if(SvttConfig==0)  call svttgeo
    if(SvttConfig==1)  call svttgeo1
    if(SvttConfig==2)  call svttgeo2
    if(SvttConfig==3)  call svttgeo3
    if(SvttConfig==4)  call svttgeo4
    if(SvttConfig==5)  call svttgeo5
    if(SvttConfig==6)  call svttgeo6
    if(SvttConfig==7)  call svttgeo7
    if(SvttConfig==9)  call svttgeo9
    if(SvttConfig==10) call svttgeo10
    if(SvttConfig==11) call svttgeo11

  }!!end SVTT

* Set the proper configuration of the Silicon Strip Detector
* See note on top about using MOD(10) to encode the geometry
* cut, as opposed to configuration of the detector:

  if(SISD) {
       sisd_level=0
       call AgDETP new ('SISD')

* if SVT is present, position the SSD in it, otherwise need to position in CAVE (default)
       if(SVTT) { call AgDETP add ('ssdp.Placement=',1 ,1) };

* In the following, level means the version of the ssd geo code to be loaded
* It is the most important decimal place of the SisdConfig, and we just check
* for it here:

       if (SisdConfig>10) then
         sisd_level=SisdConfig/10
         SisdConfig=SisdConfig-sisd_level*10

         call AgDETP add ('ssdp.Config=',SisdConfig ,1)
         if     (sisd_level.eq.1) then
            call sisdgeo1
         elseif (sisd_level.eq.2) then
            call sisdgeo2
         elseif (sisd_level.eq.3) then
            call sisdgeo3
         elseif (sisd_level.eq.4) then
            call sisdgeo4
         elseif (sisd_level.eq.5) then
            call sisdgeo5
         elseif (sisd_level.eq.6) then
            call sisdgeo6
         else ! Unimplemented level
            write(*,*) '******************* ERROR IN PARSING THE SSD GEOMETRY LEVEL! ******************'
            if (IPRIN==0) stop 'You better stop here to avoid problems'
         endif

       else
*        The original version (pretty much obsolete)
         call AgDETP add ('ssdp.Config=',SisdConfig ,1)
         call sisdgeo
       endif

*       write(*,*) '*** Silicon Strip Detector Config and Code Level: ',SisdConfig, ' ',sisd_level

  } !!end SISD



* Back in July 2003 Yuri has discovered the discrepancy
* in the gas density. The patch for this is activated here: (was: if(CorrNum>=3) )



   if (TPCE)  {write(*,*) 'TPC';
* Back in July 2003 Yuri has discovered the discrepancy
* in the gas density. The patch for this is activated here: (was: if(CorrNum>=3) )
     if(DensConfig>0) { call AgDETP new('TPCE');  call AgDETP add ('tpcg.gasCorr=',2 ,1);}
     if (TpceConfig==1) Call tpcegeo
     if (TpceConfig==2) Call tpcegeo1
     if (TpceConfig==3) Call tpcegeo2
     if (TpceConfig==4) Call tpcegeo3
   }
   write(*,*) 'FTPC'
   if (ftpc) then
        if(FtpcConfig==0) Call ftpcgeo
        if(FtpcConfig==1) Call ftpcgeo1
*       and look at the support pieces, was: if(CorrNum==0)
        if(SupoConfig==0)  Call supogeo
        if(SupoConfig==1)  Call supogeo1
   endif

* FTPC readout electronics barrel
   if (FTRO) Call ftrogeo

   write(*,*) 'BTOF'
* - tof system should be on (for year 2):      DETP BTOF BTOG.choice=2
   If (BTOF) { 
     call AgDETP new ('BTOF')
     call AgDETP add ('btog.choice=',BtofConfig,1)
* X.Dong
     if(Itof>5) {
         call AgDETP add ('btog.X0=',tofX0,1)
         call AgDETP add ('btog.Z0=',tofZ0,1)
     }
* X.Dong.end

      if(Itof.eq.1) write(*,*) '*****  ATTENTION : OLD VERSION OF BTOF NOT IMPLEMENTED - NO TOF CREATED *****'
      if(Itof.eq.2) call btofgeo2
      if(Itof.eq.4) call btofgeo4
      if(Itof.eq.5) call btofgeo5
      if(Itof.eq.6) call btofgeo6       !X.Dong
   }

   Call AGSFLAG('SIMU',1)


********************* Vertex Position Detector *******************
   If (LL>0 & VPDD) then
     call AgDETP new ('VPDD')
     call AgDETP add ('vpdv.vpdConfig=',VpddConfig,1);
     if(VpddConfig <7) call vpddgeo
     if(VpddConfig==7) call vpddgeo2
   endif

********************** BARREL CALORIMETER ************************
*  - Set up the parameters for the barrel calorimeter
   If (CALB) {
     call AgDETP new ('CALB')
     if (emsEdit)  call AgDETP add ('calg.nmodule=',Nmod, 2)
     if (emsEdit)  call AgDETP add ('calg.shift=',  shift,2)

       if(CalbConfig==0) then
           write(*,*) '************** Creating the 1996-2003 version of the Barrel Calorimeter'
           Call calbgeo
       endif

       if(CalbConfig==1) then
           write(*,*) '************** Creating the 2004-2006 version of the Barrel Calorimeter'
           Call calbgeo1
       endif

       if(CalbConfig==2) then
           write(*,*) '************** Creating the 2007-     version of the Barrel Calorimeter'
           Call calbgeo2
       endif

   }
******************************************************************
*  - Set up the parameters for the RICH counter
   if (LL>0 & RICH) then
      call AgDETP new ('Rich')
      if (richConfig>0) call AgDETP add ('Rich.Version=', richConfig,1)
      if (richPos>0) call AgDETP add ('Rich.Position=',richPos,1)
      if (richPos>0) call AgDETP add ('Rich.Cversion=',richPos,1)
   endif
   if (RICH) Call richgeo

******************************************************************
*  - Set up the parameters for the endcap calorimeter
   If (ECAL) then
      call AgDETP new ('ECAL')
      call AgDETP add ('emcg.OnOff='   ,EcalConfig,1)
      call AgDETP add ('emcg.FillMode=',ecalFill,1)
      Call ecalgeo
   endif

******************************************************************
* The rest of steering:

   if (BBCM)                   Call bbcmgeo

   if (FPDM.and.FpdmConfig==0) Call fpdmgeo
   if (FPDM.and.FpdmConfig==1) Call fpdmgeo1
   if (FPDM.and.FpdmConfig==2) Call fpdmgeo2
   if (FPDM.and.FpdmConfig==3) Call fpdmgeo3

   if (ZCAL)                   Call zcalgeo
   if (MAGP)                   Call magpgeo

   if (MUTD.and.MutdConfig==1) Call mutdgeo
   if (MUTD.and.MutdConfig==2) Call mutdgeo2
   if (MUTD.and.MutdConfig==3) Call mutdgeo3

   if (PIXL.and.PixlConfig==-1)Call pixlgeo00
   if (PIXL.and.PixlConfig==1) Call pixlgeo
   if (PIXL.and.PixlConfig==2) Call pixlgeo1
   if (PIXL.and.PixlConfig==3) Call pixlgeo2
   if (PIXL.and.PixlConfig==4) Call pixlgeo3
   if (PIXL.and.PixlConfig==5) then
         call AgDETP new ('PIXL')
         call AgDETP add ('PXLV.LadVer=',2.0,1)
         call pixlgeo3
   endif

   if (ISTB.and.IstbConfig==-1)Call istbgeo00
   if (ISTB.and.IstbConfig==1) Call istbgeo
   if (ISTB.and.IstbConfig==2) Call istbgeo1
   if (ISTB.and.IstbConfig==3) Call istbgeo2
   if (ISTB.and.IstbConfig==4) Call istbgeo3
   if (ISTB.and.IstbConfig==5) Call istbgeo4
   if (ISTB.and.IstbConfig==6) Call istbgeo5
   if (ISTB.and.IstbConfig==7) Call istbgeo6


   if (GEMB.and.GembConfig>0)  Call gembgeo

   if (FSTD.and.FstdConfig>0)  then
      if(FstdConfig==2) then
         call AgDETP new ('FSTD')
         call AgDETP add ('fstg.Rmax=',22.3,1)
      endif
        Call fstdgeo
   endif

   if (FGTD) then
     if (FgtdConfig==1)    Call fgtdgeo  ! old, decomissioned
     if (FgtdConfig==2)    Call fgtdgeo1
     if (FgtdConfig==3)    Call fgtdgeo2
   endif

   if (IGTD) then
       if(IgtdConfig==2) then
           call AgDETP new ('IGTD')
           call AgDETP add ('igtv.Config=',IgtdConfig ,1)
       endif
       Call igtdgeo
   endif

   if (HPDT.and.HpdtConfig>0)  Call hpdtgeo

   if (ITSP)                   Call itspgeo
******************************************************************
* If PHMD is present and a non-zero version of the Photon Multiplicity Detector
* is defined, pass the version number to its constructor
* and create it:

   if  (PHMD.and.PhmdConfig>0) then
      call AgDETP new ('PHMD')
      call AgDETP add ('PMVR.Config=', PhmdConfig,1)
      call phmdgeo
   endif


********************************************************************
   if(DUMM) then
      call dummgeo
   endif
****************  Magnetic Field  ********************************
*
* - reset magnetic field value (default is 5): DETP MFLD MFLG.Bfield=5
   If (MFLD) {
!//       call AgDETP new ('MFLD')
!//       if (MFLD & magField!=5)   call AgDETP add ('MFLG(1).Bfield=' ,magField  ,1)
!//       if (MFLD & MfldConfig!=0) call AgDETP add ('MFLG(1).version=',MfldConfig,1)
!//       Call mfldgeo;
      Call mfldgeo(magField);
      call gufld(magX,magB);
      write(*,*) 'MFLD magField,Bz = ',magField,magB(3)

   }
*
   if JVOLUM>0
   { Call ggclos
     If IDEBUG>0 { CALL ICLRWK(0,1); Call GDRAWC('CAVE',1,.2,10.,10.,.03,.03)}
   }
   IDEBUG = IPRIN
   ITEST  = min(IPRIN,1)
   Call agphysi
*                      automatic subevent size selection
   If NtrSubev > 0
   { Call MZNEED(IXDIV,1000,'G')
     NLEFT    = max(10,IQUEST(11)/1200)
     MLEFT    = 10**Int(Alog10(Float(Nleft))-1)
     NtrSubEv = MLEFT*(NLEFT/MLEFT)
     Prin1 NtrSubEv; (' Ntrack per subevent = ',i6)
   }
   end
