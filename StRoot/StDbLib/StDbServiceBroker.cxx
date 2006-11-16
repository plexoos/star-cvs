#include "StDbServiceBroker.h"
#include "ChapiStringUtilities.h"
#include "mysql.h"

using namespace std;
using namespace chapi_string_utilities;

typedef vector<string>::const_iterator VCI;

using st_db_service_broker::MyScatalogVersion; 
using st_db_service_broker::NO_USER;
using st_db_service_broker::NO_DOMAIN;
using st_db_service_broker::NightBegins;  
using st_db_service_broker::NightEnds;  

using stl_xml_tree::sep;

string nn[] = 
  {
    "SCATALOG",
    "SITE",
    "SERVER",
    "HOST",
    "ACCESS"
  };

enum 
  {
    SCATALOG,
    SITE,
    SERVER,
    HOST,
    ACCESS
  };

string NAME = "NAME";
string DBNAME = "DBNAME";
string PORT = "PORT";
string USER = "USER";
string WHEN_ACTIVE = "WHEN_ACTIVE";
static MYSQL *conn;

char* Socket = 0;


static std::map<std::string, std::string> DomainRules;  
static bool InitDomainRules()
{
  // rules to select a recognized XML attribute value based on $DOMAINNAME env. variable.

  DomainRules["nersc.gov"] = "LBL";
  DomainRules["rhic.bnl.gov"] = "BNL";

  // any other sites here ?

  return true;
}
static bool rules_init_dummy = InitDomainRules();


//////////////////////////////////////////////////////
StDbServiceBroker::StDbServiceBroker(const string xmlbase) : 
  MyHostList(vector<ChapiDbHost>()),
  MyStatus(st_db_service_broker::NO_ERROR)
{
  char* whoami = getenv("USER");
  if (!whoami) whoami = getenv("LOGNAME");
  if (!whoami) 
    {
      MyStatus = NO_USER;
      return;  
    }
  char* whereami = getenv("DOMAINNAME");

  if (!whereami)
    {
      MyStatus = NO_DOMAIN;
      return;
    }

  string site = DomainRules[whereami];
  if (site=="")
    {
      MyStatus = NO_DOMAIN;
      return;
    }

  StlXmlTree* f = new StlXmlTree();
  string ScatalogKey = sep+nn[SCATALOG];
  f->InsertKeyValuePair(ScatalogKey, MyScatalogVersion);

  string QualifiedScatalog = sep + StlXmlTree::QualifyParent(nn[SCATALOG],MyScatalogVersion);
  string SiteKey = QualifiedScatalog + sep + nn[SITE];
  f->InsertKeyValuePair(SiteKey, "NAME="+site+";");

  string QualifiedSite = StlXmlTree::QualifyParent(SiteKey,"NAME="+site+";");
  string ServerKey = QualifiedSite + sep + nn[SERVER];

  struct tm *tp;  
  time_t timeNow;
  timeNow = time(NULL);
  tp = localtime(&timeNow);
  short hour = tp->tm_hour;
  string DayOrNight;
  if (hour<NightBegins && hour >NightEnds) 
    {
      DayOrNight = "day";
    }
  else
    {
      DayOrNight = "night";
    }

  f->InsertKeyValuePair(ServerKey, USER+"="+(string)whoami+";"+WHEN_ACTIVE+"="+DayOrNight+";");

#ifdef DEBUG
  f->ShowTree();
#endif

  ParsedXml = StlXmlTree(xmlbase,f);
  if (ParsedXml.GetStatus()==stl_xml_tree::NO_XML_BASE)
    {
      MyStatus = st_db_service_broker::NO_XML_BASE;
      cerr <<"StDbServiceBroker::StDbServiceBroker: no XML description of services found \n";
    }

#ifdef DEBUG
  ParsedXml.ShowTree();
#endif

  xmlCleanupParser();
  delete f;
}
//////////////////////////////////////////////////////
StDbServiceBroker::StDbServiceBroker
(const string xmlbase, const string xmlfilter) : 
  MyHostList(vector<ChapiDbHost>()),
  MyStatus(st_db_service_broker::NO_ERROR)
{
  StlXmlTree* f = new StlXmlTree(xmlfilter);
  f->ShowTree();
  ParsedXml = StlXmlTree(xmlbase,f);
  ParsedXml.ShowTree();
  xmlCleanupParser();
  delete f;
}
//////////////////////////////////////////////////////
void StDbServiceBroker::DoLoadBalancing()
{
  FormHostList();
#ifdef DEBUG
  PrintHostList();
#endif
  RecommendHost();
#ifdef DEBUG
  cout << " go to "<<GiveHostName()<<" port "<<GiveHostPort()<<"\n";
#endif
}
//////////////////////////////////////////////////////
void StDbServiceBroker::FormHostList()
{
  /*
We expect:
-- a single SCATALOG version
-- possibly access to multiple sites
-- obviously multiple hosts
  */
  
  MyHostList.clear();
  string key = StlXmlTree::MakeKey("",nn[SCATALOG]);
  vector<string> versions = ParsedXml.LookUpValueByKey(key);
  if (versions.size()!=1) return;
  string my_version = versions[0];

  vector<string> sites = ParsedXml.LookUpValueByKey(key,my_version,nn[SITE]);
  
  for (VCI i = sites.begin(); i!=sites.end(); ++i)
    {
      vector<string> services = ParsedXml.LookUpValueByKey(key,*i,nn[SERVER]);
      
      for (VCI ii = services.begin(); ii!=services.end(); ++ii)
	{
	  vector<string> hosts = ParsedXml.LookUpValueByKey(key,*ii,nn[HOST]);
	  
	  for (VCI iii = hosts.begin(); iii!=hosts.end(); ++iii)
	    {
	      map<string,string> host_data = StlXmlTree::ParseAttributeString(*iii);

	      int port;
	      if (!from_string<int>(port,host_data[PORT],std::dec))
	      {
		// error: non-numeric port string
		port = DefaultPort;
	      }
	      ChapiDbHost host_entry = ChapiDbHost(host_data[NAME],port);
	      MyHostList.push_back(host_entry);
	    }
	  cut_string_after_sub(key,">");
	  cut_string_after_sub(key,"(");
	}
	  cut_string_after_sub(key,">");
	  cut_string_after_sub(key,"(");
    }
}
//////////////////////////////////////////////////////
void StDbServiceBroker::PrintHostList()
{
  cout << " MyHostList contains:\n";
  for (vector<ChapiDbHost>::const_iterator i = MyHostList.begin(); i!=MyHostList.end(); ++i)
    {
      cout << (*i).HostName << "\n";
    }
}
//////////////////////////////////////////////////////
void StDbServiceBroker::RecommendHost()
{
  unsigned long nproc_min = ULONG_MAX;
  if (MyHostList.size()==0)
    {
      cerr<<" StDbServiceBroker::RecommendHost() has no hosts to choose from !\n";
      return;
    }
  for (vector<ChapiDbHost>::const_iterator I=MyHostList.begin(); I!=MyHostList.end(); ++I)
    {
      conn = mysql_init(0);

      if (conn==0)
        {
          cerr << "StDbServiceBroker::RecommendHost() mysql_init(0) failed \n";
          continue;
        }

      if (mysql_real_connect
	  (conn,((*I).HostName).c_str(), "loadbalancer","lbdb","test",(*I).Port,Socket,0)==NULL)
        {
          cerr << "StDbServiceBroker::RecommendHost() mysql_real_connect "<< 
	    conn << " "<<((*I).HostName).c_str()<<" "<<DefaultPort<<" failed\n";
          mysql_close(conn);
          continue;
        }

      if (mysql_query(conn, "show processlist") != 0 )
        {
          cerr <<"StDbServiceBroker::RecommendHost() show processlist failed\n";
          continue;
        }

      MYSQL_RES *res_set = mysql_store_result(conn);
      unsigned long nproc = mysql_num_rows(res_set);
#ifdef DEBUG
       cout <<" Server "<<((*I).HostName).c_str()<< " "<< nproc << " processes \n";
#endif
      mysql_close(conn);

      if (nproc<nproc_min)
        {
          nproc_min = nproc;
	  MyBestHost = I;
        }
    }
}
////////////////////////////////////////////////////////////////
string StDbServiceBroker::GiveHostName()
{
  return (*MyBestHost).HostName;
}
////////////////////////////////////////////////////////////////
short StDbServiceBroker::GiveHostPort()
{
  return (*MyBestHost).Port;
}
////////////////////////////////////////////////////////////////
