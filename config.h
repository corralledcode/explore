//
// Created by peterglenn on 12/21/25.
//

#ifndef EXPLORE_CONFIG_H
#define EXPLORE_CONFIG_H
#include <QProcess>
#include <regex>

enum measuretype { mtbool, mtdiscrete, mtcontinuous, mtset, mttuple, mtstring, mtgraph, mtuncast };
inline const std::map<std::string, measuretype>& measuretypestrings
{{"mtbool", mtbool},
{"mtdiscrete", mtdiscrete},
{"mtcontinuous", mtcontinuous},
{"mtset", mtset},
{"mttuple", mttuple},
{"mtstring", mtstring},
{"mtgraph", mtgraph},
{"mtuncast", mtuncast}};

#define LONGINT long int
#define FLAGCALCEXECUTABLEPATH "~/CLionProjects/flagcalc/cmake-build-debug/flagcalc"
#define FLAGCALCHEADER "PTH=${PTH:-'../bin'}"
#define FLAGCALCINVOKE "$PTH/flagcalc"
#define GLOBALCRITCOUNT 3
#define STRINGDELIMITER ';'
#define STRINGDELIMITER_STR ";"
#define CMDLINEMAXLENGTH 131072
#define DEFAULTDIRECTORY "~/CLionProjects/explore/"


// ----

#define VERBOSE_CMPFINGERPRINTLEVEL "cmp"
#define VERBOSE_ENUMISOMORPHISMSLEVEL "enum"

#define VERBOSE_DONTLISTISOS "Noiso"
#define VERBOSE_LISTGRAPHS "graphs"
#define VERBOSE_LISTFINGERPRINTS "fp"
#define VERBOSE_ISOS "Iso"
#define VERBOSE_RUNTIMES "rt"
#define VERBOSE_VERBOSITYRUNTIME "vrunt"
#define VERBOSE_VERBOSITYFILEAPPEND "vappend"
#define VERBOSE_MINIMAL "min"
#define VERBOSE_MANTELSTHEOREM "Mantel"
#define VERBOSE_FINGERPRINT "Fp"
#define VERBOSE_SAMPLERANDOMMATCHING "srm"
#define VERBOSE_FPMINIMAL "FpMin"
#define VERBOSE_FPNONE "fpnone"
#define VERBOSE_APPLYCRITERION "crit"
#define VERBOSE_SUBOBJECT "subobj"
#define VERBOSE_PAIRWISEDISJOINT "pd"
#define VERBOSE_APPLYSET "set"
#define VERBOSE_SETVERBOSE "allsets"
#define VERBOSE_CRITVERBOSE "allcrit"
#define VERBOSE_MEASVERBOSE "allmeas"
#define VERBOSE_TALLYVERBOSE "alltally"

#define VERBOSE_APPLYSTRING "strmeas"
#define VERBOSE_APPLYGRAPH "measg"
#define VERBOSE_RANDOMSUMMARY "randomizer"

#define VERBOSE_ALL "Noiso graphs fp Iso rt vrunt vappend min Mantel Fp srm FpMin subobj pd set allsets randomizer"
#define VERBOSE_DEFAULT "Noiso graphs fp Iso rt vrunt vappend min Mantel Fp srm FpMin crit rm fpnone vrunt subobj pd set allsets randomizer"

#define VERBOSE_FORDB "db"

// ---

enum class formulaoperator
{foliteral,fofunction, foconstant, foderef,
    foqforall, foqexists,
    foplus, fominus, fotimes, fodivide, foexponent, fomodulus, // 11
    folte, folt, foe, fone, fogte, fogt, founion, fodupeunion, fointersection, foelt, // 21
    foand,foor,foxor,fonot,foimplies,foiff,foif,fotrue,fofalse,fovariable, fovariablederef, //32
    foqsum, foqproduct, foqmin, foqmax, foqaverage, foqrange,
    foqtally, foqcount, foqset, foqdupeset, foqtuple, foqunion, foqdupeunion, foqintersection,
    foqmedian, foqmode,
    foqany, foqexistsunique, foqanyn, foqexistsnunique, foqexistsn, foqforalln,
    foswitch, focases, foin, fonaming, foas,
    fosetminus, fosetxor, fomeet, fodisjoint,
    fothreaded, fogpu,
    forpartition, forsort};

inline const std::map<std::string,formulaoperator> stringoperatorsmap
        {{"AND",formulaoperator::foand},
        {"OR",formulaoperator::foor},
        {"NOT",formulaoperator::fonot},
        {"XOR",formulaoperator::foxor},
        {"IMPLIES",formulaoperator::foimplies},
        {"IFF",formulaoperator::foiff},
        {"IF",formulaoperator::foif},
        {"SET",formulaoperator::foqset},
        {"SETD",formulaoperator::foqdupeset},
        {"TUPLE", formulaoperator::foqtuple},
        {"FORALL",formulaoperator::foqforall},
        {"EXISTS",formulaoperator::foqexists},
        {"ANY", formulaoperator::foqany},
        {"EXISTSUNIQUE", formulaoperator::foqexistsunique},
        {"ANYN",formulaoperator::foqanyn},
        {"EXISTSNUNIQUE", formulaoperator::foqexistsnunique},
        {"EXISTSN", formulaoperator::foqexistsn},
        {"FORALLN", formulaoperator::foqforalln},
        {"CUP",formulaoperator::founion},
        {"CUPD",formulaoperator::fodupeunion},
        {"CAP",formulaoperator::fointersection},
        {"ELT",formulaoperator::foelt},
        {"SUM",formulaoperator::foqsum},
        {"MIN",formulaoperator::foqmin},
        {"MAX",formulaoperator::foqmax},
        {"PRODUCT",formulaoperator::foqproduct},
        {"AVERAGE",formulaoperator::foqaverage},
        {"RANGE",formulaoperator::foqrange},
        {"TALLY",formulaoperator::foqtally},
        {"COUNT",formulaoperator::foqcount},
        {"BIGCUP",formulaoperator::foqunion},
        {"BIGCUPD",formulaoperator::foqdupeunion},
        {"BIGCAP", formulaoperator::foqintersection},
        {"MEDIAN", formulaoperator::foqmedian},
        {"MODE", formulaoperator::foqmode},
        {"IN", formulaoperator::foin},
        {"NAMING", formulaoperator::fonaming},
        {"AS", formulaoperator::foas},
        {"SETMINUS", formulaoperator::fosetminus},
        {"SETXOR", formulaoperator::fosetxor},
        {"MEET", formulaoperator::fomeet},
        {"DISJOINT", formulaoperator::fodisjoint},
        {"THREADED", formulaoperator::fothreaded},
        {"GPU", formulaoperator::fogpu},
        {"PARTITION", formulaoperator::forpartition},
        {"SORT", formulaoperator::forsort}};

// ---

inline const std::vector<std::string> measures
    {"abs","acos", "asin", "atan", "atan2", "bell", "ceil", "cos", "exp", "floor", "gamma",
    "isinf", "log", "log10", "log2", "logb","mod","nchoosek", "phi", "pi", "sin", "sqrt", "stirling",
    "tan",
    "truec","cr1","forestc", "treec", "Knc", "connc", "radiusc", "circc", "diamc", "conn1c",
    "kconnc","ledgeconnc","ac", "ec", "eadjc", "bipc", "Nssc", "Separatesc", "connvssc",
    "connvc", "connvsc", "indnc", "nwisec", "toBool", "embedsc", "connvusingsetc",
    "dm", "legacygirthm", "cliquem", "connm", "radiusm", "circm", "lcircm","diamm", "girthm", "toReal",
    "dimm", "edgecm","deltam", "Deltam", "Knt", "indnt", "cyclet", "kappat", "lambdat", "vdt",
    "st", "lt", "pct", "idxt", "Nt", "cyclesvt", "Chit", "Chigreedyt", "Chiprimet", "Chiprimegreedyt",
    "Nsst", "cyclest", "toInt", "connt", "V", "Ps", "Sizedsubset", "NN", "Nulls", "E",
    "idxs", "TupletoSet", "Pathss", "Cyclesvs", "Setpartition", "nE", "Cycless", "Perms",
    "Componentss", "Edgess", "Automs", "Connc", "Ns", "Choices", "Choice2s", "as", "eadjs",
    "e2eadjs", "Epathss", "Pathsusingvsets", "Maps", "Gpathss", "Chip", "Chigreedyp", "Sp",
    "nwalksbetweenp", "Connv", "Connmatrix", "Subp", "CUDAnwalksbetweenp", "CUDAconnv"};

inline void openexternaleditor( QObject* sender, const QString& fn ) {
    QProcess *process = new QProcess(sender);

    // Determine the editor and arguments based on the operating system
    QString program;
    QStringList arguments;

#if defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    // Use 'vim' or 'nano' as an example for Linux/macOS
    // You might need to check if these are installed
    program = "gedit";
    arguments << fn;
    // Alternatives:
    // program = "nano";
    // arguments << filename;
    // program = "gedit"; // For GUI editor
    // arguments << filename;

#elif defined(Q_OS_WIN)
    // Use 'notepad.exe' as an example for Windows
    program = "notepad.exe";
    arguments << fn;
#endif

    // Start the process
    process->start(program, arguments);

    // Optional: wait for the process to finish and block the GUI
    // This is generally discouraged in GUI applications as it freezes the UI,
    // but useful if you need to ensure the user is done editing before continuing.
}



inline std::vector<std::string> parse_quoted_string(const std::string& input) {
    std::vector<std::string> tokens;
    std::string current_token;
    bool inside_quotes = false;

    bool escaped = false;
    for (char c : input) {
        if (c == '\\') {
            escaped = true;
            continue;
        }
        if (c == '"' && !escaped) {
            inside_quotes = !inside_quotes;
            // Optional: you can choose to include the quote in the token or not
            // current_token += c;
        } else if (c == ' ' && !inside_quotes) {
            if (!current_token.empty()) {
                tokens.push_back(current_token);
                current_token.clear();
            }
        } else {
            current_token += c;
        }
        escaped = false;
    }
    // Add the last token if it's not empty
    if (!current_token.empty()) {
        tokens.push_back(current_token);
    }
    return tokens;
}




inline std::vector<std::pair<std::string,std::string>>  cmdlineparseiterationtwo( const std::vector<std::string> args ) {
    std::vector<std::pair<std::string,std::string>> res {};
    for (int i = 1; i < args.size(); ++i) {

        std::regex r("([[:alnum:]]+)=((\\w|[[:punct:]]|\\s)*)"); // entire match will be 2 numbers

        std::smatch m;
        std::regex_search(args[i], m, r);

        if (m.size() > 2) {
            res.push_back({m[1],m[2]});
        } else {
            if (m.size() > 0) {
                res.push_back( {"default",m[0]});
            } else {
                res.push_back( {"default",args[i]});
            }
        }
        //for(auto v: m) {
        //    std::cout << v << std::endl;
        //    res.push_back({std::to_string(i),v});
    }
    return res;

}


inline std::vector<std::pair<std::string,std::vector<std::string>>> cmdlineparseiterationthree( const std::string arg ) {
    std::vector<std::string> res;
    std::vector<std::pair<std::string,std::vector<std::string>>> overallres {};
    std::regex r( "(-|\\w|\\(|\\)|,)+(;|\\s|:)?");

    for (std::sregex_iterator p(arg.begin(),arg.end(),r); p!=std::sregex_iterator{}; ++p) {
        std::regex r2( "(-|\\w)*");
        std::smatch m2;
        std::string tmp2 = (*p)[0];
        std::regex_search(tmp2,m2,r2);
        std::string s2 = "default";
        if (m2.size() > 0)
            s2 = m2[0];

        //std::regex r3( "\\((\\w)+,(\\w)+\\)");
        std::regex r3( "\\(([^\\)]+)\\)" );

        std::vector<std::string> parametersall {};

        for (std::sregex_iterator p3( tmp2.begin(), tmp2.end(),r3); p3 != std::sregex_iterator{}; ++p3) {
            parametersall.push_back((*p3)[1]);
        }

        std::vector<std::string> parameters {};

        if (!parametersall.empty()) {

            std::regex r4( "(.+?)(?:,|$)" );

            for (std::sregex_iterator p4( parametersall[0].begin(),parametersall[0].end(),r4); p4 != std::sregex_iterator{}; ++p4) {
                parameters.push_back((*p4)[1]);
            }

        }


        overallres.push_back({s2,parameters});
    }

    return overallres;
}








#endif //EXPLORE_CONFIG_H