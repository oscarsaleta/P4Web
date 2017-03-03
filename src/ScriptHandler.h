#include "file_tab.h"

#ifdef ANTZ
    #define MAPLE_PATH "/usr/share/maple11/bin/maple"
    #define P4_BINDIR "/home/p4/p4/bin/"
    #define TMP_DIR "/home/p4/tmp/"
#else
    #define MAPLE_PATH "/home/osr/maple2015/bin/maple"
    #define P4_BINDIR "/usr/local/p4/bin/"
    #define TMP_DIR "/tmp/"
#endif

class ScriptHandler
{
public:
    ScriptHandler(); //TODO ???
    ~ScriptHandler();
    
    void setBaseFileName(std::string fname)
    {
        baseFileName_ = fname;
    }

    std::string openTempStream(std::string prefix, std::string suffix);
    void prepareMapleFile(); // TODO: maybe return something
    void fillMapleScript(std::ofstream& f); //TODO: com passar totes les variables? struct?
    void evaluateMapleScript(); //NOTE: això ha de funcionar amb qualsevol maple script.
    void prepareSaveFile(); //TODO: mirar inputs i outputs necessaris

    // TODO copiar codi de file_vf.cpp i adaptar
    bool prepareGcf( P4POLYNOM2 f, double, double, int, int );
    bool prepareGcf_LyapunovCyl( double, double, int, int );
    bool prepareGcf_LyapunovR2( int precision, int numpoints);

private:
    std::string baseFileName_; //TODO: treure això i usar fname com arguemnt de totes les funcions?
};