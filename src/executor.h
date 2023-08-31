#include"semanticParser.h"

void executeCommand();

void executeCLEAR();
void executeCROSS();
void executeDISTINCT();
void executeEXPORT_MATRIX();
void executeEXPORT();
void executeINDEX();
void executeJOIN();
void executeLIST();
void executeLOAD_MATRIX();
void executeLOAD();
void executePRINT_MATRIX();
void executePRINT();
void executePROJECTION();
void executeRENAME_MATRIX();
void executeRENAME();
void executeSELECTION();
void executeSORT();
void executeSOURCE();
void executeTRANSPOSE_MATRIX();
void executeCHECKSYMMETRY();
void executeCOMPUTE();

bool evaluateBinOp(int value1, int value2, BinaryOperator binaryOperator);
void printRowCount(int rowCount);