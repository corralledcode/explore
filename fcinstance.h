//
// Created by peterglenn on 12/21/25.
//

#ifndef EXPLORE_FCINSTANCE_H
#define EXPLORE_FCINSTANCE_H
#include <string>
#include <vector>
#include "config.h"
#include "querytab.h"
#include "SyntaxHighlighter.h"
#include "ui_mainwindow.h"

struct randomizer {
    std::string name;
    LONGINT dim;
    bool p;
    float edgecnt;
    LONGINT cnt;
};

std::vector<std::string> parsesemicolondelimeted(const std::string& input, char delimiter);
void removestring( std::vector<std::string>& parameters, const std::string& str );
void addstring( std::vector<std::string>& parameters, const std::string& str );
std::string tosemicolondelimeted( const std::vector<std::string>& parameters );

class fcinstance {
public:
    std::vector<std::string> storedprocedurefilenames {};
    std::vector<std::string> pythonfilenames {};
    std::string outfilename;
    bool outoverwrite;
    bool outpassed; // if wished, set default check state in Qt
    std::string outputfilename;
    std::vector<std::string> graphfilenames {};
    std::vector<std::string> inlinegraphs {};

    int threadcount = 0;
    bool setthreadcount = false;

    bool fingerprint = false;
    bool isomorphisms = false;
    bool onlysortedisomorphisms = true;

    std::vector<std::string> verbositylevels {};
    std::vector<std::string> verbosityfilenames {};


    bool randomizersbool = false;
    std::vector<randomizer> randomizers {};

    std::vector<std::string> queries {};
    measuretype mt;

    std::string parse();
    void reverseparse(const std::string&);
};

class fcinstanceQtbridge {
    public:
    int tabtoduplicate = 0;
    fcinstance fc;
    Ui::MainWindow *ui;
    QTabWidget* tab;

    int visibleindex = 0;

    std::vector<MyHighlighter*> highlighters {};

    fcinstanceQtbridge( Ui::MainWindow* uiin, QueryTabWidget* tab );
    ~fcinstanceQtbridge();

    void passparameterstowidgets();

    void addstoredprocedurefiles( const std::string& filename );
    void removestoredprocedurefiles( const std::string& filename );
    void addpythonfiles( const std::string& filename );
    void removepythonfiles( const std::string& filename );
    void addgraphfiles( const std::string& filename );
    void removegraphfiles( const std::string& filename );
    void addinlinegraphs( const std::string& filename );
    void removeinlinegraphs( const std::string& filename );
    void setquery( const std::string& query );
    void setcr1( const std::string& cr1 );
    void setcr2( const std::string& cr2 );
    void setcr3( const std::string& cr3 );
    void setrandomizer( const randomizer& r );
    void clearrandomizer();
    void addverbosity( const std::string& name );
    void removeverbosity( const std::string& name );
    void addverbosityfile( const std::string& filename );
    void removeverbosityfile( const std::string& filename );
    void setquerytype( const std::string& mt );
    void populatefcparameters();
    void setoutfile( const std::string& filename );
    int runQuerypostpopulate();
    int logQuerypostpopulate();
    int runQuery();
    int logQuery();
    int runandlogQuery();


};



#endif //EXPLORE_FCINSTANCE_H