//
// Created by peterglenn on 12/21/25.
//

#include "fcinstance.h"

#include <filesystem>
#include <iostream>
#include <QProcess>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

inline bool is_number(const std::string& s)
{
    if (!s.empty() && s[0] == '-')
        return (is_number(s.substr(1,s.size()-1)));
    return !s.empty() && std::find_if(s.begin(),
        s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}

inline bool is_real(const std::string& s)
{
    char* end = nullptr;
    double val = strtod(s.c_str(), &end);
    return end != s.c_str() && *end == '\0' && val != HUGE_VAL;
}


std::string stripspaces( const std::string& in ) {
    std::string res = in;
    while (res[0] == ' ')
        res = res.substr(1,res.length()-1);
    while (res[res.length()-1] == ' ')
        res = res.substr(0,res.length()-1);
    return res;
}

std::vector<std::string> parsesemicolondelimeted(const std::string& input, char delimiter) {
    std::vector<std::string> result {};
    std::stringstream ss(input);
    std::string token;

    // Use std::getline to extract tokens separated by the specified delimiter
    while (std::getline(ss, token, delimiter)) {
        token = stripspaces(token);
        result.push_back(token);
    }


    return result;
}



std::string fcinstance::parse()
{
    std::string out {};

    // -j

    if (setthreadcount)
        out.append( "-j " + std::to_string(threadcount) + " ");

    // -r

    if (randomizersbool)
        for (auto r : randomizers) {
            if (r.p)
                out.append( "-r " + std::to_string(r.dim)
                    + " p=" + std::to_string(r.edgecnt)
                        + " " + std::to_string(r.cnt) + " " + r.name + " " );
            else
                out.append( "-r " + std::to_string(r.dim) + " "
                    + std::to_string(r.edgecnt)
                        + " " + std::to_string(r.cnt) + " " + r.name + " ");
        }

    // -d

    for (auto filename : graphfilenames)
        out.append("-d \"" + filename + "\" ");

    if (inlinegraphs.size() > 0) {
        out.append("-d ");
        for (auto inlinegraph : inlinegraphs)
            out.append("f=\"" + inlinegraph + "\" ");
    }

    // -f

    if (fingerprint)
        out.append( "-f all ");

    // -i

    if (isomorphisms)
        if (onlysortedisomorphisms)
            out.append( "-i sorted ");
        else
            out.append( "-i all ");

    // -a

    if (queries.size() > 0) {
        out.append( "-a ");
        for (auto sp : storedprocedurefilenames)
            out.append("isp=\"" + sp + "\" ");
        if (!pythonfilenames.empty()) {
            out.append(" j=1"); // one thread for Python's GIL to work
            for (auto py : pythonfilenames)
                out.append("ipy=\"" + py + "\" ");
        }
        for (int i = 0; i < queries.size()-1; i++) {
            out.append( "s" + std::to_string(i+1) + "=\"" + queries[i] + "\" " );
        }
        switch (mt) {
            case mtbool: out.append("s"); break;
            case mtdiscrete: out.append("z"); break;
            case mtcontinuous: out.append("a"); break;
            case mtset: out.append("e"); break;
            case mttuple: out.append("p"); break;
            case mtgraph: out.append("gm"); break;
            default: std::cout << "Unknown measuretype\n";
        }
        if (queries.size() > 1) {
            out.append( std::to_string(queries.size()+1));
        }
        out.append( "=\"" + queries[queries.size()-1] + "\" all ");
    }

    // -g

    if (outfilename.size() > 0) {
        out.append( "-g o=\"" + outfilename + "\" ");
        if (outoverwrite)
            out.append( "overwrite ");
        if (outpassed)
            out.append( "passed ");
    }

    // -v

    if (verbosityfilenames.size() > 0 || verbositylevels.size() > 0) {
        out.append("-v ");
        for (auto v : verbosityfilenames)
            out.append("i=\"" + v + "\" ");
        for (auto v : verbositylevels)
            out.append(v + " ");

    }

    return out;
}

void fcinstance::reverseparse(const std::string& command) {

    std::vector<std::string> rgs {};
    rgs.push_back("r1");
    rgs.push_back("r2");
    rgs.push_back("r3");
    rgs.push_back("r4");
    rgs.push_back("r5");

    std::string comment {};

    auto args = parse_quoted_string(command);
    auto args2 = cmdlineparseiterationtwo(args);
    // for (auto a : args2)
        // std::cout << a.first << " === " <<a.second << " ; ";
    // std::cout << std::endl;

    int n = 0;
    while (n < args2.size()) {
        auto p = args2[n];
        if (p.first == "default") {
            if (p.second == "-d" && n+1 < args2.size()) {
                p = args2[++n];
                while (n < args2.size() && p.second.size() > 0 && !(p.first == "default" && p.second[0] == '-')) {
                    if (p.first == "f")
                        inlinegraphs.push_back(p.second);
                    if (p.first == "default")
                        graphfilenames.push_back(p.second);
                    p = args2[++n];
                }
                if (!(p.first == "default" && !p.second.empty() && p.second[0] == '-')) {
                    --n;
                    p = args2[n];
                } else {

                    int filenameidx = n;
                    bool oncethrough = true;
                    while ((args.size() <= n+1 && oncethrough) || filenameidx < args.size()-1) {
                        if ((!p.second.empty() && p.second[0] == '-') || args[filenameidx][0] == '-') {
                            --n;
                            break;
                        }
                        if (args2.size() > filenameidx)
                            if (args2[filenameidx].first == "f") {
                                filenameidx++;
                                continue;
                            }
                        filenameidx++;
                        oncethrough = false;

                        if (args[filenameidx][0] == '-')
                            break;
                        if ((args.size() > filenameidx) && (args[filenameidx] != "std::cin")) {
                            std::string filename = args[filenameidx];
                            graphfilenames.push_back(filename);
                        } else {
                            std::cout << "Using std::cin for input\n"; // recode so this is possible
                        }
                        p = args2[++n];

                    }
                }
            } else if (p.second == "-r" && n+1 < args2.size()) {
                int dim = 5;
                int rgsidx = 0;
                bool inlinep = false;
                ++n;
                std::vector<std::string> rgparams {};
                if (args2.size() - n >= 1 && args2[n].first == "default" && is_number(args2[n].second)) {
                    dim = std::stoi(args2[n].second);
                }
                double edgecnt = dim*(dim-1)/4.0;

                if (args2.size() - n >= 2 && args2[n+1].first == "default" && is_real(args2[n+1].second)) {
                    edgecnt = std::stof(args2[n+1].second);
                }
                long int cnt = 100; // the default count when count is omitted
                if (args2.size() - n >= 3 && args2[n+2].first == "default" && is_number(args2[n+2].second)) {
                    cnt = std::stoi(args2[n+2].second);
                }

                int i;
                for (i = n; i < args2.size(); ++i) {
                    if (args2[i].first == "default" && !args2[i].second.empty() && args2[i].second[0] == '-')
                        break;
                    if (args2[i].first == "default" || args2[i].first == "r") {
                        std::vector<std::pair<std::string,std::vector<std::string>>> args22 = cmdlineparseiterationthree(args2[i].second);
                        for (int k = 0; k < args22.size(); ++k) {
                            for (int l = 0; l < rgs.size(); ++l) {
                                if (args22[k].first == rgs[l]) {
                                    rgsidx = l;
                                    rgparams = args22[k].second;
                                    //for (int k = 0; k < rgparams.size(); ++k)
                                    //    std::cout << "rgparam " << rgparams[k] << ", ";
                                    //std::cout << "\n";

                                }
                            }
                        }
                    }
                }

                if (!rgparams.empty() && is_number(rgparams[0]))
                    dim = stoi(rgparams[0]);
                if (rgparams.size() > 1) // what is function to check if double
                    edgecnt = std::stof(rgparams[1]);
                if (rgparams.size() > 2 && is_number(rgparams[2]))
                    cnt = stoi(rgparams[2]);
                for (auto p : args2) {
                    if (p.first == "p") {
                        double f = std::stof(p.second);
                        edgecnt = 0.5;
                        if (f >= 0 && f <= 1)
                            // edgecnt = (long int)(std::stof(p.second) * (dim*(dim-1)/2));
                            edgecnt = f;
                        inlinep = true;
                    }

                }
                if (rgparams.empty()) {
                    rgparams.clear();
                    rgparams.resize(3);
                    rgparams[0] = std::to_string(dim);
                    rgparams[1] = std::to_string(edgecnt);
                    rgparams[2] = std::to_string(cnt);
                }
                // rgs[rgsidx]->setparams(rgparams);

                randomizer r;
                r.cnt = cnt;
                r.edgecnt = edgecnt;
                r.p = inlinep;
                r.dim = dim;
                r.name = rgs[rgsidx];
                randomizers.push_back(r);
            } else if (p.second == "-a" && n+1 < args2.size()) {

                n++;
                queries.resize(GLOBALCRITCOUNT+1);
                while (n < args2.size() && !(args2[n].first == "default" && args2[n].second.size() > 0 && args2[n].second[0] == '-' )) {
                    int p = 0;
                    if (args2[n].first != "default")
                        if (args2[n].first.size() > 1)
                            try {
                                p = stoi(args2[n].first.substr(1,args2[n].second.length())) - 1;
                            } catch (const std::invalid_argument& e) {
                                p = 0;
                            }

                    if (args2[n].first == "z") {
                        mt = mtdiscrete;
                        queries[p] = (args2[n].second);
                    } else if (args2[n].first[0] == 'a') {
                            mt = mtcontinuous;
                        queries[p] = (args2[n].second);
                        } else if (args2[n].first[0] == 's') {
                            mt = mtbool;
                            queries[p] = (args2[n].second);
                        } else if (args2[n].first[0] == 'p') {
                            mt = mttuple;
                            queries[p] = (args2[n].second);
                        } else if (args2[n].first[0] == 'e') {
                            mt = mtset;
                            queries[p] = (args2[n].second);
                        } else if (args2[n].first[0] == 'c') {
                            mt = mtbool;
                            queries[p] = (args2[n].second);
                        } else if (args2[n].first[0] == 'i') {
                            mt = mtdiscrete;
                            queries[p] = (args2[n].second);
                        } else if (args2[n].first[0] == 'z') {
                            mt = mtdiscrete;
                            queries[p] = (args2[n].second);
                        } else if (args2[n].first[0] == 'm') {
                            mt = mtcontinuous;
                            queries[p] = (args2[n].second);
                        } else if (args2[n].first == "gm") {
                            mt = mtgraph;
                            queries[p] = (args2[n].second);
                        } else if (args2[n].first == "ipy")
                            pythonfilenames.push_back(args2[n].second);
                        else if (args2[n].first == "isp")
                            storedprocedurefilenames.push_back(args2[n].second);

                    n++;
                }
                if (!p.second.empty() && p.second[0] == '-') {
                    --n;
                }
            } else if (p.second == "-v" && n+1 < args2.size()) {
                ++n;
                while (n < args2.size() && !(args2[n].first == "default" && args2[n].second.size() > 0 && args2[n].second[0] == '-' )) {
                    if (args2[n].first == "i")
                        verbosityfilenames.push_back(args2[n].second);
                    else
                        verbositylevels.push_back(args2[n].second);
                    n++;
                }
                if (n < args2.size()) {
                    p = args2[n];
                    if (!p.second.empty() && p.second[0] == '-') {
                        --n;
                    }
                } else
                    --n;
            } else if (p.second == "-g" && n+1 < args2.size()) {
                ++n;
                outoverwrite = false;
                outpassed = false;
                while (n < args2.size() && !(args2[n].first == "default" && args2[n].second.size() > 0 && args2[n].second[0] == '-' )) {
                    if (args2[n].first == "o")
                        outfilename = args2[n].second;
                    else if (args2[n].first == "default" && args2[n].second == "overwrite")
                            outoverwrite = true;
                    else if (args2[n].first == "default" && args2[n].second == "passed")
                            outpassed = true;
                    n++;
                }
                if (n < args2.size()) {
                    p = args2[n];
                    if (!p.second.empty() && p.second[0] == '-') {
                        --n;
                    }
                } else
                    --n;
            }

        }
        ++n;
    }









    /*
    auto args3 = cmdlineparseiterationthree(command);
    for (auto a : args3) {
        std::cout << a.first << " === " << "\n";
        for (auto b : a.second)
            std::cout << " -- " << b << "\n";
    }
    */

}

fcinstanceQtbridge::fcinstanceQtbridge(Ui::MainWindow* uiin, QueryTabWidget *tabin) : ui{uiin}, tab{tabin} {
    if (tab) {
        highlighters.clear();
        highlighters.push_back(new MyHighlighter(tab->findChild<QPlainTextEdit*>("queryedit") ));
        highlighters.push_back(new MyHighlighter(tab->findChild<QPlainTextEdit*>("cr1edit") ));
        highlighters.push_back(new MyHighlighter(tab->findChild<QPlainTextEdit*>("cr2edit") ));
        highlighters.push_back(new MyHighlighter(tab->findChild<QPlainTextEdit*>("cr3edit") ));
    }
}

fcinstanceQtbridge::~fcinstanceQtbridge() {
    // if (tab) {
        // delete queryHighlighter;
        // delete crit1Highlighter;
        // delete crit2Highlighter;
        // delete crit3Highlighter;
    // }
}

void fcinstanceQtbridge::setoutfile( const std::string& filename ) {
    const QString q = filename.c_str();
    tab->findChild<QComboBox*>("outfilecombo")->setEditText(q);
    tab->findChild<QCheckBox*>("outfilecheckbox")->setChecked(true);
}

std::string tosemicolondelimeted( const std::vector<std::string>& parameters ) {
    std::string out = parameters.size() > 0 ? parameters[0] : "";
    std::string d = STRINGDELIMITER_STR;
    std::string dspace = d + " ";
    for (int i = 1; i < parameters.size(); i++)
        out.append( dspace + parameters[i]);
    return out;
}

void fcinstanceQtbridge::addverbosity( const std::string& vbtoken ) {
    const QString q = vbtoken.c_str();
    QComboBox* cb = tab->findChild<QComboBox*>("verbositycombo");
    std::vector<std::string> parameters = parsesemicolondelimeted(cb->currentText().toStdString(),STRINGDELIMITER);
    parameters.push_back(vbtoken);
    const QString r = tosemicolondelimeted(parameters).c_str();
    cb->setEditText(r);
}

void fcinstanceQtbridge::addstoredprocedurefiles( const std::string& filename ) {
    const QString q = filename.c_str();
    QComboBox* cb = tab->findChild<QComboBox*>("storedprocedurescombo");
    std::vector<std::string> parameters = parsesemicolondelimeted(cb->currentText().toStdString(),STRINGDELIMITER);
    parameters.push_back(filename);
    const QString r = tosemicolondelimeted(parameters).c_str();
    cb->setEditText(r);
}

void fcinstanceQtbridge::addpythonfiles( const std::string& filename ) {
    const QString q = filename.c_str();
    QComboBox* cb = tab->findChild<QComboBox*>("pythoncombo");
    std::vector<std::string> parameters = parsesemicolondelimeted(cb->currentText().toStdString(),STRINGDELIMITER);
    parameters.push_back(filename);
    const QString r = tosemicolondelimeted(parameters).c_str();
    cb->setEditText(r);
}



void removestring( std::vector<std::string>& parameters, const std::string& str ) {
    for (int i = 0; i < parameters.size(); i++) {
        if (parameters[i] == str) {
            parameters.erase(parameters.begin() + i);
        }
    }
}

void addstring( std::vector<std::string>& parameters, const std::string& str ) {
    for (int i = 0; i < parameters.size(); i++) {
        if (parameters[i] == str) {
            return;
        }
    }
    parameters.push_back(str);
}


void fcinstanceQtbridge::setquery( const std::string& query ) {fc.queries[GLOBALCRITCOUNT] = query;}
void fcinstanceQtbridge::setcr1( const std::string& cr1 ) {fc.queries[1] = cr1;}
void fcinstanceQtbridge::setcr2( const std::string& cr2 ) {fc.queries[2] = cr2;}
void fcinstanceQtbridge::setcr3( const std::string& cr3 ) {fc.queries[3] = cr3;}
void fcinstanceQtbridge::setrandomizer( const randomizer& r ) {fc.randomizers.clear(); fc.randomizers.push_back(r);}
void fcinstanceQtbridge::clearrandomizer() {fc.randomizers.clear();}
void fcinstanceQtbridge::addinlinegraphs( const std::string& f ) {
    const QString q = f.c_str();
    QComboBox* cb = tab->findChild<QComboBox*>("inlinegraphscombo");
    std::vector<std::string> parameters = parsesemicolondelimeted(cb->currentText().toStdString(),';');
    parameters.push_back(f);
    const QString r = tosemicolondelimeted(parameters).c_str();
    cb->setEditText(r);
    tab->findChild<QCheckBox*>("inlinegraphscheckbox")->setChecked(true);
}
void fcinstanceQtbridge::removeinlinegraphs( const std::string& f ) {
    const QString q = f.c_str();
    QComboBox* cb = tab->findChild<QComboBox*>("inlinegraphscombo");
    auto v = parsesemicolondelimeted(cb->currentText().toStdString(),STRINGDELIMITER);
    removestring(v, cb->currentText().toStdString());
    cb->setCurrentText(tosemicolondelimeted(v).c_str());
    tab->findChild<QCheckBox*>("inlinegraphscheckbox")->setChecked(true);
}
void fcinstanceQtbridge::addgraphfiles( const std::string& filename ) {
    const QString q = filename.c_str();
    QComboBox* cb = tab->findChild<QComboBox*>("graphfilescombo");
    std::vector<std::string> parameters = parsesemicolondelimeted(cb->currentText().toStdString(),STRINGDELIMITER);
    parameters.push_back(filename);
    const QString r = tosemicolondelimeted(parameters).c_str();
    cb->setEditText(r);
    tab->findChild<QCheckBox*>("graphfilescheckbox")->setChecked(true);
}
void fcinstanceQtbridge::removegraphfiles( const std::string& filename ) {
    const QString q = filename.c_str();
    QComboBox* cb = tab->findChild<QComboBox*>("graphfilescombo");
    auto v = parsesemicolondelimeted(cb->currentText().toStdString(),STRINGDELIMITER);
    removestring(v, cb->currentText().toStdString());
    cb->setCurrentText(tosemicolondelimeted(v).c_str());
    tab->findChild<QCheckBox*>("graphfilescombo")->setChecked(true);
}
void fcinstanceQtbridge::setquerytype( const std::string& mt ) {
    if (mt == "mtbool")
        fc.mt = mtbool;
    else if (mt == "mtdiscrete")
        fc.mt = mtdiscrete;
    else if (mt == "mtcontinuous")
        fc.mt = mtcontinuous;
    else if (mt == "mtset")
        fc.mt = mtset;
    else if (mt == "mttuple")
        fc.mt = mttuple;
    else if (mt == "mtgraph")
        fc.mt = mtgraph;
    else if (mt == "mtstring")
        fc.mt = mtstring;
    else {
        std::cout << "Unknown measure type " << mt << std::endl;
        fc.mt = mtbool;
    }

}

void fcinstanceQtbridge::passparameterstowidgets() {

    // -r

    ui->querytabs->currentWidget()->findChild<QCheckBox*>("randomizercheckbox")->setChecked(!fc.randomizers.empty());
    std::vector<std::string> allrandomizers {};
    for (auto r : fc.randomizers) {
        ui->querytabs->currentWidget()->findChild<QComboBox*>("randomizercombobox")->setCurrentText(r.name.c_str());
        allrandomizers.push_back(r.name);
        ui->querytabs->currentWidget()->findChild<QComboBox*>("randomizerdimcombobox")->setCurrentText(std::to_string(r.dim).c_str());
        double edgecount;
        if (r.p) {
            edgecount = r.edgecnt;
            ui->querytabs->currentWidget()->findChild<QComboBox*>("randomizeredgecountcombobox")->setCurrentText(std::to_string(edgecount).c_str());
        } else {
            edgecount = (double)r.edgecnt /( (double)r.dim * (double)(r.dim-1)/2.0);
            if (edgecount > 1) {
                edgecount = 1;
                r.edgecnt = edgecount;
                r.p = true;
            }
            ui->querytabs->currentWidget()->findChild<QComboBox*>("randomizeredgecountcombobox")->setCurrentText(std::to_string(edgecount).c_str());
        }
        ui->querytabs->currentWidget()->findChild<QComboBox*>("randomizercountcombobox")->setCurrentText(std::to_string(r.cnt).c_str());
        ui->querytabs->currentWidget()->findChild<QComboBox*>("randomizeredgecountcombobox")->setCurrentText(std::to_string(r.edgecnt).c_str());
        ui->querytabs->currentWidget()->findChild<QSlider*>("randomizeredgecountslider")->setValue(static_cast<int>(std::round((edgecount*100))));
        ui->querytabs->currentWidget()->findChild<QComboBox*>("randomizercountcombobox")->setCurrentText(std::to_string(r.cnt).c_str());

    }
    ui->querytabs->currentWidget()->findChild<QComboBox*>("randomizercombobox")->setCurrentText(tosemicolondelimeted(allrandomizers).c_str());

    // -d

    ui->querytabs->currentWidget()->findChild<QComboBox*>("graphfilescombo")->setCurrentText(tosemicolondelimeted(fc.graphfilenames).c_str());
    ui->querytabs->currentWidget()->findChild<QCheckBox*>("graphfilescheckbox")->setChecked(!fc.graphfilenames.empty());

    ui->querytabs->currentWidget()->findChild<QComboBox*>("inlinegraphscombo")->setCurrentText(tosemicolondelimeted(fc.inlinegraphs).c_str());
    ui->querytabs->currentWidget()->findChild<QCheckBox*>("inlinegraphscheckbox")->setChecked(!fc.inlinegraphs.empty());

    // -a

    fc.queries.resize(GLOBALCRITCOUNT+1);

    int n = 0;
    std::vector<std::string> newqueries {};
    newqueries.resize(GLOBALCRITCOUNT);
    for (auto q : fc.queries) {
        if (q != "")
            newqueries[n++] = q;
    }
    newqueries.resize(n);
    int m;
    for (m = 0; m < n-1; ++m) {
        fc.queries[m] = newqueries[m];
    }
    for (; m < GLOBALCRITCOUNT+1; ++m) {
        fc.queries[m] = "";
    }
    if (n > 0)
        fc.queries[GLOBALCRITCOUNT] = newqueries[n-1];

    ui->querytabs->currentWidget()->findChild<QPlainTextEdit*>("queryedit")->document()->setPlainText(fc.queries[GLOBALCRITCOUNT].c_str());
    ui->querytabs->currentWidget()->findChild<QPlainTextEdit*>("cr1edit")->document()->setPlainText(fc.queries[0].c_str());
    ui->querytabs->currentWidget()->findChild<QPlainTextEdit*>("cr2edit")->document()->setPlainText(fc.queries[1].c_str());
    ui->querytabs->currentWidget()->findChild<QPlainTextEdit*>("cr3edit")->document()->setPlainText(fc.queries[2].c_str());
    for (auto s : measuretypestrings)
        if (s.second == fc.mt)
            ui->querytabs->currentWidget()->findChild<QComboBox*>("mtcombobox")->setCurrentText(s.first.c_str());
    std::string temp {};
    for (auto v : fc.verbositylevels) {
        if (temp != "")
            temp.append("; ");
        temp.append(v);
    }
    for (auto vf : fc.verbosityfilenames) {
        if (temp != "")
            temp.append("; ");
        temp.append("i=\"" + vf+"\"");
    }
    ui->querytabs->currentWidget()->findChild<QComboBox*>("verbositycombo")->setCurrentText(temp.c_str());

    // -g

    if (fc.outfilename != "") {
        ui->querytabs->currentWidget()->findChild<QCheckBox*>("outfilecheckbox")->setChecked(true);
        ui->querytabs->currentWidget()->findChild<QComboBox*>("outfilecombo")->setCurrentText(fc.outfilename.c_str());
        ui->querytabs->currentWidget()->findChild<QCheckBox*>("outpassedcheckbox")->setChecked(fc.outpassed);
        ui->querytabs->currentWidget()->findChild<QCheckBox*>("outoverwritecheckbox")->setChecked(fc.outoverwrite);
    } else
        ui->querytabs->currentWidget()->findChild<QCheckBox*>("outfilecheckbox")->setChecked(false);

    // ...


}

void fcinstanceQtbridge::populatefcparameters() {

    auto mt = tab->findChild<QComboBox*>("mtcombobox");
    setquerytype(mt->currentText().toStdString());

    fc.storedprocedurefilenames = parsesemicolondelimeted(tab->findChild<QComboBox*>("storedprocedurescombo")->currentText().toStdString(),STRINGDELIMITER);
    fc.pythonfilenames = parsesemicolondelimeted(tab->findChild<QComboBox*>("pythoncombo")->currentText().toStdString(),STRINGDELIMITER);
    fc.verbositylevels = parsesemicolondelimeted(tab->findChild<QComboBox*>("verbositycombo")->currentText().toStdString(),STRINGDELIMITER); //
    if (tab->findChild<QCheckBox*>("graphfilescheckbox")->isChecked()) {
        fc.graphfilenames = parsesemicolondelimeted(tab->findChild<QComboBox*>("graphfilescombo")->currentText().toStdString(),STRINGDELIMITER);
        for (auto f : fc.graphfilenames) {
            tab->findChild<QComboBox*>("graphfilescombo")->addItem(f.c_str());
        }
    } else
        fc.graphfilenames.clear();
    if (tab->findChild<QCheckBox*>("inlinegraphscheckbox")->isChecked()) {
        for (auto f : fc.inlinegraphs) {
            tab->findChild<QComboBox*>("inlinegraphscombo")->addItem(f.c_str());
        }
        fc.inlinegraphs = parsesemicolondelimeted(tab->findChild<QComboBox*>("inlinegraphscombo")->currentText().toStdString(),STRINGDELIMITER);
    } else
        fc.inlinegraphs.clear();
    if (tab->findChild<QCheckBox*>("outfilecheckbox")->isChecked()) {
        fc.outfilename = tab->findChild<QComboBox*>("outfilecombo")->currentText().toStdString();
        fc.outoverwrite = tab->findChild<QCheckBox*>("outoverwritecheckbox")->isChecked();
        fc.outpassed = tab->findChild<QCheckBox*>("outpassedcheckbox")->isChecked();
        tab->findChild<QComboBox*>("outfilecombo")->addItem(fc.outfilename.c_str());
    } else
        fc.outfilename.clear();
    fc.randomizersbool = tab->findChild<QCheckBox*>("randomizercheckbox")->isChecked();
    if (fc.randomizersbool) {
        randomizer r;
        r.name = tab->findChild<QComboBox*>("randomizercombobox")->currentText().toStdString();
        r.dim = stoi(tab->findChild<QComboBox*>("randomizerdimcombobox")->currentText().toStdString());
        r.p = true; // tab->findChild<QCheckBox*>("randomizerpcheckbox")->isChecked();
        r.edgecnt = stof(tab->findChild<QComboBox*>("randomizeredgecountcombobox")->currentText().toStdString());
        r.cnt = stoi(tab->findChild<QComboBox*>("randomizercountcombobox")->currentText().toStdString());
        setrandomizer(r);
    } else {
        clearrandomizer();

    }



    auto cr1 = tab->findChild<QPlainTextEdit*>("cr1edit")->toPlainText().toStdString();
    auto cr2 = tab->findChild<QPlainTextEdit*>("cr2edit")->toPlainText().toStdString();
    auto cr3 = tab->findChild<QPlainTextEdit*>("cr3edit")->toPlainText().toStdString();
    auto query = tab->findChild<QPlainTextEdit*>("queryedit")->toPlainText().toStdString();

    fc.queries.clear();
    if (cr1 != "") {
        std::string s {};
        for (auto c : cr1)
            if (c == '"')
                s.append("\\\"");
            else
                s.push_back(c);
        fc.queries.push_back(s);
    }
    if (cr2 != "") {
        std::string s {};
        for (auto c : cr2)
            if (c == '"')
                s.append("\\\"");
            else
                s.push_back(c);
        fc.queries.push_back(s);
    }
    if (cr3 != "") {
        std::string s {};
        for (auto c : cr3)
            if (c == '"')
                s.append("\\\"");
            else
                s.push_back(c);
        fc.queries.push_back(s);
    }
    if (query != "") {
        std::string s {};
        for (auto c : query)
            if (c == '"')
                s.append("\\\"");
            else
                s.push_back(c);
        fc.queries.push_back(s);
    }
}

int fcinstanceQtbridge::logQuery() {
    populatefcparameters();
    return logQuerypostpopulate();
}

int fcinstanceQtbridge::logQuerypostpopulate() {
    const std::string filename = ui->logfilecombo->currentText().toStdString();

    // Open the file in append mode (std::ios::app)
    // The 'ofs' object is an output file stream

    std::ofstream ofs;

    std::filesystem::path file_path(filename);
    if (!std::filesystem::exists(file_path)) {
        ofs.open(filename.c_str());
        if (!ofs.is_open()) {
            std::cerr << "Error: Could not open the file " << filename << " for writing." << std::endl;
            return 1; // Return with an error code
        }
        ofs << FLAGCALCHEADER << std::endl << std::endl;
    } else
        ofs.open(filename.c_str(),std::ios::app);

    // Check if the file was opened successfully
    if (!ofs.is_open()) {
        std::cerr << "Error: Could not open the file " << filename << " for writing." << std::endl;
        return 1; // Return with an error code
    }

    // Append a new line of data


    std::string comment = tab->findChild<QPlainTextEdit*>("commenttextedit")->toPlainText().toStdString();
    if (comment != "")
        comment = "# " + comment + "\n";
    std::string command = FLAGCALCINVOKE;
    command = command + " " + fc.parse().c_str();
    ofs << comment;
    ofs << command << std::endl;
    if (comment != "")
        ofs << std::endl;

    // Close the file stream
    ofs.close();
    return 0;
}

int fcinstanceQtbridge::runandlogQuery() {
    populatefcparameters();
    int i = runQuerypostpopulate();
    int j = logQuerypostpopulate();
    return i || j;
}

int fcinstanceQtbridge::runQuery() {

    // populate fc parameters

    populatefcparameters();
    return runQuerypostpopulate();
}

int fcinstanceQtbridge::runQuerypostpopulate() {


    auto parameters = "r";

    char command[CMDLINEMAXLENGTH];

    char cdworkingdirstring[1024] = "cd ";
    strcat(cdworkingdirstring,DEFAULTDIRECTORY);
    int len = snprintf(command, sizeof(command),"%s; %s %s", cdworkingdirstring, FLAGCALCEXECUTABLEPATH, fc.parse().c_str() );

    if (len >= CMDLINEMAXLENGTH) {
        throw std::runtime_error("command too long");
    }

    // QApplication::setOverrideCursor(Qt::WaitCursor);
    // QApplication::processEvents();
#if defined(_WIN32) || defined(_WIN64)
    FILE* pipe = _popen(FLAGCALCEXECUTABLEPATH, parameters);
#else
    // std::cout << command << std::endl;

    FILE* pipe = popen(command, parameters);
#endif

    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    // QApplication::restoreOverrideCursor();

    std::array<char, 128> buffer;
    std::string result {};

    auto textedit = ui->outputtextedit;

    std::ofstream ofs;
    auto fn = ui->outputfilecombobox->currentText().toStdString();
    if (fn != "") {
        ofs.open(fn, std::ios::app);;

        if (!ofs.is_open()) {
            std::cerr << "Error: Could not open the file " << fn << std::endl;
        }
    }

    // Read the output a line or buffer at a time and append to the result string
    while (std::fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
        textedit->appendPlainText(buffer.data());
        if (ofs.is_open()) {
            ofs << buffer.data();
        }
    }
    // std::cout << result << std::endl;

    // Close the pipe and get the exit status
#if defined(_WIN32) || defined(_WIN64)
    int status = _pclose(pipe);
#else
    int status = pclose(pipe);
#endif

    // Optional: Check the exit status if needed
    // if (status == -1) { /* Handle error closing pipe */ }

    if (ofs.is_open())
        ofs.close();

    return 0;
}
