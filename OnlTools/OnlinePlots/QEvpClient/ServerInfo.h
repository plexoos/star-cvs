#ifndef ServerInfo_h
#define ServerInfo_h


#include "ui_ServerInfoUi.h"
typedef Ui_ServerInfoUi ServerInfoUi;

#include <time.h>

class ServerInfo : public QFrame,  public ServerInfoUi {

 public:
  ServerInfo( QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = 0 ) 
       : QFrame(parent, fl) {if (name) setName(name); setupUi(this); }
  public slots:
    void setRequestTime(time_t t) { request->setText( toStr(t) );    }
    void setReceiveTime(time_t t) { receive->setText( toStr(t) );    }
    void setRequestType(int t)    { requestType->setText( itoa(t) ); }
    void setReceiveType(int t)    { receiveType->setText( itoa(t) ); }

 private:
    char txt[1024];
    char* toStr(time_t t) { 
      time_t tt = t;
      tm* bla = localtime(&tt);
      strftime(txt, 10,"%H:%M:%S",bla);
      return txt; 
    }
    char* itoa(int t) { sprintf(txt,"%d",t); return txt; }

};


#endif
  
