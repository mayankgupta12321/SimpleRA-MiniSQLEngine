#include "tableCatalogue.h"
#include "matrixCatalogue.h"

using namespace std;

enum QueryType
{
    CLEAR,
    CROSS,
    DISTINCT,
    EXPORT_MATRIX,
    EXPORT,
    INDEX,
    JOIN,
    LIST,
    LOAD_MATRIX,
    LOAD,
    PRINT_MATRIX,
    PRINT,
    PROJECTION,
    RENAME_MATRIX,
    RENAME,
    SELECTION,
    SORT,
    SOURCE,
    TRANSPOSE_MATRIX,
    CHECKSYMMETRY,
    COMPUTE,
    ORDER,
    GROUP,
    UNDETERMINED
};

enum BinaryOperator
{
    LESS_THAN,
    GREATER_THAN,
    LEQ,
    GEQ,
    EQUAL,
    NOT_EQUAL,
    NO_BINOP_CLAUSE
};

enum SortingStrategy
{
    ASC,
    DESC,
    NO_SORT_CLAUSE
};

enum SelectType
{
    COLUMN,
    INT_LITERAL,
    NO_SELECT_CLAUSE
};

enum AggregateFunction
{
    SUM,
    MAX,
    MIN,
    AVG,
    NO_AGGREGATE_CLAUSE
};

class ParsedQuery
{

public:
    QueryType queryType = UNDETERMINED;

    string clearRelationName = "";

    string crossResultRelationName = "";
    string crossFirstRelationName = "";
    string crossSecondRelationName = "";

    string distinctResultRelationName = "";
    string distinctRelationName = "";

    string exportRelationName = "";

    IndexingStrategy indexingStrategy = NOTHING;
    string indexColumnName = "";
    string indexRelationName = "";

    BinaryOperator joinBinaryOperator = NO_BINOP_CLAUSE;
    string joinResultRelationName = "";
    string joinFirstRelationName = "";
    string joinSecondRelationName = "";
    string joinFirstColumnName = "";
    string joinSecondColumnName = "";

    string loadRelationName = "";
    string printRelationName = "";
    string transposeRelationName  = "";
    string checksymmetryRelationName  = "";
    string computeRelationName  = "";

    string projectionResultRelationName = "";
    vector<string> projectionColumnList;
    string projectionRelationName = "";

    string renameFromColumnName = "";
    string renameToColumnName = "";
    string renameRelationName = "";

    string renameFromMatrixName = "";
    string renameToMatrixName = "";

    SelectType selectType = NO_SELECT_CLAUSE;
    BinaryOperator selectionBinaryOperator = NO_BINOP_CLAUSE;
    string selectionResultRelationName = "";
    string selectionRelationName = "";
    string selectionFirstColumnName = "";
    string selectionSecondColumnName = "";
    int selectionIntLiteral = 0;

    string sortRelationName = "";
    string sourceFileName = "";
    vector<string> sortColumnList;
    vector<SortingStrategy> sortStrategyList;

    SortingStrategy orderSortingStrategy = NO_SORT_CLAUSE;
    string orderResultRelationName = "";
    string orderRelationName = "";
    string orderColumnName = "";

    string groupResultRelationName = "";
    string groupRelationName = "";
    string groupColumnName = "";
    BinaryOperator groupBinaryOperator = NO_BINOP_CLAUSE;
    int groupAggregateColumnValue = 0;
    AggregateFunction groupAggregateFunction = NO_AGGREGATE_CLAUSE;
    string groupAggregateColumnName = "";
    AggregateFunction groupReturnAggregateFunction = NO_AGGREGATE_CLAUSE;
    string groupReturnAggregateColumnName = "";

    ParsedQuery();
    void clear();
};

bool syntacticParse();
bool syntacticParseCLEAR();
bool syntacticParseCROSS();
bool syntacticParseDISTINCT();
bool syntacticParseEXPORT();
bool syntacticParseINDEX();
bool syntacticParseJOIN();
bool syntacticParseLIST();
bool syntacticParseLOAD();
bool syntacticParsePRINT();
bool syntacticParsePROJECTION();
bool syntacticParseRENAME();
bool syntacticParseSELECTION();
bool syntacticParseSORT();
bool syntacticParseSOURCE();
bool syntacticParseORDER();
bool syntacticParseGROUP();

bool syntacticParseLOAD_MATRIX();
bool syntacticParsePRINT_MATRIX();
bool syntacticParseRENAME_MATRIX();
bool syntacticParseEXPORT_MATRIX();
bool syntacticParseTRANSPOSE_MATRIX();
bool syntacticParseCHECKSYMMETRY();
bool syntacticParseCOMPUTE();

bool isFileExists(string tableName);
bool isQueryFile(string fileName);
