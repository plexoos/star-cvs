//StiDisplayManager.h
//M.L. Miller (Yale Software)
//04/01

//This  class manages the Sti display

#ifndef StiDisplayManager_HH
#define StiDisplayManager_HH

#include <string>
#include <map>

using std::map;
using std::string;

class TCanvas;
class TShape;
class TVolume;
class StiDrawable;

class StiDisplayManager
{
public:
    friend class nobody;
    
    typedef multimap<string, StiDrawable*> stidrawablemap;
    typedef stidrawablemap::value_type stiDrawableMapValType;
    
    //Singleton access
    static StiDisplayManager* instance();
    static void kill();

    //Action
    void addDrawable(StiDrawable*);
    //clean up after each event (e.g., remove tracks)
    void reset();
    
    void cd();
    void update();
    void draw();
    //void draw(StiDrawable*); //Update just this object

    void setInvisible(); //All
    void setInvisible(const StiDrawable*);
    void setVisible(); //All
    void setVisible(const StiDrawable*);
    void setSkeletonView();

    void setTpcVisible();
    void setTpcInvisible();
    void setSvtVisible();
    void setSvtInvisible();
    void setIfcInvisible();

    //Utility
    void print() const;

protected:
    
private:

    static StiDisplayManager* sinstance;

    //dx, dy, and dz define the volume within which objects are drawn.
    //The volume is a rectangle of lengths 2*dx, 2*dy, 2*dz
    enum StiCanvasSize {kXmin=200, kXmax=600, kYmin=100, kYmax=500};
    enum StiMainVolumeSize {kdx=200, kdy=200, kdz=240};
    
    StiDisplayManager();
    virtual ~StiDisplayManager();
    
    void setup();

    stidrawablemap mmap;
    
    TCanvas* mcanvas;
    TShape* mzone;
    TVolume* mnode;

};

#endif
